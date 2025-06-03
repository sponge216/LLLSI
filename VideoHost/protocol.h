// protocol.h
#pragma once
#include <windows.h>

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
	union {
		keyboard_data_t kbdData;
		mouse_data_t msData;
	} rtemp_data;
} rtemp_packet, * prtemp_packet;

typedef struct rtmp_packet_t {
	CHAR version;
	CHAR type; // 0 = event, 1 = stream, 2 = control
	WORD size;
	DWORD slidingWindow;
	DWORD sequenceNumber;
	CHAR flags;
	union {
		rtemp_packet rtemp;
		rtsmp_packet rtsmp;
	} rtmp_data;
} rtmp_packet, * prtmp_packet;

#define PACKET_TYPE_EVENT 0
#define PACKET_TYPE_STREAM 1
#define PACKET_TYPE_CONTROL 2

#define STREAM_TYPE_JPEG 0
#define FLAG_LAST_PACKET 0x01
#define FLAG_ACK 0x02
