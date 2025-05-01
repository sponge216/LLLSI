#include "server.h"

int test1(int testNum);
int test2(int testNum);
int test3(int testNum);

int main(int argc, char** argv) {
	//test1(1);
	//test2(2);
	test1(1);
}

// test to check if accept thread function works
int test1(int testNum) {
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	std::cout << "IN TEST  " << testNum << "\n";

	server::ServerManager* pSm = new server::ServerManager();
	pSm->snManager.eServerSocket.initListen();

	server::accept_thread_data_t tData = { 0 };
	tData.acceptParams = LPVOID(pSm);
	tData.threadParams = LPVOID(pSm);
	tData.clientHandlerFunc = server::clientHandlerFunc;

	LPVOID lpParameter = LPVOID(&tData);
	/*concurrency::pConThread pctClient = new concurrency::ConThread(NULL, server::acceptFunc, lpParameter);
	pSm->threadManager.createNewThread(pSm->snManager.eServerSocket.getSocket(), pctClient);*/
	server::acceptFunc(lpParameter);
	return 1;
}

// test to check that accept function works
int test2(int testNum) {
	std::cout << "IN TEST  " << testNum << "\n";

	server::ServerNetworkManager* pSnm = new server::ServerNetworkManager();
	return 1;
}

int test3(int testNum) {
	std::cout << "IN TEST  " << testNum << "\n";

	auto roomsSize = 10;

	server::ServerNetworkManager* pSnm = new server::ServerNetworkManager();
	server::RoomManager* pRm = new server::RoomManager;

	for (int i = 0; i < roomsSize; i++) {
		server::pRoom r = new server::Room();
		pRm->createNewRoom(i, r);

		for (int j = 0; j < roomsSize; j++) {
			auto p = new server::RoomClient;
			p->sap.first = j;
			pRm->addClientToRoom(i, p);
		}
	}

	for (int i = 0; i < roomsSize; i++) {

		for (int j = 0; j < roomsSize; j++) {
			pRm->removeClientFromRoom(i, j);
		}
		pRm->deleteRoom(i);
	}
	pRm->~RoomManager();

	return 1;
}