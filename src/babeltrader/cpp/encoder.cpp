#include "encoder.h"
#include "codec_chain.h"
#include "session.h"

NS_BABELTRADER_BEGIN

Encoder::Encoder()
	: next_(nullptr)
	, chain_(nullptr)
{
}

Encoder::~Encoder()
{
}

bool Encoder::Next(Session *session, void *data, uint32_t datalen)
{
	if (next_) {
		return next_->Encode(session, data, datalen);
	} else {
		return session->OnWrite(data, datalen);
	}
}

NS_BABELTRADER_END
