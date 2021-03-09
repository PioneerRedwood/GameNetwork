#include "NetworkClient.h"
#include "predef.h"

///////////////////////////////////////////////////////////////////////////// public

bool		NetworkClient::Init(int interval)
{
	return InitSocket(interval);
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

		// ���� �ð� ���� ������
		// ���߿� ��Ŷ �����ϸ� �ش��ϴ� ��Ŷ �����ϵ���
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

bool		NetworkClient::Send(const char* contents, unsigned size)
{
	// �������� ���� ����
	// BUFFER_SIZE�� �ɰ��� �� send ������ �󸶳�(Sequence ����)
	// ���ƾ� �ϴ��� �˾Ƴ��� ��
	// �ɰ� �����͸� BUFFER_SIZE ��ŭ Send

	// �߿�! ���� ������ �����͸� �ɰ��ٸ� Ȯ���ؾ� ��
	// ������ ����ȭ��Ű�� ������ �־������ ������?
	// ����� �������� ������ ��Ŷ���� �־��ָ� ���

	unsigned d = size / BUFFER_SIZE;
	unsigned i = size % BUFFER_SIZE;

	for (int j = 0; j < d; ++j)
	{
		char* buf = new char[BUFFER_SIZE];
		sprintf_s(&buf[0], 4, "%d", clientId);
		sprintf_s(&buf[4], 4, "%d", j);

		int dataSize = BUFFER_SIZE - (sizeof(int) << 1);
		int pos = (int&)*(contents + j * BUFFER_SIZE);

		char* temp = new char[dataSize];

		memcpy_s(temp, dataSize, contents + (j * dataSize), dataSize);
	}

	while (sendSize <= 0)
	{

		// ������ ������ ���
		// SocketBuffer.buffer[0] -> ������	unique key
		// SocketBuffer.buffer[1] -> ����	sequence
		// _ _ _ _ _ _ _ _ 8��Ʈ   -> 256�� ���� ����
		// _ _ _ _ _ _ _ _ .32��Ʈ. _ _ _ _ _ _ _ _ -> 4,294,967,296 ���� ����



	}

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

bool		NetworkClient::Shutdown()
{
	int ret = closesocket(clientSocket);
	if (ret != 0)
	{
		// closesocket() ������� ������ �켱 �α� �� ������
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