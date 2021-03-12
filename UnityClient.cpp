#include "predef.h"
#include "NetworkClient.h"

#include <chrono>
#include <thread>

#define UNITY_DLL	__declspec(dllexport)

extern "C"
{
	bool isClientAlive		= false;
	bool isConnected		= false;
	NetworkClient* client	= new NetworkClient();

	UNITY_DLL bool			Init(unsigned period, int port)
	{
		isClientAlive = client->Init(period, port);
		return isClientAlive;
	}

	UNITY_DLL bool			Connect()
	{
		if (!isClientAlive)
		{
			isConnected = client->Connect();
			return isConnected;
		}
		else
		{
			return false;
		}
	}

	/*UNITY_DLL bool			Send(const char* contents, unsigned size)
	{
		if (isClientAlive && isConnected)
		{
			return false;
		}
		else
		{
			return client->Send(contents, size);
		}
	}*/

	UNITY_DLL std::string	PopDataInDeque()
	{
		if (client->bufferDeque.empty())
		{
			return std::string("");
		}
		else
		{
			std::string retStr = std::string(client->bufferDeque[0].buffer);
			client->bufferDeque.pop_back();
			return retStr;
		}
	}

	UNITY_DLL bool			Update()
	{
		return client->Loop();
	}
}

int main()
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

	if (client.Init(period ,PORT))
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