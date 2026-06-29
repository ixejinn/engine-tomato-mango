#ifndef MANGO_SESSION_H
#define MANGO_SESSION_H

#include <vector>
#include <memory>

#include "Network/SocketAddress.h"
#include "Network/TCPNetDriver.h"
#include "Network/PacketTypes.h"

namespace tomato
{
	class Session
	{
	public:
		Session(SessionId id, const tomato::TCPSocketPtr& socket, const tomato::SocketAddress& addr) : id(id), socket(socket), addr(addr) {};

		void AppendRecvBuffer(const uint8_t* data, const int& len);
		void AppendSendBuffer(const uint8_t* data, const int& len);

		std::unique_ptr<TCPPacket> ParsePacket();
		void ConsumeSendBuffer(int len);

		void SetPort(uint16_t inPort) { addr.SetPort(inPort); }

		const tomato::TCPSocketPtr& GetSocket() const { return socket; }
		const tomato::SocketAddress& GetSocketAddress() const { return addr; }
		std::vector<uint8_t>& GetRecvBuffer() { return recvBuffer; }
		std::vector<uint8_t>& GetSendBuffer() { return sendBuffer; }

		//private:
			//tomato::TCPNetDriver driver;
		SessionId id;
		tomato::TCPSocketPtr socket;
		tomato::SocketAddress addr;
		std::vector<uint8_t> recvBuffer;
		std::vector<uint8_t> sendBuffer;
	};
}
#endif // !MANGO_SESSION_H
