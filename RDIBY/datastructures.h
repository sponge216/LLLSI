// datastructures.h

#ifndef APP_DATASTRUCTURES_H
#define APP_DATASTRUCTURES_H

#include <Windows.h>
#include <queue>
#include <stdexcept>
#include "audio.h"

template <typename T> class ThreadSafeQueue {
public:
	ThreadSafeQueue() {
		InitializeCriticalSection(&this->access);
		this->queue_semaphore = queue_semaphore = CreateSemaphore(NULL, 0, MAXINT, NULL);

	}
	/// <summary>
	/// Pushes item into the queue.
	/// Read here for more info about error codes: https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror
	/// </summary>
	/// <param name="item">- The variable being pushed into the queue</param>
	/// <returns>success - 0, failure - System Error Code</returns>
	DWORD Push(T item) {
		EnterCriticalSection(&this->access);
		this->queue.push(item);
		LeaveCriticalSection(&this->access);

		DWORD res = ReleaseSemaphore(this->queue_semaphore, 1, NULL);
		if (!res)
			return GetLastError();
		return 0;
	}
	/// <summary>
	/// Pops item from queue.
	/// </summary>
	/// <returns>failure - nullptr </returns>
	T Pop() {
		if (WaitForSingleObject(this->queue_semaphore, this->timeout) != WAIT_OBJECT_0) // check if semaphore is signaled.
			throw std::runtime_error("semaphore timed out");

		EnterCriticalSection(&this->access);
		if (this->queue.empty()) {
			LeaveCriticalSection(&this->access);
			throw std::runtime_error("empty queue");
		}
		T item = this->queue.front(); // get item
		this->queue.pop(); // remove it from queue
		LeaveCriticalSection(&this->access);
		return item;
	}
	T Peek() {

	}

private:
	std::queue<T> queue;
	CRITICAL_SECTION access;
	HANDLE queue_semaphore;
	const DWORD timeout = 3;
};

#endif //APP_DATASTRUCTURES_H