// concurrency.h

#ifndef APP_CONCURRENCY_H
#define APP_CONCURRENCY_H

#include <vector>

namespace concurrency {
#include <processthreadsapi.h>
	class ThreadManager {
	public:
		ThreadManager();
		~ThreadManager();
		bool createNewThread(DWORD WINAPI threadFunction, LPVOID params);
		bool killThread(HANDLE hThread);
		bool killLastThread(HANDLE hThread);
		bool killFirstThread(HANDLE hThread);
	private:
		std::vector<ConThread> threadVector;
	};

	// ---------------------------------------

	class ConThread {
	public:
		ConThread();
		~ConThread();


	private:
		HANDLE hThread;
		LPSECURITY_ATTRIBUTES lpThreadAttributes;
		SIZE_T dwStackSize;
		LPTHREAD_START_ROUTINE lpStartAddress;
		LPVOID lpParameter;
		DWORD dwCreationFlags;
		LPDWORD lpThreadId;
	};
}
#endif //APP_CONCURRENCY_H