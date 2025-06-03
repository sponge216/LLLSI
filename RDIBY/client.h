#pragma once

// client.h

#ifndef APP_NETWORK_CLIENT_H
#define APP_NETWORK_CLIENT_H

#include "network.h"
#include <string>
#include <iostream>
#include "concurrency.h"
#include "IClientComponent.h"
#include "stream.h"
#include "datastructures.h"
#include <chrono>
#include <thread>

namespace client {
	using namespace network;
	using namespace protocols;
	class ClientSocket;
	class EncryptedClientSocket;
	class ClientNetworkManager;
	class Client;

	constexpr auto SERVER_PORT = 36542;
	constexpr auto SERVER_IP = "89.139.17.30";
	constexpr auto SERVER_IP_X = "172.20.10.7";

	class Client {
	public:
		Client();
		~Client();
		std::string name;
		std::string roomName;
		std::string roomPassword;
		bool isHost;
	};

	/// <summary>
	/// socket for clients.
	/// </summary>
	class ClientSocket : public network::BaseSocket
	{
	public:
		ClientSocket();
		~ClientSocket();
		virtual bool initTCP(PCSTR serverIPAddr, USHORT serverPort);
		virtual bool initUDP(USHORT clientPort);
		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
		DWORD sendDataTo(CHAR* pBuffer, DWORD dwBufferLen, Addr& addr);
		DWORD recvDataFrom(CHAR* pBuffer, DWORD dwBufferLen, Addr& addr);
	};

	class EncryptedClientSocket : public ClientSocket
	{
	public:
		EncryptedClientSocket();
		~EncryptedClientSocket();

		bool initTCP(PCSTR serverIPAddr, USHORT serverPort) override;
		bool initUDP(USHORT port) override;

		DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags) override;
		DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags) override;
		DWORD sendDataTo(CHAR* pBuffer, DWORD dwBufferLen, Addr& addr);
		DWORD recvDataFrom(CHAR* pBuffer, DWORD dwBufferLen, Addr& addr);
		DWORD firstServerInteraction(Client client);
		DWORD firstHostInteraction();

	};

	/// <summary>
	/// 
	/// </summary>
	class ClientNetworkManager {

	public:
		ClientNetworkManager();
		~ClientNetworkManager();

	private:
		EncryptedClientSocket clientSocket;
		concurrency::ThreadManager threadManager;

	};
	using clientHandlerFunc = void(*)(rtmp_packet);
	using TypeHandlerPair = std::pair< CHAR, clientHandlerFunc>;
	using packetHandlingMap = std::unordered_map<CHAR, clientHandlerFunc>;
	using millisecond = std::chrono::milliseconds;

	class PacketManager : public IClientComponent {
	public:
		PacketManager();

		~PacketManager();

		virtual void sendPacket(rtmp_packet packetHeader, const  BYTE* pPacketData, SIZE_T dataSize);

		virtual void handleIncomingPackets(packetHandlingMap& funcMap);

		virtual void ackPacket(DWORD seqNum);
		Addr hostAddr;
		EncryptedClientSocket clientSock;

	private:
		std::atomic<DWORD> sequenceNumber;
	};

	// ---------------------
	class FramePacketStreamData : public stream::StreamData<CHAR*> {
	public:
		FramePacketStreamData(CHAR* data);
		FramePacketStreamData(CHAR* data, SIZE_T size);
		~FramePacketStreamData();
		CHAR* const& getData() override;
		const BYTE* getBytesDataPtr() override;
		SIZE_T getSize() override;
	private:
		SIZE_T size = -1;
	};
	class FramePacketsStreamContainer : public stream::StreamVectorContainer<FramePacketStreamData> {
	public:
		FramePacketsStreamContainer();
		~FramePacketsStreamContainer();

		FramePacketStreamData pop() override;
		FramePacketStreamData popFront() override;
		FramePacketStreamData popBack() override;
		void push(FramePacketStreamData data) override;
		void pushFront(FramePacketStreamData data) override;
		void pushBack(FramePacketStreamData data) override;
		bool isEmpty() override;
		void clear() override;
	private:
		ThreadSafeQueue<FramePacketStreamData> tsFrameQueue;
	};

	class FramePacketsStream : public stream::VectorStream<FramePacketStreamData, FramePacketsStreamContainer>, public IClientComponent {
	public:
		FramePacketsStream();
		~FramePacketsStream();

		void initStream() override;
		void initStream(PacketManager& packetManager);

	};

	// -------------------------------
	struct FrameBuffer {
		std::vector<BYTE> chunks;
		WORD totalChunks = 0;
		DWORD totalSize = 0;
		DWORD expectedSize = 0;
		DWORD frameNumber = 0;
		DWORD totalPackets = 0;
	};

	class FrameBufferStreamData : public stream::StreamData<FrameBuffer> {
	public:
		FrameBufferStreamData();
		FrameBufferStreamData(FrameBuffer data);
		~FrameBufferStreamData();
		FrameBuffer& getData() override;
		const BYTE* getBytesDataPtr() override;
		SIZE_T getSize() override;
	};
	class FrameBufferStreamContainer : public stream::StreamMapContainer<DWORD, FrameBufferStreamData> {
	public:
		FrameBufferStreamContainer();
		~FrameBufferStreamContainer();

		void remove(DWORD key) override;
		void push(DWORD key, FrameBufferStreamData value) override;
		FrameBufferStreamData& get(DWORD key) override;
		bool isEmpty() override;
		void clear() override;
	private:
		std::unordered_map<DWORD, FrameBufferStreamData> activeFrames;
	};

	class FrameBufferStream : public stream::MapStream<DWORD, FrameBufferStreamData, FrameBufferStreamContainer>, public IClientComponent {
	public:
		FrameBufferStream();
		~FrameBufferStream();

		void initStream() override;
		void initStream(FramePacketsStream& packetStream);
	private:
	};



}
#endif // APP_NETWORK_CLIENT_H