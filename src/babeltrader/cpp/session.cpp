#include "session.h"
#include "muggle/cpp/muggle_cpp.h"
#include "codec_chain.h"

NS_BABELTRADER_BEGIN

Session::Session()
	: ctx_(nullptr)
	, bytes_buf_(nullptr)
	, codec_chain_(nullptr)
	, user_data_(nullptr)
	, detached_(false)
	, last_actived_ts_(0)
{
}
Session::~Session()
{
	if (bytes_buf_) {
		delete bytes_buf_;
		bytes_buf_ = nullptr;
	}
}

void Session::SetUserData(void *user_data)
{
	user_data_ = user_data;
}
void *Session::GetUserData()
{
	return user_data_;
}

void Session::SetSocketContext(muggle::SocketContext *ctx)
{
	ctx_ = ctx;
}
muggle::SocketContext *Session::GetSocketContex()
{
	return ctx_;
}

bool Session::InitBytesBuffer(int cap)
{
	bytes_buf_ = new muggle::BytesBuffer(cap);
	if (bytes_buf_ == nullptr) {
		LOG_ERROR("failed init bytes buffer");
		return false;
	}

	return true;
}
muggle::BytesBuffer *Session::GetBytesBuffer()
{
	return bytes_buf_;
}

void Session::SetCodecChain(CodecChain *chain)
{
	codec_chain_ = chain;
}

void Session::SetDetach(bool flag)
{
	detached_ = flag;
}
bool Session::IsDetached()
{
	return detached_;
}

void Session::UpdateActiveTime(time_t ts)
{
	last_actived_ts_ = ts;
}
time_t Session::LastActiveTime()
{
	return last_actived_ts_;
}

void Session::SetAddr(const char *addr)
{
	addr_ = addr;
}
const char *Session::GetAddr()
{
	return addr_.c_str();
}

bool Session::HandleMessage()
{
	if (codec_chain_) {
		return codec_chain_->Decode(this, nullptr, 0);
	}
	return true;
}

bool Session::SendMessage(void *data, uint32_t datalen)
{
	if (codec_chain_) {
		return codec_chain_->Encode(this, data, datalen);
	}
	return OnWrite(data, datalen);
}

void Session::Close()
{
	ctx_->Shutdown();
}

bool Session::OnRead(void *data, uint32_t datalen)
{
	MUGGLE_UNUSED(data);
	MUGGLE_UNUSED(datalen);
	return true;
}

bool Session::OnWrite(void *data, uint32_t datalen)
{
	int n = ctx_->Write(data, datalen);
	if (n != (int)datalen) {
		if (n == MUGGLE_EVENT_ERROR) {
			char errmsg[512];
			snprintf(errmsg, sizeof(errmsg), "failed write message: addr=%s",
					 GetAddr());
			MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, errmsg);
		} else {
			LOG_ERROR("failed write message: addr=%s, "
					  "expect write=%u, actual write=%d",
					  GetAddr(), datalen, n);
		}
		Close();
		return false;
	}
	return true;
}

NS_BABELTRADER_END
