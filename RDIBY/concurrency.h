// concurrency.h

#ifndef APP_CONCURRENCY_H
#define APP_CONCURRENCY_H

#include <vector>
#include <iostream>
#include <processthreadsapi.h>
#include <handleapi.h>
#include <synchapi.h>
#include <memory>

namespace concurrency {

	struct HandleDeleter {
		void operator()(HANDLE h) const {
			if (h && h != INVALID_HANDLE_VALUE) {
				CloseHandle(h);
				std::cout << "Handle closed.\n";
			}
		}
	};
	
	typedef std::unique_ptr<void, HandleDeleter> UniqueHandle; //

	class ConThread {
	public:
		ConThread();
		ConThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
			SIZE_T dwStackSize,
			LPTHREAD_START_ROUTINE lpStartAddress,
			LPVOID lpParameter,
			DWORD dwCreationFlags,
			LPDWORD lpThreadId);
		~ConThread();

		HANDLE hThread;
		LPSECURITY_ATTRIBUTES lpThreadAttributes;
		SIZE_T dwStackSize;
		LPTHREAD_START_ROUTINE lpStartAddress;
		LPVOID lpParameter;
		DWORD dwCreationFlags;
		LPDWORD lpThreadId;
	};

	// ---------------------------------------
	//Developer is expected to have threads that listen to ghStopEvent
	class ThreadManager {
	public:
		ThreadManager();
		~ThreadManager();

		bool createNewThread(ConThread ctThread);

		bool killThread(HANDLE hThread);
		bool killLastThread();
		bool killFirstThread();

		ConThread* pTarget;
		std::vector<UniqueHandle> ghStopEventVector;

	private:
		std::vector<ConThread> threadVector;
		static constexpr DWORD dwTimeout = 1000;

		bool addNewStopEvent();
	};
}
#endif //APP_CONCURRENCY_H