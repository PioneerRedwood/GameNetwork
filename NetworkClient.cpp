#include "NetworkClient.h"
#include "Logger.h"
#include "Utils.h"

NetworkClient::NetworkClient()
{
	Setup(PORT);
}

NetworkClient::~NetworkClient()
{
	Shutdown();
}

void NetworkClient::Init()
{
	self = nullptr;
	int ret = WSAStartup(MAKEWORD(2,2), &data);
	if (ret != 0)
	{
		Logger::Log(LOG_ERROR, "WSAStartup failed");
		Shutdown();
	}

	char buf[10];

	struct addrinfo* hints = nullptr;
	ZeroMemory(&hints, sizeof(struct addrinfo*));
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_protocol = IPPROTO_TCP;

	ret = GetAddrInfoA(SERVERADDRESS, itoa(PORT, buf, 10), hints, &self);
	if (ret != 0)
	{
		Logger::Log(LOG_ERROR, "GetAddrInfoA() failed");
		Shutdown();
	}

	clientSocket = socket(self->ai_family, self->ai_socktype, self->ai_protocol);
	if (ret == SOCKET_ERROR)
	{
		Logger::Log(LOG_ERROR, "socket creation failed");
		Shutdown();
	}
}

void NetworkClient::Loop()
{
	fd_set readfds;
	FD_ZERO(&readfds);

	readfds = masterfds;

	int ret = select(clientSocket, &readfds, nullptr, nullptr, nullptr);
	if (ret >= 0)
	{
		if (FD_ISSET(clientSocket, &readfds))
		{
			ZeroMemory(buffer, BUFFER_SIZE);

			ret = recv(clientSocket, buffer, BUFFER_SIZE, 0);
			if (ret > 0)
			{
				Logger::Log(LOG_INFO, "received: %s\n",buffer);
			}
		}

		std::string str = Utils::GetCurrentDateTime();
		const char* now = str.c_str();

		ZeroMemory(buffer, BUFFER_SIZE);
		memcpy(buffer, now , sizeof(str));

		ret = send(clientSocket, buffer, BUFFER_SIZE, 0);
		if (ret > 0)
		{
			Logger::Log(LOG_INFO, "send: %s", buffer);
		}
	}
	else
	{
		FD_CLR(clientSocket, &readfds);
		Logger::Log(LOG_ERROR, "select() failed");
		Shutdown();
	}
}

void NetworkClient::Connect()
{
	int ret = connect(clientSocket, )
}