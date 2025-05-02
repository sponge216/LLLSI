
#include "network.h"
#include "concurrency.h"
#include "media.h"
#include "client.h"
#include "server.h"
#include "host.h"
#include "events.h"
#include "datastructures.h"
#include "IActionMediator.h"

//
//int main(int argc, char** argv) {
//	WSADATA wsa_data; // needed in order to use sockets in windows.
//
//	if (WSAStartup(MAKEWORD(2, 2), &wsa_data)) { // start the window socket application
//		perror("SETUP FAILED"); exit(1);
//	}
//	auto sock = new server::ServerSocket();
//
//	return 0;
//}