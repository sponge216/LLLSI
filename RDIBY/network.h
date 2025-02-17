// network.h

#ifndef APP_NETWORK_H
#define APP_NETWORK_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <memory>

#pragma comment(lib, "Ws2_32.lib")

namespace network {
	
	class BaseSocket {

	public:
		SOCKET sock; //TODO: FIX THESE ACCESS PERMISSIONS!!!
		virtual DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD flags) = 0;
		virtual DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) = 0;

	protected:
		struct addrinfo* pAddrInfo = NULL; // the address info struct, holds all info about the address.
		struct addrinfo hints = { 0 }; // used to set the socket's behavior and address

	};
}
#endif //APP_NETWORK_H