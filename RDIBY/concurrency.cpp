#include "concurrency.h"

namespace concurrency {
	concurrency::ThreadManager::ThreadManager() {

	}
	concurrency::ThreadManager::~ThreadManager() {

	}
	bool concurrency::ThreadManager::createNewThread(ConThread ctThread) {
		HANDLE thread = CreateThread(ctThread.lpThreadAttributes, ctThread.dwStackSize, ctThread.lpStartAddress, ctThread.lpParameter, ctThread.dwCreationFlags, ctThread.lpThreadId);
		if (!thread) {
			fprintf(stdout, "Thread creation failed");
			return false;
		}
		ctThread.hThread = thread;
		this->threadVector.push_back(ctThread);
		return true;
	}
	bool concurrency::ThreadManager::killFirstThread() {

	}
	// ---------------------------
	concurrency::ConThread::ConThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
		this->lpThreadAttributes = lpThreadAttributes;
		this->dwStackSize = dwStackSize;
		this->lpStartAddress = lpStartAddress;
		this->lpParameter = lpParameter;
		this->dwCreationFlags = dwCreationFlags;
		this->lpThreadId = lpThreadId;
	}
	concurrency::ConThread::~ConThread() {
		free(this->lpThreadAttributes);
		free(this->lpParameter);
		free(this->lpThreadId);
		CloseHandle(this->hThread);
		fprintf(stdout, "ConThread %d deconstructed", this->hThread);
	}
}