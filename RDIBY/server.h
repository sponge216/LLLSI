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
		bool init(PCSTR port, INT backlog, INT ai_family, INT ai_flags, INT ai_protocol, INT ai_socktype);
		bool send(CHAR* data) override;
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