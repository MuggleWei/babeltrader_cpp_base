#include "codec_chain.h"
#include "muggle/cpp/muggle_cpp.h"
#include "session.h"
#include "encoder.h"
#include "decoder.h"

NS_BABELTRADER_BEGIN

CodecChain::CodecChain()
	: encode_head_(nullptr)
	, encode_tail_(nullptr)
	, decode_head_(nullptr)
	, decode_tail_(nullptr)
{
}

CodecChain::~CodecChain()
{
}

bool CodecChain::Append(Encoder *encoder)
{
	if (encoder->chain_) {
		LOG_ERROR("encoder be append to multiple chain is not allowed");
		return false;
	}

	if (encode_tail_) {
		encode_tail_->next_ = encoder;
	} else {
		encode_head_ = encoder;
	}
	encode_tail_ = encoder;
	encoder->chain_ = this;

	return true;
}

bool CodecChain::Append(Decoder *decoder)
{
	if (decoder->chain_) {
		LOG_ERROR("decoder be append to multiple chain is not allowed");
		return false;
	}

	if (decode_tail_) {
		decode_tail_->next_ = decoder;
	} else {
		decode_head_ = decoder;
	}
	decode_tail_ = decoder;
	decoder->chain_ = this;

	return true;
}

bool CodecChain::Encode(Session *session, void *data, uint32_t datalen)
{
	if (encode_head_) {
		return encode_head_->Encode(session, data, datalen);
	} else {
		return session->OnWrite(data, datalen);
	}
}

bool CodecChain::Decode(Session *session, void *data, uint32_t datalen)
{
	if (decode_head_) {
		return decode_head_->Decode(session, data, datalen);
	} else {
		return session->OnRead(data, datalen);
	}
}

NS_BABELTRADER_END
