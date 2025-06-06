// concurrency.h

#ifndef APP_CONCURRENCY_H
#define APP_CONCURRENCY_H

#include <unordered_map>
#include <iostream>
#include <processthreadsapi.h>
#include <handleapi.h>
#include <synchapi.h>
#include <memory>
#include <Windows.h>

namespace concurrency {

	class CriticalSection {
	public:
		CriticalSection();
		~CriticalSection();
		void init();
		void enter();
		DWORD tryEntry();
		void release();
		void del();

		PCRITICAL_SECTION getCSPointer();

	private:
		CRITICAL_SECTION cs;
	};

	class ConditionVariable {
	public:
		ConditionVariable();
		~ConditionVariable();
		void init();
		void wake();
		void wakeAll();
		void sleep();
		void del();
		void setTimeout(DWORD timeout);
	private:
		CONDITION_VARIABLE cv;
		CriticalSection cs;
		DWORD timeout;
	};
	struct HandleDeleter {
		void operator()(HANDLE h) const {
			if (h && h != INVALID_HANDLE_VALUE) {
				CloseHandle(h);
				std::cout << "Handle closed.\n";
			}
		}
	};

	typedef std::unique_ptr<void, HandleDeleter> UniqueHandle;

	class ConThread {
	public:
		ConThread();
		ConThread(
			HANDLE hStopEvent,
			LPSECURITY_ATTRIBUTES lpThreadAttributes,
			SIZE_T dwStackSize,
			LPTHREAD_START_ROUTINE lpStartAddress,
			LPVOID lpParameter,
			DWORD dwCreationFlags,
			LPDWORD lpThreadId
		);
		ConThread(HANDLE hStopEvent, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter);
		~ConThread();

		UniqueHandle hThread;
		UniqueHandle hStopEvent;
		LPSECURITY_ATTRIBUTES lpThreadAttributes;
		SIZE_T dwStackSize;
		LPTHREAD_START_ROUTINE lpStartAddress;
		LPVOID lpParameter;
		DWORD dwCreationFlags;
		LPDWORD lpThreadId;
	};
	typedef ConThread* pConThread;
	// ---------------------------------------
	//Developer is expected to have threads that listen to ghStopEvent
	class ThreadManager {
	public:
		ThreadManager();
		~ThreadManager();

		bool createNewThread(DWORD key, pConThread ctThread);
		HANDLE createNewEvent();
		bool killThread(DWORD index);

	private:
		std::unordered_map<DWORD, pConThread> threadMap;
		static constexpr auto TIMEOUT_INTERVAL = 1000;
		static constexpr auto INITIAL_MAP_SIZE = 100;

	};
}
#endif //APP_CONCURRENCY_H