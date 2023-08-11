/******************************************************************************
 *  @file         dispatcher.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-11
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp dispatcher
 *****************************************************************************/

#ifndef BABELTRADER_CPP_DISPATCHER_H_
#define BABELTRADER_CPP_DISPATCHER_H_

#include <stdint.h>
#include "babeltrader/cpp/macro.h"
#include "babeltrader/cpp/msg.h"

NS_BABELTRADER_BEGIN

class Session;

typedef void (*FnMsgCallback)(Session *session, msg_hdr_t *hdr, void *msg);

class Dispatcher {
public:
	BABELTRADER_CPP_EXPORT
	Dispatcher();

	BABELTRADER_CPP_EXPORT
	virtual ~Dispatcher();

	BABELTRADER_CPP_EXPORT
	bool SetTotalMessage(uint32_t total_msg);

	BABELTRADER_CPP_EXPORT
	bool RegisterCallback(uint32_t msg_id, FnMsgCallback cb);

	BABELTRADER_CPP_EXPORT
	void Dispatch(Session *session, void *data, uint32_t datalen);

private:
	uint32_t total_msg_;
	FnMsgCallback *callbacks_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_DISPATCHER_H_
