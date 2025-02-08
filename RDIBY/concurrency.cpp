#include "concurrency.h"

namespace concurrency {
	concurrency::ThreadManager::ThreadManager() {

	}
	concurrency::ThreadManager::~ThreadManager() {
		DWORD dwThreadArraySize = this->threadVector.size();
		auto dwStopEventVectorSize = this->ghStopEventVector.size();

		for (auto i = 0; i < dwStopEventVectorSize; i++) {
			SetEvent(this->ghStopEventVector[i].get()); // signal event
		}

		auto phThreadArray = &(this->threadVector[0].hThread); // pointer to first thread handle from ThreadVector.
		fprintf(stdout, "Waiting on all ThreadManager's threads to finish");
		WaitForMultipleObjects(dwThreadArraySize, phThreadArray, true, concurrency::ThreadManager::dwTimeout); // Change to Ex version!!
		for (auto i = 0; i < this->ghStopEventVector.size(); i++) {
			CloseHandle(this->ghStopEventVector[i].get()); // close event
		}

		this->ghStopEventVector.~vector(); // deconstruct event vector
		this->threadVector.~vector(); // deconstruct thread vector
		fprintf(stdout, "All ThreadManager's threads finished, deconstructor finished");
	}
	bool concurrency::ThreadManager::createNewThread(ConThread ctThread) {
		auto thread = CreateThread(ctThread.lpThreadAttributes, ctThread.dwStackSize, ctThread.lpStartAddress, ctThread.lpParameter, ctThread.dwCreationFlags, ctThread.lpThreadId);
		if (!thread) {
			fprintf(stdout, "Thread creation failed");
			return false;
		}

		ctThread.hThread = thread;
		this->threadVector.push_back(ctThread);
		if (!this->addNewStopEvent()) {
			fprintf(stdout, "Error, couldn't add new stop event to thread manager");
			// TODO: add more safety measures, kill thread and alert user.
		}
		return true;
	}
	bool concurrency::ThreadManager::addNewStopEvent() {
		this->ghStopEventVector.push_back(UniqueHandle(CreateEventExA(NULL, 0, FALSE, NULL)));
		return true;
	}
	bool concurrency::ThreadManager::killFirstThread() {
		auto hEvent = this->ghStopEventVector[0].get();
		SetEvent(hEvent); // signal event
		WaitForSingleObject(this->threadVector[0].hThread, concurrency::ThreadManager::dwTimeout); 
		//TODO: check if successful!!
		return true;
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