#ifndef MANGO_WINSOCK_INITIALIZER_H
#define MANGO_WINSOCK_INITIALIZER_H
#include "Network/SocketUtil.h"

namespace tomato
{
	/**
	* Creation order requirement:
	* 1. Winsock Initializer
	* 2. NetDriver
	*
	* NetDriver creates and initializes sockets internally.
	* WSAStartup() is called inside Winsock Initializer,
	* so it must exist before NetDriver.
	*/

	class WinsockInitializer
	{
	public:
		WinsockInitializer() { SocketUtil::InitWinsock(); }
		~WinsockInitializer() { SocketUtil::CleanUp(); }
	};
}

#endif // !MANGO_WINSOCK_INITIALIZER_H