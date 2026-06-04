#include "Network/SocketUtil.h"
#include "Network/TCPSocket.h"
#include "Utils/Logger.h"

namespace tomato
{
	bool SocketUtil::InitWinsock()
	{
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != NO_ERROR)
		{
			TMT_ERR << "Failed to SocketUtil::InitWinsock";
			return false;
		}
		return true;
	}

	void SocketUtil::CleanUp()
	{
		WSACleanup();
	}

	int SocketUtil::Select(const std::vector<TCPSocketPtr>* inReadSet, std::vector<TCPSocketPtr>* outReadSet, const std::vector<TCPSocketPtr>* inWriteSet, std::vector<TCPSocketPtr>* outWriteSet, const std::vector<TCPSocketPtr>* inExceptSet, std::vector<TCPSocketPtr>* outExceptSet)
	{
		// nfds means "the highest socket descriptor + 1".
		// nfds is required by POSIX select() (max fd + 1).
		// Winsock ignores this value, but we keep it for portability.
		int nfds = 0;

		fd_set read, write, except;

		fd_set* readPtr = FillSetFromVector(read, inReadSet, nfds);
		fd_set* writePtr = FillSetFromVector(write, inWriteSet, nfds);
		fd_set* exceptPtr = FillSetFromVector(except, inExceptSet, nfds);

		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;

		int toRet = select(nfds + 1, readPtr, writePtr, exceptPtr, &timeout);
		if (toRet > 0)
		{
			FillVectorFromSet(outReadSet, inReadSet, read);
			FillVectorFromSet(outWriteSet, inWriteSet, write);
			FillVectorFromSet(outExceptSet, inExceptSet, except);
		}

		return toRet;
	}

	inline fd_set* SocketUtil::FillSetFromVector(fd_set& outSet, const std::vector<TCPSocketPtr>* inSockets, int& ioNaxNfds)
	{
		if (inSockets)
		{
			FD_ZERO(&outSet);

			for (const TCPSocketPtr& socket : *inSockets)
			{
				FD_SET(socket->socket_, &outSet);
#if !_WIN32
				ioNaxNfds = std::max(ioNaxNfds, socket->socket_);
#endif // !_WIN32
			}
			return &outSet;
		}
		else
			return nullptr;
	}

	inline void SocketUtil::FillVectorFromSet(std::vector<TCPSocketPtr>* outSockets, const std::vector<TCPSocketPtr>* inSockets, const fd_set& inSet)
	{
		if (inSockets && outSockets)
		{
			outSockets->clear();
			for (const TCPSocketPtr& socket : *inSockets)
			{
				if (FD_ISSET(socket->socket_, &inSet))
					outSockets->push_back(socket);
			}
		}
	}

}