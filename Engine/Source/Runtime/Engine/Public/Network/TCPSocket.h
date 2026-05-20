#ifndef MANGO_TCP_SOCKET_H
#define MANGO_TCP_SOCKET_H

#include <typeinfo>

#include "Network/NetworkFwd.h"

namespace tomato
{
	class TCPSocket
	{
	public:

		~TCPSocket();

		static TCPSocketPtr CreateTCPSocket();

		int Connect(const SocketAddress& inAddress);
		int Bind(const SocketAddress& inToAddress);
		int Listen(int inBackLog = 32);
		TCPSocketPtr Accept(SocketAddress& inFromAddress);
		int Send(const void* inData, int inLen);
		int Receive(void* inBuffer, int inLen);

		int GetSocketAddress(SocketAddress& inFromAddress, int inLen);
	private:
		friend class SocketUtil;
		TCPSocket(SOCKET inSocket) : socket_(inSocket) {};
		SOCKET socket_;
	};
}

#endif // !TOMATO_TCP_SOCKET_H