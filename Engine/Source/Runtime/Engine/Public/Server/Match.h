#ifndef MAGNO_MATCH_H
#define MANGO_MATCH_H

#include <bitset>
#include <vector>

#include "Containers/SPSCQueue.h"
#include "Network/NetConnection.h"

#include "MatchTypes.h"

namespace tomato
{
	enum class MatchState
	{
		Init,
		InfoSent,
		WaitPeerReady,
		AllReady,
		Failed,
		Processing,
	};

	class Match
	{
	public:
		explicit Match(const MatchContext& ctx);

		MatchUpdateResult Update(float dt, SPSCQueue<SendCommandPtr, 256>& sendRequestQ);

		MatchState CollectNetConnection(const std::vector<MatchRequest>& matchRequest);
		MatchState RequestToSendNetConnection(SPSCQueue<SendCommandPtr, 256>& sendRequestQ, const std::vector<MatchRequest>& matchRequest);

		void SetPeerAck(int idx, int set);

		const uint8_t GetMatchPlayerNum() const { return ctx_.playerNum; }
		const int GetPlayerId(const SessionId& client) const;
		MatchState GetState() const { return ctx_.state; }
		MatchId GetMatchId() const { return ctx_.matchId; }
		const std::vector<MatchRequest>& GetMatchRequest() const { return ctx_.players; }
		const std::vector<NetConnection>& GetNetConnection() const { return conn; }

	private:
		MatchContext ctx_;
		std::vector<NetConnection> conn;

		std::bitset<MatchConstants::MAX_MATCH_PLAYER> peerAck;

		float timer_{ 0.f }; //check for waiting, timeout
	};
}

#endif // !MAGNO_MATCH_H
