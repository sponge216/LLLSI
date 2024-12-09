// protocols.h

#ifndef APP_PROTOCOLS_H
#define APP_PROTOCOLS_H

#include <Windows.h>


class RTMPacket {
public:
	RTMPacket() {


	}
private:
	CHAR version;
	CHAR type;
	WORD length;
	LPVOID data;
};

class RTEMPacket : public RTMPacket {
public:
	RTEMPacket() {

	}
private:
	CHAR eventType;
};

class RTSMPacket : public RTMPacket {
public:
	RTSMPacket() {

	}
private:
	CHAR streamType;
	CHAR offset;
	WORD slidingWindow;
	DWORD sequenceNumber;
	CHAR flags;
};
#endif //APP_PROTOCOLS_H