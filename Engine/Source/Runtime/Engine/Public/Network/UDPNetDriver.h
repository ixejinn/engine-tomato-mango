#ifndef MANGO_UDPNETDRIVER_H
#define MANGO_UDPNETDRIVER_H

#include "Network/NetworkFwd.h"
#include "Network/NetTypes.h"
namespace tomato
{
	/**
	* Handles OS-level socket creation, initialization, and shutdown.
	* Performs UDP recv/send and passes received packets to the upper layer (NetworkService).
	* All external network I/O, including communication with the match server, is handled here.
	*
	* This class does not manage threads and only performs network I/O.
	* It does not access game logic or NetConnection directly.
	*
	* Implemented for local environment.
	*/
	class UDPNetDriver
	{
	public:
		UDPNetDriver();
		~UDPNetDriver();

		bool InitSocket();
		bool SendPacket(const void* buffer, int& byteSentCount, const SocketAddress& inToAddress);
		// Returns true if data was received, false otherwise.
		bool RecvPacket(void* buffer, int& receivedBytes, SocketAddress& outFromAddress);

		const uint16_t& GetPort() const { return port; }
	private:

		uint16_t port;
		UDPSocketPtr socket_;
	};

}

#endif // !TOMATO_NETDRIVER_H