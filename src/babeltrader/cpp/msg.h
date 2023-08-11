/******************************************************************************
 *  @file         msg.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-11
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp message
 *****************************************************************************/

#ifndef BABLETRADER_CPP_MSG_H_
#define BABLETRADER_CPP_MSG_H_

#include <stdint.h>
#include "babeltrader/cpp/macro.h"

NS_BABELTRADER_BEGIN

	; // for avoid LSP complain
#pragma pack(push)
#pragma pack(1)

#define BABELTRADER_CPP_MSG_HDR_MAGIC_WORD "BBTR"
#define BABELTRADER_CPP_MSG_HDR_FLAG_ENDIAN 0
#define BABELTRADER_CPP_MSG_HDR_FLAG_VER 1
#define BABELTRADER_CPP_MSG_HDR_FLAG_REVERSED1 2
#define BABELTRADER_CPP_MSG_HDR_FLAG_REVERSED2 3

typedef struct msg_hdr {
	char magic[4]; //!< magic word
	char flags[4]; //!< flags
	uint32_t msg_id; //!< message id
	uint32_t payload_len; //!< payload length (not include head)
} msg_hdr_t;

#define BABELTRADER_CPP_NEW_STACK_MSG(msgid, msgtype, var) \
	char msg_placeholder_##var[sizeof(msg_hdr_t) + sizeof(msgtype)]; \
	memset(msg_placeholder_##var, 0, sizeof(msg_placeholder_##var)); \
	msg_hdr_t *hdr_##var = (msg_hdr_t*)msg_placeholder_##var; \
	hdr_##var->msg_id = msgid; \
	msgtype *var = (msgtype*)(hdr_##var + 1);

#define BABELTRADER_CPP_SEND_MSG(var) \
	SendMessage(hdr_##var, sizeof(msg_hdr_t) + sizeof(*var));

#pragma pack(pop)

NS_BABELTRADER_END

#endif // !BABLETRADER_CPP_MSG_H_
