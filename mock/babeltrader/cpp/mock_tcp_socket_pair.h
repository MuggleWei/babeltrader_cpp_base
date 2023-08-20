#ifndef BABELTRADER_CPP_MOCK_TCP_SOCKET_PAIR_H_
#define BABELTRADER_CPP_MOCK_TCP_SOCKET_PAIR_H_

#include "babeltrader/cpp/mock_macro.h"

NS_BABELTRADER_BEGIN

class MockTCPSocketPair
{
public:
	BABELTRADER_CPP_MOCK_EXPORT 
	MockTCPSocketPair();
	BABELTRADER_CPP_MOCK_EXPORT 
	virtual ~MockTCPSocketPair();

	BABELTRADER_CPP_MOCK_EXPORT 
	bool Init();

private:
	void ReleaseSocketContext(muggle::SocketContext *ctx);

public:
	muggle::SocketContext *socket_pair[2];
};

NS_BABELTRADER_END

#endif // !BABELTRADER_TEST_MOCK_TCP_SOCKET_PAIR_H_
