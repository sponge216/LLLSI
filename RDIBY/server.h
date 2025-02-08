// server.h

#ifndef APP_NETWORK_SERVER_H
#define APP_NETWORK_SERVER_H

#include "network.h"
#include <iostream>
#include "concurrency.h"
namespace server {
	constexpr auto DEFAULT_BACKLOG = 20;
	constexpr auto DEFAULT_PORT = "30527";

	class ServerSocket : public network::BaseSocket {

	public:
		ServerSocket();
		~ServerSocket();
		bool init(PCSTR port = DEFAULT_PORT, INT backlog = DEFAULT_BACKLOG, INT ai_family = AF_INET, INT ai_flags = AI_PASSIVE, INT ai_protocol = IPPROTO_TCP, INT ai_socktype = SOCK_STREAM);
		bool send(CHAR* data, SIZE_T dwTypeSize) override;
		bool recv() override;
	};

	// --------------------------------------- //

	class ServerNetworkManager {
	public:
		ServerNetworkManager();
		~ServerNetworkManager();

	private:
		ServerSocket serverSocket;
		concurrency::ThreadManager threadManager;
	};
}
#endif //APP_NETWORK_SERVER_H