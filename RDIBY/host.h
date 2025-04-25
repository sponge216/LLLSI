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
		bool initTCP(PCSTR pAddrStr, USHORT port);
		bool initUDP(PCSTR pAddrStr, PCSTR port);
		bool initListen(DWORD backlog);
		inline DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags = 0) override;
		inline DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags = 0) override;
	};

	// --------------------------------------- //
	class EncryptedHostSocket :
		public network::BaseSocket
	{

	public:
		EncryptedHostSocket();
		~EncryptedHostSocket();
		bool initTCP(PCSTR pAddrStr, USHORT port);
		bool initUDP(PCSTR pAddrStr, PCSTR port);
		bool initListen(DWORD backlog);

		inline DWORD sendData(SOCKET sock, CHAR* pData, DWORD dwTypeSize, DWORD dwLen, DWORD flags = 0) override;
		inline DWORD recvData(SOCKET sock, CHAR* pBuffer, DWORD dwBufferLen, DWORD flags = 0) override;

		DWORD firstServerInteraction();
		DWORD firstClientInteraction();

	};
	/// <summary>
		/// 
		/// </summary>
	class HostNetworkManager {

	public:
		HostNetworkManager();
		~HostNetworkManager();

	private:
		EncryptedHostSocket hostSocket;

	};
}
#endif //APP_NETWORK_HOST_H