#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/babeltrader_cpp.h"
#include "demo_msg.h"
#include "tcp_client_handle.h"
#include "tcp_client_peer.h"

typedef struct sys_args {
	char host[64];
	char port[16];
} sys_args_t;

bool parse_sys_args(int argc, char **argv, sys_args_t *args)
{
	char str_usage[1024];
	snprintf(str_usage, sizeof(str_usage),
			 "Usage: %s -H <host> -P <port>\n"
			 "\t-h, --help  print help information\n"
			 "\t-H, --host  listen/connect host\n"
			 "\t-P, --port  listen/connect port",
			 argv[0]);
	memset(args, 0, sizeof(*args));

	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "host", required_argument, NULL, 'H' },
		{ "port", required_argument, NULL, 'P' },
		{ NULL, 0, NULL, 0 }
	};

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "hH:P:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h': {
			LOG_INFO("%s", str_usage);
			exit(EXIT_SUCCESS);
		} break;
		case 'H': {
			strncpy(args->host, optarg, sizeof(args->host) - 1);
		} break;
		case 'P': {
			strncpy(args->port, optarg, sizeof(args->port) - 1);
		} break;
		}
	}

	if (args->host[0] == '\0') {
		LOG_WARNING("run without 'host', set 127.0.0.1 by default");
		strncpy(args->host, "127.0.0.1", sizeof(args->host) - 1);
	}
	if (args->port[0] == '\0') {
		LOG_WARNING("run without 'port', set 8080 by default");
		strncpy(args->port, "8080", sizeof(args->port) - 1);
	}

	LOG_INFO("\n"
			 "----- input args -----\n"
			 "host=%s\n"
			 "port=%s\n"
			 "----------------------",
			 args->host, args->port);

	return true;
}

#define REGISTER_CALLBACK(msg_id, funcname) \
	dispatcher.RegisterCallback(msg_id, TcpClientPeer::s_##funcname);
void register_callbacks(Dispatcher &dispatcher)
{
	REGISTER_CALLBACK(DEMO_MSG_ID_PONG, OnPong);
	REGISTER_CALLBACK(DEMO_MSG_ID_RSP_LOGIN, OnRspLogin);
	REGISTER_CALLBACK(DEMO_MSG_ID_RSP_SUM, OnRspSum);
}

void run_tcp_client(const char *host, const char *port)
{
	// register callbacks
	Dispatcher dispatcher;
	dispatcher.SetTotalMessage(MAX_DEMO_MSG_ID);
	register_callbacks(dispatcher);

	// codec
	MsgToBytesEncoder encoder;
	BytesToMsgDecoder decoder;
	CodecChain codec_chain;
	codec_chain.Append(&encoder);
	codec_chain.Append(&decoder);

	// evloop
	NetEventLoop evloop(8, 0);

	// client handle
	TcpClientHandle handle;
	handle.SetServerAddr(host, port);
	handle.SetEventLoop(&evloop);
	handle.SetCodecChain(&codec_chain);
	handle.SetDispatcher(&dispatcher);

	evloop.SetHandle(&handle);
	evloop.SetTimerInterval(3000);

	handle.Connect();

	evloop.Run();
}

int main(int argc, char *argv[])
{
	if (!muggle::Log::ComplicatedInit(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL)) {
		fprintf(stderr, "failed init log");
		exit(EXIT_FAILURE);
	}

	if (muggle_socket_lib_init() != 0) {
		LOG_ERROR("failed initalize socket library");
		exit(EXIT_FAILURE);
	}

	sys_args_t args;
	parse_sys_args(argc, argv, &args);

	run_tcp_client(args.host, args.port);

	return 0;
}
