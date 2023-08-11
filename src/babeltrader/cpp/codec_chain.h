/******************************************************************************
 *  @file         codec_chain.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-11
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp codec chain
 *****************************************************************************/

#ifndef BABELTRADER_CPP_CODEC_CHAIN_H_
#define BABELTRADER_CPP_CODEC_CHAIN_H_

#include <stdint.h>
#include "babeltrader/cpp/macro.h"

NS_BABELTRADER_BEGIN

class Encoder;
class Decoder;
class Session;

class CodecChain
{
public:
	BABELTRADER_CPP_EXPORT
	CodecChain();

	BABELTRADER_CPP_EXPORT
	virtual ~CodecChain();

	BABELTRADER_CPP_EXPORT
	bool Append(Encoder *encoder);

	BABELTRADER_CPP_EXPORT
	bool Append(Decoder *encoder);

	BABELTRADER_CPP_EXPORT
	bool Encode(Session *session, void *data, uint32_t datalen);

	BABELTRADER_CPP_EXPORT
	bool Decode(Session *session, void *data, uint32_t datalen);

protected:
	Encoder *encode_head_;
	Encoder *encode_tail_;
	Decoder *decode_head_;
	Decoder *decode_tail_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_CODEC_CHAIN_H_
