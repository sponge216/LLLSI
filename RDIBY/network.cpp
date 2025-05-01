#include "network.h"

namespace network {
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

#ifndef APP_NETWORK_WSA_DATA
#define APP_NETWORK_WSA_DATA
#endif // APP_NETWORK_WSA_DATA
	int res = fprintf(stdout, "error: %d\n", WSAGetLastError());
}
