class BaseSocket {

public:
	virtual bool initialize() = 0;

protected:
	SOCKET sock;
	struct addrinfo* addrInfo; // the address info struct, holds all info about the address.
	struct addrinfo* hints; // used to set the socket's behavior and address
private:
	static const WSADATA wsaData;
};