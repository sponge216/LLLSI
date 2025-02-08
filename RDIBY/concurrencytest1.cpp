#include "concurrencytest1.h"


typedef struct {
	HANDLE hStopEvent;
	int i;
} threadParam;
DWORD WINAPI threadFunc(LPVOID param) {
	threadParam params = *((threadParam*)(param));
	while (WaitForSingleObject(params.hStopEvent, 1000)) {
		/*printf("%d\n", params.i);*/
	}
	return 1;
}
int main(int argc, char** argv) {
	auto threadManager = concurrency::ThreadManager();

	for (int i = 0; i < 100; i++) {
		concurrency::ConThread cThread;
		auto hEvent = CreateEventA(NULL, false, false, NULL);
		cThread.hStopEvent = concurrency::UniqueHandle(hEvent);
		cThread.lpStartAddress = threadFunc;
		threadParam params = { hEvent,i };
		cThread.lpParameter = LPVOID(&params);
		threadManager.createNewThread(i, &cThread);
	}
	for (int i = 0; i < 100; i++) {
		threadManager.killThread(i);
	}
}