#ifndef _STREAM_
#define _STREAM_

#include <string>

/// <summary>
/// 4����Ʈ ���� �ڷḦ �аų� ���µ� ������ �ִ� Ŭ����
/// System.IO.Stream�� Ŭ������ ����
/// 
/// Read & Write ���� ����..
/// </summary>

class Stream
{
public:
	Stream();
	~Stream() { if (buffer != nullptr) delete[] buffer; }

	int GetPosition() { return position; }

	int ReadBytes(char* val, int count);
	void WriteBytes(const char* val, int count);

private:
	int position = 0;
	char* buffer;
};

#endif // _STERAM_