// network.h

#ifndef APP_NETWORK_H
#define APP_NETWORK_H
#define DEFAULT_PORT "30527"

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")


class BaseSocket {

public:
	bool initialize() {
		this->hints.ai_family = AF_INET; // set internet family.
		this->hints.ai_socktype = SOCK_STREAM; //set socket type. We use TCP so we set it to sock_stream.
		this->hints.ai_protocol = IPPROTO_TCP;
		this->hints.ai_flags = AI_PASSIVE;

		auto res = getaddrinfo(NULL, DEFAULT_PORT, &this->hints, &this->addrInfo);

		this->sock = socket(this->addrInfo->ai_family, this->addrInfo->ai_socktype, this->addrInfo->ai_protocol);
	}

protected:
	static WSADATA wsaData;
	SOCKET sock;
	struct addrinfo* addrInfo = NULL, // the address info struct, holds all info about the address.
		hints = { 0 }; // used to set the socket's behavior and address
};

/// <summary>
///  socket for hosts.
/// </summary>
class HostSocket : public BaseSocket {

public:
	HostSocket();
};
/// <summary>
/// socket for clients.
/// </summary>
class ClientSocket : public BaseSocket {

public:
	ClientSocket();
};
/// <summary>
/// socket for servers.
/// </summary>
class ServerSocket : public BaseSocket {

public:
	ServerSocket();

};

/// <summary>
/// 
/// </summary>
class ClientNetworkManager {

public:
	ClientNetworkManager();

private:
	ClientSocket clientSocket;

};
/// <summary>
/// 
/// </summary>
class HostNetworkManager {

public:
	HostNetworkManager();

private:
	HostSocket hostSocket;

};
/// <summary>
/// 
/// </summary>
class ServerNetworkManager {
public:
	ServerNetworkManager();

private:
	ServerSocket serverSocket;

};
#endif //APP_NETWORK_H