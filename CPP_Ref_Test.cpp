//////////////////////////////////////////////////////////// mutex test

#include "predef.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

#include <thread>
#include <mutex>
#include <unordered_set>

std::mutex _mutex;

template<typename Diff>
void Log(Diff d)
{
	std::cout << "("
		<< std::chrono::duration_cast<std::chrono::milliseconds>(d).count()
		<< "ms)" << std::flush;
}

void Runnit()
{
	// mutex를 해제하는 파괴자를 가진 클래스
	std::lock_guard<std::mutex> guard(_mutex);

	std::cout << "Runnit! " << std::this_thread::get_id() << '\n';
	auto t1 = std::chrono::high_resolution_clock::now();

	volatile int sum = 0;

	for (int i = 0; i < 5; ++i)
	{
		for (int n = 0; n < 10000; ++n)
		{
			for (int m = 0; m < 20000; ++m)
			{
				sum += m * n;
			}
		}
		auto now = std::chrono::high_resolution_clock::now();
		Log(now - t1);
	}

	std::cout << "exit.. " << std::this_thread::get_id() << '\n';
}

int main()
{
	/*char* c = new char[4]();
	sprintf_s(c, 4, "%d", (unsigned short)255);
	int size = (int&)*c;

	std::cout << "cstr: " << &c[0] << " " << &c[1] << " " << &c[2] << " " << &c[3] << std::endl;

	std::cout << size << " char " << c << std::endl;

	char* temp = new char[100 - (sizeof(int) * 2)];
	std::cout << (sizeof(int) << 1);*/

	struct SocketBuffer
	{
		int totalSize;
		int currentSize;

		char buffer[100] = { 0, };

		SocketBuffer()
		{
			totalSize = -1;
			currentSize = 0;

			ZeroMemory(buffer, 100);
		}
	};


	std::deque<SocketBuffer> buffList;

	// those are defined somewhere
	int bufferSize = 100;
	int id = 255;

	// param1: const char* contents
	const char* contents =
		"asdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdasd";
	// param2: size of contents
	unsigned size = 622;
	
	// locale variables
	int contentReadSize = bufferSize - (sizeof(int) << 1);

	unsigned d = size / contentReadSize;
	unsigned i = size % contentReadSize;
	
	SocketBuffer buf;
	int seq = 0;
	// locale variables

	if (d > 0)
	{
		for (seq = 0; seq < d; ++seq)
		{
			ZeroMemory(buf.buffer, bufferSize);

			sprintf_s(&buf.buffer[0], sizeof(int), "%d", id);
			sprintf_s(&buf.buffer[sizeof(int)], sizeof(int), "%d", seq);

			const char* pos = (contents + (seq * contentReadSize));

			memcpy_s(buf.buffer + (sizeof(int) << 1), contentReadSize, pos, contentReadSize);

			buffList.push_back(buf);
		}
	}

	if (i > 0)
	{
		ZeroMemory(buf.buffer, bufferSize);

		sprintf_s(&buf.buffer[0], sizeof(int), "%d", id);
		sprintf_s(&buf.buffer[4], sizeof(int), "%d", seq);

		const char* pos = (contents + seq * contentReadSize);

		memcpy_s(buf.buffer + (sizeof(int) << 1), contentReadSize, pos, i);
		
		sprintf_s(&buf.buffer[bufferSize - sizeof(int)], sizeof(int), "%d", '\0');

		buffList.push_back(buf);
	}

	std::vector<int, std::string> vec;
	std::map<int, char*> _map;
	std::string str = std::string("");

	// 서버 측에서 패킷 파싱
	for (SocketBuffer& buffer : buffList)
	{
		// 해당 id에서 온 패킷의 시퀀스가 총 몇까지 있는지
		// 
		// buffer.buffer[9] ~ 
		if (buffer.buffer[0] == 255)
		{
			// 시퀀스에 맞게 정렬
			
			if (buffer.buffer[1] != 0)
			{
				// point 형식을 쓰는게 좋을듯
				char* buf = new char[contentReadSize];
				memcpy_s(buf, contentReadSize, buffer.buffer + (sizeof(int) << 1), contentReadSize);
				
				std::string tempStr = std::string(buf);
				int tempInt = atoi(buffer.buffer[1]);

				vec.insert(std::make_pair((int)buffer.buffer[1], tempStr));
				str.append(buf);
				delete[] buf;
			}
		}
	}

	std::cout << str << std::flush;

	return 0;
}

////////////////////////////////////////////////// Server Test

#include "predef.h"
#include "NetworkServer.h"

#include <stdio.h>
#include <thread>
#include <mutex>
#include <Windows.h>

void RunServer(bool* isActive)
{
	std::cout << std::this_thread::get_id() << " Server working.. \n";

	NetworkServer* server = new NetworkServer();
	server->Init();

	auto time = std::chrono::high_resolution_clock::now();

	while (true)
	{
		if (!*isActive)
		{
			break;
		}

		auto now = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - time).count();

		if (diff > 3)
		{
			time = now;
			std::cout << (bool)*isActive << " working..\n";
			server->Loop();
		}
	}
	delete server;
}

int __main()
{
	bool* isActive = new bool(true);

	std::thread serverThread(RunServer, isActive);
	std::string str;

	while (true)
	{
		std::cin >> str;
		if (str.compare("q") == 0)
		{
			*isActive = false;
			break;
		}
		else
		{
			std::cout << str << std::endl << "> ";
		}
	}
	serverThread.join();

	return 0;
}