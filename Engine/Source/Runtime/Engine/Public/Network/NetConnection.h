#ifndef MANGO_NETCONNECTION_H
#define MANGO_NETCONNECTION_H

#include <typeinfo>
#include <string>

#include "Network/NetworkFwd.h"
#include "Network/PacketTypes.h"

namespace tomato
{
	/**
	* Stores network information of one connected peer.
	* Packet loss and ordering are not guaranteed.
	* This struct does not perform any network I/O and is used only for storing state.
	*
	* The following fields may be added later:
	*  - State	   : Connecting, Connected, Disconnected
	*  - Heartbeat : time when the last packet was received (lastSeenTime)
	*    If no packet is received for a certain time, the connection is considered lost.
	*/
	class NetBitWriter;
	class NetBitReader;
	struct NetConnection
	{
		uint32_t sessionId{ 0 };

		uint16_t matchId;
		PlayerId playerId;
		std::string name{};
		SocketAddress addr;
	};
}

#endif // !MANGO_NETCONNECTION_H