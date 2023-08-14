#ifndef TCP_LISTEN_HANDLE_H_
#define TCP_LISTEN_HANDLE_H_

#include <vector>
#include <map>
#include <set>
#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/babeltrader_cpp.h"

USING_NS_MUGGLE;
USING_NS_BABELTRADER;

class TcpListenHandle : public NetEventHandle
{
public:
	TcpListenHandle();
	virtual ~TcpListenHandle();

	virtual void OnAddCtx(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnConnect(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnMessage(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnClose(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnRelease(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnTimer(NetEventLoop *evloop) override;

	void SetCodecChain(CodecChain *codec_chain);
	void SetDispatcher(Dispatcher *dispatcher);
	void SetWorkers(std::vector<NetEventLoop*> *workers);

private:
	int worker_idx_;
	CodecChain *codec_chain_;
	Dispatcher *dispatcher_;
	std::vector<NetEventLoop*> *worker_evloops_;
	std::map<std::string, int> user_workder_dict_;
	std::set<SocketContext*> ctx_set_;
	time_t last_timer_trigger_;
};

#endif // !TCP_LISTEN_HANDLE_H_
