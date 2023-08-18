#include "gtest/gtest.h"
#include "babeltrader/cpp/mock.h"
#include "demo_msg.h"

USING_NS_BABELTRADER;

class TestCodec : public ::testing::Test {
public:
	virtual void SetUp() override
	{
		codec_chain_.Append(&encoder_);
		codec_chain_.Append(&decoder_);

		bool ret = socket_pair_.Init();
		ASSERT_TRUE(ret);

		socket_pair_.socket_pair[0]->SetUserData(&write_session_);
		write_session_.SetSocketContext(socket_pair_.socket_pair[0]);
		write_session_.InitBytesBuffer(1024 * 16);
		write_session_.SetCodecChain(&codec_chain_);

		socket_pair_.socket_pair[1]->SetUserData(&read_session_);
		read_session_.SetSocketContext(socket_pair_.socket_pair[1]);
		read_session_.InitBytesBuffer(1024 * 16);
		read_session_.SetCodecChain(&codec_chain_);
	}

	void CheckHead(msg_hdr_t *hdr, uint32_t msg_id, uint32_t payload_len)
	{
		ASSERT_EQ(*(uint32_t *)hdr->magic,
				  *(uint32_t *)BABELTRADER_CPP_MSG_HDR_MAGIC_WORD);
		ASSERT_EQ(hdr->flags[BABELTRADER_CPP_MSG_HDR_FLAG_VER], 1);
		ASSERT_EQ(hdr->msg_id, msg_id);
		ASSERT_EQ(hdr->payload_len, payload_len);
	}

protected:
	BytesToMsgDecoder decoder_;
	MsgToBytesEncoder encoder_;
	CodecChain codec_chain_;

	MockSession write_session_;
	MockSession read_session_;
	MockTCPSocketPair socket_pair_;
};

#define NEW_STACK_MSG BABELTRADER_CPP_NEW_STACK_MSG
#define SEND_MSG(var) \
	write_session_.SendMessage(hdr_##var, sizeof(msg_hdr_t) + sizeof(*var))

TEST_F(TestCodec, CodecPing)
{
	NEW_STACK_MSG(DEMO_MSG_ID_PING, demo_msg_ping_t, req);
	req->sec = 5;
	req->nsec = 6;

	write_session_.cb_write = [&](void *data, uint32_t datalen) {
		CheckHead((msg_hdr_t *)data, DEMO_MSG_ID_PING,
				  datalen - sizeof(msg_hdr_t));

		int n = write_session_.GetSocketContex()->Write(data, datalen);
		ASSERT_EQ(n, datalen);
	};

	bool ret = SEND_MSG(req);
	ASSERT_TRUE(ret);

	ret = decoder_.ReadBytes(&read_session_);
	ASSERT_TRUE(ret);

	read_session_.cb_read = [&](void *data, uint32_t datalen) {
		CheckHead((msg_hdr_t *)data, DEMO_MSG_ID_PING,
				  datalen - sizeof(msg_hdr_t));
		demo_msg_ping_t *msg = (demo_msg_ping_t*)((msg_hdr_t *)data + 1);
		ASSERT_EQ(msg->sec, req->sec);
		ASSERT_EQ(msg->nsec, req->nsec);
	};
	ret = codec_chain_.Decode(&read_session_, NULL, 0);
	ASSERT_TRUE(ret);
}
