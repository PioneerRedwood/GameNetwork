#include "predef.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

#include <thread>
#include <mutex>

std::map<int, std::string> maps;
std::mutex mutex;

void RunClient(bool* isActive)
{
	std::cout << std::this_thread::get_id() << "Client working.. \n";

	NetworkClient* client = new NetworkClient();
	
	// Ÿ�̸Ӹ� �����ϴµ��� ����� �������� ���� ������ ��
	auto time = std::chrono::high_resolution_clock::now();
	*isActive = client->Init(1);

	while (*isActive)
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - time).count();

		if (diff > 3)
		{
			time = now;
			std::cout << (bool)*isActive << " working..\n";
			*isActive = client->Loop();
		}
	}
	delete client;
}

int main()
{
	bool* t1_switch = new bool(true);

	std::thread clientThreads1(RunClient, t1_switch);
	
	std::string str;

	while (true)
	{
		std::cin >> str;
		if (str.compare("q") == 0)
		{
			*t1_switch = false;

			break;
		}
		else
		{
			std::cout << str << std::endl << "> ";
		}
	}

	clientThreads1.join();

	return 0;
}