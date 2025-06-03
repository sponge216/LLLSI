#pragma once

#ifndef APP_HOST_H
#define APP_HOST_H
#include "framework.h"


namespace host {
#define PACKET_SIZE 4096
#define RECENT_PACKETS_SIZE 200
#define IP_ADDRESS "127.0.0.1"
#define CLIENT_IP_ADDRESS "127.0.0.1"
#define HOST_PORT 36542
#define CLIENT_PORT 36543

#include "IHostComponent.h"
#include "stream.h"
#include <mutex>

	using namespace Gdiplus;
	using namespace video;
	extern sockaddr_in clientAddr; // Client's IP address

	using clientHandlerFunc = void(*)(rtmp_packet);
	using TypeHandlerPair = std::pair< CHAR, clientHandlerFunc>;
	using packetHandlingMap = std::unordered_map<CHAR, clientHandlerFunc>;
	using millisecond = std::chrono::milliseconds;

	struct packet_t {
		rtmp_packet header;
		BYTE* data;
	};

	class PacketManager : public IHostComponent {
	public:
		PacketManager();

		~PacketManager();

		virtual void sendPacket(rtmp_packet packetHeader, const  BYTE* pPacketData, SIZE_T dataSize, bool savePacket);

		virtual void addRecentPacket(DWORD seqNum, std::vector<CHAR>& packet);

		virtual void removeRecentPacket(DWORD seqNum);

		template<typename StreamData, typename StreamContainer>
		void sendStream(stream::VectorStream<StreamData, StreamContainer>& stream) {
			DWORD frameNum = 0;
			
			const CHAR headerSize = sizeof(rtmp_packet);

			while (IHostComponent::isProgramOn()) {
				WORD chunkSize = stream.getFrameSize();
				const WORD payloadSize = chunkSize - sizeof(rtmp_packet);

				auto frame = stream.frontFrame();
				auto& frameData = frame.getData();

				SIZE_T frameSize = frame.getSize();
				WORD totalPackets = (WORD)((frameSize + payloadSize - 1) / payloadSize);

				const auto data = frame.getBytesDataPtr();
				DWORD totalFrameSize = frameSize;

				std::cout << "Total packets for frame " << frameNum << " is " << totalPackets << " Frame size is " << totalFrameSize << std::endl;

				for (WORD i = 0; i < totalPackets; i++) {
					SIZE_T dataOffset = (SIZE_T)(i * payloadSize);
					SIZE_T dataSize = std::min<SIZE_T>(payloadSize, totalFrameSize - dataOffset);
					SIZE_T totalPacketSize = dataSize + headerSize;

					rtmp_packet pkt = { 0 };
					pkt.version = 1;
					pkt.type = PACKET_TYPE_STREAM;
					pkt.size = (WORD)totalPacketSize;
					pkt.rtmp_data.rtsmp.streamType = stream.getStreamType();
					pkt.rtmp_data.rtsmp.frameNumber = frameNum;
					pkt.rtmp_data.rtsmp.offset = i;
					pkt.slidingWindow = totalFrameSize;
					pkt.rtmp_data.rtsmp.totalPackets = totalPackets;
					pkt.sequenceNumber = this->sequenceNumber++;
					pkt.flags = (i == totalPackets - 1) ? FLAG_LAST_PACKET : 0;

					this->sendPacket(pkt, data + dataOffset, dataSize, true);

					this->ackFlag = false;
					//while (!ackFlag);
				}
				frameNum++;
			};
		};

		virtual void handleIncomingPackets(packetHandlingMap& funcMap);

		virtual void ackPackets();

	private:
		SOCKET hostSock;
		std::unordered_map<DWORD, std::vector<CHAR>> recentPackets;
		std::mutex recentPacketsMutex; // Muted to protect recentPackets
		ThreadSafeQueue<std::vector<CHAR>> packetsQueue;
		std::atomic<bool> ackFlag{ true };
		std::atomic<DWORD> sequenceNumber;
		millisecond ackSleep;
	};
};
#endif // APP_HOST_H