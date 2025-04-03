#include "events.h"

namespace events {
	events::KeyboardEventManager::KeyboardEventManager() {

	}

	LRESULT __stdcall events::KeyboardEventManager::KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
		if (nCode >= 0)
		{
			// the action is valid: HC_ACTION.
			/*queue_cell_t cell = { (void*)lParam, wParam,0 };
			pushToQueue(proc_queue, cell);*/
		}

		// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
		return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
	}
	void events::KeyboardEventManager::SetKeyboardHook(LRESULT(*HookFunction)(int nCode, WPARAM wParam, LPARAM lParam)) {
		if (!(keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, HookFunction, NULL, 0)))
		{
			LPCWSTR a = L"Failed to install hook!";
			LPCWSTR b = L"Error";
			MessageBox(NULL, a, b, MB_ICONERROR);
		}
	}
	void events::KeyboardEventManager::InitiateDefaultKeyboardHook() {
		this->SetKeyboardHook(events::KeyboardEventManager::KeyboardHookCallback);
	}


	DWORD events::KeyboardFilter::findMatchingFlag(DWORD flag) {
		DWORD flags = 0;
		switch (flag) {

		case WM_KEYDOWN:
			flags = 0;
			break;

		case WM_SYSKEYDOWN:
			flags = 0;
			break;

		case WM_KEYUP:
			flags = KEYEVENTF_KEYUP;
			break;

		case WM_SYSKEYUP:
			flags = KEYEVENTF_KEYUP;
			break;

		}
		return flags;
	}
}
//-------------------------------------------------------------

events::MouseEventManager::MouseEventManager() {

}

LRESULT __stdcall events::MouseEventManager::MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0)
	{
		// the action is valid: HC_ACTION.
		/*queue_cell_t cell = { (void*)lParam, wParam,0 };
		pushToQueue(proc_queue, cell);*/
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}
void events::MouseEventManager::SetMouseHook(LRESULT(*HookFunction)(int nCode, WPARAM wParam, LPARAM lParam)) {
	if (!(mouseHook = SetWindowsHookEx(WH_MOUSE_LL, HookFunction, NULL, 0)))
	{
		LPCWSTR a = L"Failed to install hook!";
		LPCWSTR b = L"Error";
		MessageBox(NULL, a, b, MB_ICONERROR);
	}
}
void events::MouseEventManager::InitiateDefaultMouseHook() {
	this->SetMouseHook(events::MouseEventManager::MouseHookCallback);
}

DWORD events::MouseFilter::findMatchingFlag(DWORD flag) {
	DWORD flags = MOUSEEVENTF_MOVE; // default event is moving.
	switch (flag) {

	case WM_LBUTTONDOWN:
		flags = MOUSEEVENTF_LEFTDOWN;
		break;

	case WM_LBUTTONUP:
		flags = MOUSEEVENTF_LEFTUP;
		break;

	case WM_MOUSEWHEEL:
		flags = MOUSEEVENTF_WHEEL;
		break;

	case WM_RBUTTONDOWN:
		flags = MOUSEEVENTF_RIGHTDOWN;
		break;

	case WM_RBUTTONUP:
		flags = MOUSEEVENTF_RIGHTUP;
		break;
	}
	return flags;
}
