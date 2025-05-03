// client.h

#ifndef APP_NETWORK_CLIENT_H
#define APP_NETWORK_CLIENT_H

#include "network.h"
#include <string>
#include <iostream>
namespace client {
	class ClientSocket;
	class EncryptedClientSocket;
	class ClientNetworkManager;
	class Client;
	constexpr auto SERVER_PORT = 36542;
	constexpr auto SERVER_IP = "93.172.145.134";
	constexpr auto SERVER_IP_X = "172.20.10.7";

	typedef struct first_server_interaction_t {
		CHAR roomName[16];
		CHAR clientName[16];
		CHAR roomPassword[16];
		BYTE clientNameLength;
		BYTE roomNameLength;
		BYTE roomPasswordLength;
		bool isHost;
	}first_server_interaction, * pfirst_server_interaction;

	class Client {
	public:
		Client();
		~Client();
		std::string name;
		std::string roomName;
		std::string roomPassword;
	};

	/// <summary>
	/// socket for clients.
	/// </summary>
	class ClientSocket : public network::BaseSocket
	{
	public:
		ClientSocket();
		~ClientSocket();
		bool initTCP(PCSTR pAddrStr, USHORT port);
		bool initUDP(PCSTR pAddrStr, USHORT port);
		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
	};

	class EncryptedClientSocket : public ClientSocket
	{
	public:
		EncryptedClientSocket();
		~EncryptedClientSocket();

		bool initTCP(PCSTR pAddrStr, USHORT port);
		bool initUDP(PCSTR pAddrStr, USHORT port);

		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;

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

	};
}
#endif // APP_NETWORK_CLIENT_H