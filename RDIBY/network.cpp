#include "network.h"

namespace network {
	network::ClientSocket::ClientSocket() {
		this->initialize();
	}
	bool network::ClientSocket::initialize() {
		if (this->addrInfo != NULL) {
			this->hints = { 0 };
			this->addrInfo = NULL;
		}
		return true;
	}
}
