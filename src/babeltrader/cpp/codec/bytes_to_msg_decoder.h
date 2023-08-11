/******************************************************************************
 *  @file         bytes_to_msg_decoder.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-11
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp bytes to message decoder
 *****************************************************************************/

#ifndef BABELTRADER_CPP_BYTES_TO_MSG_DECODER_H_
#define BABELTRADER_CPP_BYTES_TO_MSG_DECODER_H_

#include "babeltrader/cpp/decoder.h"

NS_BABELTRADER_BEGIN

class BytesToMsgDecoder : public Decoder {
public:
	BABELTRADER_CPP_EXPORT
	BytesToMsgDecoder();

	BABELTRADER_CPP_EXPORT
	virtual ~BytesToMsgDecoder();

	BABELTRADER_CPP_EXPORT
	virtual bool Decode(Session *session, void *data,
						uint32_t datalen) override;

	BABELTRADER_CPP_EXPORT
	bool ReadBytes(Session *session);

	BABELTRADER_CPP_EXPORT
	void SetRecvUnitSize(uint32_t recv_unit_size);

	BABELTRADER_CPP_EXPORT
	void SetMsgLenLimit(uint32_t msg_len_limit);

protected:
	uint32_t recv_unit_size_;
	uint32_t msg_len_limit_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_BYTES_TO_MSG_DECODER_H_
