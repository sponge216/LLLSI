// events.h

#ifndef APP_EVENTS_H
#define APP_EVENTS_H

#include <Windows.h>

namespace events {

	/*LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
	LRESULT __stdcall KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);*/

	// struct for mouse-movement events.
	typedef struct mouse_data {
		LONG dx; // delta x
		LONG dy; // delta y
		DWORD mouse_flags;
		DWORD mouse_data;
	}mouse_data_t, * pmouse_data;

	//struct for keyboard press/release events.
	typedef struct keyboard_data {
		DWORD vkCode;
		DWORD keyboard_flags;
	}keyboard_data_t, * pkeyboard_data;

	typedef struct packet_data {
		DWORD type;

		union {
			keyboard_data_t kbd_data;
			mouse_data_t ms_data;
		} DUMMYUNIONNAME;

	}packet_data_t, * ppacket_data;

	class MouseFilter {
	public:
		DWORD findMatchingFlag(DWORD flag);
	};

	class KeyboardFilter {
	public:
		DWORD findMatchingFlag(DWORD flag);
	};

	class MouseEventManager {

	public:
		MouseEventManager();
		~MouseEventManager();

		/// <summary>
		/// Use this if you're using the default application provided by this infrastructure.
		/// </summary>	
		void InitiateDefaultMouseHook();
		void SetMouseHook(LRESULT(*HookFunction)(int nCode, WPARAM wParam, LPARAM lParam));
		static HHOOK mouseHook;
		static MSLLHOOKSTRUCT mouseStruct; // mouse struct to hold info given from hook

		static LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);

	private:
		
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
		/// <param name="HookFunction">-user provided callback function</param>
		void SetKeyboardHook(LRESULT(*HookFunction)(int nCode, WPARAM wParam, LPARAM lParam));

		static HHOOK keyboardHook;
		static KBDLLHOOKSTRUCT keyboardStruct; // keyboard struct to hold info given from hook

		static LRESULT __stdcall KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
	private:
	
	};

	class EventManager {
	public:
		EventManager();
		~EventManager();
	private:
		MouseEventManager mouseEventManager;
		KeyboardEventManager keyboardEventManager;
	};
}
#endif //APP_EVENTS_H