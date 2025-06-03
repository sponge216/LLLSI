// server.h

#ifndef APP_NETWORK_SERVER_H
#define APP_NETWORK_SERVER_H

#define MAX_MSG_SIZE 1024

#include "network.h"
#include "concurrency.h"
#include <iostream>
#include "IServerActionMediator.h"


namespace server {
	using namespace network;
	using namespace protocols;

	constexpr auto DEFAULT_BACKLOG = 20;
	constexpr auto DEFAULT_PORT = "36542";
	constexpr auto ROOM_SIZE = 20;

	class ServerSocket;
	class EncryptedServerSocket;
	class ServerNetworkManager;

	class NetworkAction : public Action {
	public:
		ActionData* getActionData() override;
	};



	class ServerSocket : public BaseSocket {
	public:
		ServerSocket();
		~ServerSocket();

		bool init(PCSTR port = DEFAULT_PORT, INT ai_family = AF_INET, INT ai_flags = AI_PASSIVE, INT ai_protocol = IPPROTO_TCP, INT ai_socktype = SOCK_STREAM);
		bool initListen(DWORD backlog = DEFAULT_BACKLOG);
		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
		SocketAddrPair acceptNewConnection(SOCKET serverSocket);
	private:
		std::vector<SocketAddrPair> sapVector;
	};


	// --------------------------------------- //

	class EncryptedServerSocket : public ServerSocket {
	public:
		EncryptedServerSocket();
		~EncryptedServerSocket();

		bool initListen(DWORD backlog = DEFAULT_BACKLOG);

		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
		SocketAddrPair acceptNewConnection();
		DWORD firstEncryptionInteraction(SocketAddrPair sap);
		//TODO: DEFINE ALL OF THEM WHENEVER YOU BUILD THE CRYPTO LIB
		SOCKET getSocket() {
			return ServerSocket::sock;
		}
	private:

	};

	class ServerNetworkManager : public IServerActionListener {
	public:
		ServerNetworkManager();
		~ServerNetworkManager();

		EncryptedServerSocket eServerSocket;
		concurrency::ThreadManager threadManager;
		bool killSNM = false;

		virtual void executeAction(Action* pAction) override;

		virtual first_server_client_interaction firstClientInteraction(SocketAddrPair sap);

	private:

	};

	// --------------------------------------- //

}
#endif //APP_NETWORK_SERVER_H