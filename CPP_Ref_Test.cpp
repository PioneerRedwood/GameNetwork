//////////////////////////////////////////////////////////// mutex test

#include "predef.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

#include <thread>
#include <mutex>

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

int _main()
{
	std::thread t1(Runnit);
	std::thread t2(Runnit);

	t1.join();
	t2.join();

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