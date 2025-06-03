#define _CRT_SECURE_NO_WARNINGS 

#include "framework.h"
#include <set>
#include <mutex>
#include "datastructures.h"
#include "video.h"
#include <chrono>
#include "host.h"

using namespace Gdiplus;
using namespace host;

// threads
void senderThread();
void receiverThread();
void ackThread();

// functions
void convertPacketToINPUT(rtemp_packet packet, INPUT* input);
void controlPacketHandler(rtmp_packet packet);
void eventPacketHandler(rtmp_packet packet);

// Nameless namespace that initializes gdiplus
namespace {
	struct GdiPlusBootstrap {
		GdiPlusBootstrap() {
			video::InitGdiplus();
			if (gdiRes != Gdiplus::Ok) {
				std::cerr << "GDI+ failed to start: " << gdiRes << "\n";
				std::terminate();
			}
		}
		~GdiPlusBootstrap() {
			Gdiplus::GdiplusShutdown(video::gdiToken);
		}
	};
	// **Define it before** any other globals in this file:
	static GdiPlusBootstrap _gdiBootstrap;
};

//global vars
VideoStream screenStream;
PacketManager packetManager;

int main() {
	std::thread receiver(receiverThread);
	std::thread acker(ackThread);
	std::thread sender(senderThread);

	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(CLIENT_PORT);
	inet_pton(AF_INET, CLIENT_IP_ADDRESS, &clientAddr.sin_addr);
	int clientAddrLen = 0;

	screenStream.setStreamType(PACKET_TYPE_STREAM);
	screenStream.setFrameSize(PACKET_SIZE);
	screenStream.initStream();

	IHostComponent::killProgram();

	receiver.join();
	acker.join();
	sender.join();
	Gdiplus::GdiplusShutdown(video::gdiToken);
	return 0;
}

static void senderThread() {
	packetManager.sendStream<VideoStreamData, VideoStreamContainer>(screenStream);
}

static void receiverThread() {
	packetHandlingMap handlerMap = packetHandlingMap();
	handlerMap.insert(TypeHandlerPair(PACKET_TYPE_CONTROL, controlPacketHandler));
	handlerMap.insert(TypeHandlerPair(PACKET_TYPE_EVENT, eventPacketHandler));

	packetManager.handleIncomingPackets(handlerMap);
}

// ACK Thread - periodically wakes up and resends packets that haven't been ACK'ed
static void ackThread() {
	packetManager.ackPackets();
}

// Formats an EVENT packet into an INPUT struct
static void convertPacketToINPUT(rtemp_packet packet, INPUT* input)
{
	KEYBDINPUT* pKi = &input->ki;
	MOUSEINPUT* pMi = &input->mi;
	BYTE i = 0;

	input->type = packet.eventType;
	if (input->type == INPUT_MOUSE) {
		//MOUSE
		pMi->time = 0;
		pMi->dx = packet.rtemp_data.msData.dx;
		pMi->dy = packet.rtemp_data.msData.dy;
		pMi->dwFlags = packet.rtemp_data.msData.mouse_flags;
		pMi->mouseData = packet.rtemp_data.msData.mouse_data;

	}
	else if (input->type == INPUT_KEYBOARD) {
		//KEYBOARD
		pKi->time = 0;
		pKi->wVk = packet.rtemp_data.kbdData.vkCode;
		pKi->dwFlags = packet.rtemp_data.kbdData.keyboard_flags;
		pKi->wScan = MapVirtualKeyA(pKi->wVk, MAPVK_VK_TO_VSC);

	}
}

static void controlPacketHandler(rtmp_packet packet) {
	std::cout << "acking packet: " << packet.sequenceNumber << std::endl;
	packetManager.removeRecentPacket(packet.sequenceNumber);
}

static void eventPacketHandler(rtmp_packet packet) {
	/*INPUT input[1] = { 0 };
	convertPacketToINPUT(packet.rtmp_data.rtemp, input);
	UINT valread = SendInput(1, input, sizeof(INPUT));*/
}