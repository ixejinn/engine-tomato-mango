#ifndef MANGO_TCPNETDRIVER_H
#define MANGO_TCPNETDRIVER_H

#include "Network/NetworkFwd.h"
#include "Network/NetTypes.h"

namespace tomato
{
	
	class TCPNetDriver
	{
	public:
		explicit TCPNetDriver();
		~TCPNetDriver();

		bool InitDedicatedServerMode();
		bool InitClientMode();

		int SendPacket(uint8_t* buffer, int size);
		int RecvPacket(uint8_t* buffer, int size);

		const TCPSocketPtr GetSocket() const { return socket_; }

		bool IsConnectedToServer() const { return connectedToServer; }

	private:
		TCPSocketPtr socket_;

		bool connectedToServer = false;
	};

}

#endif // !MANGO_TCPNETDRIVER_H