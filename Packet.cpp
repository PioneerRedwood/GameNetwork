#ifndef _PACKET_
#define _PACKET_

#include "predef.h"

///
///	Packet Data Dictionary ��Ŷ ������ ����
/// 
/// �ش� ������ ���� ��Ŷ�� �Ľ� ������ �޶����ϴ�.
/// �׻� ù��° 4����Ʈ���� �ش� ������ �����Ͱ� ���ϴ�. (16������ ǥ��. snprintf() �Լ��� ���. ���� 000 ~ 999)
/// Conncetion Request	-- ���� ��û(Ŭ���̾�Ʈ���� ������) -- 0x010 -- Prefix
/// Connection Response -- ���� �㰡(�������� Ŭ���̾�Ʈ��) -- 0x011 -- Prefix
/// Connection Deny		-- ���� �ź�(�������� Ŭ���̾�Ʈ��) -- 0x012 -- Prefix
/// Keep Alive			-- ���� ����(���� ��� ����)		- 0x013 -- 
/// Huge Data			-- ��뷮 ���۽�(���� ��� ����)		- 0x014 --
/// Disconnect			-- ���� ����(���� ��� ����)		- 0x015 --
/// Multicast			-- ��Ƽĳ��Ʈ(�������� Ŭ���̾�Ʈ��) - 0x016 -- 
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
