#include "tcp_server_handle.h"
#include "babeltrader/cpp/session.h"
#include "tcp_server_peer.h"

TcpServerHandle::TcpServerHandle()
	: last_timer_trigger_(0)
	, codec_chain_(nullptr)
	, dispatcher_(nullptr)
{
}
TcpServerHandle::~TcpServerHandle()
{
}

void TcpServerHandle::OnAddCtx(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);
	MUGGLE_UNUSED(ctx);
	LOG_INFO("TCP Listen handle add context");
}
void TcpServerHandle::OnConnect(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	char addr[MUGGLE_SOCKET_ADDR_STRLEN];
	muggle_socket_remote_addr(ctx->GetSocket(), addr, sizeof(addr), 0);

	LOG_INFO("session connection: addr=%s", addr);

	TcpServerPeer *session = new TcpServerPeer();

	ctx->SetUserData(session);

	session->SetSocketContext(ctx);
	session->InitBytesBuffer(8 * 1024 * 1024);
	session->SetCodecChain(codec_chain_);
	session->UpdateActiveTime(time(NULL));
	session->SetAddr(addr);
	session->SetDispatcher(dispatcher_);

	ctx_set_.insert(ctx);
}
void TcpServerHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session *)ctx->GetUserData();
	if (!session->HandleMessage()) {
		LOG_WARNING("failed handle message: addr=%s", session->GetAddr());
		ctx->Shutdown();
		return;
	}
}
void TcpServerHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session *)ctx->GetUserData();
	LOG_INFO("session disconnection: addr=%s", session->GetAddr());
}
void TcpServerHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerPeer *session = (TcpServerPeer *)ctx->GetUserData();
	if (session) {
		LOG_INFO("session release: addr=%s", session->GetAddr());
		delete session;
	}
	ctx_set_.erase(ctx);

	// TODO: testxxx
	LOG_INFO("bye");
	evloop->Exit();
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
