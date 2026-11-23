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
		void HandleEnqueue(SessionId client, const SocketAddress& inAddress, const std::string& name);
		void HandleCancel(SessionId client);
		void HandleIntroResult(SessionId client, MatchId matchId, int set);
		void HandleMatchingCancel(SessionId client, MatchId matchId);

		void HandleGameEnd(SessionId client, MatchId matchId);
		void HandleLeave(SessionId client, MatchId matchId);
		void HandleDisconnect(SessionId client);
		bool MarkPlayer(SessionId client, MatchId matchId, MatchPlayerState state); // activeMatches의 client state를 갱신

		void SendCancelResult(SessionId to, bool result);
		void SendPeerStatus(SessionId to, MatchId matchId, uint8_t targetPlayerId, PeerState peerState);
		void FinishIfDone(MatchId matchId); // 끝난 게임 정리

		bool CheckPopulation();
		bool CheckMatchTime(float dt);
		bool GetMatchRequestFromPQ(MatchRequest& req);
		bool CreateMatchContext(MatchContext& ctx);

		void ProcessMatchResult(float dt);
		void ReQueing(const MatchId& matchId);
		void RemoveRequest(const SessionId& client);

	private:
		void PrintActiveMatches(); // debug

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

		std::unordered_map<SessionId, ClientMatchInfo> clientInfo_; /* 매치 Cancel 판단을 위한 맵 */
		std::unordered_map<MatchId, ActiveMatch> activeMatches_;	/* 게임 중 피어 연결 관리를 위한 맵 */

		float waitTime{ 0.f };
		uint8_t matchPlayerNum;
	};
}

#endif // !MANGO_MATCH_MANAGER_H
