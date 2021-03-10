#include "NetworkServer.h"

#include "Logger.h"
#include "DataStorage.h"

///////////////////////////////////////////////////////////////////////////// public

NetworkServer::NetworkServer()
{
	Setup(PORT, 1);
}

NetworkServer::~NetworkServer()
{
	closesocket(masterSocket);
}

bool		NetworkServer::Init()
{
	return InitSocket() && Bind() && Listen();
}

void		NetworkServer::Shutdown()
{
	WSACleanup();
	int ret = closesocket(masterSocket);
	if (ret == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "shutdown() failed");
	}
	else
	{
		std::cout << "closesocket() executed\n";
	}
}

void		NetworkServer::Loop()
{
	fd_set tempfds;
	FD_ZERO(&tempfds);

	tempfds = masterfds;
	int ret = select(maxfd + 1, &tempfds, nullptr, nullptr, &timeout);
	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "select() failed");
		Shutdown();
	}
	else if (ret == 0)
	{
		Logger::Log(LOG_INFO, "select() worked .. but there is no change in fds");
	}
	else
	{
		for (SOCKET socket : clientDeque)
		{
			if (FD_ISSET(socket, &tempfds))
			{
				if (masterSocket == socket)
				{
					// master ���ο� ���� ó��
					Connect();
				}
				else
				{
					Receive(socket);
				}
			}
		}
	}
}

uint16_t	NetworkServer::SendMessage(uint16_t conn, const char* messageBuffer)
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

void		NetworkServer::Setup(int port, int interval)
{
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret != 0)
	{
		Logger::Log(LOG_ERROR, "WSAStartup failed");
		Shutdown();
	}
	else
	{
		Logger::Log(LOG_INFO, "WSAStartup success");
	}

	masterSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (masterSocket == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "socket creation failed");
	}
	else
	{
		Logger::Log(LOG_INFO, "socket created");
	}

	FD_ZERO(&masterfds);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

	ZeroMemory(buffer, BUFFER_SIZE);

	timeout.tv_sec = interval;
	timeout.tv_usec = 0;
}

bool		NetworkServer::InitSocket()
{
	int opt = 1;
	int ret = setsockopt(masterSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));

	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "setsockopt() failed");
		Shutdown();
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "setsockopt() success");
		return true;
	}
}

bool		NetworkServer::Bind()
{
	int ret = bind(masterSocket, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "bind() failed");
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "bind() success");
		return true;
	}

	FD_SET(masterSocket, &masterfds);
	maxfd = masterSocket;
}

bool		NetworkServer::Listen()
{
	int ret = listen(masterSocket, BACKLOG);

	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "listen() failed");
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "listen() success");
		return true;
	}
}

bool		NetworkServer::Connect()
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

		wchar_t host[NI_MAXHOST] = { 0, };
		wchar_t service[NI_MAXSERV] = { 0, };

		if (GetNameInfoW((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			std::wstring hs(host);
			std::wstring serv(service);
			Logger::Log(LOG_INFO, "Connection on port: %s:%s\n", std::string(hs.begin(), hs.end()).c_str(), std::string(serv.begin(), serv.end()).c_str());

			// ù ���� �� ������ �޽���
			ZeroMemory(buffer, BUFFER_SIZE);
			std::string welcomeMsg = "Welcome!";

			sprintf_s(&buffer[0], 4, "%d", tempSocket);
			sprintf_s(&buffer[4], sizeof(welcomeMsg), "%s", welcomeMsg.c_str());

			int ret = send(tempSocket, buffer, BUFFER_SIZE, 0);
			if (ret < 0)
			{
				Logger::Log(LOG_ERROR, "send() msg greeting failed");
				return false;
			}
			else
			{
				clientDeque.push_back(tempSocket);
				clientMap.insert(std::make_pair(tempSocket, clientId++));
				return true;
			}
		}
	}

	return false;
	// Call New Connection event (callback)
}

bool		NetworkServer::Receive(SOCKET client_fd)
{
	int ret = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (ret <= 0)
	{
		if (ret == SOCKET_ERROR)
		{
			// Call disconnected callback
			closesocket(client_fd);
			FD_CLR(client_fd, &masterfds);
			clientMap.erase(client_fd);
			return false;
		}
		else
		{
			Logger::Log(LOG_ERROR, "recv() failed");
			return false;
		}
		closesocket(client_fd);
		FD_CLR(client_fd, &masterfds);
		clientMap.erase(client_fd);
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "received << %s", buffer);
		// ��Ŷ �Ľ�
		// ParsingPacket()
		// buffer[0] : clientId
		// buffer[1] : seq
		return true;
	}
	
}

///////////////////////////////////////////////////////////////////////////// private 