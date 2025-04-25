#include "network.h"
namespace network {
#ifndef APP_NETWORK_WSA_DATA
#define APP_NETWORK_WSA_DATA
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
};
