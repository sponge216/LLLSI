#include "client.h"

//TODO: CHANGE ALL FUNCTIONS TO RETURN ERROR CODES!!

namespace client {
	using namespace client;
	client::Client::Client() {
		this->isHost = false;
	}
	client::Client::~Client() {

	}

	client::ClientSocket::ClientSocket() {
		this->sock = -1;
	}
	bool client::ClientSocket::initTCP(PCSTR serverIPAddr, USHORT serverPort) {
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

		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		inet_pton(AF_INET, serverIPAddr, &serverAddr.sin_addr.s_addr);
		serverAddr.sin_port = htons(serverPort);

		//----------------------
		// Connect to server.
		iResult = connect(this->sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
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
		closesocket(this->sock);
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

	DWORD client::ClientSocket::sendDataTo(CHAR* pBuffer, DWORD dwBufferLen, Addr& addr) {
		return sendto(this->sock, pBuffer, dwBufferLen, 0, (sockaddr*)&addr.addr, addr.length);
	}
	DWORD client::ClientSocket::recvDataFrom(CHAR* pBuffer, DWORD dwBufferLen, Addr& addr) {
		return recvfrom(this->sock, pBuffer, dwBufferLen, 0, (sockaddr*)&addr.addr, &addr.length);
	}

	// --------------------------------------- //
	//TODO: PUT CODE IN ALL OF THESE!

	client::EncryptedClientSocket::EncryptedClientSocket() {

	}

	client::EncryptedClientSocket::~EncryptedClientSocket() {

	}

	bool client::EncryptedClientSocket::initTCP(PCSTR serverIPAddr, USHORT serverPort) {
		return ClientSocket::initTCP(serverIPAddr, serverPort);
	}

	bool client::EncryptedClientSocket::initUDP(USHORT port) {
		return ClientSocket::initUDP(port);
	}

	DWORD client::EncryptedClientSocket::sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) {
		return	ClientSocket::sendData(sock, pData, dwTypeSize, dwLen, flags);
	}
	DWORD client::EncryptedClientSocket::recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) {
		return ClientSocket::recvData(sock, pBuffer, dwBufferLen, flags);
	}
	DWORD client::EncryptedClientSocket::sendDataTo(CHAR* pBuffer, DWORD dwBufferLen, Addr& addr) {
		return ClientSocket::sendDataTo(pBuffer, dwBufferLen, addr);
	}
	DWORD client::EncryptedClientSocket::recvDataFrom(CHAR* pBuffer, DWORD dwBufferLen, Addr& addr) {
		return ClientSocket::recvDataFrom(pBuffer, dwBufferLen, addr);
	}

	DWORD client::EncryptedClientSocket::firstServerInteraction(Client client) {
		if (client.name.size() > 16 ||
			client.roomName.size() > 16 ||
			client.roomPassword.size() > 16)
			return -1;

		first_server_client_interaction fsi = { 0 };
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

	PacketManager::PacketManager() {
	};

	PacketManager::~PacketManager() {
	};

	void PacketManager::sendPacket(rtmp_packet packetHeader, const BYTE* pPacketData, SIZE_T dataSize) {
		SIZE_T totalPacketSize = sizeof(packetHeader) + dataSize;
		std::vector<CHAR> packet(totalPacketSize);

		::memcpy_s(packet.data(), totalPacketSize, &packetHeader, sizeof(packetHeader));
		if (dataSize > 0)
			::memcpy_s(packet.data() + sizeof(packetHeader), dataSize, pPacketData, dataSize);

		this->clientSock.sendDataTo(packet.data(), totalPacketSize, this->hostAddr);
	};


	void PacketManager::handleIncomingPackets(packetHandlingMap& funcMap) {


		rtmp_packet pkt = { 0 };

		while (IClientComponent::isProgramOn()) {
			if (this->clientSock.recvDataFrom((char*)&pkt, sizeof(pkt), this->hostAddr) < 0)
				continue;

			funcMap[pkt.type](pkt);
		}
	};

	void PacketManager::ackPacket(DWORD seqNum) {
		rtmp_packet ackPkt = { 0 };
		ackPkt.type = PACKET_TYPE_CONTROL;
		ackPkt.sequenceNumber = seqNum;
		this->clientSock.sendDataTo((char*)&ackPkt, sizeof(ackPkt), this->hostAddr);
	}


	client::FramePacketsStream::FramePacketsStream() {
		this->continueStream();
	}

	client::FramePacketsStream::~FramePacketsStream() {

	}
	void client::FramePacketsStream::initStream() {};

	void client::FramePacketsStream::initStream(PacketManager& packetManager) {
		rtmp_packet_t pkt = { 0 };
		int len = sizeof(packetManager.hostAddr);
		Addr recvAddr = Addr();

		int packetSize = 0;
		while (IClientComponent::isProgramOn() && this->isStreamOn()) {
			CHAR* packet = new CHAR[this->getFrameSize()];
			if ((packetSize = packetManager.clientSock.recvDataFrom(packet, this->getFrameSize(), recvAddr)) < 0) {
				delete[] packet;
				continue;
			}
			memcpy_s(&pkt, sizeof(pkt), packet, sizeof(pkt));

			packetManager.ackPacket(pkt.sequenceNumber);
			auto item = FramePacketStreamData(packet, packetSize);

			this->streamContainer.push(item);
		}
	}

	// client Stream Data
	client::FramePacketStreamData::FramePacketStreamData(CHAR* data) { this->data = data; };
	client::FramePacketStreamData::FramePacketStreamData(CHAR* data, SIZE_T size) { this->data = data; this->size = size; };
	client::FramePacketStreamData::~FramePacketStreamData() {};
	CHAR* const& client::FramePacketStreamData::getData() { return this->data; };
	const BYTE* client::FramePacketStreamData::getBytesDataPtr() { return (BYTE*)this->data; };
	SIZE_T client::FramePacketStreamData::getSize() { return this->size; }

	// client Stream Container
	client::FramePacketsStreamContainer::FramePacketsStreamContainer() {};
	client::FramePacketsStreamContainer::~FramePacketsStreamContainer() {};
	client::FramePacketStreamData client::FramePacketsStreamContainer::pop() { return this->tsFrameQueue.Pop(); };
	client::FramePacketStreamData client::FramePacketsStreamContainer::popFront() { return this->tsFrameQueue.Pop(); };
	client::FramePacketStreamData client::FramePacketsStreamContainer::popBack() { return this->tsFrameQueue.Pop(); };
	void client::FramePacketsStreamContainer::push(FramePacketStreamData data) { this->tsFrameQueue.Push(data); };
	void client::FramePacketsStreamContainer::pushFront(FramePacketStreamData data) { this->tsFrameQueue.Push(data); };
	void client::FramePacketsStreamContainer::pushBack(FramePacketStreamData data) { this->tsFrameQueue.Push(data); };
	bool client::FramePacketsStreamContainer::isEmpty() { return this->tsFrameQueue.isEmpty(); };
	void client::FramePacketsStreamContainer::clear() { this->tsFrameQueue.Clear(); };

	// --------------------------------------------------

	void client::FrameBufferStream::initStream(FramePacketsStream& packetStream/*stream::VectorStream<DWORD>& constructedFramesStream*/) {
		rtmp_packet pkt = { 0 };
		WORD sizeOfRTMP = sizeof(rtmp_packet);
		WORD packetSize = 0;
		while (isProgramOn()) {
			while (!this->isStreamOn() && !packetStream.isStreamOn());// loop so it doesnt send when the receiver is overwhelmed

			FramePacketStreamData packet = packetStream.frontFrame();
			memcpy_s(&pkt, sizeof(pkt), packet.getBytesDataPtr(), sizeof(pkt)); // extract packet header from buffer

			if (pkt.type != PACKET_TYPE_STREAM) continue;

			packetSize = pkt.size;
			rtsmp_packet& rts = pkt.rtmp_data.rtsmp;
			DWORD frameNum = rts.frameNumber;

			auto& frameData = this->streamContainer.get(frameNum);
			auto& fb = frameData.getData();
			if (fb.chunks.empty()) {
				fb.expectedSize = pkt.slidingWindow; // size of current frame
				fb.frameNumber = frameNum;
				fb.chunks.resize(fb.expectedSize);
			}

			fb.totalPackets++;
			fb.totalSize += packetSize - sizeOfRTMP;

			memcpy_s(fb.chunks.data() + rts.offset * packetStream.getFrameSize(),
				size_t(packetSize - sizeOfRTMP),
				packet.getBytesDataPtr() + sizeOfRTMP,
				size_t(packetSize - sizeOfRTMP));
			delete[] packet.getBytesDataPtr();

			if (fb.totalSize == fb.expectedSize) {
				//frameNumQueue.Push(frameNum);
			}

		}
	}

	// client Stream Data
	client::FrameBufferStreamData::FrameBufferStreamData() { };
	client::FrameBufferStreamData::FrameBufferStreamData(FrameBuffer data) { this->data = data; };
	client::FrameBufferStreamData::~FrameBufferStreamData() {};
	client::FrameBuffer& client::FrameBufferStreamData::getData() { return this->data; };
	const BYTE* client::FrameBufferStreamData::getBytesDataPtr() { return this->data.chunks.data(); };
	SIZE_T client::FrameBufferStreamData::getSize() { return this->data.chunks.size(); }

	// client Stream Container
	client::FrameBufferStreamContainer::FrameBufferStreamContainer() {};
	client::FrameBufferStreamContainer::~FrameBufferStreamContainer() {};
	FrameBufferStreamData& client::FrameBufferStreamContainer::get(DWORD key) { return this->activeFrames[key]; };
	void client::FrameBufferStreamContainer::push(DWORD key, FrameBufferStreamData value) { this->activeFrames.emplace(key, value); };
	bool client::FrameBufferStreamContainer::isEmpty() { return this->activeFrames.empty(); };
	void client::FrameBufferStreamContainer::clear() { this->activeFrames.clear(); };
	void client::FrameBufferStreamContainer::remove(DWORD key) { this->activeFrames.erase(key); };
};

