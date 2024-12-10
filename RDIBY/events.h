// events.h

#ifndef APP_EVENTS_H
#define APP_EVENTS_H

#include <Windows.h>

class MouseEventManager {

public:
	MouseEventManager();
	~MouseEventManager();

	/// <summary>
	/// Use this if you're using the default application provided by this infrastructure.
	/// </summary>	
	void InitiateDefaultMouseHook();
	void SetMouseHook(LRESULT(*HookFunction)(int nCode, WPARAM wParam, LPARAM lParam));

private:
	HHOOK mouseHook;
	MSLLHOOKSTRUCT mouseStruct; // mouse struct to hold info given from hook

	static LRESULT KeyboardHookCallback();
};

class KeyboardEventManager {

public:
	KeyboardEventManager();
	~KeyboardEventManager();

	/// <summary>
	/// Use this if you're using the default application provided by this infrastructure.
	/// </summary>
	void InitiateDefaultKeyboardHook();

	/// <summary>
	/// Sets HookFunction as the callback function for the keyboard hook.
	/// </summary>
	/// <param name="HookFunction">user provided callback function</param>
	void SetKeyboardHook(LRESULT(*HookFunction)(int nCode, WPARAM wParam, LPARAM lParam));


private:
	HHOOK keyboardHook;
	KBDLLHOOKSTRUCT keyboardStruct; // keyboard struct to hold info given from hook

	static LRESULT MouseHookCallback();
};

class EventManager {
public:
	EventManager();
	~EventManager();
private:
	MouseEventManager mouseEventManager;
	KeyboardEventManager keyboardEventManager;
};

#endif //APP_EVENTS_H