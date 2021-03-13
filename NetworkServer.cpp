#include "NetworkServer.h"

#include "Logger.h"
#include "DataStorage.h"

bool		NetworkServer::Init(long sec)
{
	return InitSocket(sec);
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

bool		NetworkServer::Loop()
{
	bool isOK = false;

	if (tempSocket <= 0)
	{
		isOK = Connect();
	}

	tempfds = masterfds;

	int ret = select(maxfd + 1, &tempfds, nullptr, nullptr, &timeout);
	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "select() failed %d", WSAGetLastError());
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
					// master 새로운 연결 처리
					isOK = Connect();
				}
				else
				{
					isOK = Receive(socket);
				}
			}
		}
	}

	return isOK;
}

void		NetworkServer::Setup()
{
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret != 0)
	{
		Logger::Log(LOG_ERROR, "WSAStartup failed %d", WSAGetLastError());
		Shutdown();
	}

	masterSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (masterSocket == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "socket creation failed %d", WSAGetLastError());
	}

	FD_ZERO(&masterfds);
	FD_ZERO(&tempfds);

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

	ZeroMemory(buffer, BUFFER_SIZE);
}

bool		NetworkServer::InitSocket(long sec)
{
	//int opt = 1;
	//int ret = setsockopt(masterSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));
	//if (ret < 0)
	//{
	//	Logger::Log(LOG_ERROR, "setsockopt() failed %d", WSAGetLastError());
	//	Shutdown();
	//	return false;
	//}
	//else
	//{
	//	Logger::Log(LOG_INFO, "setsockopt() success");
	//	return true;
	//}

	bool result = false;

	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	int ret = bind(masterSocket, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "bind() failed %d", WSAGetLastError());
		result = false;
	}
	else
	{
		result = true;
	}
	

	FD_SET(masterSocket, &masterfds);
	maxfd = masterSocket;

	ret = listen(masterSocket, BACKLOG);

	if (ret < 0)
	{
		Logger::Log(LOG_ERROR, "listen() failed %d", WSAGetLastError());
		result = false;
	}
	else
	{
		result = true;
	}

	return result;
}

bool		NetworkServer::Connect()
{
	sockaddr_in client;
	int clientSize = sizeof(struct sockaddr_in);

	tempSocket = accept(masterSocket, (struct sockaddr*)&client, &clientSize);

	if (tempSocket < 0)
	{
		Logger::Log(LOG_ERROR, "accept() failed %d", WSAGetLastError());
	}
	else
	{
		unsigned long arg = 1;
		if (ioctlsocket(tempSocket, FIONBIO, &arg) != 0)
		{
			Logger::Log(LOG_ERROR, "set non-blocking failed %d", WSAGetLastError());
			return false;
		}

		FD_SET(tempSocket, &masterfds);

		wchar_t host[NI_MAXHOST] = { 0, };
		wchar_t service[NI_MAXSERV] = { 0, };

		if (GetNameInfoW((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			// 새로운 접속 컨테이너에 저장
			std::wstring hs(host);
			std::wstring serv(service);
			Logger::Log(LOG_INFO, "New connection! %s:%s\n", std::string(hs.begin(), hs.end()).c_str(), std::string(serv.begin(), serv.end()).c_str());

			clientDeque.push_back(tempSocket);
			clientMap.insert(std::make_pair(tempSocket, clientId++));

			// 응답 패킷 전송
			if (AssemblePacket(PacketType::ConnectionResponse, "Welcome!", 9) < 0)
			{
				Logger::Log(LOG_ERROR, "AssemblePacket() failed");
				return false;
			}
			else
			{
				if (!sendStack.empty())
				{
					if (send(tempSocket, sendStack.top().buffer, BUFFER_SIZE, 0))
					{
						Logger::Log(LOG_INFO, "send: %s", sendStack.top().buffer);
						return true;
					}
					else
					{
						Logger::Log(LOG_ERROR, "send() msg greeting failed %d", WSAGetLastError());
						return false;
					}
				}
			}
		}
	}

	return false;
}

bool		NetworkServer::Receive(SOCKET client_fd)
{
	int ret = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (ret <= 0)
	{
		if (ret == SOCKET_ERROR)
		{
			
			closesocket(client_fd);
			FD_CLR(client_fd, &masterfds);
			clientMap.erase(client_fd);
			return false;
		}
		else
		{
			return false;
		}
		Logger::Log(LOG_ERROR, "recv() failed");
		closesocket(client_fd);
		FD_CLR(client_fd, &masterfds);
		clientMap.erase(client_fd);
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "%d : %s", client_fd, buffer);

		// 패킷 파싱
		// ParsingPacket()
		return true;
	}
	
}
