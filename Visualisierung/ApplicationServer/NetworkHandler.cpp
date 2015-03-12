#include <iostream>
#include "NetworkHandler.h"

using namespace std;

NetworkHandler::NetworkHandler() {
	init();
}

NetworkHandler::NetworkHandler(int websocket_port) {
	init(websocket_port);
}

void NetworkHandler::init() {
	websocket_server = new WebSocketServer();
}

void NetworkHandler::init(int websocket_port) {
	websocket_server = new WebSocketServer(websocket_port);
}
/**
 * Handles all the server loops he takes care of.
 */
int NetworkHandler::run() {
	int websocket_server_return_value = 0;
	while(!websocket_server_return_value) {
		websocket_server_return_value = websocket_server->run();
		// add other servers loops here
	}
	return websocket_server_return_value;
}

void NetworkHandler::destroy() {
	websocket_server->destroy();
}