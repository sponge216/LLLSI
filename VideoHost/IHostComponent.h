#pragma once
#ifndef APP_IHOSTCOMPONENT_H 
#define APP_IHOSTCOMPONENT_H 
#include <atomic>


class IHostComponent {
public:
	
	static void killProgram();
	static bool isProgramOn();

private:
	static std::atomic<bool> globalOn;
};
#endif // APP_IHOSTCOMPONENT_H