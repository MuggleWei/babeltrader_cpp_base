/******************************************************************************
 *  @file         decoder.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-11
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp encoder
 *****************************************************************************/

#ifndef BABELTRADER_CPP_ENCODER_H_
#define BABELTRADER_CPP_ENCODER_H_

#include <stdint.h>
#include "babeltrader/cpp/macro.h"

NS_BABELTRADER_BEGIN

class Session;
class CodecChain;

class Encoder
{
	friend class CodecChain;

public:
	BABELTRADER_CPP_EXPORT
	Encoder();

	BABELTRADER_CPP_EXPORT
	virtual ~Encoder();

	BABELTRADER_CPP_EXPORT
	virtual bool Encode(Session *session, void *data, uint32_t datalen) = 0;

	BABELTRADER_CPP_EXPORT
	bool Next(Session *session, void *data, uint32_t datalen);

protected:
	Encoder *next_;
	CodecChain *chain_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_ENCODER_H_
