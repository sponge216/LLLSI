#include "client.h"

//TODO: CHANGE ALL FUNCTIONS TO RETURN ERROR CODES!!

namespace client {
	client::ClientSocket::ClientSocket() {
		this->sock = -1;
	}
	bool client::ClientSocket::initTCP(PCSTR pAddrStr, USHORT port) {
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
	bool client::ClientSocket::initUDP(PCSTR pAddrStr, USHORT port) {
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

		this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
	client::ClientSocket::~ClientSocket() {

	}
	DWORD client::ClientSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) {
		DWORD dwMsgLen = dwTypeSize * dwLen;
		if (dwMsgLen <= 0) return -1;

		DWORD res = send(sock, pData, dwMsgLen, flags);
		return res;
	}
	DWORD client::ClientSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return recv(sock, pBuffer, dwBufferLen, flags);

	}

	// --------------------------------------- //
	//TODO: PUT CODE IN ALL OF THESE!

	client::EncryptedClientSocket::EncryptedClientSocket() {

	}
	client::EncryptedClientSocket::~EncryptedClientSocket() {

	}

	bool client::EncryptedClientSocket::initTCP() {
		return true;
	}

	bool client::EncryptedClientSocket::initUDP() {
		return true;
	}

	DWORD client::EncryptedClientSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) {
		return 1;
	}
	DWORD client::EncryptedClientSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return 1;
	}

	DWORD client::EncryptedClientSocket::firstServerInteraction() {
		return 1;
	}

	DWORD client::EncryptedClientSocket::firstHostInteraction() {
		return 1;
	}

	// --------------------------------------- //

	client::ClientNetworkManager::ClientNetworkManager() {

	}
	client::ClientNetworkManager::~ClientNetworkManager() {

	}

}
