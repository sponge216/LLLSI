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
	constexpr auto SERVER_IP = "89.138.4.113";
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

	typedef union {
		sockaddr_in sockAddr4;
		CHAR name[16];
	}server_msg_data_t;
	/// <summary>
	/// msgType - the type of message.
	/// </summary>
	typedef struct {
		UCHAR msgType;
		server_msg_data_t msgData;
	} server_room_msg_t;

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
		bool initTCP(PCSTR clientAddr, USHORT clientPort);
		bool initUDP(USHORT clientPort);
		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
	};

	class EncryptedClientSocket : public ClientSocket
	{
	public:
		EncryptedClientSocket();
		~EncryptedClientSocket();

		bool initTCP(PCSTR pAddrStr, USHORT port);
		bool initUDP(USHORT port);

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