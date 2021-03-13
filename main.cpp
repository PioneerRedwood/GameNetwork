#include "predef.h"
#include "NetworkServer.h"

#include <chrono>
#include <thread>
#include "NetworkClient.h"

#define SIZE 128

// server main
int main()
{
#if 0
	// 1바이트에 16진수를 넣는건 seq(-127~127)에서나 가능할 듯 하다
	// 100바이트
	char* buffer = new char[SIZE];

	// 2,000,000 seq를 표현할 수 있을까
	// 4바이트에 저장
	snprintf(&buffer[0], sizeof(int), "%d", MSG_CONNECTION_REQUEST);

	char* buff = new char[sizeof(int)];
	memcpy_s(buff, sizeof(int), &buffer[0], sizeof(int));
	std::cout << buff << std::endl;

	int j = 0;
	for (int i = 0; i < SIZE; ++i)
	{
		std::cout << (j | buffer[i]) << " ";
		std::cout << (j & buffer[i]) << "\n";
	}

#else
	double period = 0.5;

	NetworkServer server;
	DataStorage::GetInstance()->SetValue("filelog", true);
	DataStorage::GetInstance()->SetValue("consolelog", true);

	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

	if (server.Init(period))
	{
		while (true)
		{
			std::chrono::duration<double> now = std::chrono::system_clock::now() - startTime;
			if (now.count() >= period)
			{
				startTime = std::chrono::system_clock::now();
				if (!server.Loop())
				{
					continue;
				}
			}
		}

		//while (true)
		//{
		//	
		//	/*if (server.Connect())
		//	{
		//		
		//	}
		//	else
		//	{
		//		std::chrono::duration<double> now = std::chrono::system_clock::now() - startTime;
		//		if (now.count() >= period)
		//		{
		//			std::cout << "lisening.. \n";
		//			startTime = std::chrono::system_clock::now();
		//			continue;
		//		}
		//	}*/
		//}
	}

	server.Shutdown();

#endif
	return 0;
}

// client main
int clientMain()
{
	/*
		auto tempClient = [](double period)
		{
			DataStorage::GetInstance()->SetValue("filelog", true);

			std::cout << std::this_thread::get_id() << " worked..\n";
			NetworkClient client;
			std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

			if (client.Init(1, PORT))
			{
				if (client.Connect())
				{
					while (true)
					{
						std::chrono::duration<double> now = std::chrono::system_clock::now() - startTime;
						if (now.count() >= period)
						{
							startTime = std::chrono::system_clock::now();
							if (!client.Loop())
							{
								break;
							}
						}
					}
				}
			}
			client.Shutdown();
			std::cout << std::this_thread::get_id() << " exit..\n";
		};

		//std::thread clientThread = std::thread(tempClient, 3.0);
		//serverThread.join();
	*/

	std::cout << std::this_thread::get_id() << " worked..\n";
	double period = 0.25;

	NetworkClient client;
	DataStorage::GetInstance()->SetValue("filelog", true);
	DataStorage::GetInstance()->SetValue("consolelog", true);

	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

	if (client.Init(period, PORT))
	{
		while (true)
		{
			if (client.Connect())
			{
				while (true)
				{
					std::chrono::duration<double> now = std::chrono::system_clock::now() - startTime;
					if (now.count() >= period)
					{
						startTime = std::chrono::system_clock::now();
						if (!client.Loop())
						{
							break;
						}
					}
				}
			}
			else
			{
				std::chrono::duration<double> now = std::chrono::system_clock::now() - startTime;
				if (now.count() >= period)
				{
					startTime = std::chrono::system_clock::now();
					continue;
				}
			}
		}
	}

	client.Shutdown();
	std::cout << std::this_thread::get_id() << " exit..\n";

	return 0;
}