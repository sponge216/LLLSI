#pragma once
#ifndef APP_IClientComponent_H 
#define APP_IClientComponent_H 
#include <atomic>


class IClientComponent {
public:

	static void killProgram();
	static bool isProgramOn();

private:
	static std::atomic<bool> globalOn;
};
#endif // APP_IClientComponent_H