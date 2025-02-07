// concurrency.h

#ifndef APP_CONCURRENCY_H
#define APP_CONCURRENCY_H

#include <vector>

namespace concurrency {
#include <processthreadsapi.h>
#include <handleapi.h>
#include <synchapi.h>
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

	class ThreadManager {
	public:
		ThreadManager();
		~ThreadManager();

		bool createNewThread(ConThread ctThread);

		bool killThread(HANDLE hThread);
		bool killLastThread();
		bool killFirstThread();
		
		std::vector<HANDLE> ghStopEventVector;
	private:
		std::vector<ConThread> threadVector;
		static constexpr DWORD dwTimeout = 1000;

		bool addNewStopEvent();
	};
}
#endif //APP_CONCURRENCY_H