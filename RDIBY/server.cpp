#include "server.h"

namespace server {

	server::ServerSocket::ServerSocket() {
		this->init();
	}
	bool server::ServerSocket::init(PCSTR port, INT backlog, INT ai_family, INT ai_flags, INT ai_protocol, INT ai_socktype) {
		if (this->pAddrInfo != NULL) {
			this->hints = { 0 };
			this->pAddrInfo = NULL;
		}
		SOCKET serverSocket;
		struct addrinfo* pAddrInfo = NULL; // the address info struct, holds all info about the address.
		struct addrinfo hints = { 0 }; // used to set the socket's behavior and address

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
		if (listen(serverSocket, backlog) < 0) {
			fprintf(stdout, "Listen failed on Server Socket with error: %ld\n", WSAGetLastError());
			return false;
		}

		this->hints = hints;
		this->pAddrInfo = pAddrInfo;
		return true;
	}
	server::ServerSocket::~ServerSocket() {
		fprintf(stdout, "Closing Server Socket. FD - %llu", this->sock);

		freeaddrinfo(this->pAddrInfo);
		closesocket(this->sock);
	}

	bool server::ServerSocket::send(CHAR* data, SIZE_T dwTypeSize) {
		return true;
	}
	bool server::ServerSocket::recv() {
		return true;
	}

	// --------------------------------------- //

	server::ServerNetworkManager::ServerNetworkManager() {

	}
	server::ServerNetworkManager::~ServerNetworkManager() {
	}
}