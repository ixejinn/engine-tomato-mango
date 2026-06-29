#ifndef MANGO_SESSION_MANAGER_H
#define MANGO_SESSION_MANAGER_H

#include <chrono>
#include <unordered_map>
#include <memory>

#include "Session.h"
#include "Network/PacketTypes.h"

namespace tomato
{
	class SessionManager
	{
	public:
		// For TCP
		void GenerateSession(const tomato::TCPSocketPtr& client, const tomato::SocketAddress& inAddr);
		bool ValidateSession(const SessionId& id);
		bool ValidateSession(const tomato::TCPSocketPtr& client);
		bool RemoveSession(const tomato::TCPSocketPtr& client);

		std::unique_ptr<TCPPacket> AppendRecvBuffer(const SessionId& client, const uint8_t* inData, const int& len);
		void AppendSendBuffer(const SessionId& client, const uint8_t* inData, const int& len);

		void SetSessionPort(SessionId& id, uint16_t inPort);

		void GetWritableSockets(std::vector<tomato::TCPSocketPtr>& outVector);
		SessionId GetSessionId(const tomato::TCPSocketPtr& client) { return socketToId[client]; }
		Session* GetSession(const tomato::TCPSocketPtr& socket);
		const tomato::SocketAddress& GetAddress(const SessionId& id);

	private:
		std::unordered_map<SessionId, Session> tcpSessions;
		std::unordered_map<tomato::TCPSocketPtr, SessionId> socketToId;

		SessionId nextId_ = 1;
	};
}

#endif // !MANGO_SESSION_MANAGER_H
