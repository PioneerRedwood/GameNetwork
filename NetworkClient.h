#ifndef _NETWORK_CLIENT_
#define _NETWORK_CLIENT_

#include "predef.h"
#include "Packet.cpp"
#include <queue>
#include <stack>

class NetworkClient
{
public:

	struct SocketBuffer
	{
		char buffer[BUFFER_SIZE] = { 0, };

		SocketBuffer()
		{
			ZeroMemory(buffer, BUFFER_SIZE);
		}
	};

	std::stack<SocketBuffer> sendStack;
	std::stack<SocketBuffer> recvStack;

	bool Init(long sec, int port);
	bool Connect();
	bool Loop();

	bool Shutdown();

private:
	int clientId = -1;

	WSAData data;
	fd_set	masterfds;

	struct timeval timeout;

	SOCKET	clientSocket;
	SOCKADDR_IN address;

	char buffer[BUFFER_SIZE];

	bool InitSocket(long sec, int port);

	// 서버로 보낼 패킷 조립
	// 패킷 조립 -> 송신 덱에 넣기까지 성공하면 true or false
	bool AssemblePacket(PacketType type, const char* data, size_t size)
	{
		switch (type)
		{
		case PacketType::ConnectionRequest:
		{
			if (size > BUFFER_SIZE - sizeof(char))
			{
				return false;
			}
			SocketBuffer sockBuffer;
			sockBuffer.buffer[0] = (int)PacketType::ConnectionRequest & 0xff;

			snprintf(&sockBuffer.buffer[1], size, "%s", data);

			sendStack.push(sockBuffer);
			return true;
			break;
		}
		case PacketType::KeepAlive:
		{
			if (size > BUFFER_SIZE - (sizeof(char)))
			{
				return false;
			}
			SocketBuffer sockBuffer;
			sockBuffer.buffer[0] = (int)PacketType::KeepAlive & 0xff;

			snprintf(&sockBuffer.buffer[1], size, "%s", data);
			// memcpy_s가 안된다??..
			//memcpy_s(&sockBuffer.buffer + 1, size, data, size);

			sendStack.push(sockBuffer);
			return true;
			break;
		}
		case PacketType::HugeData:
		{
			break;
		}
		case PacketType::Disconnect:
		{
			break;
		}
		default:
			break;
		}

		return false;
	}

	// 서버에서 온 패킷 파싱
	// 패킷 분해 -> 수신 덱에 넣기까지 성공하면 true or false
	bool ParsingPacket(const char* data, int size)
	{
		PacketType type = (PacketType)(data[0] & 0xff);
		switch (type)
		{
		case PacketType::ConnectionResponse:
		{
			clientId = ((UINT8*)data)[1] << 0;
			return true;
			break;
		}
		case PacketType::KeepAlive:
		{
			SocketBuffer sockBuffer;
			memcpy_s(&sockBuffer.buffer, size, data, size);

			recvStack.push(sockBuffer);
			return true;
			break;
		}
		case PacketType::Multicast:
		{
			break;
		}
		case PacketType::ConnectionDeny:
		{
			break;
		}
		case PacketType::HugeData:
		{
			break;
		}
		case PacketType::Disconnect:
		{
			break;
		}
		default:
			break;
		}

		return false;
	}
};

#endif // !_NETWORKCLIENT_
