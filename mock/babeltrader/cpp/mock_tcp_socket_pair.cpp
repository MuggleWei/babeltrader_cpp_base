#include "mock_tcp_socket_pair.h"
#include "muggle/cpp/muggle_cpp.h"
#include <thread>

NS_BABELTRADER_BEGIN

MockTCPSocketPair::MockTCPSocketPair()
	: socket_pair{}
{
}
MockTCPSocketPair::~MockTCPSocketPair()
{
	for (int i = 0; i < 2; i++) {
		if (socket_pair[i]) {
			muggle_socket_ctx_close(socket_pair[i]->GetContext());
			free(socket_pair[i]->GetContext());
			delete socket_pair[i];
		}
	}
}

static muggle::SocketContext *PackSocketContext(muggle_socket_t fd,
												int socktype)
{
	muggle_socket_context_t *ctx =
		(muggle_socket_context_t *)malloc(sizeof(muggle_socket_context_t));
	if (muggle_socket_ctx_init(ctx, fd, NULL, socktype) != 0) {
		free(ctx);
		muggle_socket_close(fd);
		return nullptr;
	}

	muggle::SocketContext *socket_ctx = new muggle::SocketContext(ctx);
	if (socket_ctx == nullptr) {
		free(ctx);
		muggle_socket_close(fd);
		return nullptr;
	}

	return socket_ctx;
}

bool MockTCPSocketPair::Init()
{
	// listen
	muggle::SocketContext *listen_ctx =
		muggle::SocketUtils::TCPListen("127.0.0.1", "0", 2);
	if (listen_ctx == nullptr) {
		LOG_ERROR("failed listen");
		return false;
	}

	char listen_addr[64];
	if (muggle_socket_local_addr(listen_ctx->GetSocket(), listen_addr,
								 sizeof(listen_addr), 0) == nullptr) {
		LOG_ERROR("failed get listen addr");
		return false;
	}

	// connect to listen
	char *p = listen_addr;
	while (*p != '\0') {
		if (*p == ':') {
			break;
		}
		p++;
	}
	if (*p == '\0') {
		return false;
	}
	*p++ = '\0';

	muggle::SocketContext *ctx1 = nullptr;
	std::thread th([&ctx1, &listen_addr, &p] {
		ctx1 = muggle::SocketUtils::TCPConnect(listen_addr, p, 3);
	});

	muggle_socket_t fd = accept(listen_ctx->GetSocket(), NULL, NULL);
	if (fd == MUGGLE_INVALID_SOCKET) {
		LOG_ERROR("failed accept socket");

		th.join();
		if (ctx1) {
			ReleaseSocketContext(ctx1);
		}

		ReleaseSocketContext(listen_ctx);
		return false;
	}

	th.join();

	// pack ctx2
	muggle::SocketContext *ctx2 = PackSocketContext(fd, 0);
	if (ctx2 == nullptr) {
		LOG_ERROR("failed pack socket context");
		if (ctx1) {
			ReleaseSocketContext(ctx1);
		}

		ReleaseSocketContext(listen_ctx);
		return false;
	}

	// delete listen
	ReleaseSocketContext(listen_ctx);

	// set pair
	this->socket_pair[0] = ctx1;
	this->socket_pair[1] = ctx2;

	return true;
}

void MockTCPSocketPair::ReleaseSocketContext(muggle::SocketContext *ctx)
{
	if (ctx) {
		muggle_socket_ctx_close(ctx->GetContext());
		free(ctx->GetContext());
		delete ctx;
	}
}

NS_BABELTRADER_END
