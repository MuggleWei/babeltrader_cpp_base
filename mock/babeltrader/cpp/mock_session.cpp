#include "mock_session.h"

NS_BABELTRADER_BEGIN

bool MockSession::OnRead(void *data, uint32_t datalen)
{
	if (cb_read) {
		cb_read(data, datalen);
		return true;
	} else {
		return false;
	}
}

bool MockSession::OnWrite(void *data, uint32_t datalen)
{
	if (cb_write) {
		cb_write(data, datalen);
		return true;
	} else {
		return false;
	}
}

NS_BABELTRADER_END
