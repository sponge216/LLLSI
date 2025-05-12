// IServerActionMediator.h
#ifndef APP_SERVER_MEDIATOR_H
#define APP_SERVER_MEDIATOR_H
#include "IActionMediator.h"
#
class ServerMediator : public Mediator {
public:
	ServerMediator() : Mediator() {

	};
	~ServerMediator() {

	};

};

class IServerActionListener : public IActionListener {
public:
	IServerActionListener() : IActionListener() {

	}
};

#endif //APP_SERVER_MEDIATOR_H