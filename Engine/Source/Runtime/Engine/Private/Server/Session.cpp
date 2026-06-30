#include <cstring>
#include "Server/Session.h"

namespace tomato
{
	void Session::AppendRecvBuffer(const uint8_t* data, const int& len)
	{
		recvBuffer.insert(recvBuffer.end(), data, data + len);
	}

	void Session::AppendSendBuffer(const uint8_t* data, const int& len)
	{
		sendBuffer.insert(sendBuffer.end(), data, data + len);
	}

	std::unique_ptr<TCPPacket> Session::ParsePacket()
	{
		if (recvBuffer.size() < sizeof(uint16_t))
			return nullptr;

		uint16_t size;
		std::memcpy(&size, recvBuffer.data(), sizeof(size));

		if (recvBuffer.size() < size)
			return nullptr;

		auto packet = std::make_unique<TCPPacket>(size, id);
		std::memcpy(packet->buffer.data(), recvBuffer.data() + sizeof(uint16_t), size);

		recvBuffer.erase(recvBuffer.begin(), recvBuffer.begin() + size);

		return packet;
	}

	void Session::ConsumeSendBuffer(int len)
	{
		sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + len);
	}
}