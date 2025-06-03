// datastructures.h

#ifndef APP_DATASTRUCTURES_H
#define APP_DATASTRUCTURES_H

#include <Windows.h>
#include <queue>
#include <stdexcept>


template <typename T> class ThreadSafeQueue {
public:

	ThreadSafeQueue() {
		InitializeCriticalSection(&this->access);
		this->queue_semaphore = CreateSemaphore(NULL, 0, MAXINT, NULL);
	}
	~ThreadSafeQueue() {
		DeleteCriticalSection(&this->access);
		CloseHandle(this->queue_semaphore);
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
	/// <returns>
	/// success - first item from queue
	/// runtime errors - "semaphore timed out", "empty queue"
	/// </returns>
	T Pop() {
		if (WaitForSingleObject(this->queue_semaphore, this->timeout) != WAIT_OBJECT_0)
			throw std::runtime_error("semaphore timed out");

		EnterCriticalSection(&this->access);
		if (this->queue.empty()) {
			LeaveCriticalSection(&this->access);
			throw std::runtime_error("empty queue");
		}
		T item = this->queue.front();
		this->queue.pop();
		LeaveCriticalSection(&this->access);
		return item;
	}

	T Peek() {
		EnterCriticalSection(&this->access);
		if (this->queue.empty()) {
			LeaveCriticalSection(&this->access);
			throw std::runtime_error("empty queue");
		}
		T item = this->queue.front();
		LeaveCriticalSection(&this->access);
		return item;
	}

private:
	std::queue<T> queue;
	CRITICAL_SECTION access;
	HANDLE queue_semaphore;
	const DWORD timeout = 3;
};
#endif //APP_DATASTRUCTURES_H