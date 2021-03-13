#ifndef _PACKET_
#define _PACKET_

#include "predef.h"

///
///	Packet Data Dictionary 패킷 데이터 도감
/// 
/// 해당 유형에 따라 패킷별 파싱 과정이 달라집니다.
/// 항상 첫번째 4바이트에는 해당 유형의 데이터가 담깁니다. (16진수로 표현. snprintf() 함수를 사용. 범위 000 ~ 999)
/// Conncetion Request	-- 연결 요청(클라이언트에서 서버로) -- 0x010 -- Prefix
/// Connection Response -- 연결 허가(서버에서 클라이언트로) -- 0x011 -- Prefix
/// Connection Deny		-- 연결 거부(서버에서 클라이언트로) -- 0x012 -- Prefix
/// Keep Alive			-- 연결 지속(양측 모두 가능)		- 0x013 -- 
/// Huge Data			-- 대용량 전송시(양측 모두 가능)		- 0x014 --
/// Disconnect			-- 연결 해제(양측 모두 가능)		- 0x015 --
/// Multicast			-- 멀티캐스트(서버에서 클라이언트로) - 0x016 -- 
/// 

enum class PacketType
{
	ConnectionRequest = 0x010,
	ConnectionResponse = 0x011,
	ConnectionDeny = 0x012,
	KeepAlive = 0x013,
	HugeData = 0x014,
	Disconnect = 0x015,
	Multicast = 0x016,
};

#endif // !_PACKET_
