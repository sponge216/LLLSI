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
		//TODO: deconstructor is called recursively on an object's members, no point in this line.
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
		auto hEvent = this->ghStopEventVector.front().get();
		auto hThread = this->threadVector.front().hThread;

		this->ghStopEventVector.erase(this->ghStopEventVector.begin() + 1);
		this->threadVector.erase(this->threadVector.begin() + 1);

		SetEvent(hEvent); // signal event
		WaitForSingleObject(hThread, concurrency::ThreadManager::dwTimeout);
		//TODO: check if successful!!
		return true;
	}
	bool concurrency::ThreadManager::killLastThread() {
		auto hEvent = this->ghStopEventVector.back().get();
		auto hThread = this->threadVector.back().hThread;

		this->ghStopEventVector.pop_back();
		this->threadVector.pop_back();

		SetEvent(hEvent); // signal event
		WaitForSingleObject(hThread, concurrency::ThreadManager::dwTimeout);
		//TODO: check if successful!!
		return true;
	}
	bool concurrency::ThreadManager::killThread(DWORD index) {
		auto hEvent = this->ghStopEventVector[index].get();
		auto hThread = this->threadVector[index].hThread;

		this->ghStopEventVector.erase(this->ghStopEventVector.begin() + index + 1);
		this->threadVector.erase(this->threadVector.begin() + index + 1);

		SetEvent(hEvent); // signal event
		WaitForSingleObject(hThread, concurrency::ThreadManager::dwTimeout);
		//TODO: check if successful!!
		return true;
	}
	// ---------------------------	
	concurrency::ConThread::ConThread() {
		this->lpThreadAttributes = NULL;
		this->dwStackSize = 0;
		this->lpStartAddress = NULL;
		this->lpParameter = NULL;
		this->dwCreationFlags = 0;
		this->lpThreadId = NULL;
		this->hThread = NULL;
	}
	concurrency::ConThread::ConThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
		this->lpThreadAttributes = lpThreadAttributes;
		this->dwStackSize = dwStackSize;
		this->lpStartAddress = lpStartAddress;
		this->lpParameter = lpParameter;
		this->dwCreationFlags = dwCreationFlags;
		this->lpThreadId = lpThreadId;
		this->hThread = NULL;
	}
	concurrency::ConThread::~ConThread() {
		CloseHandle(this->hThread);
		fprintf(stdout, "ConThread %d deconstructed", this->hThread);
	}
}