// room.h

#ifndef APP_SERVER_ROOM_H
#define APP_SERVER_ROOM_H


#include "concurrency.h"
#include <iostream>
#include "IActionMediator.h"
#include "serverNetwork.h"

namespace server {
	class RoomManager;
	class RoomMessage;


	// --------------------------------------- //

	typedef class RoomClient {
	public:
		RoomClient();
		RoomClient(SocketAddrPair, CHAR*, bool);
		~RoomClient();

		SocketAddrPair sap = SAP_NULL;
		CHAR* name = NULL;
		bool isHost = false;
		bool isLeaving = false;
	}RoomClient, * pRoomClient, RoomHost, * pRoomHost;

	typedef class Room {
	public:
		Room();
		Room(pRoomHost, DWORD, DWORD);
		~Room();
		DWORD getRoomSize();

		std::vector<pRoomClient> roomVector;
		pRoomHost prHost;
		size_t roomID;
		size_t roomPassword;
	} *pRoom;

	class RoomManager : public IServerActionListener {
	public:
		RoomManager();
		~RoomManager();

		void executeAction(Action* pAction) override;

		bool createNewRoom(size_t roomID, pRoom room);
		bool deleteRoom(size_t roomID);

		bool addClientToRoom(size_t roomID, pRoomClient pClient);
		bool removeClientFromRoom(size_t roomID, pRoomClient prClient);
		pRoom getRoomPtr(size_t roomID);
	private:
		std::unordered_map<size_t, pRoom> roomMap; // ID -> room map
	};

	class RoomMessageParser {
	public:
		RoomMessageParser();
		~RoomMessageParser();
		RoomMessage parseMessage(CHAR* pBuffer);


		auto static const ENTRY_MSG = 0; // when client first enters
		auto static const LEAVE_MSG = 1; // when client leaves
		auto static const CLOSE_ROOM_MSG = 2; // when host closes room without starting it
		auto static const START_PUNCH_MSG = 3; // when host closes room and wants to start nat hole punching
	};

	// --------------------------------------- //

	class RoomAction : public Action {
	public:
		ActionData* getActionData() override;
	};
}

#endif //APP_SERVER_ROOM_H