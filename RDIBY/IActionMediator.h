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
	virtual void requestAction(Action* pAction) {
		this->acquireQueue();
		this->ptsQueue->Push(pAction);
		this->releaseQueue();
	}
	virtual void executeAction(Action* pAction) = 0;

	void acquireQueue() {
		this->pCriticalSection->enter();
	}
	void releaseQueue() {
		this->pCriticalSection->release();
	}
	void setCriticalSection(concurrency::CriticalSection* pCriticalSection) {
		this->pCriticalSection = pCriticalSection;
	}
	void setPtsQueue(ThreadSafeQueue<Action*>* ptsQueue) {
		this->ptsQueue = ptsQueue;
	}
private:
	concurrency::CriticalSection* pCriticalSection;
	ThreadSafeQueue<Action*>* ptsQueue;

};

/// <summary>
/// interface for actions that ActionListeners need to execute.
/// </summary>
class Action {
public:
	Action() {
		this->typeID = -1;
		this->actionID = -1;
		this->pData = nullptr;
		this->pCv = nullptr;
		this->returnBuffer = nullptr;
	};
	Action(DWORD typeID, DWORD actionID, ActionData* pData) {
		this->typeID = typeID;
		this->actionID = actionID;
		this->pData = pData;
		this->pCv = nullptr;
		this->returnBuffer = nullptr;
	};
	Action(DWORD typeID, DWORD actionID, ActionData* pData, concurrency::ConditionVariable* pCv, LPVOID returnBuffer) {
		this->typeID = typeID;
		this->actionID = actionID;
		this->pData = pData;
		this->pCv = pCv;
		this->returnBuffer = returnBuffer;
	};

	DWORD typeID; // ID of the type of the action (which class it belongs to)
	DWORD actionID; // ID that signifies what the action is supposed to be. (create, delete, etc)
	concurrency::ConditionVariable* pCv; // condition variable to ping that the action is done.
	LPVOID returnBuffer;
	ActionData* pData; // pointer to action-related data
	virtual ActionData* getActionData();

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
		this->ptsQueue = new ThreadSafeQueue<Action*>();
		this->pCriticalSection = new concurrency::CriticalSection();
		this->run = true;
	};
	~Mediator() {
		this->run = false;
		for (IActionListener* ialPtr : *this->pListenersVector) {
			ialPtr->setCriticalSection(nullptr);
			ialPtr->setPtsQueue(nullptr);
		}
		delete this->pListenersVector;
		delete this->ptsQueue;
		delete this->pCriticalSection;
	}
	ThreadSafeQueue<Action*>* getTSQPointer() {
		return this->ptsQueue;
	}
	void addListener(IActionListener* pListener) {
		auto pListenersVector = this->pListenersVector;
		pListenersVector->push_back(pListener);
		pListener->setPtsQueue(this->ptsQueue);
		pListener->setCriticalSection(this->pCriticalSection);

	};
	void removeListener(IActionListener* pListener) {
		auto pListenersVector = this->pListenersVector;
		auto plvStart = pListenersVector->begin();
		auto plvEnd = pListenersVector->end();

		pListener->setPtsQueue(nullptr);
		pListener->setCriticalSection(nullptr);
		auto res = std::find(plvStart, plvEnd, pListener);
		if (res != plvEnd)
			pListenersVector->erase(res);
	};

private:
	std::vector<IActionListener*>* pListenersVector; // pointer to vector of IActionListener pointers
	ThreadSafeQueue<Action*>* ptsQueue;
	concurrency::CriticalSection* pCriticalSection;
	bool run;
};
#endif // APP_ACTION_MEDIATOR_H