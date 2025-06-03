// header.h : include file for standard system include files,
// or project specific include files
#pragma once
#define WIN32_LEAN_AND_MEAN

#include "client.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2def.h>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h> 
#include <Shlwapi.h>

#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>  // For std::mutex
#include <queue>
#include "protocols.h"
#include "datastructures.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Gdiplus.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "shlwapi.lib")


//

#pragma once

#include "targetver.h"
// Windows Header Files
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>





typedef struct event_hook_data_t {
	LPVOID lParam; // struct pointer
	WPARAM wParam; // msg type
	BYTE type; // keyboard/mouse
}event_hook_data, * pevent_hook_data;

#include <unordered_map>
#include <iostream>
#include <processthreadsapi.h>
#include <handleapi.h>
#include <synchapi.h>
#include <memory>
#include "concurrency.h"