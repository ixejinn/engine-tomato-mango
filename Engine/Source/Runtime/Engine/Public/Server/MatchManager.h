#ifndef MANGO_MATCH_MANAGER_H
#define MANGO_MATCH_MANAGER_H

#include <unordered_map>
#include <queue>
#include <vector>
#include "Containers/SPSCQueue.h"

#include "Network/PacketTypes.h"
#include "Match.h"

namespace tomato
{
	class MatchManager
	{
	public:
		MatchManager(SPSCQueue<MatchRequestCommand, 128>& requestQ, SPSCQueue<SendCommandPtr, 256>& netSendRequestQ)
			: MatchRequestQueue(requestQ), NetSendRequestQueue(netSendRequestQ) {
		};

		void Update(float dt);

		void ProcessMatchRequest();
		void HandleEnqueue(const SessionId& client, const tomato::SocketAddress& inAddress, const std::string& name);
		void HandleCancel(const SessionId& client);
		void HandleIntroResult(const SessionId&, MatchId& matchId, const int& set);

		bool CheckPopulation();
		bool GetMatchRequestFromPQ(MatchRequest& req);
		bool CreateMatchContext(MatchContext& ctx);

		void ProcessMatchResult(float dt);
		void ReQueing(const MatchId& matchId);

	private:
		struct Compare {
			bool operator()(const MatchRequest& a, const MatchRequest& b) const {
				if (a.retryCount != b.retryCount)
					return a.retryCount < b.retryCount;

				return a.enqueueTime > b.enqueueTime;
			}
		};

	private:
		MatchId nextMatchID_ = 1;

		tomato::SPSCQueue<SendCommandPtr, 256>& NetSendRequestQueue;
		tomato::SPSCQueue<MatchRequestCommand, 128>& MatchRequestQueue;

		std::unordered_map<SessionId, MatchRequest> requests;
		std::priority_queue<MatchRequest, std::vector<MatchRequest>, Compare> pq;
		std::unordered_map<MatchId, Match> matches;
	};
}

#endif // !MANGO_MATCH_MANAGER_H
