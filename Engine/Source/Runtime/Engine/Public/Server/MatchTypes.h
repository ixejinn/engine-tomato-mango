#ifndef MANGO_MATCH_TYPES_H
#define MANGO_MATCH_TYPES_H

#include <typeinfo>
#include <vector>

#include "Network/TCPSocket.h"
#include "Network/PacketTypes.h"

namespace tomato
{
	namespace MatchConstants
	{
		constexpr uint8_t		MAX_MATCH_PLAYER = 1;
		constexpr float			CONNECT_TIMEOUT_SEC = 10.0f;
		constexpr ServerTimeMs	START_SERVER_TIME = 10000;
	}

	enum class MatchRequestAction : uint8_t
	{
		NONE,
		Enqueue,
		Cancel,
		Success,
		Failed,

		COUNT
	};

	struct MatchRequestCommand
	{
		SessionId sessionId;
		tomato::SocketAddress address;
		std::string name;
		MatchId matchId;
		MatchRequestAction action;
	};

	struct SendRequestCommand
	{
		SessionId sessionId;
		uint16_t size;
		std::vector<uint8_t> data;

		explicit SendRequestCommand(SessionId id, uint16_t size)
			: sessionId(id), size(size), data(size) {
		}
	};
	using SendCommandPtr = std::unique_ptr<SendRequestCommand>;

	struct MatchRequest
	{
		SessionId sessionId;
		tomato::SocketAddress address;
		std::string name;
		RequestId requestId;
		ServerTimeMs enqueueTime;
		int retryCount;
	};

	enum class MatchState;
	struct MatchContext
	{
		MatchId matchId;
		MatchState state;
		MatchRequest players[MatchConstants::MAX_MATCH_PLAYER];
	};

	enum class MatchUpdateResult
	{
		None,
		ReadyToStart,
		Failed,

		COUNT
	};
}

#endif // !MANGO_MATCH_TYPES_H