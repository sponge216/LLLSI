// events.h

#ifndef APP_EVENTS_H
#define APP_EVENTS_H


class MouseEventManager {

public:
	MouseEventManager();
};

class KeyboardEventManager {

public:
	KeyboardEventManager();
};

class EventManager {
public:
	EventManager();
private:
	MouseEventManager mouseEventManager;
	KeyboardEventManager keyboardEventManager;
};

#endif //APP_EVENTS_H