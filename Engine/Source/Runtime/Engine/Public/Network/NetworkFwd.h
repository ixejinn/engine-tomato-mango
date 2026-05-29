#ifndef MANGO_NETWORKFWD_H
#define MANGO_NETWORKFWD_H

#include <memory>
#include <WinSock2.h>

namespace tomato
{
	class SocketAddress;
	
	class UDPSocket;
	class TCPSocket;

	using UDPSocketPtr = std::shared_ptr<UDPSocket>;
	using TCPSocketPtr = std::shared_ptr<TCPSocket>;

	class UDPNetDriver;
	class TCPNetDriver;

	enum class NetMode;

	class NetBitReader;
	class NetBitWriter;

	class ClientNetwork;

	struct NetMassege;
	struct InputCommand;
}

#endif // !MANGO_NETWORKFWD_H
