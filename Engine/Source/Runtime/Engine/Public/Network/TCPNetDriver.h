#ifndef MANGO_TCPNETDRIVER_H
#define MANGO_TCPNETDRIVER_H

#include "Network/NetworkFwd.h"
#include "Network/NetworkConfig.h"
#include "Network/NetTypes.h"
#include "Network/SocketAddress.h"
namespace tomato
{
	
	class TCPNetDriver
	{
	public:
		explicit TCPNetDriver();
		~TCPNetDriver();

		bool InitDedicatedServerMode();
		bool InitClientMode(const SocketAddress& serverAddr);
		bool InitClientMode();

		int SendPacket(uint8_t* buffer, int size);
		int RecvPacket(uint8_t* buffer, int size);

		int WaitReable(int timeoutMs);
		void Shutdown();

		const TCPSocketPtr GetSocket() const { return socket_; }

	private:
		TCPSocketPtr socket_;
	};

}

#endif // !MANGO_TCPNETDRIVER_H