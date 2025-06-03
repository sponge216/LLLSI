// client.cpp 

#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "client.h"
#include "protocols.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include "Resource.h"
/*"10.100.102.27"*/
#define CLIENT_IP_ADDRESS "127.0.0.1"
#define HOST_IP_ADDRESS "127.0.0.1" 
#define HOST_PORT 36542
#define CLIENT_PORT 36543
#define PACKET_SIZE 4096
#define BMP_ARR_SIZE 10
#define PAYLOAD_SIZE (PACKET_SIZE - sizeof(rtmp_packet_t))

using namespace Gdiplus;
using namespace protocols;
using namespace client;
using namespace network;

#define MAX_LOADSTRING 100

/// <summary>
/// struct that holds information and data of a frame
/// </summary>


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// threads
void receiverThread(ClientSocket& sock);
void consumerThread(std::unordered_map<DWORD, FrameBuffer>& activeFrames);
void timerThread(std::unordered_map<DWORD, FrameBuffer>& activeFrames);
void decoderThread(std::unordered_map<DWORD, FrameBuffer>& activeFrames);
void eventSenderThread(ClientSocket& sock);

// image-related functions
Bitmap* decodeImage(const std::vector<BYTE>& data, IStream*& pStream);
void displayFrame(Bitmap* bmp, HDC& hdc, HWND hwnd);
Bitmap* createBitmapFromImageStream(IStream* pStream);
void setRealTimeScheduling();

// event-related functions
void SetKeyboardHook();
void SetMouseHook();
void ReleaseKeyboardHook();
void ReleaseMouseHook();
LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);

using NumBmpPair = std::pair<DWORD, Bitmap*>;


struct CompareNumBmpPairs
{
	bool operator()(const NumBmpPair& a, const NumBmpPair& b) const
	{
		return a.first > b.first;
	}
};

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

std::atomic<bool> globalOn{ true }; // Global flag that indicates if program is running or not

HWND hwndMain = nullptr; // Window handle
IStream* pStream = nullptr; // Frame Stream pointer

std::mutex bmpMutex;  // Mutex to protect access to the Bitmap object

std::condition_variable receiverReady;  // Condition variable for the receiver thread
std::atomic<bool> receiverDone{ false };  // Flag to indicate if receiver is done filling the buffer
std::atomic<bool> loadFrame{ false }; // Flag to indicate if frame needs to be loaded

ThreadSafeQueue<DWORD> frameNumQueue = ThreadSafeQueue<DWORD>(); // Queue for frame numbers
ThreadSafePrioQueue<NumBmpPair, CompareNumBmpPairs> frameNumBmpQueue = ThreadSafePrioQueue<NumBmpPair, CompareNumBmpPairs>(); // Queue for frame bmps
std::vector<Bitmap*> usedFramesVector = std::vector<Bitmap*>(); // Vector for old frames
ThreadSafeQueue<CHAR*> packetQueue = ThreadSafeQueue<CHAR*>(); // Queue for packets

std::atomic<DWORD> framesBufferingSize = 10; // Amount of frames that needs to be kept
std::atomic<DWORD> latestRemovedFrame = 0;
std::atomic<DWORD> latestFrame = 0;
std::mutex dataMutex;

HHOOK _kbd_hook; //keyboard hook
HHOOK _ms_hook; // mouse hook
KBDLLHOOKSTRUCT kbdStruct; // keyboard struct to hold info given from hook
MSLLHOOKSTRUCT msStruct; // mouse struct to hold info given from hook
ThreadSafeQueue<event_hook_data> eventDataQueue = ThreadSafeQueue<event_hook_data>();

Addr hostAddr; // host's IP address

// Set current thread's priority to RealTime
void setRealTimeScheduling() {
	// Set the thread priority to highest for real-time scheduling on Windows 
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	// Init GDI+ once
	GdiplusStartupInput gsi;
	ULONG_PTR gToken;
	GdiplusStartup(&gToken, &gsi, nullptr);

	// Window initialization

	ClientSocket clientSock = ClientSocket();
	clientSock.initUDP(CLIENT_PORT);
	//------------------0----
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(CLIENT_PORT);
	clientAddr.sin_addr.s_addr = INADDR_ANY;

	sockaddr_in hostAddress;
	hostAddress.sin_family = AF_INET;
	hostAddress.sin_port = htons(HOST_PORT);
	inet_pton(AF_INET, HOST_IP_ADDRESS, &hostAddress.sin_addr);
	hostAddr = Addr(hostAddress, sizeof(hostAddress));

	// Map that holds all frames
	std::unordered_map<DWORD, FrameBuffer> activeFrames;

	//  Run threads
	std::thread receiver(receiverThread, std::ref(clientSock));
	std::thread consumer1(consumerThread, std::ref(activeFrames));
	std::thread decoder(decoderThread, std::ref(activeFrames));
	std::thread timer(timerThread, std::ref(activeFrames));
	//std::thread eventSender(eventSenderThread, std::ref(clientSock));

	SetKeyboardHook();
	SetMouseHook();

	// Main window message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	globalOn = false;

	// Cleanup
	clientSock.~ClientSocket();
	timer.join();
	decoder.join();
	consumer1.join();
	receiver.join();
	//eventSender.join();
	ReleaseMouseHook();
	ReleaseKeyboardHook();
	WSACleanup();
	GdiplusShutdown(gToken);

	return 0;
}

// Receiver thread - Receives packets from host and passes them onto the consumer.
void receiverThread(ClientSocket& sock) {
	setRealTimeScheduling();
	rtmp_packet_t pkt = { 0 };
	int len = sizeof(hostAddr);
	Addr recvAddr = Addr();

	receiverDone = true;
	receiverReady.notify_all();

	int packetSize = 0;
	while (globalOn) {
		CHAR* packet = new CHAR[PACKET_SIZE];
		if ((packetSize = sock.recvDataFrom(packet, PACKET_SIZE, recvAddr)) < 0) {
			delete[] packet;
			continue;
		}
		memcpy_s(&pkt, sizeof(pkt), packet, sizeof(pkt));

		rtmp_packet_t ackPkt = { 0 };
		ackPkt.type = PACKET_TYPE_CONTROL;
		ackPkt.sequenceNumber = pkt.sequenceNumber;
		ackPkt.flags = FLAG_ACK;

		int sentSize = sock.sendDataTo((char*)&ackPkt, sizeof(ackPkt), hostAddr);

		// Process the received packet
		{
			std::lock_guard<std::mutex> lock(dataMutex);
			packetQueue.Push(packet);
		}
	}
}

// Consumer Thread - Consumes packets from Receiver, reconstructs frames and passes the finished frames to Decoder.
void consumerThread(std::unordered_map<DWORD, FrameBuffer>& activeFrames) {
	setRealTimeScheduling();
	rtmp_packet pkt = { 0 };
	WORD sizeOfRTMP = sizeof(rtmp_packet);
	WORD packetSize = 0;

	while (globalOn) {
		// Wait until there is data available to consume
		while (globalOn && !packetQueue.isEmpty()) {
			std::unique_lock<std::mutex> lock(dataMutex);

			CHAR* packet = packetQueue.Pop();
			memcpy_s(&pkt, sizeof(pkt), packet, sizeof(pkt)); // extract packet header from buffer

			if (pkt.type != PACKET_TYPE_STREAM) continue;

			packetSize = pkt.size;
			rtsmp_packet& rts = pkt.rtmp_data.rtsmp;
			DWORD frameNum = rts.frameNumber;

			FrameBuffer& fb = activeFrames[frameNum];
			if (fb.chunks.empty()) {
				fb.expectedSize = pkt.slidingWindow; // size of current frame
				fb.frameNumber = frameNum;
				latestFrame = frameNum;
				fb.chunks.resize(fb.expectedSize);
			}

			fb.totalPackets++;
			fb.totalSize += packetSize - sizeOfRTMP;

			memcpy_s(fb.chunks.data() + rts.offset * PAYLOAD_SIZE,
				size_t(packetSize - sizeOfRTMP),
				packet + sizeOfRTMP,
				size_t(packetSize - sizeOfRTMP));
			delete[] packet;

			if (fb.totalSize == fb.expectedSize) {
				frameNumQueue.Push(frameNum);
			}
		}
	}
}

// Decoder Thread - receives constructed frames and formats them into a displayable BMP.
void decoderThread(std::unordered_map<DWORD, FrameBuffer>& activeFrames) {
	while (globalOn) {
		while (globalOn && !frameNumQueue.isEmpty()) {
			DWORD frameNumber = frameNumQueue.Pop();
			FrameBuffer& fb = activeFrames[frameNumber];

			Bitmap* newBmp;
			if (newBmp = decodeImage(fb.chunks, pStream)) {
				std::lock_guard<std::mutex> lck(bmpMutex);
				frameNumBmpQueue.Push(NumBmpPair(frameNumber, newBmp));
			}
			activeFrames.erase(frameNumber); // Clean up completed frame
		}
	}
}

// Timer Thread - Program's timer that dictates the FPS and triggers repaint messages.
void timerThread(std::unordered_map<DWORD, FrameBuffer>& activeFrames) {
	while (globalOn) {
		loadFrame = true;
		InvalidateRect(hwndMain, NULL, FALSE);
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}
}

// Event Sender Thread - Receives events from hooks through eventDataQueue, formats them and sends them to host.
void eventSenderThread(ClientSocket& sock) {
	rtmp_packet packet = { 0 };
	packet.type = PACKET_TYPE_EVENT;
	DWORD flags = 0;
	float screenWidth = static_cast<float>(GetSystemMetrics(SM_CXSCREEN)); // Screen width in pixels
	float scaleX = 65535.0f / screenWidth; // Scale factor for mouse movement
	float screenHeight = static_cast<float>(GetSystemMetrics(SM_CYSCREEN)); // Screen width in pixels
	float scaleY = 65535.0f / screenHeight; // Scale factor for mouse movement
	while (globalOn) {
		while (globalOn && !eventDataQueue.isEmpty())
		{
			event_hook_data data = eventDataQueue.Pop();

			if (data.type == 0) {
				MSLLHOOKSTRUCT* pMsStruct = (MSLLHOOKSTRUCT*)(data.lParam);
				pMsStruct->pt.x = static_cast<LONG>(pMsStruct->pt.x * scaleX);
				pMsStruct->pt.y = static_cast<LONG>(pMsStruct->pt.y * scaleY);
				flags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE; // default event is moving. 

				switch (data.wParam) {

				case WM_LBUTTONDOWN:
					flags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE;
					break;

				case WM_LBUTTONUP:
					flags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;
					break;

				case WM_MOUSEWHEEL:
					flags = MOUSEEVENTF_WHEEL | MOUSEEVENTF_ABSOLUTE;
					break;

				case WM_RBUTTONDOWN:
					flags = MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_ABSOLUTE;
					break;

				case WM_RBUTTONUP:
					flags = MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_ABSOLUTE;
					break;
				}

				packet.rtmp_data.rtemp.eventType = 0;
				packet.rtmp_data.rtemp.rtemp_data.msData = { pMsStruct->pt.x, pMsStruct->pt.y, flags, pMsStruct->mouseData };
			}
			else if (data.type == 1) {
				KBDLLHOOKSTRUCT* pKbdStruct = (KBDLLHOOKSTRUCT*)(data.lParam);
				switch (data.wParam) {
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
				packet.rtmp_data.rtemp.eventType = 1;
				packet.rtmp_data.rtemp.rtemp_data.kbdData = { pKbdStruct->vkCode, flags };
			}
			sock.sendDataTo((char*)&packet, sizeof(packet), hostAddr);
		}
	}
}

//Releases keyboard hook.
void ReleaseKeyboardHook()
{
	UnhookWindowsHookEx(_kbd_hook);
}
//Releases mouse hook.
void ReleaseMouseHook()
{
	UnhookWindowsHookEx(_ms_hook);
}

//Mouse callback function. Called when a mouse input event is raised.
LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		// the action is valid: HC_ACTION.
		event_hook_data data = { (void*)lParam, wParam,INPUT_MOUSE };
		eventDataQueue.Push(data);
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_ms_hook, nCode, wParam, lParam);
}

//Keyboard callback function. Called when a keyboard input event is raised.
LRESULT __stdcall KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		event_hook_data data = { (void*)lParam,wParam,INPUT_KEYBOARD };
		eventDataQueue.Push(data);
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_kbd_hook, nCode, wParam, lParam);
}

void SetKeyboardHook()
{

	if (!(_kbd_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookCallback, NULL, 0)))
	{
		LPCWSTR a = L"Failed to install hook!";
		LPCWSTR b = L"Error";
		MessageBox(NULL, a, b, MB_ICONERROR);
	}
}

void SetMouseHook()
{

	if (!(_ms_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, NULL, 0)))
	{
		LPCWSTR a = L"Failed to install hook!";
		LPCWSTR b = L"Error";
		MessageBox(NULL, a, b, MB_ICONERROR);
	}
}

// Create bitmap from raw byte data
Bitmap* decodeImage(const std::vector<BYTE>& data, IStream*& pStream) {
	if (pStream)
		pStream->Release();
	pStream = nullptr;

	if ((pStream = SHCreateMemStream(data.data(), data.size())) == NULL) { // gets here
		std::cerr << "Failed to create IStream from memory." << std::endl;
		return nullptr;
	}

	// Create Bitmap from stream and check the status
	return createBitmapFromImageStream(pStream); // GETS HERE
}

// Create bitmap from stream
Bitmap* createBitmapFromImageStream(IStream* pStream) {
	if (pStream) {
		Bitmap* bmp = new Bitmap(pStream, FALSE);
		if (bmp->GetLastStatus() == Ok) {
			return bmp;
		}
		delete bmp;
	}
	std::cerr << "Bitmap creation failed!" << std::endl;
	return nullptr;
}

void displayFrame(Bitmap* bmp, HDC& hdc, HWND hwnd) {
	if (!(bmp && hdc)) {
		return;
	}

	// Check if the bitmap is valid and initialized correctly
	if (bmp->GetLastStatus() != Ok) {
		return;
	}

	// Get the dimensions of the window's client area (not the entire screen)
	RECT rect;
	GetClientRect(hwnd, &rect);  // hwnd is the window handle, this gets the client area
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	int bmpWidth = bmp->GetWidth();
	int bmpHeight = bmp->GetHeight();

	// Create a memory DC to hold the bitmap
	HDC hdcMem = CreateCompatibleDC(hdc);
	if (!hdcMem) {
		return;
	}

	// Ensure bmp is valid and get the HBITMAP
	HBITMAP hBitmap;
	Status status = bmp->GetHBITMAP(Color(255, 255, 255), &hBitmap);
	if (status != Ok) {
		DeleteDC(hdcMem);
		return;
	}

	// Select the bitmap into the memory DC
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBitmap);

	// Stretch the bitmap to fit the window's client area
	SetStretchBltMode(hdc, HALFTONE);
	BOOL result = StretchBlt(
		hdc, 0, 0, windowWidth, windowHeight,   // Target DC (window's client area) and dimensions
		hdcMem, 0, 0, bmpWidth, bmpHeight,      // Source DC and dimensions (bitmap size)
		SRCCOPY                                 // Copy the source image
	);

	// Clean up
	SelectObject(hdcMem, hOldBmp);
	DeleteDC(hdcMem);
	DeleteObject(hBitmap);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
// Window procedure for handling window messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	switch (uMsg) {
	case WM_PAINT: {
		// Start painting
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);  // Get the device context for painting
		std::lock_guard<std::mutex> lck(bmpMutex);

		if (!loadFrame || frameNumBmpQueue.isEmpty()) {
			EndPaint(hwnd, &ps);  // Release the painting resources
			return 0;
		}

		NumBmpPair numBmpPair = frameNumBmpQueue.Pop();
		auto currBmp = numBmpPair.second;
		if (currBmp) {

			displayFrame(currBmp, hdc, hwnd);  // Use the hdc from BeginPaint
			loadFrame = false;
			delete currBmp;

		}
		else {
			std::cerr << "Bitmap is null!" << std::endl;
		}

		// End painting
		EndPaint(hwnd, &ps);  // Release the painting resources
		return 0;

	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hwndMain = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


	if (!hwndMain)
	{
		return FALSE;
	}

	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);

	return TRUE;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

