#include "client.h"

namespace client {
	client::ClientSocket::ClientSocket() {
		this->initTCP();
	}
	bool client::ClientSocket::initTCP() {
		if (this->addrInfo != NULL) {
			this->hints = { 0 };
			this->addrInfo = NULL;
		}
		return true;
	}
	bool client::ClientSocket::initUDP() {
		return true;
	}
	client::ClientSocket::~ClientSocket() {

	}

	// --------------------------------------- //

	client::ClientNetworkManager::ClientNetworkManager() {

	}
	client::ClientNetworkManager::~ClientNetworkManager() {

	}

}
