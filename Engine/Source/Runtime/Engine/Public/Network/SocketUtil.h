#ifndef MANGO_SOCKET_UTIL_H
#define MANGO_SOCKET_UTIL_H

#include <vector>
#include "Network/NetworkFwd.h"

namespace tomato
{
	class SocketUtil
	{
	public:
		/**
		 * WinSock 초기화
		 * 프로그램 시작 시 단 한 번만 호출되어야 하며,
		 * CreateSocket 이전에 반드시 선행
		 */
		static bool InitWinsock();

		/**
		 * Winsock 종료 처리
		 * 프로그램 종료 시 호출
		 */
		static void CleanUp();

		static int Select(const std::vector<TCPSocketPtr>* inReadSet,
			std::vector<TCPSocketPtr>* outReadSet,
			const std::vector<TCPSocketPtr>* inWriteSet,
			std::vector<TCPSocketPtr>* outWriteSet,
			const std::vector<TCPSocketPtr>* inExceptSet,
			std::vector<TCPSocketPtr>* outExceptSet);

	private:
		inline static fd_set* FillSetFromVector(fd_set& outSet, const std::vector<TCPSocketPtr>* inSockets, int& ioNaxNfds);
		inline static void FillVectorFromSet(std::vector<TCPSocketPtr>* outSockets, const std::vector<TCPSocketPtr>* inSockets, const fd_set& inSet);
	};
}

#endif // !MANGO_SOCKET_UTIL_H
