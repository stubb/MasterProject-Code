#ifndef __NETWORKHANDLER_H__
#define __NETWORKHANDLER_H__

#include "WebSocketServer.h"

extern MonkeyMediaProcessor* mmp;

using namespace std;

class NetworkHandler {

	private:
		WebSocketServer* websocket_server;
		int websocket_server_port = 9000;
		void init();
		void init(int websocket_port);

	public:
		NetworkHandler();
		NetworkHandler(int websocket_port);
		int run();
		void destroy();
};

#endif