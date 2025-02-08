#include "concurrency.h"

namespace concurrency {
	concurrency::ThreadManager::ThreadManager() {
		this->threadMap.reserve(concurrency::ThreadManager::INITIAL_MAP_SIZE);
	}

	concurrency::ThreadManager::~ThreadManager() {
		for (std::pair<DWORD, pConThread> pairThread : this->threadMap) {
			auto pcThread = pairThread.second;
			auto hStopEvent = pcThread->hStopEvent.get();
			auto hThread = pcThread->hThread.get();

			SetEvent(hStopEvent); // signal event
			WaitForSingleObject(hThread, concurrency::ThreadManager::TIMEOUT_INTERVAL); // TODO: MAKE CHECKS IF TIMED OUT!
			pcThread->~ConThread();
		}

		fprintf(stdout, "All ThreadManager's threads finished, deconstructor finished");
	}

	bool concurrency::ThreadManager::createNewThread(DWORD key, pConThread ctThread) {
		auto thread = CreateThread(
			ctThread->lpThreadAttributes,
			ctThread->dwStackSize,
			ctThread->lpStartAddress,
			ctThread->lpParameter,
			ctThread->dwCreationFlags,
			ctThread->lpThreadId);
		if (!thread) {
			fprintf(stdout, "Thread creation failed");
			return false;
		}

		ctThread->hThread = UniqueHandle(thread);
		this->threadMap[key] = ctThread;
		return true;
	}

	bool concurrency::ThreadManager::killThread(DWORD key) {
		auto pcThread = this->threadMap[key];
		auto hEvent = pcThread->hStopEvent.get();
		auto hThread = pcThread->hThread.get();

		SetEvent(hEvent); // signal event
		WaitForSingleObject(hThread, concurrency::ThreadManager::TIMEOUT_INTERVAL); //TODO: check if successful!!

		this->threadMap.erase(key); //TODO: check if successful!!
		return true;
	}

	// -------------------------------------------------

	concurrency::ConThread::ConThread() {
		this->lpThreadAttributes = NULL;
		this->dwStackSize = 0;
		this->lpStartAddress = NULL;
		this->lpParameter = NULL;
		this->dwCreationFlags = 0;
		this->lpThreadId = NULL;
		this->hThread = NULL;
		this->hStopEvent = NULL;

	}

	concurrency::ConThread::ConThread(HANDLE hStopEvent, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
		this->lpThreadAttributes = lpThreadAttributes;
		this->dwStackSize = dwStackSize;
		this->lpStartAddress = lpStartAddress;
		this->lpParameter = lpParameter;
		this->dwCreationFlags = dwCreationFlags;
		this->lpThreadId = lpThreadId;
		this->hThread = NULL;
		this->hStopEvent = UniqueHandle(hStopEvent);
	}

	concurrency::ConThread::~ConThread() {
		fprintf(stdout, "ConThread  deconstructed");
	}
}