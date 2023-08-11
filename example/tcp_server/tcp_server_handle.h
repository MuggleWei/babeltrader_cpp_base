#ifndef TCP_SERVER_HANDLE_H_
#define TCP_SERVER_HANDLE_H_

#include <set>
#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/babeltrader_cpp.h"

USING_NS_MUGGLE;
USING_NS_BABELTRADER;

class TcpServerHandle : public NetEventHandle
{
public:
	TcpServerHandle();
	virtual ~TcpServerHandle();

	virtual void OnAddCtx(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnConnect(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnMessage(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnClose(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnRelease(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnTimer(NetEventLoop *evloop) override;

	void SetCodecChain(CodecChain *codec_chain);
	void SetDispatcher(Dispatcher *dispatcher);

private:
	std::set<SocketContext*> ctx_set_;
	time_t last_timer_trigger_;

	CodecChain *codec_chain_;
	Dispatcher *dispatcher_;
};

#endif // !TCP_SERVER_HANDLE_H_
