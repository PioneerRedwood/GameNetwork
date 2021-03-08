#ifndef _NETWORK_CLIENT_
#define _NETWORK_CLIENT_

#include "predef.h"

class NetworkClient
{
public:
	NetworkClient() {};
	~NetworkClient() {};

	bool Init(int interval);
	bool Loop();

private:
	WSAData data;
	fd_set	masterfds;

	struct timeval timeout;

	SOCKET	clientSocket;
	SOCKADDR_IN address;

	char buffer[BUFFER_SIZE];

	bool InitSocket(int interval);
	bool Connect();
	bool Shutdown();
};

#endif // !_NETWORKCLIENT_
