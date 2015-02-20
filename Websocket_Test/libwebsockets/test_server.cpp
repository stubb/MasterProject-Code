#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <getopt.h>
#include <libwebsockets.h>

#ifdef __linux__
	#include <cstring>
#endif

using namespace std;

unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 1280 * 720 * 4 +  LWS_SEND_BUFFER_POST_PADDING];
unsigned int position = 0;

static int callback_http(struct libwebsocket_context * that,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason, void *user,
	void *in, size_t len)
{
	return 0;
}

static int callback_save_data(struct libwebsocket_context * that,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason,
	void *user, void *in, size_t len)
{
	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED:
		cout << "Connection established" << endl;
		break;
	case LWS_CALLBACK_RECEIVE: {
		memcpy(buf + position, in, len);
		position += len;
		const size_t remaining = libwebsockets_remaining_packet_payload(wsi);
		if (remaining == 0) {
			cout <<  "Received data:" << position << endl;
			cout << buf << endl;
			cout << "Mach hier was damit!" << endl;
			// reset position
			position = 0;
		}
		break;
	}
	case LWS_CALLBACK_CLOSED: {
		cout << "Connection closed" << endl;
		break;
	}
	default:
		break;
	}

	return 0;
}

static struct libwebsocket_protocols protocols[] = {
	{
		"http-only",   // name
		callback_http, // callback
		0              // per_session_data_size
	},
	{
		"callback_save_data",
		callback_save_data,
		0,
		65536				// rx_buffer_size
	},
	{
		NULL, NULL, 0   /* End of list */
	}
};

static struct option options[] = {
	{ "help",	no_argument,		NULL, 'h' },
	{ "port",	required_argument,	NULL, 'p' },
	{ NULL, 0, 0, 0 }
};

int main(int argc, char **argv) {
	int n = 0;
	struct libwebsocket_context *context;
	// server url will be http://localhost:9000
	// we're not using ssl
	// no special options
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);
	info.port = 9000;
	info.gid = -1;
	info.uid = -1;
	info.protocols = protocols;

	while (n >= 0) {
		n = getopt_long(argc, argv, "ci:hsp:d:", options, NULL);
		if (n < 0)
			continue;
		switch (n) {
		case 'p':
			info.port = atoi(optarg);
			break;
		case 'h':
			fprintf(stderr, "Usage: Applicationserver "
					"[--port=<p>]\n");
			exit(1);
		}
	}

	// create libwebsocket context representing this server
	context = libwebsocket_create_context(&info);

	if (context == NULL) {
		fprintf(stderr, "libwebsocket init failed\n");
		return -1;
	}

	printf("starting server...\n");

	// infinite loop, to end this server send SIGTERM. (CTRL+C)
	while (1) {
		libwebsocket_service(context, 50);
		// libwebsocket_service will process all waiting events with their
		// callback functions and then wait 50 ms.
		// (this is a single threaded webserver and this will keep our server
		// from generating load while there are not requests to process)
	}

	libwebsocket_context_destroy(context);

	return 0;
}
