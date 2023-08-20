#ifndef BABELTRADER_CPP_MOCK_SESSION_H_
#define BABELTRADER_CPP_MOCK_SESSION_H_

#include "babeltrader/cpp/mock_macro.h"
#include <functional>

NS_BABELTRADER_BEGIN

class MockSession : public babeltrader::Session
{
public:
	BABELTRADER_CPP_MOCK_EXPORT 
	virtual bool OnRead(void *data, uint32_t datalen) override;

	BABELTRADER_CPP_MOCK_EXPORT 
	virtual bool OnWrite(void *data, uint32_t datalen) override;

public:
	std::function<void(void*, uint32_t)> cb_read;
	std::function<void(void*, uint32_t)> cb_write;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_MOCK_SESSION_H_
