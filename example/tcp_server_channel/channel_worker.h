#ifndef CHANNEL_WORKER_H_
#define CHANNEL_WORKER_H_

#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/babeltrader_cpp.h"

USING_NS_MUGGLE;
USING_NS_BABELTRADER;

class ChannelWorker
{
public:
	ChannelWorker();
	virtual ~ChannelWorker();

	void SetChannerl(Channel *chan);
	void SetDispatcher(Dispatcher *dispatcher);

	void Run();

private:
	Channel *chan_;
	Dispatcher *dispatcher_;
};

#endif // !CHANNEL_WORKER_H_
