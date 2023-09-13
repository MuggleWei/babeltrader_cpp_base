/******************************************************************************
 *  @file         session.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-10
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp session
 *****************************************************************************/

#ifndef BABELTRADER_CPP_SESSION_H_
#define BABELTRADER_CPP_SESSION_H_

#include <string>
#include <time.h>
#include "muggle/cpp/memory/bytes_buffer.h"
#include "muggle/cpp/net/socket_context.h"
#include "babeltrader/cpp/macro.h"

NS_BABELTRADER_BEGIN

class CodecChain;

class Session {
public:
	BABELTRADER_CPP_EXPORT
	Session();
	BABELTRADER_CPP_EXPORT
	virtual ~Session();

	BABELTRADER_CPP_EXPORT
	void SetSocketContext(muggle::SocketContext *ctx);
	BABELTRADER_CPP_EXPORT
	muggle::SocketContext* GetSocketContex();
	
	BABELTRADER_CPP_EXPORT
	bool InitBytesBuffer(int cap);
	BABELTRADER_CPP_EXPORT
	muggle::BytesBuffer* GetBytesBuffer();

	BABELTRADER_CPP_EXPORT
	void SetCodecChain(CodecChain *chain);

	BABELTRADER_CPP_EXPORT
	void SetUserData(void *user_data);
	BABELTRADER_CPP_EXPORT
	void* GetUserData();

	BABELTRADER_CPP_EXPORT
	void SetDetach(bool flag);
	BABELTRADER_CPP_EXPORT
	bool IsDetached();

	BABELTRADER_CPP_EXPORT
	void UpdateActiveTime(time_t ts);
	BABELTRADER_CPP_EXPORT
	time_t LastActiveTime();

	BABELTRADER_CPP_EXPORT
	void SetAddr(const char *addr);
	BABELTRADER_CPP_EXPORT
	const char* GetAddr();

	BABELTRADER_CPP_EXPORT
	bool HandleMessage();
	BABELTRADER_CPP_EXPORT
	bool SendMessage(void *data, uint32_t datalen);
	
	BABELTRADER_CPP_EXPORT
	void Close();

	BABELTRADER_CPP_EXPORT
	virtual bool OnRead(void *data, uint32_t datalen);

	BABELTRADER_CPP_EXPORT
	virtual bool OnWrite(void *data, uint32_t datalen);

	/**
	 * @brief write into socket context
	 *
	 * @param data     data
	 * @param datalen  length of data
	 *
	 * @return number of bytes write into socket
	 */
	BABELTRADER_CPP_EXPORT
	virtual int Write(void *data, uint32_t datalen);

	/**
	 * @brief read from socket context
	 *
	 * @param buf  buffer store data
	 * @param len  length of want read
	 *
	 * @return number of bytes read from socket
	 */
	BABELTRADER_CPP_EXPORT
	virtual int Read(void *buf, size_t len);

protected:
	muggle::SocketContext *ctx_;
	muggle::BytesBuffer *bytes_buf_;
	CodecChain *codec_chain_;
	void *user_data_;

	bool detached_;
	time_t last_actived_ts_;
	std::string addr_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_SESSION_H_
