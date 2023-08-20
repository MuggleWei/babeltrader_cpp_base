#include "tcp_listen_handle.h"
#include "tcp_server_session.h"

TcpListenHandle::TcpListenHandle()
	: worker_idx_(0)
	, codec_chain_(nullptr)
	, dispatcher_(nullptr)
	, worker_evloops_(nullptr)
	, last_timer_trigger_(0)
{
}
TcpListenHandle::~TcpListenHandle()
{
}

void TcpListenHandle::OnAddCtx(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);
	MUGGLE_UNUSED(ctx);

	LOG_INFO("TCP Listen handle add context");
}
void TcpListenHandle::OnConnect(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	char addr[MUGGLE_SOCKET_ADDR_STRLEN];
	muggle_socket_remote_addr(ctx->GetSocket(), addr, sizeof(addr), 0);

	LOG_INFO("session connection: addr=%s", addr);

	TcpServerSession *session = new TcpServerSession();

	ctx->SetUserData(session);

	session->SetSocketContext(ctx);
	session->InitBytesBuffer(8 * 1024 * 1024);
	session->SetCodecChain(codec_chain_);
	session->UpdateActiveTime(time(NULL));
	session->SetAddr(addr);
	session->SetDispatcher(dispatcher_);

	ctx_set_.insert(ctx);
}
void TcpListenHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session *)ctx->GetUserData();
	if (!session->HandleMessage()) {
		LOG_WARNING("failed handle message: addr=%s", session->GetAddr());
		ctx->Shutdown();
		return;
	}
}
void TcpListenHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerSession *session = (TcpServerSession *)ctx->GetUserData();
	if (session == nullptr) {
		return;
	}

	if (session->IsDetached()) {
		const std::string &user_id = session->GetUserID();
		int idx = 0;
		auto it = user_workder_dict_.find(user_id);
		if (it == user_workder_dict_.end()) {
			idx = worker_idx_++;
			idx %= worker_evloops_->size();
			worker_idx_ %= worker_evloops_->size();

			user_workder_dict_[user_id] = idx;
		} else {
			idx = it->second;
		}

		LOG_INFO("move socket context to worker event loop: "
				"addr=%s, user_id=%s, worker_idx=%d",
				session->GetAddr(), user_id.c_str(), idx);

		session->SetDetach(false);
		ctx->ClearFlags();

		NetEventLoop *worker = (*worker_evloops_)[idx];
		worker->AddContext(ctx);

		ctx_set_.erase(ctx);
	} else {
		LOG_INFO("session disconnection: addr=%s", session->GetAddr());
	}
}
void TcpListenHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerSession *session = (TcpServerSession *)ctx->GetUserData();
	if (session) {
		LOG_INFO("session release: addr=%s", session->GetAddr());
		delete session;
	}
	ctx_set_.erase(ctx);
}
void TcpListenHandle::OnTimer(NetEventLoop *evloop)
{
	MUGGLE_UNUSED(evloop);

	time_t curr_ts = time(NULL);
	time_t elapsed = curr_ts - last_timer_trigger_;
	if (elapsed < 5) {
		return;
	}
	last_timer_trigger_ = curr_ts;

	for (SocketContext *ctx : ctx_set_) {
		Session *session = (Session *)ctx->GetUserData();
		time_t elapsed = curr_ts - session->LastActiveTime();
		if (elapsed > 15) {
			ctx->Shutdown();
		}
	}
}

void TcpListenHandle::SetCodecChain(CodecChain *codec_chain)
{
	codec_chain_ = codec_chain;
}
void TcpListenHandle::SetDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}
void TcpListenHandle::SetWorkers(std::vector<NetEventLoop *> *workers)
{
	worker_evloops_ = workers;
}
