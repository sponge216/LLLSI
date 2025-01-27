// network.h

#ifndef APP_NETWORK_H
#define APP_NETWORK_H

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace network {

#define DEFAULT_PORT "30527"

	class BaseSocket {

	public:
		virtual bool send(CHAR* pData) = 0;
		virtual bool recv() = 0;

	protected:
		SOCKET sock;
		struct addrinfo* addrInfo = NULL; // the address info struct, holds all info about the address.
		struct addrinfo* hints = { 0 }; // used to set the socket's behavior and address

	};

	class ITCPSocket {
	public:
		virtual bool initTCP() = 0;

	};

	class IUDPSocket {
	public:
		virtual bool initUDP() = 0;

	};
}
#endif //APP_NETWORK_H