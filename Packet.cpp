#include "predef.h"
#include "StreamWrapper.h"

namespace Packet
{
	// =============================================================== //
	//|																  |//
	//|	H e A d e R													  |//
	//|																  |//
	//|																  |//
	//|																  |//
	//|																  |//
	//|																  |//
	//|																  |//
	//|																  |//
	//|																  |//
	//|																  |//
	//|																  |//
	//|																  |//
	// =============================================================== //


	// HEADER
	//| [type: 2byte] || [Sequence: 4byte] || []|//


	enum class PacketType
	{
		ConnectionRequest,
		ConnectionResponse,
		ConnectionDenied,
		KeepAlive,
		Disconnect,
		Payload,
		InvalidPacket,
	};

	class PacketHeader
	{
	
	};

	class ConnectionRequestPacket
	{
		
	};

	class ConnectionResponsePacket
	{
		
	};

	class KeepAlivePacket
	{
		
	};

	class PayloadPacket
	{
		
	};

	class ConnectionDenyPacket
	{
		
	};
}
