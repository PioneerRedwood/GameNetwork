#include "NetworkClient.h"
#include "predef.h"

bool		NetworkClient::Init(long sec, int port)
{
	return InitSocket(sec, port);
}

bool		NetworkClient::Loop()
{
	fd_set readfds;
	FD_ZERO(&readfds);

	readfds = masterfds;

	int ret = select(clientSocket, &readfds, nullptr, nullptr, &timeout);
	if (ret >= 0)
	{
		if (FD_ISSET(clientSocket, &readfds))
		{
			ZeroMemory(buffer, BUFFER_SIZE);

			ret = recv(clientSocket, buffer, BUFFER_SIZE, 0);
			if (ret > 0)
			{
				Logger::Log(LOG_INFO, "received: %s\n", std::string(buffer).c_str());

				if (ParsingPacket(buffer, ret))
				{
					if (!recvStack.empty())
					{
						Logger::Log(LOG_INFO, "Deal with recvStack.top().buffer %s", recvStack.top().buffer);
						recvStack.pop();
					}
				}
				else
				{
					Logger::Log(LOG_ERROR, "ParsingPacket() failed");
				}
			}
			else
			{
				Logger::Log(LOG_ERROR, "recv() failed %d", WSAGetLastError());
			}
		}

		ret = 0;
		ZeroMemory(buffer, BUFFER_SIZE);

		if (clientId == -1)
		{
			if (AssemblePacket(PacketType::ConnectionRequest, "Connection Request", 19))
			{
				// send
				ret = send(clientSocket, sendStack.top().buffer, BUFFER_SIZE, 0);
				if (ret > 0)
				{
					Logger::Log(LOG_INFO, "send: %s", sendStack.top().buffer[2]);
					sendStack.pop();
				}
				else
				{
					Logger::Log(LOG_ERROR, "send() failed %d", WSAGetLastError());
				}
			}
		}
		else
		{
			std::string str = Utils::GetCurrentDateTime();
			const char* now = str.c_str();

			if (AssemblePacket(PacketType::KeepAlive, now, sizeof(str)))
			{
				ret = send(clientSocket, sendStack.top().buffer, BUFFER_SIZE, 0);
				if (ret > 0)
				{
					Logger::Log(LOG_INFO, "send: %s", sendStack.top().buffer[1]);
					sendStack.pop();
				}
				else
				{
					Logger::Log(LOG_ERROR, "send() failed %d", WSAGetLastError());
				}
			}
		}
	}
	else
	{
		FD_CLR(clientSocket, &readfds);
		Logger::Log(LOG_ERROR, "select() failed %d", WSAGetLastError());
		Shutdown();
		return false;
	}

	return true;
}

bool		NetworkClient::Connect()
{
	int ret = connect(clientSocket, (struct sockaddr*)&address, (int)sizeof(address));
	if (ret == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "connect() failed %d", WSAGetLastError());
		//Shutdown();
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "connected %s:%d", SERVERADDRESS, PORT);
	}

	unsigned long arg = 1;
	if (ioctlsocket(clientSocket, FIONBIO, &arg) != 0)
	{
		Logger::Log(LOG_ERROR, "set non-blocking failed %d", WSAGetLastError());
		return false;
	}

	FD_SET(clientSocket, &masterfds);
	return true;
}

bool		NetworkClient::Shutdown()
{
	int ret = closesocket(clientSocket);
	if (ret != 0)
	{
		// closesocket() 설계상의 문제로 우선 로그 안 남기기로
		Logger::Log(LOG_ERROR, "shutdown() failed");
		return false;
	}
	else
	{
		std::cout << "closesocket() executed\n";
		return true;
	}
}

bool		NetworkClient::InitSocket(long sec, int port)
{
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret != 0)
	{
		Logger::Log(LOG_ERROR, "WSAStartup failed %d", WSAGetLastError());
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "WSAStartup success");
	}

	address.sin_family = AF_INET;
	InetPtonW(AF_INET, Utils::StringToPCWSTR(SERVERADDRESS).c_str(), &address.sin_addr.S_un.S_addr);
	address.sin_port = htons(port);

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ret == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "socket creation failed %d", WSAGetLastError());
		Shutdown();
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "socket creation success");
	}

	FD_ZERO(&masterfds);

	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	return true;
}
