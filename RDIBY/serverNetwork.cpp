#include "serverNetwork.h"

namespace server {
	using namespace network;

	server::ServerSocket::ServerSocket() {
		this->init();
	}

	server::ServerSocket::~ServerSocket() {
		fprintf(stdout, "Closing Server Socket. FD - %llu", this->sock);
		freeaddrinfo(this->pAddrInfo);
		closesocket(this->sock);
	}

	bool server::ServerSocket::init(PCSTR port, INT ai_family, INT ai_flags, INT ai_protocol, INT ai_socktype) {
		if (this->pAddrInfo != NULL) {
			this->hints = { 0 };
			this->pAddrInfo = NULL;
		}

		SOCKET serverSocket;
		struct addrinfo* pAddrInfo = NULL; // the address info struct, holds all info about the address.
		struct addrinfo hints = { 0 }; // used to set the socket's behavior and address
		ZeroMemory(&hints, sizeof(hints));

		hints.ai_family = ai_family; // set internet family.
		hints.ai_socktype = ai_socktype; //set socket type. We use TCP so we set it to sock_stream.
		hints.ai_protocol = ai_protocol;
		hints.ai_flags = ai_flags;

		// Resolve the local address and port to be used by the server
		int res = getaddrinfo(NULL, port, &hints, &pAddrInfo);
		if (res != 0) {
			printf("getaddrinfo failed: %d\n", res);
			WSACleanup();
			return false;
		}

		if ((serverSocket = socket(pAddrInfo->ai_family, pAddrInfo->ai_socktype, pAddrInfo->ai_protocol)) <= 1) {
			printf("Error at socket: %ld\n", WSAGetLastError());
			freeaddrinfo(pAddrInfo);
			WSACleanup();
			return false;
		}

		if (serverSocket == INVALID_SOCKET) {
			printf("Error at socket: %ld\n", WSAGetLastError());
			freeaddrinfo(pAddrInfo);
			WSACleanup();
			return false;
		}

		if (bind(serverSocket, pAddrInfo->ai_addr, (INT)pAddrInfo->ai_addrlen) == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(pAddrInfo);
			closesocket(serverSocket);
			WSACleanup();
			return false;
		}

		printf("Server Socket initiated!\n");

		this->sock = serverSocket;
		this->hints = hints;
		this->pAddrInfo = pAddrInfo;
		return true;
	}

	bool server::ServerSocket::initListen(DWORD backlog) {
		if (listen(this->sock, backlog) < 0) {
			fprintf(stdout, "Listen failed on Server Socket with error: %ld\n", WSAGetLastError());
			return false;
		}
		return true;
	}

	DWORD server::ServerSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) {
		DWORD dwMsgLen = dwTypeSize * dwLen;
		if (dwMsgLen <= 0) return -1;

		DWORD res = send(sock, pData, dwMsgLen, flags);
		return res;
	}

	DWORD server::ServerSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return recv(sock, pBuffer, dwBufferLen, flags);

	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="serverSocket">server's socket</param>
	/// <param name="addr">address struct buffer for holding addr info. Default value is NULL</param>
	/// <param name="addrLen">length of addr. Default value is NULL</param>
	/// <returns>Socket Address Pair if successful, otherwise SAP_NULL</returns>
	SocketAddrPair server::ServerSocket::acceptNewConnection(SOCKET serverSocket) {
		Addr* pAddr = new Addr();

		SOCKET clientSocket = accept(serverSocket, (sockaddr*)&pAddr->addr, &pAddr->length);
		if (clientSocket <= 1) {
			printf("Error at establishing new client connection: %ld\n", WSAGetLastError());
			WSACleanup();
			return SAP_NULL;
		}
		SocketAddrPair sadRes = SocketAddrPair(clientSocket, pAddr);
		std::cout << "Accepting new connection!";

		return sadRes;
	}

	// --------------------------------------- //

	server::EncryptedServerSocket::EncryptedServerSocket() {

	}
	server::EncryptedServerSocket::~EncryptedServerSocket() {

	}

	bool server::EncryptedServerSocket::initListen(DWORD backlog) {
		ServerSocket::initListen(backlog);

		return true;
	};

	DWORD server::EncryptedServerSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) {
		//TODO: add encryption stuff

		return ServerSocket::sendData(sock, pData, dwTypeSize, dwLen, flags);
	};

	DWORD server::EncryptedServerSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		DWORD len = ServerSocket::recvData(sock, pBuffer, dwBufferLen, flags);
		//TODO: decrypt stuff

		return len;
	};

	SocketAddrPair server::EncryptedServerSocket::acceptNewConnection() {
		return ServerSocket::acceptNewConnection(this->getSocket());
	};

	DWORD server::EncryptedServerSocket::firstEncryptionInteraction(SocketAddrPair sap) {
		//TODO: do encryption stuff
		return 1;
	};

	// --------------------------------------- // 

	server::ServerNetworkManager::ServerNetworkManager() {

	}

	server::ServerNetworkManager::~ServerNetworkManager() {
		this->killSNM = true;

	}

	void server::ServerNetworkManager::executeAction(Action* pAction) {
		NetworkAction* pnAction = static_cast<NetworkAction*>(pAction);
		//TODO: execute action
	}

	first_server_client_interaction server::ServerNetworkManager::firstClientInteraction(SocketAddrPair sap) {
		first_server_client_interaction idData = { 0 };
		DWORD res = this->eServerSocket.sendData(sap.first, (CHAR*)&idData, sizeof(idData), 1, 0);
		res = this->eServerSocket.recvData(sap.first, (CHAR*)&idData, sizeof(idData), 0);

		return idData;
	}

	// --------------------------------------- //

}