// datastructures.h

#ifndef APP_DATASTRUCTURES_H
#define APP_DATASTRUCTURES_H

#include <Windows.h>
#include <queue>
#include <stdexcept>

template <typename T> class ThreadSafeQueue {
public:
	ThreadSafeQueue() {}

	~ThreadSafeQueue() {}

	/// <summary>
	/// Pushes item into the queue.
	/// Read here for more info about error codes: https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror
	/// </summary>
	/// <param name="item">- The variable being pushed into the queue</param>
	/// <returns>success - 0, failure - System Error Code</returns>
	DWORD Push(T item) {}

	/// <summary>
	/// Pops item from queue.
	/// </summary>
	/// <returns>
	/// success - first item from queue
	/// runtime errors - "semaphore timed out", "empty queue"
	/// </returns>
	T Pop() {}

	T Peek() {}

private:
	std::queue<T> queue;
	CRITICAL_SECTION access;
	HANDLE queue_semaphore;
	const DWORD timeout = 3;
};

#endif //APP_DATASTRUCTURES_H