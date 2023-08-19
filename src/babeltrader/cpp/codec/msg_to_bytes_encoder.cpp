#include "msg_to_bytes_encoder.h"
#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/msg.h"

NS_BABELTRADER_BEGIN

static const char *s_magic = BABELTRADER_CPP_MSG_HDR_MAGIC_WORD;
static const uint8_t s_protocol_ver = BABELTRADER_CPP_PROTOCOL_VERSION;

MsgToBytesEncoder::MsgToBytesEncoder()
{
}

MsgToBytesEncoder::~MsgToBytesEncoder()
{
}

bool MsgToBytesEncoder::Encode(Session *session, void *data, uint32_t datalen)
{
	msg_hdr_t *hdr = (msg_hdr_t *)data;
	memcpy(hdr->magic, s_magic, 4);
	hdr->flags[BABELTRADER_CPP_MSG_HDR_FLAG_ENDIAN] = MUGGLE_ENDIANNESS;
	hdr->flags[BABELTRADER_CPP_MSG_HDR_FLAG_VER] = s_protocol_ver;
	hdr->payload_len = datalen - sizeof(msg_hdr_t);

	return Next(session, data, datalen);
}

NS_BABELTRADER_END
