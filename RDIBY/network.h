// network.h

#ifndef APP_NETWORK_H

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace network {

#define APP_NETWORK_H
#define DEFAULT_PORT "30527"

	class BaseSocket {

	public:
		virtual bool initializeTCP() = 0;
		virtual bool initializeUDP() = 0;

	protected:
		SOCKET sock;
		struct addrinfo* addrInfo = NULL; // the address info struct, holds all info about the address.
		struct addrinfo* hints = { 0 }; // used to set the socket's behavior and address
	private:
		static const WSADATA wsaData;
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
		~ClientSocket();
		bool initializeTCP() override;
		bool initializeUDP() override;
	};
	/// <summary>
	/// socket for servers.
	/// </summary>
	class ServerSocket : public BaseSocket {

	public:
		ServerSocket();
		~ServerSocket();
		bool initialize();
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
		~HostNetworkManager();

	private:
		HostSocket hostSocket;

	};
	/// <summary>
	/// 
	/// </summary>
	class ServerNetworkManager {
	public:
		ServerNetworkManager();
		~ServerNetworkManager();

	private:
		ServerSocket serverSocket;

	};
}
#endif //APP_NETWORK_H