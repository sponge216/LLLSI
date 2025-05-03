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
	constexpr auto DEFAULT_PORT = "36542";
	constexpr auto ROOM_SIZE = 20;
	constexpr auto MAX_NAME_SIZE = 10;

	class ServerSocket;
	class EncryptedServerSocket;
	class ServerManager;
	class ServerMediator;
	class ServerNetworkManager;
	class RoomManager;
	class RoomMessage;
	class ServerAction;
	class IServerActionListener;
	class Addr;

	typedef std::pair<SOCKET, Addr*> SocketAddrPair;
	constexpr SocketAddrPair SAP_NULL = SocketAddrPair(NULL, NULL);	// CREATE NULL CASE

	DWORD WINAPI clientHandlerFunc(LPVOID);
	DWORD WINAPI acceptFunc(LPVOID);
	bool compareSocketAddrPair(SocketAddrPair a, SocketAddrPair b);

	typedef union {
		sockaddr_in sockAddr4;
		sockaddr_in6 sockAddr6;
	}ip_addr_info_t;

	typedef union {
		ip_addr_info_t ipAddrInfo;
		CHAR name[MAX_NAME_SIZE];
	}server_msg_data_t;
	/// <summary>
	/// msgType - the type of message.
	/// </summary>
	typedef struct {
		UCHAR msgType;
		server_msg_data_t msgData;
	} server_room_msg_t;

	typedef struct {
		char roomName[16];
		char clientName[16];
		char roomPassword[16];
		BYTE clientNameLength;
		BYTE roomNameLength;
		BYTE roomPasswordLength;
		bool isHost;
	}interaction_data_t, * pinteraction_data;

	typedef struct {
		SocketAddrPair sap;
		LPVOID params;
	}thread_data_t, * pthread_data_t;

	typedef struct {
		LPVOID acceptParams;
		LPVOID threadParams;
		DWORD(__stdcall* clientHandlerFunc)(LPVOID);
	}accept_thread_data_t;

	class Addr {
	public:
		Addr() {
			this->addr = { 0 };
			this->length = nullptr;
		}
		sockaddr_in addr;
		int* length;
	};

	class IServerActionListener : public IActionListener {
	public:
		IServerActionListener() : IActionListener() {

		}
	};

	class NetworkAction : public Action {
	public:
		ActionData* getActionData() override;
	};

	class RoomAction : public Action {
	public:
		ActionData* getActionData() override;
	};

	class ServerSocket : public network::BaseSocket {
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

	// --------------------------------------- //

	class ServerMediator : public Mediator {
	public:
		ServerMediator();
		~ServerMediator();

	};

	// --------------------------------------- //

	// TODO: FIX NESTING LEVELS OF ROOM MANAGER!


	// --------------------------------------- //

	class ServerNetworkManager : public IServerActionListener {
	public:
		ServerNetworkManager();
		~ServerNetworkManager();

		EncryptedServerSocket eServerSocket;
		concurrency::ThreadManager threadManager;
		bool killSNM = false;

		void executeAction(Action* pAction) override;

		bool acceptFunc(LPVOID params);
		interaction_data_t firstClientInteraction(SocketAddrPair sap);

		//TODO: FIX ACCESS LEVELS!@!!!
	private:

	};

	// --------------------------------------- //

	typedef class RoomClient {
	public:
		RoomClient();
		RoomClient(SocketAddrPair, char*, bool);
		~RoomClient();

		SocketAddrPair sap = SAP_NULL;
		char* name = NULL;
		bool isHost = false;
	}RoomClient, * pRoomClient, RoomHost, * pRoomHost;

	typedef class Room {
	public:
		Room();
		Room(pRoomHost, DWORD, DWORD);
		~Room();
		DWORD getRoomSize();

		std::vector<pRoomClient>* pRoomVector;
		pRoomHost prHost;
		DWORD roomID;
		DWORD roomPassword;
	} *pRoom;

	class RoomManager : public IServerActionListener {
	public:
		RoomManager();
		~RoomManager();

		void executeAction(Action* pAction) override;

		bool createNewRoom(DWORD roomID, pRoom room);
		bool deleteRoom(DWORD roomID);

		bool addClientToRoom(DWORD roomID, pRoomClient pClient);
		bool removeClientFromRoom(DWORD roomID, pRoomClient prClient);
		pRoom getRoomPtr(DWORD roomID);
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
	};

	// --------------------------------------- //

	class ServerManager {
	public:
		ServerManager();
		~ServerManager();

		bool endRoom(DWORD roomID, bool startStreaming);

		ServerNetworkManager snManager;
		RoomManager roomManager;
		concurrency::ThreadManager threadManager;
		ServerMediator sMediator;
	private:

	};


}
#endif //APP_NETWORK_SERVER_H