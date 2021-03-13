#ifndef _NETWORK_SERVER_
#define _NETWORK_SERVER_

#include "predef.h"
#include "Packet.cpp"
#include <stack>

class NetworkServer
{
public:

	NetworkServer()
	{
		Setup();
	}
	~NetworkServer()
	{
		closesocket(masterSocket);
	}

	struct SocketBuffer
	{
		char buffer[BUFFER_SIZE] = { 0, };

		SocketBuffer()
		{
			ZeroMemory(buffer, BUFFER_SIZE);
		}
	};

	bool Init(long sec);
	void Shutdown();
	bool Loop();
	bool Connect();

	uint16_t	SendMessage(uint16_t conn, const char* messageBuffer)
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

private:
	WSAData data;
	fd_set masterfds;
	fd_set tempfds;

	struct timeval timeout;

	uint16_t maxfd;

	SOCKET masterSocket;
	SOCKET tempSocket;

	sockaddr_in	server_addr;

	char buffer[BUFFER_SIZE];

	void Setup();
	bool InitSocket(long sec);
	bool Receive(SOCKET fd);

	unsigned int clientId = 0;
	std::deque<SOCKET> clientDeque;
	std::map<SOCKET, unsigned int> clientMap;

	std::stack<SocketBuffer> sendStack;

	// 클라이언트로 보낼 패킷 조립
	int AssemblePacket(PacketType type, const char* data, size_t size)
	{
		switch (type)
		{
		case PacketType::ConnectionResponse:
		{
			if (size > (BUFFER_SIZE - (sizeof(int) << 1)))
			{
				return -1;
			}
			
			// '\0'로 끝나니까 최대로 읽어봤자 앞에꺼 밖에 데이터가 없음..
			// null 종료 문자열에서는 사용해봤자 소용 없음

			SocketBuffer sockBuffer;
			sockBuffer.buffer[0] = (int)(PacketType::ConnectionResponse) & 0xff;
			sockBuffer.buffer[1] = (int)clientId & 0xff;

			snprintf(&sockBuffer.buffer[2], size, "%s", data);
			
			sendStack.push(sockBuffer);
			return 1;
			break;
		}
		case PacketType::KeepAlive:
		{	
			
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

		return -1;
	}

	// 클라이언트에서 온 패킷 파싱
	bool ParsingPacket()
	{

	}
};

#endif
