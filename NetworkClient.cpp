#include "NetworkClient.h"
#include "predef.h"

///////////////////////////////////////////////////////////////////////////// public

bool		NetworkClient::Init(int interval)
{
	return InitSocket(interval) && Connect();
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
			}
			else
			{
				Logger::Log(LOG_ERROR, "recv() failed");
			}
		}

		// 현재 시각 정보 보내기
		// 나중에 패킷 설계하면 해당하는 패킷 전송하도록
		std::string str = Utils::GetCurrentDateTime();
		const char* now = str.c_str();

		ZeroMemory(buffer, BUFFER_SIZE);
		memcpy(buffer, now, sizeof(str));

		ret = send(clientSocket, buffer, BUFFER_SIZE, 0);
		if (ret > 0)
		{
			Logger::Log(LOG_INFO, "send: %s", buffer);
		}
		else
		{
			Logger::Log(LOG_ERROR, "send() failed");
		}
	}
	else
	{
		FD_CLR(clientSocket, &readfds);
		Logger::Log(LOG_ERROR, "select() failed");
		Shutdown();
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////// public

///////////////////////////////////////////////////////////////////////////// private

bool		NetworkClient::InitSocket(int interval)
{
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret != 0)
	{
		Logger::Log(LOG_ERROR, "WSAStartup failed");
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "WSAStartup success");
	}

	address.sin_family = AF_INET;
	InetPtonW(AF_INET, Utils::StringToPCWSTR(SERVERADDRESS).c_str(), &address.sin_addr.S_un.S_addr);
	address.sin_port = htons(PORT);

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ret == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "socket creation failed");
		Shutdown();
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "socket creation success");
	}

	FD_ZERO(&masterfds);

	timeout.tv_sec = interval;
	timeout.tv_usec = 0;

	return true;
}

bool		NetworkClient::Connect()
{
	int ret = connect(clientSocket, (struct sockaddr*)&address, (int)sizeof(address));
	if (ret == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "connect() failed");
		Shutdown();
		return false;
	}
	else
	{
		Logger::Log(LOG_INFO, "connected %s:%d", SERVERADDRESS, PORT);
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

///////////////////////////////////////////////////////////////////////////// private