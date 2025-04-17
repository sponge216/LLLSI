// IActionMediator.h

#include <vector>
#include "datastructures.h"

#ifndef APP_ACTION_MEDIATOR_H
#define APP_ACTION_MEDIATOR_H

class IActionListener {
public:
	virtual void requestAction(Action action) = 0;
	virtual void executeAction(Action action) = 0;
};

class Action {

};

class Mediator {
public:
	Mediator() {
		this->pListenersVector = new std::vector<IActionListener*>;
		this->run = true;
	};
	void addListener(IActionListener* pListener) {
		auto pListenersVector = this->pListenersVector;
		pListenersVector->push_back(pListener);
	};
	void removeListener(IActionListener* pListener) {
		auto pListenersVector = this->pListenersVector;
		auto plvStart = pListenersVector->begin();
		auto plvEnd = pListenersVector->begin();

		auto res = std::find(plvStart, plvEnd, pListener);
		if (res != plvEnd)
			pListenersVector->erase(res);
	};
private:
	std::vector<IActionListener*>* pListenersVector; // pointer to vector of IActionListener pointers
	bool run;
};



#endif // APP_ACTION_MEDIATOR_H