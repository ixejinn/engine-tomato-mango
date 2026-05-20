#ifndef MANGO_WINSOCK_CONTEXT
#define MANGO_WINSOCK_CONTEXT
#include "Network/SocketUtil.h"

namespace tomato
{
	/**
	* Creation order requirement:
	* 1. WinsockContext
	* 2. NetDriver
	*
	* NetDriver creates and initializes sockets internally.
	* WSAStartup() is called inside WinsockContext,
	* so it must exist before NetDriver.
	*/

	class WinsockContext
	{
	public:
		WinsockContext() { SocketUtil::InitWinsock(); }
		~WinsockContext() { SocketUtil::CleanUp(); }
	};
}

#endif // !MANGO_WINSOCK_CONTEXT