// concurrency.h

#ifndef APP_CONCURRENCY_H
#define APP_CONCURRENCY_H

class ThreadManager {
public:
	ThreadManager();
	bool createNewThread();
	bool killThread();

};

#endif //APP_CONCURRENCY_H