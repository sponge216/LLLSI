// host.h
#ifndef APP_NETWORK_HOST_H
#define APP_NETWORK_HOST_H

#include "network.h"
namespace host {
	/// <summary>
		///  socket for hosts.
		/// </summary>
	class HostSocket :
		public network::BaseSocket,
		public network::ITCPSocket,
		public network::IUDPSocket {

	public:
		HostSocket();
		~HostSocket();
		bool initTCP() override;
		bool initUDP() override;
		bool send(CHAR* data) override;
		bool recv() override;
	};

	// --------------------------------------- //

	/// <summary>
		/// 
		/// </summary>
	class HostNetworkManager {

	public:
		HostNetworkManager();
		~HostNetworkManager();

	private:
		HostSocket hostSocket;
		static const WSADATA wsaData;

	};
}
#endif //APP_NETWORK_HOST_H