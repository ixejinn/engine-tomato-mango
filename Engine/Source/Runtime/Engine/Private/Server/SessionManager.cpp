#include "Server/SessionManager.h"

namespace tomato
{
	void SessionManager::GenerateSession(const TCPSocketPtr& client, const SocketAddress& inAddr)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		if (socketToId.find(client) == socketToId.end())
		{
			socketToId.try_emplace(client, nextId_);
			tcpSessions.try_emplace(nextId_, nextId_, client, inAddr);
			++nextId_;
		}
	}

	bool SessionManager::ValidateSession(const SessionId& id)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		return tcpSessions.find(id) != tcpSessions.end();
	}

	bool SessionManager::ValidateSession(const TCPSocketPtr& client)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		return socketToId.find(client) != socketToId.end();
	}

	bool SessionManager::RemoveSession(const TCPSocketPtr& socket)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		auto it = socketToId.find(socket);
		if (it == socketToId.end())
			return false;

		return RemoveSessionLocked(it->second);
	}

	bool SessionManager::RemoveSession(SessionId id)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		return RemoveSessionLocked(id);
	}

	bool SessionManager::RemoveSessionLocked(SessionId id)
	{
		auto it = tcpSessions.find(id);
		if (it == tcpSessions.end())
			return false;

		socketToId.erase(it->second.GetSocket());
		tcpSessions.erase(it);

		return true;
	}

	std::unique_ptr<TCPPacket> SessionManager::AppendRecvBuffer(const SessionId& client, const uint8_t* inData, const int& len)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		auto it = tcpSessions.find(client);

		if (it == tcpSessions.end())
			return nullptr;

		it->second.AppendRecvBuffer(inData, len);

		return it->second.ParsePacket();
	}

	void SessionManager::AppendSendBuffer(const SessionId& client, const uint8_t* inData, const int& len)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		auto it = tcpSessions.find(client);

		if (it != tcpSessions.end())
			it->second.AppendSendBuffer(inData, len);
	}

	void SessionManager::SetSessionPort(SessionId& id, uint16_t inPort)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		auto it = tcpSessions.find(id);

		if (it != tcpSessions.end())
			it->second.addr.SetPort(inPort);
	}

	void SessionManager::GetWritableSockets(std::vector<TCPSocketPtr>& outVector)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		for (auto& session : tcpSessions)
			outVector.push_back(session.second.GetSocket());
	}

	SessionId SessionManager::GetSessionId(const tomato::TCPSocketPtr& client)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		auto it = socketToId.find(client);

		return it != socketToId.end() ? it->second : 0; // 0 = 없음
	}

	Session* SessionManager::GetSession(const TCPSocketPtr& socket)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		auto sit = socketToId.find(socket);
		if (sit == socketToId.end())
			return nullptr;

		auto it = tcpSessions.find(sit->second);
		return it != tcpSessions.end() ? &it->second : nullptr;
	}

	SocketAddress SessionManager::GetAddress(SessionId id)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		auto it = tcpSessions.find(id);
		if (it != tcpSessions.end())
			return it->second.addr;

		throw std::runtime_error("session id not found");
		return SocketAddress{};
	}
}