#include "server.h"

namespace server {

	server::ServerManager::ServerManager() {

	}

	server::ServerManager::~ServerManager() {

	}

	bool server::ServerManager::endRoom(pRoom roomPtr, bool startStreaming) {
		pRoomHost prHost = roomPtr->host;
		SOCKET hostSock = prHost->sap.first;
		sockaddr_in* pHostAddr = prHost->sap.second;
		ServerNetworkManager* pSnm = &this->snManager;

		server_room_msg_t clientResponse = { 0 };
		server_room_msg_t msg = { 0 };
		msg.msgType = 0;
		msg.msgData.ipAddrInfo.sockAddr4 = *pHostAddr;

		auto pClientsVec = roomPtr->pRoomVector;

		if (startStreaming) {
			for (pRoomClient prClient : *pClientsVec) {
				SOCKET clientSock = prClient->sap.first;
				pSnm->eServerSocket.sendData(clientSock, (CHAR*)&msg, sizeof(msg), 1, 0);
				pSnm->eServerSocket.recvData(clientSock, (CHAR*)&clientResponse, sizeof(clientResponse), 0);
				//TODO: if (verify response)
			}
		}

		for (pRoomClient prClient : *pClientsVec) {
			SOCKET clientSock = prClient->sap.first;
			closesocket(clientSock);
		}

		return true;
	}

	// --------------------------------------- //

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
	server::SocketAddrPair server::ServerSocket::acceptNewConnection(SOCKET serverSocket, sockaddr_in* addr, int* addrLen) {
		sockaddr_in tempAddr = { 0 };
		if (addr == NULL)
			addr = &tempAddr;

		SOCKET clientSocket = accept(serverSocket, (sockaddr*)addr, addrLen);
		if (clientSocket <= 1) {
			printf("Error at establishing new client connection: %ld\n", WSAGetLastError());
			WSACleanup();
			return server::SAP_NULL;
		}
		SocketAddrPair sadRes = SocketAddrPair(clientSocket, addr);
		return sadRes;
	}

	// --------------------------------------- //

	server::ServerNetworkManager::ServerNetworkManager() {

	}

	server::ServerNetworkManager::~ServerNetworkManager() {
		this->killSNM = true;
		this->threadManager.~ThreadManager();
	}

	/// <summary>
	/// server's response to an incoming client connection.
	/// </summary>
	/// <param name="clientHandlerFunc">- function that handles the new client</param>
	/// <param name="params">- parameters that need to be passed to clientHandlerFunc</param>
	/// <returns></returns>
	DWORD WINAPI server::acceptFunc(LPVOID params) {
		accept_thread_data_t* pData = static_cast<accept_thread_data_t*>(params);

		ServerManager* pSm = static_cast<ServerManager*>(pData->acceptParams);
		ServerNetworkManager* pSnm = static_cast<ServerNetworkManager*>(&pSm->snManager);
		while (!pSnm->killSNM) {
			SocketAddrPair sapRes = pSnm->eServerSocket.acceptNewConnection();
			if (server::compareSocketAddrPair(sapRes, server::SAP_NULL))
				continue;

			DWORD res = pSnm->eServerSocket.firstEncryptionInteraction(sapRes); // make sure communication is encrypted!
			if (!res) {
				//TODO: kill interaction successfuly.
			}

			HANDLE hStopEvent = pSnm->threadManager.createNewEvent();

			thread_data_t tData = { sapRes, pData->threadParams };
			LPVOID lpParameter = LPVOID(&tData);

			concurrency::pConThread pctClient = new concurrency::ConThread(hStopEvent, pData->clientHandlerFunc, lpParameter);
			pSnm->threadManager.createNewThread(sapRes.first, pctClient);
		}
		return true;
	}

	// the thread function that handles the client. 
	// it uses the first interaction function to get essential data from the client.
	DWORD WINAPI server::clientHandlerFunc(LPVOID params) {
		pthread_data_t ptData = static_cast<pthread_data_t>(params);

		ServerManager* pSm = static_cast<ServerManager*>(ptData->params);
		ServerNetworkManager* pSnm = &pSm->snManager;
		SocketAddrPair sap = ptData->sap;

		interaction_data_t res = pSnm->firstClientInteraction(sap); // first client interaction, first exchange protocol.
		DWORD roomID = 0; // TODO: make sure to add a hashing function for roomName and put it here!
		DWORD roomPassword = 0; // TODO: hash res.password;

		if (res.isHost) {
			RoomHost roomHost = { sap,res.userName };

			pRoom proom = new Room();
			proom->dwCurrRoomSize++;
			proom->host = &roomHost;
			proom->roomID = roomID;
			proom->roomPassword = roomPassword;

			pSm->roomManager.createNewRoom(roomID, proom); // creates new room, if new client is a host.
		}
		else {
			RoomClient roomClient = { sap,res.userName };
			pRoomClient prc = &roomClient; // pointer to the new room client
			bool cRes = pSm->roomManager.addClientToRoom(roomID, prc);
			if (!cRes) {
				//TODO: kill interaction
			}
		}

		bool on = true;
		SOCKET clientSock = sap.first;
		CHAR pBuffer[MAX_MSG_SIZE] = { 0 };

		while (on) {
			pSnm->eServerSocket.recvData(clientSock, pBuffer, MAX_MSG_SIZE, 0);
		}
		// TODO: add a while loop that checks for additional messages.
		// Such as: start room ,leave room, etc.
		// dont forget, when client leaves room kill their connection!

		return 1;
	}

	interaction_data_t server::ServerNetworkManager::firstClientInteraction(SocketAddrPair sap) {
		interaction_data_t idData = { 0 };

		return idData;
	}

	// --------------------------------------- //

	server::RoomManager::RoomManager() {

	};

	server::RoomManager::~RoomManager() {
		for (std::pair<DWORD, pRoom> room : this->roomMap) {
			auto proom = room.second;
			delete proom->pRoomVector;

		}
	};

	bool server::RoomManager::createNewRoom(DWORD roomID, pRoom ptrRoom) {
		this->roomMap.insert(std::make_pair(roomID, ptrRoom));

		return true;
	};

	bool server::RoomManager::deleteRoom(DWORD roomID) {
		delete this->roomMap[roomID]->pRoomVector;
		this->roomMap.erase(roomID);

		return true;
	};

	bool server::RoomManager::addClientToRoom(DWORD roomID, pRoomClient pClient) {
		auto ptrRoom = this->roomMap[roomID];
		ptrRoom->pRoomVector->push_back(pClient);

		std::cout << "Added client " << pClient->sap.first << "to room " << roomID;

		return true;
	};

	bool server::RoomManager::removeClientFromRoom(DWORD roomID, SOCKET clientSock) {
		auto ptrRoom = this->roomMap[roomID];
		std::vector<pRoomClient>* pRoomVector = ptrRoom->pRoomVector;

		for (DWORD i = 0; i < ptrRoom->dwCurrRoomSize; i++) {
			if (pRoomVector->at(i)->sap.first == clientSock)
				pRoomVector->erase(pRoomVector->begin() + i);
		}
		std::cout << "removed client " << clientSock << "from room " << roomID;

		return true;
	};

	// --------------------------------------- //

	server::RoomMessageParser::RoomMessageParser() {

	}

	server::RoomMessageParser::~RoomMessageParser() {

	}
}