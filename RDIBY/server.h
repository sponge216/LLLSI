// server.h

#ifndef APP_SERVER_H
#define APP_SERVER_H

#define MAX_MSG_SIZE 1024

#include "network.h"
#include "concurrency.h"
#include <iostream>
#include "IServerActionMediator.h"
#include "room.h"
#include "serverNetwork.h"

namespace server {
	using namespace network;
	using namespace protocols;

	class ServerManager;
	class ServerAction;

	DWORD WINAPI clientHandlerFunc(LPVOID);
	DWORD WINAPI acceptFunc(LPVOID);

	typedef struct client_handler_thread_data {
		SocketAddrPair sap;
		LPVOID params;
	}thread_data_t, * pthread_data_t;

	typedef struct accept_thread_data {
		LPVOID acceptParams;
		LPVOID threadParams;
		DWORD(__stdcall* clientHandlerFunc)(LPVOID);
	}accept_thread_data_t;

	class ServerManager {
	public:
		ServerManager();
		~ServerManager();

		virtual bool endRoom(size_t roomID, bool startStreaming);

		ServerNetworkManager snManager;
		RoomManager roomManager;
		concurrency::ThreadManager threadManager;
		ServerMediator sMediator;
	private:

	};

	class ServerAction : public Action {
	public:
		ActionData* getActionData() override;
	};
}
#endif // APP_SERVER_H
