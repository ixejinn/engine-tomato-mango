#ifndef MANGO_SESSION_MANAGER_H
#define MANGO_SESSION_MANAGER_H

#include <chrono>
#include <unordered_map>
#include <memory>
#include <mutex>

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
		bool RemoveSession(const tomato::TCPSocketPtr& socket);
		bool RemoveSession(SessionId id);

		std::unique_ptr<TCPPacket> AppendRecvBuffer(const SessionId& client, const uint8_t* inData, const int& len);
		void AppendSendBuffer(const SessionId& client, const uint8_t* inData, const int& len);

		void SetSessionPort(SessionId& id, uint16_t inPort);

		void GetWritableSockets(std::vector<tomato::TCPSocketPtr>& outVector);
		SessionId GetSessionId(const tomato::TCPSocketPtr& client);
		Session* GetSession(const tomato::TCPSocketPtr& socket);
		tomato::SocketAddress GetAddress(SessionId id);

	private:
		// 락을 잡지 않는 내부 함수, 락 안에서 다른 락 함수를 부르면 데드락
		bool RemoveSessionLocked(SessionId id);

	private:
		std::mutex mtx_;
		std::unordered_map<SessionId, Session> tcpSessions;
		std::unordered_map<tomato::TCPSocketPtr, SessionId> socketToId;

		SessionId nextId_ = 1;
	};
}

#endif // !MANGO_SESSION_MANAGER_H
