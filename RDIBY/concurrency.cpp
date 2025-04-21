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
		this->threadMap.insert(std::pair<DWORD, pConThread>(key, ctThread));
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
	/// <summary>
	/// creates a new event.
	/// </summary>
	/// <returns>NULL - creation failed, use GetLastError() to know why.</returns>
	HANDLE concurrency::ThreadManager::createNewEvent() {
		HANDLE hStopEvent = CreateEventA(NULL, true, false, NULL);

		return hStopEvent;
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
	concurrency::ConThread::ConThread(HANDLE hStopEvent, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter) {
		this->lpThreadAttributes = NULL;
		this->dwStackSize = 0;
		this->lpStartAddress = lpStartAddress;
		this->lpParameter = lpParameter;
		this->dwCreationFlags = 0;
		this->lpThreadId = NULL;
		this->hThread = NULL;
		this->hStopEvent = UniqueHandle(hStopEvent);

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

	// -------------------------------------------------

	concurrency::CriticalSection::CriticalSection() {
		this->cs = { 0 };
		this->init();

	}
	concurrency::CriticalSection::~CriticalSection() {
		this->del();

	}
	void concurrency::CriticalSection::init() {
		InitializeCriticalSection(&this->cs);

	}
	void concurrency::CriticalSection::enter() {
		EnterCriticalSection(&this->cs);

	}
	DWORD concurrency::CriticalSection::tryEntry() {
		return TryEnterCriticalSection(&this->cs);

	}
	void concurrency::CriticalSection::release() {
		LeaveCriticalSection(&this->cs);

	}
	void concurrency::CriticalSection::del() {
		DeleteCriticalSection(&this->cs);

	}
	PCRITICAL_SECTION concurrency::CriticalSection::getCSPointer() {
		return &this->cs;

	}

	// -------------------------------------------------

	concurrency::ConditionVariable::ConditionVariable() {
		this->cv = { 0 };
		this->init();

	}
	concurrency::ConditionVariable::~ConditionVariable() {
		this->del();

	}
	void concurrency::ConditionVariable::init() {
		InitializeConditionVariable(&this->cv);

	}
	void concurrency::ConditionVariable::wake() {
		WakeConditionVariable(&this->cv);

	}
	void concurrency::ConditionVariable::wakeAll() {
		WakeAllConditionVariable(&this->cv);

	}
	void concurrency::ConditionVariable::sleep() {
		this->cs.enter();
		SleepConditionVariableCS(&this->cv, this->cs.getCSPointer(), this->timeout);
		this->cs.release();

	}
	void concurrency::ConditionVariable::setTimeout(DWORD timeout) {
		this->timeout = timeout;

	}
	void concurrency::ConditionVariable::del() {

	}
}