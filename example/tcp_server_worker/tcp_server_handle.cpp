#include "tcp_server_handle.h"
#include "tcp_server_peer.h"

TcpServerHandle::TcpServerHandle()
	: codec_chain_(nullptr)
	, dispatcher_(nullptr)
	, worker_idx_(0)
	, last_timer_trigger_(0)
{
}
TcpServerHandle::~TcpServerHandle()
{
}

void TcpServerHandle::OnAddCtx(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);
	MUGGLE_UNUSED(ctx);

	TcpServerPeer *session = (TcpServerPeer *)ctx->GetUserData();
	LOG_INFO("worker[%d] add context: addr=%s", worker_idx_,
			 session->GetAddr());

	ctx_set_.insert(ctx);
}
void TcpServerHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerPeer *session = (TcpServerPeer *)ctx->GetUserData();
	if (!session->HandleMessage()) {
		LOG_WARNING("failed handle message: addr=%s", session->GetAddr());
		ctx->Shutdown();
	}
}
void TcpServerHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerPeer *session = (TcpServerPeer *)ctx->GetUserData();
	LOG_INFO("disconnect: addr=%s", session->GetAddr());
}
void TcpServerHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerPeer *session = (TcpServerPeer *)ctx->GetUserData();
	LOG_INFO("session release: addr=%s", session->GetAddr());
	delete session;

	ctx_set_.erase(ctx);
}
void TcpServerHandle::OnTimer(NetEventLoop *evloop)
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

void TcpServerHandle::SetCodecChain(CodecChain *codec_chain)
{
	codec_chain_ = codec_chain;
}
void TcpServerHandle::SetDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}
void TcpServerHandle::SetWorkerIdx(int idx)
{
	worker_idx_ = idx;
}
