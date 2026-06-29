#ifndef MAGNO_MATCH_H
#define MANGO_MATCH_H

#include <bitset>

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

		MatchState CollectNetConnection(const MatchRequest* matchRequest);
		MatchState RequestToSendNetConnection(SPSCQueue<SendCommandPtr, 256>& sendRequestQ, const MatchRequest* matchRequest);

		void SetPeerAck(int idx, int set);

		const int GetPlayerId(const SessionId& client) const;
		MatchState GetState() const { return ctx_.state; }
		MatchId GetMatchId() const { return ctx_.matchId; }
		const MatchRequest* GetMatchRequest() const { return ctx_.players; }
		const NetConnection* GetNetConnection() const { return conn; }

	private:
		MatchContext ctx_;
		NetConnection conn[MatchConstants::MAX_MATCH_PLAYER];

		std::bitset<MatchConstants::MAX_MATCH_PLAYER> peerAck;

		float timer_{ 0.f }; //check for waiting, timeout
	};
}

#endif // !MAGNO_MATCH_H
