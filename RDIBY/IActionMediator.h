// IActionMediator.h

#ifndef APP_ACTION_MEDIATOR_H
#define APP_ACTION_MEDIATOR_H

#include <vector>
#include "datastructures.h"
#include "concurrency.h"

class Action;
class IActionListener;
class ActionData;	
class Mediator;
/// <summary>
/// interface for classes that want to interact with a Mediator.
/// </summary>
class IActionListener {
public:
	ThreadSafeQueue<Action>* ptsQueue;
	virtual void requestAction(Action action) = 0;
	virtual void executeAction(Action action) = 0;
};

/// <summary>
/// interface for actions that ActionListeners need to execute.
/// </summary>
class Action {
public:
	virtual ActionData* getActionData() = 0;

private:
	DWORD typeID; // ID of the type of the action (which class it belongs to)
	DWORD actionID; // ID that signifies what the action is supposed to be. (create, delete, etc)
	concurrency::ConditionVariable* cv; // condition variable to ping that the action is done.
	LPVOID returnBuffer;
	ActionData* pData; // pointer to action-related data

};

/// <summary>
/// interface to store action-related data.
/// </summary>
class ActionData {
public:

};

/// <summary>
/// class that controls requests between several objects.
/// </summary>
class Mediator {
public:
	Mediator() {
		this->pListenersVector = new std::vector<IActionListener*>;
		this->ptsQueue = new ThreadSafeQueue<Action>();
		this->run = true;
	};
	~Mediator() {
		this->pListenersVector->~vector();
		this->ptsQueue->~ThreadSafeQueue();
	}
	ThreadSafeQueue<Action>* getTSQPointer() {
		return this->ptsQueue;
	}
	void addListener(IActionListener* pListener) {
		auto pListenersVector = this->pListenersVector;
		pListenersVector->push_back(pListener);
		pListener->ptsQueue = this->ptsQueue;
	};
	void removeListener(IActionListener* pListener) {
		auto pListenersVector = this->pListenersVector;
		auto plvStart = pListenersVector->begin();
		auto plvEnd = pListenersVector->begin();

		auto res = std::find(plvStart, plvEnd, pListener);
		if (res != plvEnd)
			pListenersVector->erase(res);

		pListener->ptsQueue = nullptr;
	};
private:
	std::vector<IActionListener*>* pListenersVector; // pointer to vector of IActionListener pointers
	ThreadSafeQueue<Action>* ptsQueue;
	bool run;
};
#endif // APP_ACTION_MEDIATOR_H