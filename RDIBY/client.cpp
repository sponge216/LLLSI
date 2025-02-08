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
	bool client::ClientSocket::send(CHAR* data, SIZE_T dwTypeSize) {
		return true;
	}
	bool client::ClientSocket::recv() {
		return true;
	}

	// --------------------------------------- //

	client::ClientNetworkManager::ClientNetworkManager() {

	}
	client::ClientNetworkManager::~ClientNetworkManager() {

	}

}
