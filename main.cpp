
#include "predef.h"
#include "NetworkServer.h"

#include <chrono>
#include <thread>
#include "NetworkClient.h"


int main()
{
#if 0
	// 1����Ʈ�� 16������ �ִ°� seq(-127~127)������ ������ �� �ϴ�
	// 100����Ʈ
	char* buffer = new char[SIZE];

	// 2,000,000 seq�� ǥ���� �� ������
	// 4����Ʈ�� ����
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
	// server 
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
	}

	server.Shutdown();
#endif
	return 0;
}

// client main
int clientMain()
{
	
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


