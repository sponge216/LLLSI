// datastructures.cpp

#include "datastructures.h"

template <typename T>
ThreadSafeQueue<T>::ThreadSafeQueue() {
	InitializeCriticalSection(&this->access);
	this->queue_semaphore = CreateSemaphore(NULL, 0, MAXINT, NULL);
}

template <typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() {
	DeleteCriticalSection(&this->access);
	CloseHandle(this->queue_semaphore);
}

template <typename T>
DWORD ThreadSafeQueue<T>::Push(T item) {
	EnterCriticalSection(&this->access);
	this->queue.push(item);
	LeaveCriticalSection(&this->access);

	DWORD res = ReleaseSemaphore(this->queue_semaphore, 1, NULL);
	if (!res)
		return GetLastError();
	return 0;
}

template <typename T>
T ThreadSafeQueue<T>::Pop() {
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

template <typename T>
T ThreadSafeQueue<T>::Peek() {
	EnterCriticalSection(&this->access);
	if (this->queue.empty()) {
		LeaveCriticalSection(&this->access);
		throw std::runtime_error("empty queue");
	}
	T item = this->queue.front();
	LeaveCriticalSection(&this->access);
	return item;
}
