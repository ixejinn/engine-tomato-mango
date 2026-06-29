#include "Server/SessionManager.h"

namespace tomato
{
	void SessionManager::GenerateSession(const TCPSocketPtr& client, const SocketAddress& inAddr)
	{
		if (socketToId.find(client) == socketToId.end())
		{
			socketToId.try_emplace(client, nextId_);
			tcpSessions.try_emplace(nextId_, nextId_, client, inAddr);
			++nextId_;
		}
	}

	bool SessionManager::ValidateSession(const SessionId& id)
	{
		return tcpSessions.find(id) != tcpSessions.end();
	}

	bool SessionManager::ValidateSession(const TCPSocketPtr& client)
	{
		return socketToId.find(client) != socketToId.end();
	}

	bool SessionManager::RemoveSession(const TCPSocketPtr& client)
	{
		auto it = socketToId.find(client);

		if (it == socketToId.end())
			return false;

		tcpSessions.erase(it->second);
		socketToId.erase(client);

		return true;
	}

	std::unique_ptr<TCPPacket> SessionManager::AppendRecvBuffer(const SessionId& client, const uint8_t* inData, const int& len)
	{
		auto it = tcpSessions.find(client);

		if (it == tcpSessions.end())
			return nullptr;

		it->second.AppendRecvBuffer(inData, len);

		return it->second.ParsePacket();
	}

	void SessionManager::AppendSendBuffer(const SessionId& client, const uint8_t* inData, const int& len)
	{
		auto it = tcpSessions.find(client);

		if (it != tcpSessions.end())
			it->second.AppendSendBuffer(inData, len);
	}

	void SessionManager::SetSessionPort(SessionId& id, uint16_t inPort)
	{
		auto it = tcpSessions.find(id);

		if (it != tcpSessions.end())
			it->second.addr.SetPort(inPort);
	}

	void SessionManager::GetWritableSockets(std::vector<TCPSocketPtr>& outVector)
	{
		for (auto& session : tcpSessions)
			outVector.push_back(session.second.GetSocket());
	}

	Session* SessionManager::GetSession(const TCPSocketPtr& socket)
	{
		auto it = tcpSessions.find(socketToId.find(socket)->second);
		if (it != tcpSessions.end())
			return &it->second;

		return nullptr;
	}

	const SocketAddress& SessionManager::GetAddress(const SessionId& id)
	{
		auto it = tcpSessions.find(id);
		if (it != tcpSessions.end())
			return it->second.addr;
	}
}