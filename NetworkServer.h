#ifndef _NETWORK_SERVER_
#define _NETWORK_SERVER_

#include "predef.h"

class NetworkServer
{
public:
	NetworkServer();
	~NetworkServer();

	void Init();
	void Shutdown();
	void Loop();

	// callbacks?
	uint16_t SendMessage(uint16_t conn, const char* messageBuffer);

private:
	WSAData data;
	fd_set masterfds;

	struct timeval timeout;

	uint16_t maxfd;

	SOCKET masterSocket;
	SOCKET tempSocket;

	sockaddr_in	server_addr;

	char buffer[BUFFER_SIZE];

	void Setup(int port, int interval);
	void InitSocket();
	void Bind();
	void Listen();
	void Connect();
	void Receive(int fd);
};

#endif
