#ifndef MANGO_TCPNETDRIVER_H
#define MANGO_TCPNETDRIVER_H

#include "Network/NetworkFwd.h"
#include "Network/NetCommon.h"

namespace tomato
{
	
	class TCPNetDriver
	{
	public:
		explicit TCPNetDriver(NetMode mode);
		~TCPNetDriver();

		bool InitListenServerMode();
		bool InitClientMode();

		int SendPacket(uint8_t* buffer, int size);
		int RecvPacket(uint8_t* buffer, int size);

		const TCPSocketPtr GetSocket() const { return socket_; }

		bool IsConnectedToServer() const { return connectedToServer; }
	private:

		NetMode mode_;
		TCPSocketPtr socket_;

		bool connectedToServer = false;
	};

}

#endif // !MANGO_TCPNETDRIVER_H