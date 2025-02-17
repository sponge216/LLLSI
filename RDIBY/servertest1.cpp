#include "server.h"
int test1(int testNum);
int test2(int testNum);

int main(int argc, char** argv) {
	test1(1);
	test2(2);
}

// test to check if accept thread function works
int test1(int testNum) {
	std::cout << "IN TEST  " << testNum << "\n";

	server::ServerNetworkManager* pSnm = new server::ServerNetworkManager();
	HANDLE hStopEvent = CreateEventA(NULL, true, false, NULL);
	if (hStopEvent == NULL) {
		std::cout << "failed to create stop event. last error: " << GetLastError() << "\n";
	}
	LPVOID lpParameter = (LPVOID)pSnm;

	concurrency::pConThread pctClient = new concurrency::ConThread(hStopEvent, pSnm->acceptThreadFunc, lpParameter);
	pSnm->threadManager.createNewThread(pSnm->serverSocket.sock, pctClient);

	return 1;
}

// test to check that accept function works
int test2(int testNum) {
	std::cout << "IN TEST  " << testNum << "\n";

	server::ServerNetworkManager* pSnm = new server::ServerNetworkManager();
	pSnm->acceptFunc(pSnm);

	return 1;
}