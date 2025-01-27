#include "host.h"

namespace host {
	host::HostSocket::HostSocket() {
		this->initTCP();
	}
	bool host::HostSocket::initTCP() {
		if (this->addrInfo != NULL) {
			this->hints = { 0 };
			this->addrInfo = NULL;
		}
		return true;
	}
	bool host::HostSocket::initUDP() {
		return true;
	}
	host::HostSocket::~HostSocket() {

	}
	// --------------------------------------- //

	host::HostNetworkManager::HostNetworkManager() {

	}
	host::HostNetworkManager::~HostNetworkManager() {
	}
}