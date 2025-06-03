#include "client.h"

//TODO: CHANGE ALL FUNCTIONS TO RETURN ERROR CODES!!

namespace client {
	client::Client::Client() {

	}
	client::Client::~Client() {

	}

	client::ClientSocket::ClientSocket() {
		this->sock = -1;
	}
	bool client::ClientSocket::initTCP(PCSTR clientAddr, USHORT clientPort) {
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
		inet_pton(AF_INET, clientAddr, &clientService.sin_addr.s_addr);
		clientService.sin_port = htons(clientPort);

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
	bool client::ClientSocket::initUDP(USHORT clientPort) {
		sockaddr_in cAddr = { 0 };
		cAddr.sin_family = AF_INET;
		cAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		cAddr.sin_port = htons(clientPort);

		if ((this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= 1) {
			perror("SOCKET FAILED");
			exit(1);
		}
		if (this->sock == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			WSACleanup();
			exit(1);
		}
		if (bind(this->sock, (sockaddr*)&cAddr, sizeof(cAddr)) == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			closesocket(this->sock);
			WSACleanup();
			exit(1);
		}
		listen(this->sock, 10);
		printf("UDP client socket all good!\n");
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

	bool client::EncryptedClientSocket::initTCP(PCSTR pAddrStr, USHORT port) {
		return ClientSocket::initTCP(pAddrStr, port);
	}

	bool client::EncryptedClientSocket::initUDP( USHORT port) {
		return ClientSocket::initUDP(port);
	}

	DWORD client::EncryptedClientSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) {
		return	ClientSocket::sendData(sock, pData, dwTypeSize, dwLen, flags);
	}
	DWORD client::EncryptedClientSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return ClientSocket::recvData(sock, pBuffer, dwBufferLen, flags);
	}

	DWORD client::EncryptedClientSocket::firstServerInteraction(Client client) {
		if (client.name.size() > 16 ||
			client.roomName.size() > 16 ||
			client.roomPassword.size() > 16)
			return -1;

		first_server_interaction_t fsi = { 0 };
		DWORD res = this->recvData(this->sock, (CHAR*)&fsi, sizeof(fsi), 0);
		std::cout << "First interaction going well!\n";

		for (int i = 0; i < client.name.size(); i++)
			fsi.clientName[i] = client.name[i];
		fsi.clientNameLength = client.name.size();

		for (int i = 0; i < client.roomName.size(); i++)
			fsi.roomName[i] = client.roomName[i];
		fsi.roomNameLength = client.roomName.size();

		for (int i = 0; i < client.roomPassword.size(); i++)
			fsi.roomPassword[i] = client.roomPassword[i];
		fsi.roomPasswordLength = client.roomPassword.size();

		fsi.isHost = client.isHost;
		return this->sendData(this->sock, (CHAR*)&fsi, sizeof(fsi), 1, 0);
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
