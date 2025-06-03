// network.h

#ifndef APP_NETWORK_H
#define APP_NETWORK_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <memory>
#include "protocols.h"
#pragma comment(lib, "Ws2_32.lib")

namespace network {
	class Addr;
	typedef std::pair<SOCKET, Addr*> SocketAddrPair;
	constexpr SocketAddrPair SAP_NULL = SocketAddrPair(NULL, NULL);	// CREATE NULL CASE

	bool compareSocketAddrPair(SocketAddrPair a, SocketAddrPair b);


	class BaseSocket {

	public:
		SOCKET sock; //TODO: FIX THESE ACCESS PERMISSIONS!!!
		virtual DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) = 0;
		virtual DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) = 0;

	protected:
		struct addrinfo* pAddrInfo = NULL; // the address info struct, holds all info about the address.
		struct addrinfo hints = { 0 }; // used to set the socket's behavior and address

	};

	class Addr {
	public:
		Addr() {
			this->length = sizeof(this->addr);
			this->addr = { 0 };
		}
		Addr(sockaddr_in addr, int length) {
			memcpy_s(&this->addr, sizeof(this->addr), &addr, sizeof(addr));
			this->length = length;
		}
		sockaddr_in addr;
		int length;
	};
}
#endif //APP_NETWORK_H