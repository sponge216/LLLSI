#include "server.h"

namespace server {
	using namespace network;

	server::ServerManager::ServerManager() {
		this->sMediator.addListener(static_cast<IActionListener*>(&this->snManager));
		this->sMediator.addListener(static_cast<IActionListener*>(&this->roomManager));

	}

	server::ServerManager::~ServerManager() {

	}

	bool server::ServerManager::endRoom(size_t roomID, bool startStreaming) {
		pRoom roomPtr = this->roomManager.getRoomPtr(roomID);
		pRoomHost prHost = roomPtr->prHost;
		SOCKET hostSock = prHost->sap.first;
		sockaddr_in* pHostAddr = &prHost->sap.second->addr;
		ServerNetworkManager* pSnm = &this->snManager;

		server_room_msg_t msgToHost = { 0 };
		server_room_msg_t msgToClient = { 0 };
		server_room_msg_t leaveMsg = { 0 };
		leaveMsg.msgType = RoomMessageParser::LEAVE_MSG;

		auto clientsVec = roomPtr->roomVector;

		if (startStreaming) {
			msgToClient.msgType = RoomMessageParser::START_PUNCH_MSG;
			msgToHost.msgType = RoomMessageParser::START_PUNCH_MSG;
			memcpy_s(&msgToClient.msgData.sockAddr4, sizeof(msgToClient.msgData.sockAddr4), pHostAddr, sizeof(*pHostAddr));

			// send host all the clients addrs
			for (pRoomClient prClient : clientsVec) {
				sockaddr_in* pClientAddr = &prClient->sap.second->addr;

				ZeroMemory(&msgToHost, sizeof(msgToHost));
				msgToHost.msgType = RoomMessageParser::START_PUNCH_MSG;
				memcpy_s(&msgToHost.msgData.sockAddr4, sizeof(msgToHost.msgData.sockAddr4), pClientAddr, sizeof(*pClientAddr));

				pSnm->eServerSocket.sendData(hostSock, (CHAR*)&msgToHost, sizeof(msgToHost), 1, 0);
			}

			// send host addr to clients
			for (pRoomClient prClient : clientsVec) {
				SOCKET clientSock = prClient->sap.first;
				prClient->isLeaving = true;
				pSnm->eServerSocket.sendData(clientSock, (CHAR*)&msgToClient, sizeof(msgToClient), 1, 0);
			}
		}

		for (pRoomClient prClient : clientsVec) {
			SOCKET clientSock = prClient->sap.first;
			pSnm->eServerSocket.sendData(clientSock, (CHAR*)&leaveMsg, sizeof(leaveMsg), 1, 0);
			std::cout << "Sending off client " << clientSock << std::endl;
		}
		pSnm->eServerSocket.sendData(hostSock, (CHAR*)&leaveMsg, sizeof(leaveMsg), 1, 0);
		std::cout << "Sending off host " << hostSock << std::endl;

		this->roomManager.deleteRoom(roomID);
		return true;
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

			// Convert the IP address (in network byte order) to a string
			wchar_t ipAddress[INET_ADDRSTRLEN];
			InetNtop(AF_INET, &sapRes.second->addr.sin_addr, ipAddress, INET_ADDRSTRLEN);

			// Extract the port number (convert from network byte order to host byte order)
			u_short port = ntohs(sapRes.second->addr.sin_port);

			std::wcout << "IP Address: " << ipAddress << std::endl;
			std::cout << "Port: " << port << std::endl;

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

		first_server_client_interaction idRes = pSnm->firstClientInteraction(sap); // first client interaction, first exchange protocol.
		std::string roomNameStr = std::string(idRes.roomName);
		std::string roomPassStr = std::string(idRes.roomPassword);

		std::hash<std::string> hasher;
		size_t roomID = hasher(roomNameStr);
		size_t roomPassHash = hasher(roomPassStr);

		RoomClient rClient = RoomClient(sap, idRes.clientName, idRes.isHost);
		pRoomClient prClient = &rClient;

		Room room = Room();
		pRoom proom = &room;

		std::cout << "Handling client " << sap.first << "\n";
		std::cout << "Client name: " << prClient->name << " Is client host? " << idRes.isHost << "\n";
		if (idRes.isHost) {
			proom->prHost = prClient;
			proom->roomID = roomID;
			proom->roomPassword = roomPassHash;

			std::cout << "Creating room " << idRes.roomName << "\n";
			bool cRes = pRm->createNewRoom(roomID, proom); // creates new room, if new client is a host.
			if (!cRes) {
				//TODO: kill interaction
				pSnm->threadManager.killThread(sap.first);
				closesocket(sap.first);
				delete sap.second;
				return 1;
			}
		}
		else {
			proom = pRm->getRoomPtr(roomID);
			if (proom != nullptr && proom->roomPassword == roomPassHash)
				bool cRes = pRm->addClientToRoom(roomID, prClient);
			else {

				//TODO: kill interaction
				pSnm->threadManager.killThread(sap.first);
				closesocket(sap.first);
				delete sap.second;
				return 1;
			}
		}

		bool on = true;
		SOCKET clientSock = sap.first;
		server_room_msg_t msgBuffer = { 0 };
		std::cout << "going into client " << clientSock << " loop\n";

		while (on) {
			DWORD recvRes = pSnm->eServerSocket.recvData(clientSock, (CHAR*)&msgBuffer, sizeof(msgBuffer), 0);
			if (!recvRes) {
				std::cout << "receive from client " << clientSock << " failed!\n";
				on = false;
				if (prClient->isLeaving)  // check if leaving because the room is closing
					break;
				if (prClient->isHost)
					pSm->endRoom(roomID, false);
				else {
					pSm->roomManager.removeClientFromRoom(roomID, prClient);
				}
				closesocket(clientSock);

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
				if (!prClient->isHost)
					continue;

				pSm->endRoom(roomID, true);
				break;
			}

		}
		// TODO: add a while loop that checks for additional messages.
		// Such as: start room ,leave room, etc.
		// dont forget, when client leaves room kill their connection!
		std::cout << "Client " << clientSock << " is leaving!" << std::endl;
		pSnm->threadManager.killThread(sap.first);

		return 1;
	}

	// --------------------------------------- //

	
}