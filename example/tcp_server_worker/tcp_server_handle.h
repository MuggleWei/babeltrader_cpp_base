#ifndef TCP_SERVER_HANDLE_H_
#define TCP_SERVER_HANDLE_H_

#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/babeltrader_cpp.h"
#include <set>

USING_NS_MUGGLE;
USING_NS_BABELTRADER;

class TcpServerHandle : public NetEventHandle {
public:
	TcpServerHandle();
	virtual ~TcpServerHandle();

	virtual void OnAddCtx(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnMessage(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnClose(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnRelease(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnTimer(NetEventLoop *evloop) override;

	void SetCodecChain(CodecChain *codec_chain);
	void SetDispatcher(Dispatcher *dispatcher);
	void SetWorkerIdx(int idx);

private:
	CodecChain *codec_chain_;
	Dispatcher *dispatcher_;
	int worker_idx_;

	std::set<SocketContext*> ctx_set_;
	time_t last_timer_trigger_;
};

#endif // !TCP_SERVER_HANDLE_H_
