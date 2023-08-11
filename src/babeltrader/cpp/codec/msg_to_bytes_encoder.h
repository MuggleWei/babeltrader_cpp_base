/******************************************************************************
 *  @file         msg_to_bytes_encoder.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-11
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp message to bytes encoder
 *****************************************************************************/

#ifndef BABELTRADER_CPP_MSG_TO_BYTES_ENCODER_H_
#define BABELTRADER_CPP_MSG_TO_BYTES_ENCODER_H_

#include "babeltrader/cpp/encoder.h"

NS_BABELTRADER_BEGIN

class MsgToBytesEncoder : public Encoder {
public:
	BABELTRADER_CPP_EXPORT
	MsgToBytesEncoder();

	BABELTRADER_CPP_EXPORT
	virtual ~MsgToBytesEncoder();

	BABELTRADER_CPP_EXPORT
	virtual bool Encode(Session *session, void *data,
						uint32_t datalen) override;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_MSG_TO_BYTES_ENCODER_H_
