// audio.h

#ifndef APP_AUDIO_H
#define APP_AUDIO_H


class AudioCapture {
public:
	AudioCapture();
	bool getCurrentFrame();

};

class AudioCodec {
public:
	AudioCodec();
	bool setCodec();
	bool encodeFrame();
	bool decodeFrame();
};

class AudioManager {
public:
	AudioCapture AudioCapture;
	AudioCodec AudioCodec;

	AudioManager();
};
#endif //APP_AUDIO_H