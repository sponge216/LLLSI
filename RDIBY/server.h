// server.h

#ifndef APP_NETWORK_SERVER_H
#define APP_NETWORK_SERVER_H

#include "network.h"
namespace server {
	/// <summary>
	/// socket for servers.
	/// </summary>
	class ServerSocket : public network::BaseSocket, public network::ITCPSocket {

	public:
		ServerSocket();
		~ServerSocket();
		bool initTCP();
		bool send(CHAR* data) override;
		bool recv() override;
	};

	// --------------------------------------- //

	/// <summary>

	/// </summary>
	class ServerNetworkManager {
	public:
		ServerNetworkManager();
		~ServerNetworkManager();

	private:
		ServerSocket serverSocket;

	};
}
#endif //APP_NETWORK_SERVER_H