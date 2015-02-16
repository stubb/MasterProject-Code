#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>

#ifdef __linux__
	#include <cstring>
#endif

static int 
callback_http(struct libwebsocket_context * that,
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
	case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
		printf("connection established\n");
		break;
	case LWS_CALLBACK_RECEIVE: { 
		printf("received data:%zu: %s\n Mach was damit!", len, (char *) in);
		break;
	}
	default:
		break;
	}

	return 0;
}

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",   // name
		callback_http, // callback
		0              // per_session_data_size
	},
	{
		"callback_save_data", // protocol name - very important!
		callback_save_data,   // callback
		0                          // we don't use any per session data
	},
	{
		NULL, NULL, 0   /* End of list */
	}
};

int main(void) {
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
