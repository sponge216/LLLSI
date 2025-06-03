#include "client.h"
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"Gdiplus.lib")

using namespace Gdiplus;

int test1();
int test2();
void printSockAddrIn(const sockaddr_in& addr);

auto static const ENTRY_MSG = 0; // when client first enters
auto static const LEAVE_MSG = 1; // when client leaves
auto static const CLOSE_ROOM_MSG = 2; // when host closes room without starting it
auto static const START_PUNCH_MSG = 3; // when host closes room and wants to start nat hole punching


int main(int argc, CHAR** argv) {

	test1();
}

int test1() { // as host

	client::EncryptedClientSocket ecs;
	ecs.initTCP(client::SERVER_IP, client::SERVER_PORT);

	client::Client cl;
	std::string isHost;
	std::cout << "Enter user name (max 15 letters)" << std::endl;
	std::cin >> cl.name;
	std::cout << "Enter room name (max 15 letters)" << std::endl;
	std::cin >> cl.roomName;
	std::cout << "Enter password (max 15 letters)" << std::endl;
	std::cin >> cl.roomPassword;
	while (true) {
		std::cout << "Are you a host? Y/N" << std::endl;
		std::cin >> isHost;
		if (!isHost.compare("Y") || !isHost.compare("N"))
			break;
	}
	cl.isHost = (!isHost.compare("Y")) ? true : false;

	sockaddr_in hostAddr;
	int addrLen = sizeof(hostAddr);

	// Get the local address and port for the socket
	if (getsockname(ecs.sock, (sockaddr*)&hostAddr, &addrLen) == SOCKET_ERROR) {
		std::cerr << "Error getting socket information: " << WSAGetLastError() << std::endl;
		return 1;
	}

	// Convert the IP address to a string
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(hostAddr.sin_addr), ipStr, INET_ADDRSTRLEN);

	// Get the port number
	u_short hostPort = ntohs(hostAddr.sin_port);
	std::cout << "Print initial host: " << std::endl;
	printSockAddrIn(hostAddr);

	if (ecs.firstServerInteraction(cl) == -1) {
		std::cout << "Invalid parameters!" << std::endl;
		return 1;
	}


	DWORD res = 0;
	bool on = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // sleep for 1000, wait till client joins.
	client::server_room_msg_t startPunch = { 0 };
	startPunch.msgType = START_PUNCH_MSG;
	ecs.sendData(ecs.sock, (CHAR*)&startPunch, sizeof(startPunch), 1, 0);

	client::server_room_msg_t msgBuffer = { 0 };
	std::vector< sockaddr_in> clientAddrVec;
	while (on) {
		res = ecs.recvData(ecs.sock, (CHAR*)&msgBuffer, sizeof(msgBuffer), 0);
		if (msgBuffer.msgType == START_PUNCH_MSG) {
			clientAddrVec.insert(clientAddrVec.end(), msgBuffer.msgData.sockAddr4);
		}
		if (msgBuffer.msgType == LEAVE_MSG) {
			on = false;
			std::cout << "server told to leave!" << std::endl;
			closesocket(ecs.sock);
		}
	}

	ecs.initUDP(hostPort);
	std::cout << "Client is: " << std::endl;
	printSockAddrIn(clientAddrVec[1]);
	std::cout << "server says host on TCP is: " << std::endl;
	printSockAddrIn(clientAddrVec[0]);

	sockaddr_in serverAddr;
	sockaddr_in clientTest;
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, client::SERVER_IP, &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(client::SERVER_PORT + 1);

	int len = sizeof(serverAddr);
	sendto(ecs.sock, (char*)&msgBuffer, sizeof(msgBuffer), 0, (sockaddr*)&serverAddr, len);
	recvfrom(ecs.sock, (char*)&msgBuffer, sizeof(msgBuffer), 0, (sockaddr*)&serverAddr, &len);
	memcpy_s(&clientTest, sizeof(clientTest), &msgBuffer.msgData.sockAddr4, sizeof(msgBuffer.msgData.sockAddr4));


	std::cout << "server says host on UDP is: " << std::endl;
	printSockAddrIn(clientTest);

	msgBuffer.msgType = 1;
	while (1) {
		for (sockaddr_in clientAddr : clientAddrVec) {
			sendto(ecs.sock, (CHAR*)&msgBuffer, sizeof(msgBuffer), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
		}
	}
	return 1;
}

int test2() { // as client
	std::cout << "Client test!" << std::endl;

	client::EncryptedClientSocket ecs;
	ecs.initTCP(client::SERVER_IP, client::SERVER_PORT);
	client::Client cl;
	std::string isHost;
	std::cout << "Enter user name (max 15 letters)" << std::endl;
	std::cin >> cl.name;
	std::cout << "Enter room name (max 15 letters)" << std::endl;
	std::cin >> cl.roomName;
	std::cout << "Enter password (max 15 letters)" << std::endl;
	std::cin >> cl.roomPassword;
	while (true) {
		std::cout << "Are you a host? Y/N" << std::endl;
		std::cin >> isHost;
		if (!isHost.compare("Y") || !isHost.compare("N"))
			break;
	}
	cl.isHost = (!isHost.compare("Y")) ? true : false;

	sockaddr_in clientAddr;
	int addrLen = sizeof(clientAddr);

	// Get the local address and port for the socket
	if (getsockname(ecs.sock, (sockaddr*)&clientAddr, &addrLen) == SOCKET_ERROR) {
		std::cerr << "Error getting socket information: " << WSAGetLastError() << std::endl;
		return 1;
	}

	// Convert the IP address to a string
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);

	// Get the port number
	u_short clientPort = ntohs(clientAddr.sin_port);

	std::cout << "Print initial client: " << std::endl;
	printSockAddrIn(clientAddr);

	if (ecs.firstServerInteraction(cl) == -1) {
		std::cout << "Invalid parameters!" << std::endl;
		return 1;
	}

	DWORD res = 0;
	bool on = true;

	client::server_room_msg_t msgBuffer = { 0 };
	std::vector< sockaddr_in> clientAddrVec;
	sockaddr_in hostAddr;
	while (on) {
		res = ecs.recvData(ecs.sock, (CHAR*)&msgBuffer, sizeof(msgBuffer), 0);

		if (msgBuffer.msgType == START_PUNCH_MSG) {
			clientAddrVec.insert(clientAddrVec.end(), msgBuffer.msgData.sockAddr4);
		}
		if (msgBuffer.msgType == LEAVE_MSG) {
			on = false;
			std::cout << "server told to leave!" << std::endl;
			closesocket(ecs.sock);
			break;
		}
	}
	ecs.initUDP(clientPort);

	std::cout << "Host is: " << std::endl;
	printSockAddrIn(clientAddrVec[0]);
	std::cout << "server says client on TCP is: " << std::endl;
	printSockAddrIn(clientAddrVec[1]);

	sockaddr_in serverAddr;
	sockaddr_in clientTest;
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, client::SERVER_IP, &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(client::SERVER_PORT + 1);
	int len = sizeof(serverAddr);
	std::cout << "server is: " << std::endl;
	printSockAddrIn(serverAddr);

	sendto(ecs.sock, (char*)&msgBuffer, sizeof(msgBuffer), 0, (sockaddr*)&serverAddr, len);
	recvfrom(ecs.sock, (char*)&msgBuffer, sizeof(msgBuffer), 0, (sockaddr*)&clientTest, &len);
	memcpy_s(&clientTest, sizeof(clientTest), &msgBuffer.msgData.sockAddr4, sizeof(msgBuffer.msgData.sockAddr4));

	std::cout << "server says client on UDP is: " << std::endl;
	printSockAddrIn(clientTest);

	len = sizeof(hostAddr);
	while (1) {
		int bytesRead = recvfrom(ecs.sock, (CHAR*)&msgBuffer, sizeof(msgBuffer), 0, (sockaddr*)&hostAddr, &len);
		if (bytesRead == SOCKET_ERROR) {
			std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
			continue; // Try again or handle the error
		}
		std::cout << "Received message with type: " << msgBuffer.msgType << std::endl;		std::cout << "Got through recv" << std::endl;
		std::cout << "Data: " << msgBuffer.msgType << std::endl;
	}
	return 1;
}

void printSockAddrIn(const sockaddr_in& addr) {
	// Convert the IP address (in network byte order) to a string
	wchar_t ipAddress[INET_ADDRSTRLEN];
	InetNtop(AF_INET, &addr.sin_addr, ipAddress, INET_ADDRSTRLEN);

	// Extract the port number (convert from network byte order to host byte order)
	u_short port = ntohs(addr.sin_port);

	std::wcout << "IP Address: " << ipAddress << std::endl;
	std::cout << "Port: " << port << std::endl;
}