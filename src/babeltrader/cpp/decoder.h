/******************************************************************************
 *  @file         decoder.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-11
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp decoder
 *****************************************************************************/

#ifndef BABELTRADER_CPP_DECODER_H_
#define BABELTRADER_CPP_DECODER_H_

#include <stdint.h>
#include "babeltrader/cpp/macro.h"

NS_BABELTRADER_BEGIN

class Session;
class CodecChain;

class Decoder
{
	friend class CodecChain;

public:
	BABELTRADER_CPP_EXPORT
	Decoder();

	BABELTRADER_CPP_EXPORT
	virtual ~Decoder();

	BABELTRADER_CPP_EXPORT
	virtual bool Decode(Session *session, void *data, uint32_t datalen) = 0;

	BABELTRADER_CPP_EXPORT
	bool Next(Session *session, void *data, uint32_t datalen);

protected:
	Decoder *next_;
	CodecChain *chain_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_DECODER_H_
