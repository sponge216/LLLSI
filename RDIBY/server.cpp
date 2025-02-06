#include "server.h"

namespace server {

	server::ServerSocket::ServerSocket() {
		this->init();
	}
	bool server::ServerSocket::init(PCSTR port = DEFAULT_PORT, INT backlog = DEFAULT_BACKLOG, INT ai_family = AF_INET, INT ai_flags = AI_PASSIVE, INT ai_protocol = IPPROTO_TCP, INT ai_socktype = SOCK_STREAM) {
		if (this->addrInfo != NULL) {
			this->hints = { 0 };
			this->addrInfo = NULL;
		}
		SOCKET serverSocket;
		struct addrinfo* addrInfo = NULL; // the address info struct, holds all info about the address.
		struct addrinfo hints = { 0 }; // used to set the socket's behavior and address

		hints.ai_family = ai_family; // set internet family.
		hints.ai_socktype = ai_socktype; //set socket type. We use TCP so we set it to sock_stream.
		hints.ai_protocol = ai_protocol;
		hints.ai_flags = ai_flags;

		// Resolve the local address and port to be used by the server
		int res = getaddrinfo(NULL, port, &hints, &addrInfo);
		if (res != 0) {
			printf("getaddrinfo failed: %d\n", res);
			WSACleanup();
			return false;
		}

		if ((serverSocket = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol)) <= 1) {
			printf("Error at socket: %ld\n", WSAGetLastError());
			freeaddrinfo(addrInfo);
			WSACleanup();
			return false;
		}

		if (serverSocket == INVALID_SOCKET) {
			printf("Error at socket: %ld\n", WSAGetLastError());
			freeaddrinfo(addrInfo);
			WSACleanup();
			return false;
		}

		if (bind(serverSocket, addrInfo->ai_addr, (INT)addrInfo->ai_addrlen) == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(addrInfo);
			closesocket(serverSocket);
			WSACleanup();
			return false;
		}

		printf("Server Socket initiated with all good!\n");
		if (listen(serverSocket, backlog) < 0) {
			printf("Listen failed on Server Socket with error: %d\n", WSAGetLastError);
			return false;
		}

		this->hints = hints;
		this->addrInfo = addrInfo;
		return true;
	}
	server::ServerSocket::~ServerSocket() {

	}

	// --------------------------------------- //

	server::ServerNetworkManager::ServerNetworkManager() {

	}
	server::ServerNetworkManager::~ServerNetworkManager() {
	}
}