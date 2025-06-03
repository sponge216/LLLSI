// protocols.h

#ifndef APP_PROTOCOLS_H
#define APP_PROTOCOLS_H

#include <ws2def.h>
namespace protocols {
	constexpr auto PACKET_TYPE_EVENT = 0;
	constexpr auto PACKET_TYPE_STREAM = 1;
	constexpr auto PACKET_TYPE_CONTROL = 2;

	constexpr auto STREAM_TYPE_JPEG = 0;
	constexpr auto FLAG_LAST_PACKET = 0x01;
	constexpr auto FLAG_ACK = 0x02;

	typedef struct mouse_data_t {
		LONG dx; // delta x
		LONG dy; // delta y
		DWORD mouse_flags;
		DWORD mouse_data;
	}mouse_data, * pmouse_data;

	//struct for keyboard press/release events.
	typedef struct keyboard_data_t {
		DWORD vkCode;
		DWORD keyboard_flags;
	}keyboard_data, * pkeyboard_data;

	typedef struct rtsmp_packet_t {
		CHAR streamType;
		DWORD frameNumber;
		WORD offset;
		WORD totalPackets;

	} rtsmp_packet, * prtsmp_packet;

	typedef struct rtemp_packet_t {
		CHAR eventType;
		union rtemp_data {
			keyboard_data_t kbdData;
			mouse_data_t msData;
		} rtemp_data;
	} rtemp_packet, * prtemp_packet;

	typedef struct rtmp_packet_t {
		CHAR version;
		CHAR type; // 0 = event, 1 = stream
		WORD size;
		DWORD slidingWindow;
		DWORD sequenceNumber;
		CHAR flags;
		union rtmp_data {
			rtemp_packet rtemp;
			rtsmp_packet rtsmp;
		} rtmp_data;
	} rtmp_packet, * prtmp_packet;

	typedef union server_msg_data {
		sockaddr_in sockAddr4;
		CHAR name[16];
	}server_msg_data_t;
	/// <summary>
	/// msgType - the type of message.
	/// </summary>
	typedef struct server_room_msg {
		UCHAR msgType;
		server_msg_data_t msgData;
	} server_room_msg_t;

	typedef struct first_server_client_interaction_t {
		CHAR roomName[16];
		CHAR clientName[16];
		CHAR roomPassword[16];
		BYTE clientNameLength;
		BYTE roomNameLength;
		BYTE roomPasswordLength;
		bool isHost;
	}first_server_client_interaction, * pfirst_server_client_interaction;
}
#endif //APP_PROTOCOLS_H