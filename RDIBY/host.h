// host.h
#ifndef APP_NETWORK_HOST_H
#define APP_NETWORK_HOST_H

#include "network.h"
namespace host {
	/// <summary>
		///  socket for hosts.
		/// </summary>
	class HostSocket :
		public network::BaseSocket
	{

	public:
		HostSocket();
		~HostSocket();
		bool initTCP();
		bool initUDP();
		inline DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD flags = 0) override;
		inline DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags = 0) override;
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