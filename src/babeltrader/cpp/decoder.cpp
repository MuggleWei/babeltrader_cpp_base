#include "decoder.h"
#include "codec_chain.h"
#include "session.h"

NS_BABELTRADER_BEGIN

Decoder::Decoder()
	: next_(nullptr)
	, chain_(nullptr)
{
}

Decoder::~Decoder()
{
}

bool Decoder::Next(Session *session, void *data, uint32_t datalen)
{
	if (next_) {
		return next_->Decode(session, data, datalen);
	} else {
		return session->OnRead(data, datalen);
	}
}

NS_BABELTRADER_END
