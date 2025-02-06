// video.h

#ifndef APP_VIDEO_H
#define APP_VIDEO_H

class VideoCapture {
public:
	VideoCapture();
	bool getCurrentFrame();

};

class VideoCodec {
public:
	VideoCodec();
	bool setCodec();
	bool encodeFrame();
	bool decodeFrame();
};

class VideoManager {
public:
	VideoCapture videoCapture;
	VideoCodec videoCodec;

	VideoManager();
};

#endif //APP_VIDEO_H