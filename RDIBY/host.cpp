#include "host.h"

namespace host {
	host::HostSocket::HostSocket() {
		this->initTCP();
	}
	bool host::HostSocket::initTCP() {
		if (this->pAddrInfo != NULL) {
			this->hints = { 0 };
			this->pAddrInfo = NULL;
		}
		return true;
	}
	bool host::HostSocket::initUDP() {
		return true;
	}
	host::HostSocket::~HostSocket() {

	}
	bool host::HostSocket::send(CHAR* data, SIZE_T dwTypeSize) {
		return true;
	}
	bool host::HostSocket::recv() {
		return true;
	}
	// --------------------------------------- //

	host::HostNetworkManager::HostNetworkManager() {

	}
	host::HostNetworkManager::~HostNetworkManager() {
	}
}