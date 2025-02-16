#include "client.h"

namespace client {
	client::ClientSocket::ClientSocket() {
		this->initTCP();
	}
	bool client::ClientSocket::initTCP() {
		if (this->pAddrInfo != NULL) {
			this->hints = { 0 };
			this->pAddrInfo = NULL;
		}
		return true;
	}
	bool client::ClientSocket::initUDP() {
		return true;
	}
	client::ClientSocket::~ClientSocket() {

	}
	DWORD client::ClientSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD flags) {
		return 1;
	}
	DWORD client::ClientSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return 1;
	 }

	// --------------------------------------- //

	client::ClientNetworkManager::ClientNetworkManager() {

	}
	client::ClientNetworkManager::~ClientNetworkManager() {

	}

}
