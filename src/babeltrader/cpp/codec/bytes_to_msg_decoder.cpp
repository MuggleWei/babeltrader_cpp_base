#include "bytes_to_msg_decoder.h"
#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/session.h"
#include "babeltrader/cpp/msg.h"

NS_BABELTRADER_BEGIN

static const char *s_magic = BABELTRADER_CPP_MSG_HDR_MAGIC_WORD;
static const uint8_t s_protocol_ver = BABELTRADER_CPP_PROTOCOL_VERSION;

BytesToMsgDecoder::BytesToMsgDecoder()
	: Decoder()
	, recv_unit_size_(1024 * 16)
	, msg_len_limit_(1024 * 64)
{
}

BytesToMsgDecoder::~BytesToMsgDecoder()
{
}

bool BytesToMsgDecoder::Decode(Session *session, void *data, uint32_t datalen)
{
	MUGGLE_UNUSED(data);
	MUGGLE_UNUSED(datalen);

	// read bytes into bytes buffer
	if (!ReadBytes(session)) {
		return false;
	}

	// parse message
	muggle::BytesBuffer *bytes_buf = session->GetBytesBuffer();
	msg_hdr_t msg_header;
	while (true) {
		if (!bytes_buf->Fetch((int)sizeof(msg_header), &msg_header)) {
			// readable bytes less than sizeof message header
			break;
		}

		// check magic word
		if (*(uint32_t *)msg_header.magic != *(uint32_t *)s_magic) {
			LOG_ERROR("invalid magic word: addr=%s", session->GetAddr());
			return false;
		}

		// check endianness
		if (msg_header.flags[BABELTRADER_CPP_MSG_HDR_FLAG_ENDIAN] !=
			MUGGLE_ENDIANNESS) {
			LOG_ERROR("invalid endianness: addr=%s", session->GetAddr());
			return false;
		}

		// check version
		if (msg_header.flags[BABELTRADER_CPP_MSG_HDR_FLAG_VER] !=
			s_protocol_ver) {
			LOG_ERROR("invalid version: addr=%s", session->GetAddr());
			return false;
		}

		// check reversed bytes
		if (msg_header.flags[BABELTRADER_CPP_MSG_HDR_FLAG_REVERSED1] != '\0') {
			LOG_ERROR("reversed1 bytes be set: addr=%s", session->GetAddr());
			return false;
		}
		if (msg_header.flags[BABELTRADER_CPP_MSG_HDR_FLAG_REVERSED2] != '\0') {
			LOG_ERROR("reversed2 bytes be set: addr=%s", session->GetAddr());
			return false;
		}

		// check message length
		uint32_t total_bytes =
			(uint32_t)sizeof(msg_hdr_t) + msg_header.payload_len;
		if (msg_len_limit_ > 0) {
			if (total_bytes > msg_len_limit_) {
				LOG_ERROR(
					"invalid message length: "
					"addr=%s, msg_id=%lu, msg_total_len=%lu, msg_len_limit=%lu",
					session->GetAddr(), (unsigned long)msg_header.msg_id,
					(unsigned long)total_bytes, (unsigned long)msg_len_limit_);
				return false;
			}
		}

		// check readable
		int readable = bytes_buf->Readable();
		if ((uint32_t)readable < total_bytes) {
			break;
		}

		// handle message
		msg_hdr_t *p_msg = (msg_hdr_t *)bytes_buf->ReaderFc((int)total_bytes);
		bool ret = true;
		if (p_msg == nullptr) {
			// discontinuous memory
			void *buf = malloc(total_bytes);
			bytes_buf->Read((int)total_bytes, buf);

			ret = Next(session, buf, total_bytes);

			free(buf);
		} else {
			// continuous memory
			ret = Next(session, p_msg, total_bytes);

			bytes_buf->ReaderMove((int)total_bytes);
		}
		if (!ret) {
			return false;
		}
	}

	return true;
}

bool BytesToMsgDecoder::ReadBytes(Session *session)
{
	muggle::BytesBuffer *bytes_buf = session->GetBytesBuffer();
	if (bytes_buf == nullptr) {
		LOG_ERROR("failed get session bytes buffer, addr=%s",
				  session->GetAddr());
		return false;
	}

	muggle::SocketContext *ctx = session->GetSocketContex();
	if (ctx == nullptr) {
		LOG_ERROR("failed get socket context, addr=%s", session->GetAddr());
		return false;
	}

	while (true) {
		void *p = bytes_buf->WriterFc((int)recv_unit_size_);
		if (p == nullptr) {
			LOG_WARNING("bytes buffer full: %s", session->GetAddr());
			break;
		}

		int n = ctx->Read(p, (size_t)recv_unit_size_);
		if (n > 0) {
			bytes_buf->WriterMove(n);
		}

		if (n < (int)recv_unit_size_) {
			break;
		}
	}

	return true;
}

void BytesToMsgDecoder::SetRecvUnitSize(uint32_t recv_unit_size)
{
	if (recv_unit_size == 0) {
		return;
	}
	recv_unit_size_ = recv_unit_size;
}

void BytesToMsgDecoder::SetMsgLenLimit(uint32_t msg_len_limit)
{
	msg_len_limit_ = msg_len_limit;
}

NS_BABELTRADER_END
