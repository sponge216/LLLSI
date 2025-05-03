#include "server.h"

namespace server {
	bool server::compareSocketAddrPair(SocketAddrPair a, SocketAddrPair b) {
		return a.first == b.first && a.second == b.second;
	}

	server::ServerManager::ServerManager() {
		this->sMediator.addListener(static_cast<IActionListener*>(&this->snManager));
		this->sMediator.addListener(static_cast<IActionListener*>(&this->roomManager));

	}

	server::ServerManager::~ServerManager() {

	}

	bool server::ServerManager::endRoom(DWORD roomID, bool startStreaming) {
		pRoom roomPtr = this->roomManager.getRoomPtr(roomID);
		pRoomHost prHost = roomPtr->prHost;
		SOCKET hostSock = prHost->sap.first;
		sockaddr_in hostAddr = prHost->sap.second->addr;
		ServerNetworkManager* pSnm = &this->snManager;

		RoomMessage clientResponse = RoomMessage();
		server_room_msg_t msg = { 0 };
		msg.msgType = 0;
		msg.msgData.sockAddr4 = hostAddr;

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

	server::ServerMediator::ServerMediator() : Mediator() {

	}
	server::ServerMediator::~ServerMediator() {

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
	server::SocketAddrPair server::ServerSocket::acceptNewConnection(SOCKET serverSocket) {
		Addr* pAddr = new Addr();

		SOCKET clientSocket = accept(serverSocket, (sockaddr*)&pAddr->addr, pAddr->length);
		if (clientSocket <= 1) {
			printf("Error at establishing new client connection: %ld\n", WSAGetLastError());
			WSACleanup();
			return server::SAP_NULL;
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

	interaction_data_t server::ServerNetworkManager::firstClientInteraction(SocketAddrPair sap) {
		interaction_data_t idData = { 0 };
		DWORD res = this->eServerSocket.sendData(sap.first, (CHAR*)&idData, sizeof(idData), 1, 0);
		res = this->eServerSocket.recvData(sap.first, (CHAR*)&idData, sizeof(idData), 0);

		return idData;
	}

	// --------------------------------------- //

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
		ServerMediator* pSMediator = static_cast<ServerMediator*>(&pSm->sMediator);
		SocketAddrPair sapRes = SAP_NULL;

		while (!pSnm->killSNM) {

			sapRes = pSnm->eServerSocket.acceptNewConnection();
			if (server::compareSocketAddrPair(sapRes, server::SAP_NULL))
				continue;

			DWORD res = pSnm->eServerSocket.firstEncryptionInteraction(sapRes); // make sure communication is encrypted!
			if (!res) {
				//TODO: kill interaction successfuly.
			}

			HANDLE hStopEvent = pSnm->threadManager.createNewEvent();

			thread_data_t tData = { sapRes, pData->threadParams };
			LPVOID lpParameter = LPVOID(&tData);
			std::cout << "Client accepted! going into handling\n";
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
		ServerMediator* pSMediator = static_cast<ServerMediator*>(&pSm->sMediator);
		RoomManager* pRm = static_cast<RoomManager*>(&pSm->roomManager);
		SocketAddrPair sap = ptData->sap;

		interaction_data_t idRes = pSnm->firstClientInteraction(sap); // first client interaction, first exchange protocol.
		DWORD roomID = 0; // TODO: make sure to add a hashing function for roomName and put it here!
		DWORD roomPassword = 0; // TODO: hash res.password;
		RoomClient* prClient = new RoomClient(sap, idRes.clientName, idRes.isHost);
		pRoom proom;

		std::cout << "Handling client " << sap.first << "\n";
		std::cout << "Client name: " << prClient->name << " Is client host? " << idRes.isHost << "\n";
		if (idRes.isHost) {
			proom = new Room();

			proom->prHost = prClient;
			proom->roomID = roomID;
			proom->roomPassword = roomPassword;

			std::cout << "Creating room " << idRes.roomName << "\n";
			bool cRes = pRm->createNewRoom(roomID, proom); // creates new room, if new client is a host.
			if (!cRes) {
				//TODO: kill interaction
				pSnm->threadManager.killThread(sap.first);
				closesocket(sap.first);
				delete sap.second;
				delete prClient;
				delete proom;
				return 1;
			}
		}
		else {
			bool cRes = pRm->addClientToRoom(roomID, prClient);
			if (!cRes) {
				//TODO: kill interaction
				pSnm->threadManager.killThread(sap.first);
				closesocket(sap.first);
				delete sap.second;
				delete prClient;
				return 1;
			}
			proom = pRm->getRoomPtr(roomID);
		}

		bool on = true;
		SOCKET clientSock = sap.first;
		server_room_msg_t msgBuffer = { 0 };
		std::cout << "going into client " << sap.first << " loop\n";

		while (on) {
			DWORD recvRes = pSnm->eServerSocket.recvData(clientSock, (CHAR*)&msgBuffer, sizeof(RoomMessage), 0);
			if (!recvRes) {
				std::cout << "receive from client " << clientSock << " failed!\n";
				on = false;
				break;
			}

			switch (msgBuffer.msgType) {
			case RoomMessageParser::LEAVE_MSG:
				if (prClient->isHost)
					pSm->endRoom(roomID, false);
				else
					pRm->removeClientFromRoom(roomID, prClient);
				on = false;
				break;
			case RoomMessageParser::START_PUNCH_MSG:
				pSm->endRoom(roomID, true);
				on = false;
				break;
			}

		}
		// TODO: add a while loop that checks for additional messages.
		// Such as: start room ,leave room, etc.
		// dont forget, when client leaves room kill their connection!

		pSnm->threadManager.killThread(sap.first);
		closesocket(sap.first);
		delete sap.second;
		delete prClient;
		if (idRes.isHost)
			delete proom;
		return 1;
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
		if (pClient == nullptr || ptrRoom == nullptr)
			return false;

		ptrRoom->pRoomVector->push_back(pClient);
		std::cout << "Added client " << pClient->sap.first << "to room " << roomID;

		return true;
	};

	bool server::RoomManager::removeClientFromRoom(DWORD roomID, pRoomClient prClient) {
		auto ptrRoom = this->roomMap[roomID];
		std::vector<pRoomClient>* pRoomVector = ptrRoom->pRoomVector;
		auto roomSize = ptrRoom->getRoomSize();

		for (DWORD i = 0; i < roomSize; i++) {
			if (pRoomVector->at(i) == prClient)
				pRoomVector->erase(pRoomVector->begin() + i);
		}
		std::cout << "removed client " << prClient->name << "from room " << roomID;

		return true;
	};

	pRoom server::RoomManager::getRoomPtr(DWORD roomID) {
		return this->roomMap[roomID];
	}
	void server::RoomManager::executeAction(Action* pAction) {
		RoomAction* prAction = static_cast<RoomAction*>(pAction);
		//TODO: execute the action
	}

	// --------------------------------------- //

	server::RoomMessageParser::RoomMessageParser() {

	}

	server::RoomMessageParser::~RoomMessageParser() {

	}

	// --------------------------------------- //
	server::Room::Room() {
		this->pRoomVector = new std::vector<pRoomClient>;
		this->prHost = nullptr;
		this->roomID = 0;
		this->roomPassword = 0;

	}
	server::Room::Room(pRoomHost prHost, DWORD roomID, DWORD password) {
		this->pRoomVector = new std::vector<pRoomClient>;
		this->prHost = prHost;
		this->roomID = roomID;
		this->roomPassword = password;
	}
	server::Room::~Room() {

	}

	DWORD server::Room::getRoomSize() {
		return this->pRoomVector->size() + (this->prHost != nullptr ? 1 : 0);
	}
	// --------------------------------------- //

	server::RoomClient::RoomClient() {
		this->sap = SAP_NULL;
		this->name = NULL;
		this->isHost = false;
	}
	server::RoomClient::RoomClient(SocketAddrPair sap, CHAR* name, bool isHost) {
		this->sap = sap;
		this->name = name;
		this->isHost = isHost;
	}
	server::RoomClient::~RoomClient() {

	}
}