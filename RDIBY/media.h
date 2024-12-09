#ifndef APP_MEDIA_H
#define APP_MEDIA_H

#include "audio.h"
#include "video.h"


class MediaManager {
public:
	MediaManager();

private:
	AudioManager audioManager;
	VideoManager videoManager;
};
#endif //APP_MEDIA_H