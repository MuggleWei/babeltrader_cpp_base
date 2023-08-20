#include "channel_worker.h"
#include "chan_msg.h"
#include "tcp_server_session.h"

ChannelWorker::ChannelWorker()
	: chan_(nullptr)
	, dispatcher_(nullptr)
{
}
ChannelWorker::~ChannelWorker()
{
}

void ChannelWorker::SetChannerl(Channel *chan)
{
	chan_ = chan;
}
void ChannelWorker::SetDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}

void ChannelWorker::Run()
{
	while (true) {
		chan_msg_t *msg = (chan_msg_t *)chan_->Read();

		switch (msg->msg_type) {
		case CHAN_MSG_DATA: {
			dispatcher_->Dispatch((Session *)msg->ctx->GetUserData(), msg->data,
								  msg->datalen);
		} break;
		case CHAN_MSG_CONNECT: {
			// do nothing
		} break;
		case CHAN_MSG_DISCONNECT: {
			SocketContext *ctx = msg->ctx;
			if (ctx->RefCntRelease() == 0) {
				TcpServerSession *session = (TcpServerSession *)ctx->GetUserData();
				if (session) {
					LOG_INFO("session release: addr=%s", session->GetAddr());
					delete session;
				}

				ctx->Close();
				delete ctx;
			}
		} break;
		}

		if (msg->data) {
			free(msg->data);
		}
		free(msg);
	}
}
