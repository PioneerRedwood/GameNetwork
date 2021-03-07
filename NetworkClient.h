#ifndef _NETWORK_CLIENT_
#define _NETWORK_CLIENT_

#include "predef.h"

class NetworkClient
{
public:
	NetworkClient();
	~NetworkClient();

	void Init();
	void Loop();
	void Connect();

private:
	WSAData data;
	fd_set	masterfds;

	SOCKET clientSocket;

	struct addrinfo* self;
	struct sockaddr_in address;

	char buffer[BUFFER_SIZE];

	void Setup(int port);
	void InitSocket();
	void Shutdown();
};

#endif // !_NETWORKCLIENT_


//class NetworkServer
//{
//public:
//	NetworkServer();
//	~NetworkServer();
//
//	void Init();
//	void Shutdown();
//	void Loop();
//
//	// callbacks?
//
//	uint16_t SendMessage(uint16_t conn, const char* messageBuffer);
//
//private:
//	WSAData data;
//	fd_set masterfds;
//
//	uint16_t maxfd;
//
//	int masterSocket;
//	int tempSocket;
//
//	sockaddr_in			server_addr;
//
//	char buffer[BUFFER_SIZE];
//
//	void Setup(int port);
//	void InitSocket();
//	void Bind();
//	void Listen();
//	void Connect();
//	void Receive(int fd);
//};
