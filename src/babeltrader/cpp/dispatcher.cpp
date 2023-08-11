#include "dispatcher.h"
#include "muggle/cpp/muggle_cpp.h"
#include "session.h"
#include "msg.h"

NS_BABELTRADER_BEGIN

Dispatcher::Dispatcher()
	: total_msg_(0)
	, callbacks_(nullptr)
{
}

Dispatcher::~Dispatcher()
{
	if (callbacks_) {
		free(callbacks_);
		callbacks_ = nullptr;
	}
}

bool Dispatcher::SetTotalMessage(uint32_t total_msg)
{
	if (total_msg_ != 0) {
		LOG_ERROR("failed to repeated set dispatcher total message");
		return false;
	}

	callbacks_ = (FnMsgCallback *)malloc(sizeof(FnMsgCallback) * total_msg);
	if (callbacks_ == nullptr) {
		LOG_ERROR("failed set dispatcher total message");
		return false;
	}
	total_msg_ = total_msg;

	for (uint32_t i = 0; i < total_msg_; i++) {
		callbacks_[i] = nullptr;
	}

	return true;
}

bool Dispatcher::RegisterCallback(uint32_t msg_id, FnMsgCallback cb)
{
	if (msg_id >= total_msg_) {
		LOG_ERROR("failed register callback function, message id beyond max"
				  "msg_id=%lu, total_msg=%lu",
				  (unsigned long)msg_id, (unsigned long)total_msg_);
		return false;
	}

	FnMsgCallback callback = callbacks_[msg_id];
	if (callback != nullptr) {
		LOG_ERROR("failed register callback function, "
				  "message already register: msg_id=%lu",
				  (unsigned long)msg_id);
		return false;
	}

	callbacks_[msg_id] = cb;

	return true;
}

void Dispatcher::Dispatch(Session *session, void *data, uint32_t datalen)
{
	MUGGLE_UNUSED(datalen);

	msg_hdr_t *hdr = (msg_hdr_t *)data;
	if (hdr->msg_id < total_msg_) {
		FnMsgCallback cb = callbacks_[hdr->msg_id];
		if (cb) {
			cb(session, hdr, (void *)(hdr + 1));
		} else {
			LOG_WARNING("recv message without callback: addr=%s, msg_id=%lu",
						session->GetAddr(), (unsigned long)hdr->msg_id);
		}
	} else {
		LOG_ERROR("recv invalid message id: addr=%s, msg_id=%lu",
				  session->GetAddr(), (unsigned long)hdr->msg_id);
		session->Close();
	}
}

NS_BABELTRADER_END
