#ifndef CHANNEL_MSG_H_
#define CHANNEL_MSG_H_

#include "muggle/cpp/net/socket_context.h"

enum {
	CHAN_MSG_DATA = 0,
	CHAN_MSG_CONNECT,
	CHAN_MSG_DISCONNECT,
};

typedef struct chan_msg {
	muggle::SocketContext *ctx;
	uint32_t msg_type;
	void *data;
	uint32_t datalen;
} chan_msg_t;

#endif // !CHANNEL_MSG_H_
