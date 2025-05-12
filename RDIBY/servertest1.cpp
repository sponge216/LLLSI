#include "server.h"
#include "client.h"

int test1(int testNum);
int test2(int testNum);
int test3(int testNum);

int main(int argc, CHAR** argv) {
	//test1(1);
	//test2(2);
	test1(1);
}

// test to check if accept thread function works
int test1(int testNum) {
	std::cout << "IN TEST  " << testNum << "\n";

	server::ServerManager* pSm = new server::ServerManager();
	pSm->snManager.eServerSocket.initListen();

	server::accept_thread_data_t tData = { 0 };
	tData.acceptParams = LPVOID(pSm);
	tData.threadParams = LPVOID(pSm);
	tData.clientHandlerFunc = server::clientHandlerFunc;

	LPVOID lpParameter = LPVOID(&tData);
	concurrency::pConThread pctClient = new concurrency::ConThread(NULL, server::acceptFunc, lpParameter);
	pSm->threadManager.createNewThread(pSm->snManager.eServerSocket.getSocket(), pctClient);

	client::EncryptedClientSocket ecs = client::EncryptedClientSocket();
	ecs.initUDP(36543);
	server::server_room_msg_t msg = { 0 };
	sockaddr_in inadr;
	int len = sizeof(inadr);
	while (1) {
		recvfrom(ecs.sock, (char*)&msg, sizeof(msg), 0, (sockaddr*)&inadr, &len);
		wchar_t ipAddress[INET_ADDRSTRLEN];
		InetNtop(AF_INET, &inadr.sin_addr, ipAddress, INET_ADDRSTRLEN);

		// Extract the port number (convert from network byte order to host byte order)
		u_short port = ntohs(inadr.sin_port);

		std::wcout << "IP Address udp: " << ipAddress << std::endl;
		std::cout << "Port udp: " << port << std::endl;

		memcpy_s(&msg.msgData.sockAddr4, sizeof(msg.msgData.sockAddr4), &inadr, sizeof(inadr));
		sendto(ecs.sock, (char*)&msg, sizeof(msg), 0, (sockaddr*)&inadr, len);
	}
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
	std::vector<server::RoomClient*> vec;
	for (int i = 0; i < roomsSize; i++) {
		server::pRoom r = new server::Room();
		pRm->createNewRoom(i, r);

		for (int j = 0; j < roomsSize; j++) {
			server::RoomClient* p = new server::RoomClient;
			p->sap.first = j;
			pRm->addClientToRoom(i, p);
			vec.push_back(p);
		}
	}

	for (int i = 0; i < roomsSize; i++) {

		for (int j = 0; j < roomsSize; j++) {
			pRm->removeClientFromRoom(i, vec.back());
			vec.pop_back();
		}
		pRm->deleteRoom(i);
	}
	delete pSnm;
	delete pRm;

	return 1;
}