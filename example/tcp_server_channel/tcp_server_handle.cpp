#include "tcp_server_handle.h"
#include "babeltrader/cpp/session.h"
#include "tcp_server_peer.h"
#include "chan_msg.h"

TcpServerHandle::TcpServerHandle()
	: last_timer_trigger_(0)
	, codec_chain_(nullptr)
	, chan_(nullptr)
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
	session->SetChannel(chan_);

	ctx_set_.insert(ctx);

	// notify channel connection
	ctx->RefCntRetain();
	NotifyChannel(CHAN_MSG_CONNECT, ctx);
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
	if (session) {
		LOG_INFO("session disconnection: addr=%s", session->GetAddr());

		// notify channel disconnection
		NotifyChannel(CHAN_MSG_DISCONNECT, ctx);
	}
	ctx_set_.erase(ctx);
}
void TcpServerHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerPeer *session = (TcpServerPeer *)ctx->GetUserData();
	if (session) {
		LOG_INFO("session release: addr=%s", session->GetAddr());
		delete session;
	}
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
void TcpServerHandle::SetChannel(Channel *chan)
{
	chan_ = chan;
}

void TcpServerHandle::NotifyChannel(uint32_t chan_msg_type, SocketContext *ctx)
{
	chan_msg_t *chan_msg = (chan_msg_t *)malloc(sizeof(chan_msg_t));
	memset(chan_msg, 0, sizeof(*chan_msg));
	chan_msg->ctx = ctx;
	chan_msg->msg_type = chan_msg_type;
	chan_->Write(chan_msg);
}
