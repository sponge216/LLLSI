#include "server.h"

namespace server {
	server::ServerSocket::ServerSocket() {
		this->initTCP();
	}
	bool server::ServerSocket::initTCP() {
		if (this->addrInfo != NULL) {
			this->hints = { 0 };
			this->addrInfo = NULL;
		}
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