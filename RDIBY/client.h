// client.h

#ifndef APP_NETWORK_CLIENT_H
#define APP_NETWORK_CLIENT_H

#include "network.h"
#include <string>
#include <iostream>
#include "concurrency.h"
namespace client {
	using namespace network;
	using namespace protocols;
	class ClientSocket;
	class EncryptedClientSocket;
	class ClientNetworkManager;
	class Client;

	constexpr auto SERVER_PORT = 36542;
	constexpr auto SERVER_IP = "89.139.17.30";
	constexpr auto SERVER_IP_X = "172.20.10.7";

	class Client {
	public:
		Client();
		~Client();
		std::string name;
		std::string roomName;
		std::string roomPassword;
		bool isHost;
	};

	/// <summary>
	/// socket for clients.
	/// </summary>
	class ClientSocket : public network::BaseSocket
	{
	public:
		ClientSocket();
		~ClientSocket();
		virtual bool initTCP(PCSTR serverIPAddr, USHORT serverPort);
		virtual bool initUDP(USHORT clientPort);
		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
		DWORD sendDataTo(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, Addr& addr);
		DWORD recvDataFrom(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, Addr& addr);
	};

	class EncryptedClientSocket : public ClientSocket
	{
	public:
		EncryptedClientSocket();
		~EncryptedClientSocket();

		bool initTCP(PCSTR serverIPAddr, USHORT serverPort) override;
		bool initUDP(USHORT port) override;

		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
		DWORD sendDataTo(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, Addr& addr);
		DWORD recvDataFrom(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, Addr& addr);
		DWORD firstServerInteraction(Client client);
		DWORD firstHostInteraction();

	};

	/// <summary>
	/// 
	/// </summary>
	class ClientNetworkManager {

	public:
		ClientNetworkManager();
		~ClientNetworkManager();

	private:
		EncryptedClientSocket clientSocket;
		concurrency::ThreadManager threadManager;

	};
}
#endif // APP_NETWORK_CLIENT_H