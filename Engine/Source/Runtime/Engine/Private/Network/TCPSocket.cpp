#include "Network/TCPSocket.h"
#include "Network/SocketAddress.h"
#include "Utils/Logger.h"

#include <WS2tcpip.h>

namespace tomato
{
	TCPSocket::~TCPSocket()
	{
		closesocket(socket_);
	}

	TCPSocketPtr TCPSocket::CreateTCPSocket()
	{
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (s != INVALID_SOCKET)
		{
			return TCPSocketPtr(new TCPSocket(s));
		}
		else
		{
			TMT_ERR << "Failed to TCPSocket::CreateCocket";
			return nullptr;
		}
	}

	int TCPSocket::Connect(const SocketAddress& inAddress)
	{
		int err = connect(socket_, &inAddress.sockAddr_, inAddress.GetSize());
		if (err < 0)
		{
			TMT_ERR << "Failed to TCPSocket::Connect";
			return -WSAGetLastError();
		}

		return NO_ERROR;
	}

	int TCPSocket::Bind(const SocketAddress& inToAddress)
	{
		int err = bind(socket_, &inToAddress.sockAddr_, inToAddress.GetSize());
		if (err != 0)
		{
			TMT_ERR << "Failed to TCPSocket::Bind";
			return WSAGetLastError();
		}

		return NO_ERROR;
	}

	int TCPSocket::Listen(int inBackLog)
	{
		int err = listen(socket_, inBackLog);
		if (err < 0)
		{
			TMT_ERR << "Failed to TCPSocket::Listen";
			return -WSAGetLastError();
		}

		return NO_ERROR;
	}

	std::shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress& inFromAddress)
	{
		socklen_t length = inFromAddress.GetSize();
		SOCKET newSocket = accept(socket_, &inFromAddress.sockAddr_, &length);

		if (newSocket != INVALID_SOCKET)
			return TCPSocketPtr(new TCPSocket(newSocket));
		else
		{
			TMT_ERR << "Failed to TCPSocket::Accept";
			return nullptr;
		}
	}

	int TCPSocket::Send(const void* inData, int inLen)
	{
		int bytesSentCount = send(socket_, static_cast<const char*>(inData), inLen, 0);
		if (bytesSentCount < 0)
		{
			TMT_ERR << "Failed to TCPSocket::Send";
			return -WSAGetLastError();
		}

		return bytesSentCount;
	}

	int TCPSocket::Receive(void* inBuffer, int inLen)
	{
		int bytesReceivedCount = recv(socket_, static_cast<char*>(inBuffer), inLen, 0);
		if (bytesReceivedCount < 0)
		{
			TMT_ERR << "Failed to TCPSocket::Receive";
			return -WSAGetLastError();
		}

		return bytesReceivedCount;
	}

	int TCPSocket::GetSocketAddress(SocketAddress& inFromAddress, int inLen)
	{
		int error = getpeername(socket_, &inFromAddress.sockAddr_, &inLen);
		if (error != SOCKET_ERROR)
			return error;
		else
		{
			TMT_ERR << "Failed to TCPSocket::GetSocketAddress";
			return WSAGetLastError();
		}
	}
}