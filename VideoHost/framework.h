// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files

#include <winsock2.h>
#include <WS2tcpip.h>     
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h> 
#include <Shlwapi.h>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <iostream>
#include "protocol.h"
#include <mutex>
#include "datastructures.h"
#include "video.h"
#include <chrono>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "gdiplus.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "shlwapi.lib")


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
