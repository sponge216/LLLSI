#include "host.h"

namespace host {
	host::HostSocket::HostSocket() {

	}

	host::HostSocket::~HostSocket() {

	}

	bool host::HostSocket::initTCP(PCSTR pAddrStr, USHORT port) {
		int iResult = 0;

		if (this->pAddrInfo != NULL) {
			this->hints = { 0 };
			this->pAddrInfo = NULL;
		}
		if (this->sock != -1) {
			iResult = closesocket(this->sock);
			if (iResult == SOCKET_ERROR)
				wprintf(L"socket was already open, failed to close. function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (this->sock == INVALID_SOCKET) {
			wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}
		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port of the server to be connected to.

		SOCKADDR_IN clientService;
		clientService.sin_family = AF_INET;
		inet_pton(AF_INET, pAddrStr, &clientService.sin_addr.s_addr);
		clientService.sin_port = htons(port);

		//----------------------
		// Connect to server.
		iResult = connect(this->sock, (SOCKADDR*)&clientService, sizeof(clientService));
		if (iResult == SOCKET_ERROR) {
			wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
			iResult = closesocket(this->sock);
			if (iResult == SOCKET_ERROR)
				wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}
		return true;
	}

	bool host::HostSocket::initUDP(PCSTR pAddrStr, PCSTR port) {
		struct addrinfo* server_addr = NULL, // the server's address info struct, that holds all info about the address.
			hints; // used to set the socket's behavior and address.

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET; // set internet family.
		hints.ai_socktype = SOCK_STREAM; //set socket type. We use TCP so we set it to sock_stream.
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		int res = getaddrinfo(NULL, port, &hints, &server_addr);
		if (res != 0) {
			printf("getaddrinfo failed: %d\n", res);
			WSACleanup();
			exit(1);
		}
		if ((this->sock = socket(server_addr->ai_family, server_addr->ai_socktype, server_addr->ai_protocol)) <= 1) {
			perror("SOCKET FAILED");
			exit(1);
		}
		if (this->sock == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(server_addr);
			WSACleanup();
			exit(1);
		}
		if (bind(this->sock, server_addr->ai_addr, (int)server_addr->ai_addrlen) == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(server_addr);
			closesocket(this->sock);
			WSACleanup();
			exit(1);
		}

		printf("all good!\n");
		return true;
	}

	bool host::HostSocket::initListen(DWORD backlog) {
		if (listen(this->sock, backlog) < 0) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			return false;
		}
		return true;
	}

	DWORD host::HostSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) {
		DWORD dwMsgLen = dwTypeSize * dwLen;
		if (dwMsgLen <= 0) return -1;

		DWORD res = send(sock, pData, dwMsgLen, flags);
		return res;
	}
	DWORD host::HostSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return recv(sock, pBuffer, dwBufferLen, flags);

	}
	// --------------------------------------- //
	//TODO: PUT CODE IN ALL OF THESE!
	host::EncryptedHostSocket::EncryptedHostSocket() {

	}
	host::EncryptedHostSocket::~EncryptedHostSocket() {

	}

	bool host::EncryptedHostSocket::initTCP(PCSTR pAddrStr, USHORT port) {
		return true;
	}

	bool host::EncryptedHostSocket::initUDP(PCSTR pAddrStr, PCSTR port) {
		return true;
	}

	bool host::EncryptedHostSocket::initListen(DWORD backlog) {
		return true;
	}

	DWORD host::EncryptedHostSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) {
		return 1;
	}
	DWORD host::EncryptedHostSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return 1;
	}

	DWORD host::EncryptedHostSocket::firstServerInteraction() {
		return 1;
	}
	DWORD host::EncryptedHostSocket::firstClientInteraction() {
		return 1;
	}

	// --------------------------------------- //
	host::HostNetworkManager::HostNetworkManager() {

	}
	host::HostNetworkManager::~HostNetworkManager() {
	}
}