#include "NetworkServer.h"

#include "Logger.h"
#include "DataStorage.h"

///////////////////////////////////////////////////////////////////////////// public

NetworkServer::NetworkServer()
{
	Setup(PORT);
}

NetworkServer::~NetworkServer()
{
	closesocket(masterSocket);
}

void NetworkServer::Init()
{
	InitSocket();
	Bind();
	Listen();
}

void NetworkServer::Shutdown()
{
	WSACleanup();
	closesocket(masterSocket);
}

void NetworkServer::Loop()
{
	fd_set tempfds;
	FD_ZERO(&tempfds);

	tempfds = masterfds;
	int ret = select(maxfd + 1, &tempfds, nullptr, nullptr, nullptr);
	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "select() failed");
		Shutdown();
	}

	// 이 부분의 효율을 올리려면 클라이언트 보관 컨테이너를 탐색하는 것이 좋음
	for (int i = 0; i <= maxfd; ++i)
	{
		if (FD_ISSET(i, &tempfds))
		{
			if (masterSocket == i)
			{
				// master에선 새로운 연결 처리
				Connect();
			}
			else
			{
				Receive(i);
			}
		}
	}
}

uint16_t NetworkServer::SendMessage(uint16_t conn, const char* messageBuffer)
{
	int sendSize = send(conn, messageBuffer, sizeof(messageBuffer), 0);
	if (sendSize < 0)
	{
		Logger::Log(LOG_ERROR, "SendMessage failed");
		return (uint16_t)0;
	}
	else
	{
		return (uint16_t)sendSize;
	}
}

///////////////////////////////////////////////////////////////////////////// public

///////////////////////////////////////////////////////////////////////////// private 

void NetworkServer::Setup(int port)
{
	int ret = WSAStartup(MAKEWORD(2,2), &data);
	if (ret != 0)
	{
		Logger::Log(LOG_ERROR, "WSAStartup failed");
		Shutdown();
	}

	masterSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (masterSocket == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "socket creation failed");
	}

	FD_ZERO(&masterfds);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

	ZeroMemory(buffer, BUFFER_SIZE);
}

void NetworkServer::InitSocket()
{
	int opt = 1;
	int ret = setsockopt(masterSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));

	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "setsockopt() failed");
		Shutdown();
	}
}

void NetworkServer::Bind()
{
	int ret = bind(masterSocket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	
	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "bind() failed");
	}

	FD_SET(masterSocket, &masterfds);
	maxfd = masterSocket;
}

void NetworkServer::Listen()
{
	int ret = listen(masterSocket, BACKLOG);

	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "listen() failed");
	}
}

void NetworkServer::Connect()
{
	sockaddr_in client;
	int clientSize = sizeof(struct sockaddr_in);

	tempSocket = accept(masterSocket, (struct sockaddr*)&client, &clientSize);
	
	if (tempSocket < 0)
	{
		Logger::Log(LOG_ERROR, "accept() failed");
	}
	else
	{
		FD_SET(tempSocket, &masterfds);

		// 클라이언트 정보 모두 담는 컨테이너 클래스에 저장하기

		wchar_t host[NI_MAXHOST] = {0, };
		wchar_t service[NI_MAXSERV] = { 0, };

		if (GetNameInfoW((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			Logger::Log(LOG_INFO, "Connection on port: %s:%s\n", server_addr.sin_addr.S_un.S_addr, service);

			// 첫 접속 시 보내는 메시지
			int ret = send(tempSocket, MSG_GREETING, sizeof(MSG_GREETING), 0);
			if (ret < 0)
			{
				Logger::Log(LOG_ERROR, "send() msg greeting failed");
			}
			else
			{
				// 이때 클라이언트 정보를 담아야 함
				// 일단 maxfd 보다 작은 만큼 소켓을 순회하도록
				if (tempSocket > maxfd)
				{
					maxfd = tempSocket;
				}
			}
		}
	}

	// Call New Connection event (callback)
}

void NetworkServer::Receive(int client_fd)
{
	int ret = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (ret <= 0)
	{
		if (ret == SOCKET_ERROR)
		{
			// Call disconnected callback
			closesocket(client_fd);
			FD_CLR(client_fd, &masterfds);
			return;
		}
		else
		{
			Logger::Log(LOG_ERROR, "recv() failed");
		}
		closesocket(client_fd);
		FD_CLR(client_fd, &masterfds);
		return;
	}
	Logger::Log(LOG_INFO, "received << %s\n", buffer);
	// Call receive success callback
	ZeroMemory(buffer, BUFFER_SIZE);
}

///////////////////////////////////////////////////////////////////////////// private 