// server.h

#ifndef APP_NETWORK_SERVER_H
#define APP_NETWORK_SERVER_H

#define MAX_MSG_SIZE 1024

#include "network.h"
#include "concurrency.h"
#include <iostream>
#include "IActionMediator.h"


namespace server {

	constexpr auto DEFAULT_BACKLOG = 20;
	constexpr auto DEFAULT_PORT = "8080";
	constexpr auto ROOM_SIZE = 20;
	constexpr auto MAX_NAME_SIZE = 10;
	typedef std::pair<SOCKET, sockaddr_in*> SocketAddrPair;
	constexpr SocketAddrPair SAP_NULL = SocketAddrPair(NULL, NULL);	// CREATE NULL CASE

	/// <summary>
	/// msgType - the type of message.
	/// </summary>
	typedef struct {
		UCHAR msgType;
		typedef union {
			typedef union {
				sockaddr_in sockAddr4;
				sockaddr_in6 sockAddr6;
			}ip_addr_info_t;
			CHAR name[MAX_NAME_SIZE];
		}server_msg_data_t;
	} server_room_msg_t;

	inline bool compareSocketAddrPair(SocketAddrPair a, SocketAddrPair b) {
		return a.first == b.first && a.second == b.second;
	}

	typedef struct {
		char* roomName;
		char* userName;
		char* password;
		bool isHost;
	}interaction_data_t, * pinteraction_data;

	typedef struct {
		SocketAddrPair sap;
		LPVOID params;
	}thread_data_t, * pthread_data_t;

	class ServerSocket : public network::BaseSocket {
	public:
		ServerSocket();
		~ServerSocket();

		bool init(PCSTR port = DEFAULT_PORT, INT ai_family = AF_INET, INT ai_flags = AI_PASSIVE, INT ai_protocol = IPPROTO_TCP, INT ai_socktype = SOCK_STREAM);
		bool initListen(DWORD backlog = DEFAULT_BACKLOG);
		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
		SocketAddrPair acceptNewConnection(SOCKET serverSocket, sockaddr_in* addr = NULL, int* addrLen = NULL);
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
		SocketAddrPair acceptNewConnection(sockaddr* addr = NULL, int* addrLen = NULL);
		DWORD firstEncryptionInteraction(SocketAddrPair sap);
		//TODO: DEFINE ALL OF THEM WHENEVER YOU BUILD THE CRYPTO LIB
	private:
		ServerSocket serverSocket;
	};

	// --------------------------------------- //

	class ServerMediator : Mediator {


	};

	// --------------------------------------- //

	// TODO: FIX NESTING LEVELS OF ROOM MANAGER!


	// --------------------------------------- //

	class ServerNetworkManager : IActionListener {
	public:
		ServerNetworkManager();
		~ServerNetworkManager();

		void requestAction(Action action) override;
		void executeAction(Action action) override;

		EncryptedServerSocket eServerSocket;
		concurrency::ThreadManager threadManager;

		bool acceptFunc(DWORD WINAPI clientHandlerFunc(LPVOID params), LPVOID params);
		interaction_data_t firstClientInteraction(SocketAddrPair sap);
		//TODO: FIX ACCESS LEVELS!@!!!
	private:
		bool killSNM = false;
	};

	// --------------------------------------- //

	typedef struct roomClient_t {
		SocketAddrPair sap = SAP_NULL;
		char* name = NULL;
	}RoomClient, * pRoomClient, RoomHost, * pRoomHost;

	typedef struct room_t {
		std::vector<pRoomClient>* pRoomVector;
		pRoomHost host = NULL;
		DWORD roomID = 0;
		DWORD roomPassword = 0;
		DWORD dwCurrRoomSize = 0;

		room_t() :pRoomVector(new std::vector<pRoomClient>), host(NULL), roomID(0), roomPassword(0), dwCurrRoomSize(0) {

		}
	} Room, * pRoom;

	class RoomManager : IActionListener {
	public:
		RoomManager();
		~RoomManager();

		void requestAction(Action action) override;
		void executeAction(Action action) override;

		bool createNewRoom(DWORD roomID, pRoom room);
		bool deleteRoom(DWORD roomID);

		bool addClientToRoom(DWORD roomID, pRoomClient pClient);
		bool removeClientFromRoom(DWORD roomID, SOCKET clientSock);
	private:
		std::unordered_map<DWORD, pRoom> roomMap; // ID -> room map
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

	class RoomMessage {
	public:
		DWORD msgType;
		CHAR* clientName;
		CHAR* password;
	};

	// --------------------------------------- //

	class ServerManager {
	public:
		ServerManager();
		~ServerManager();

		bool endRoom(pRoom roomPtr, bool startStreaming);

		ServerNetworkManager snManager;
		RoomManager roomManager;
		concurrency::ThreadManager threadManager;
	private:

	};
}
#endif //APP_NETWORK_SERVER_H