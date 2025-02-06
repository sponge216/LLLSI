#include "concurrency.h"

namespace concurrency {
	concurrency::ThreadManager::ThreadManager() {

	}
	concurrency::ThreadManager::~ThreadManager() {

	}
	bool concurrency::ThreadManager::createNewThread(DWORD WINAPI threadFunction, LPVOID params) {
		if (!(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadFunction, (LPVOID)params, 0, NULL))) {
			fprintf(stdout, "THREAD FAILED\n");
		}
	}
}