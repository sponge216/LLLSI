// room.c

#include "room.h"

namespace server {

	server::RoomManager::RoomManager() {

	};

	server::RoomManager::~RoomManager() {

	};

	bool server::RoomManager::createNewRoom(size_t roomID, pRoom ptrRoom) {
		this->roomMap.insert(std::make_pair(roomID, ptrRoom));

		return true;
	};

	bool server::RoomManager::deleteRoom(size_t roomID) {
		this->roomMap.erase(roomID);

		return true;
	};

	bool server::RoomManager::addClientToRoom(size_t roomID, pRoomClient pClient) {
		auto ptrRoom = this->roomMap[roomID];
		if (pClient == nullptr || ptrRoom == nullptr)
			return false;

		ptrRoom->roomVector.push_back(pClient);
		std::cout << "Added client " << pClient->sap.first << "to room " << roomID;

		return true;
	};

	bool server::RoomManager::removeClientFromRoom(size_t roomID, pRoomClient prClient) {
		auto ptrRoom = this->roomMap[roomID];
		std::vector<pRoomClient>* pRoomVector = &ptrRoom->roomVector;
		auto roomSize = ptrRoom->getRoomSize();

		for (DWORD i = 0; i < roomSize; i++) {
			if (pRoomVector->at(i) == prClient)
				pRoomVector->erase(pRoomVector->begin() + i);
		}
		std::cout << "removed client " << prClient->name << "from room " << roomID;

		return true;
	};

	pRoom server::RoomManager::getRoomPtr(size_t roomID) {
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
		this->roomVector = std::vector<pRoomClient>();
		this->prHost = nullptr;
		this->roomID = 0;
		this->roomPassword = 0;

	}
	server::Room::Room(pRoomHost prHost, DWORD roomID, DWORD password) {
		this->roomVector = std::vector<pRoomClient>();
		this->prHost = prHost;
		this->roomID = roomID;
		this->roomPassword = password;
	}
	server::Room::~Room() {

	}

	DWORD server::Room::getRoomSize() {
		return this->roomVector.size() + (this->prHost != nullptr ? 1 : 0);
	}
	// --------------------------------------- //

	server::RoomClient::RoomClient() {
		this->sap = SAP_NULL;
		this->name = NULL;
		this->isHost = false;
		this->isLeaving = false;
	}
	server::RoomClient::RoomClient(SocketAddrPair sap, CHAR* name, bool isHost) {
		this->sap = sap;
		this->name = name;
		this->isHost = isHost;
	}
	server::RoomClient::~RoomClient() {

	}

}