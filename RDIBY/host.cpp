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
	DWORD host::HostSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD flags) {
		return 1;
	}
	DWORD host::HostSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return 1;

	}
	// --------------------------------------- //

	host::HostNetworkManager::HostNetworkManager() {

	}
	host::HostNetworkManager::~HostNetworkManager() {
	}
}