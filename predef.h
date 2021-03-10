#ifndef _PREDEF_
#define _PREDEF_

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <errno.h>

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <memory>
#include <map>

#include "Utils.cpp"
#include "Logger.h"

#pragma comment(lib, "ws2_32.lib")

//////////////////////////////////////////////////////////

#define		LOG_ERROR			-1
#define		LOG_INFO			1

//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////

#define		SERVERADDRESS		"127.0.0.1"
#define		BUFFER_SIZE			2048				// 2KB
#define		PORT				9000
#define		BACKLOG				5

//////////////////////////////////////////////////////////

#define		MSG_CONNECT_BYTES	0x05
#define		MAX_PAYLOAD_SIZE	1200
#define		TIMEOUT				10

//////////////////////////////////////////////////////////

#endif // !_PREDEF_
