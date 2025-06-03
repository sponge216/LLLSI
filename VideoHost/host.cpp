#include "host.h"

namespace host {
	sockaddr_in clientAddr = { 0 };

	PacketManager::PacketManager() {
		// Setup networking
		WSADATA wsa;
		WSAStartup(MAKEWORD(2, 2), &wsa);
		SOCKET hostSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port of the server to be connected to.

		sockaddr_in hostAddr;
		hostAddr.sin_family = AF_INET;
		hostAddr.sin_port = htons(HOST_PORT);
		hostAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(hostSock, (sockaddr*)&hostAddr, sizeof(hostAddr)) == SOCKET_ERROR) {
			std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
			closesocket(hostSock);
			WSACleanup();
			return;
		}
		listen(hostSock, 10);
		this->hostSock = hostSock;
		this->sequenceNumber = 0;
		this->recentPackets.reserve(RECENT_PACKETS_SIZE);
		this->ackSleep = millisecond(100);
	};

	PacketManager::~PacketManager() {
		closesocket(this->hostSock);
		WSACleanup();
	};

	void PacketManager::sendPacket(rtmp_packet packetHeader, const BYTE* pPacketData, SIZE_T dataSize, bool savePacket) {
		SIZE_T totalPacketSize = sizeof(packetHeader) + dataSize;
		std::vector<CHAR> packet(totalPacketSize);

		::memcpy_s(packet.data(), totalPacketSize, &packetHeader, sizeof(packetHeader));
		if (dataSize > 0)
			::memcpy_s(packet.data() + sizeof(packetHeader), dataSize, pPacketData, dataSize);

		sendto(this->hostSock, packet.data(), packet.size(), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr)); // send
		if (savePacket)
			this->addRecentPacket(packetHeader.sequenceNumber, packet);
	};

	void PacketManager::addRecentPacket(DWORD seqNum, std::vector<CHAR>& packet) {
		std::lock_guard<std::mutex> lck(this->recentPacketsMutex);
		this->recentPackets.emplace(std::make_pair(seqNum, std::move(packet)));
	};

	void PacketManager::removeRecentPacket(DWORD seqNum) {
		std::lock_guard<std::mutex> lck(this->recentPacketsMutex);
		this->recentPackets.erase(seqNum);
	};

	void PacketManager::handleIncomingPackets(packetHandlingMap& funcMap) {
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = htons(CLIENT_PORT);
		inet_pton(AF_INET, IP_ADDRESS, &clientAddr.sin_addr);
		int clientAddrLen = sizeof(clientAddr);

		rtmp_packet pkt = { 0 };

		while (IHostComponent::isProgramOn()) {
			if (recvfrom(hostSock, (char*)&pkt, sizeof(pkt), 0, (SOCKADDR*)&clientAddr, &clientAddrLen) < 0)
				continue;

			funcMap[pkt.type](pkt);
		}
	};

	void PacketManager::ackPackets() {
		int clientAddrLen = sizeof(clientAddr);

		while (IHostComponent::isProgramOn()) {
			std::this_thread::sleep_for(this->ackSleep);
			{
				std::lock_guard<std::mutex> lock(this->recentPacketsMutex);
				if (this->recentPackets.empty())
					continue;

				for (auto& pair : this->recentPackets) {
					//std::cout << "Reattempting ack " << sequenceNumber << std::endl;
					sendto(this->hostSock, pair.second.data(), pair.second.size(), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
					this->ackFlag = true;
				}
				if (this->recentPackets.size() > RECENT_PACKETS_SIZE) {
					std::cout << "CLEARING PACKETS" << std::endl;
					this->recentPackets.clear();
				}
			}
		}
	};
};