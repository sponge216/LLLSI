// client.h

#ifndef APP_NETWORK_CLIENT_H
#define APP_NETWORK_CLIENT_H

#include "network.h"

namespace client {

#define SERVER_PORT 36542
#define SERVER_IP "172.20.10.4"
#define SERVER_IP_X "172.20.10.7"

	/// <summary>
	/// socket for clients.
	/// </summary>
	class ClientSocket : public network::BaseSocket
	{
	public:
		ClientSocket();
		~ClientSocket();
		bool initTCP(PCSTR pAddrStr, DWORD port);
		bool initUDP(PCSTR pAddrStr, DWORD port);
		inline DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		inline DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
	};

	class EncryptedClientSocket : public ClientSocket
	{
	public:
		EncryptedClientSocket();
		~EncryptedClientSocket();
		bool initTCP();
		bool initUDP();
		inline DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		inline DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
		DWORD firstInteraction();

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