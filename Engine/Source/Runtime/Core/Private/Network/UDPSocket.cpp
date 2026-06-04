#include <WS2tcpip.h>
#include "Network/UDPSocket.h"
#include "Network/SocketAddress.h"
#include "Utils/Logger.h"


namespace tomato
{
	UDPSocketPtr UDPSocket::CreateSocket()
	{
		/**
		 * UDP 기반 게임 패킷 송수신 전용 소켓 생성
		 * TCP가 아닌 이유 :
		 * - 지연 최소화
		 * - 게임 상태는 최신 패킷만 의미 있으므로 신뢰성보다 즉시성이 중요
		 */

		SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (s != INVALID_SOCKET)
			return UDPSocketPtr(new UDPSocket(s));
		else
		{
			TMT_ERR << "Failed to UDPSocket::CreateCocket";
			return nullptr;
		}
	}

	UDPSocket::~UDPSocket()
	{
		// RAII : Socket 객체 생명주기 종료 시 OS 소켓 자원 자동 해제
		closesocket(socket_);
	}

	int UDPSocket::Bind(const SocketAddress& inBindAddress)
	{
		int error = bind(socket_, &inBindAddress.sockAddr_, inBindAddress.GetSize());
		if (error != 0)
		{
			TMT_ERR << "Failed to Socket::Bind";
			return WSAGetLastError();
		}

		return NO_ERROR;
	}

	/**
	* 반환 규칙
	* > 0 : 실제 송신/수신된 바이트 수
	*   0 : non-blocking 모드 특성상 즉시 처리 불가 (WSAEWOULDBLOCK),
				  blocking 모드라면 소켓 동작이 블로킹되었어야 함
	* < 0 : -WSAGetLastError() 형태의 실제 오류 코드
	*
	* 음수로 래핑함으로써, 반환 값의 부호로 오류를 즉시 구분 가능
	*/
	int UDPSocket::SendTo(const void* inToSend, int inLength, const SocketAddress& inToAddress)
	{
		int byteSentCount = sendto(socket_, static_cast<const char*>(inToSend), inLength, 0, &inToAddress.sockAddr_, inToAddress.GetSize());

		if (byteSentCount >= 0)
			return byteSentCount;
		else
		{
			int error = WSAGetLastError();

			if (error == WSAEWOULDBLOCK)
				return 0;
			else
			{
				TMT_ERR << "Failed to Socket::SendTo";
				return -WSAGetLastError();
			}
		}
	}

	int UDPSocket::ReceiveFrom(void* inToReceive, int inMaxLength, SocketAddress& outFromAddress)
	{
		socklen_t fromLength = outFromAddress.GetSize();
		int readByteCount = recvfrom(socket_, static_cast<char*>(inToReceive), inMaxLength, 0, &outFromAddress.sockAddr_, &fromLength);

		if (readByteCount >= 0)
			return readByteCount;
		else
		{
			int error = WSAGetLastError();

			// non-blocking 모드에서 수신 데이터 없음은 정상적인 상황
			if (error == WSAEWOULDBLOCK)
				return 0;
			else if (error == WSAECONNRESET)
			{
				//TMT_INFO << "Connection reset from " << outFromAddress.ToString();
				return -WSAECONNRESET;
			}
			else
			{
				//TMT_ERR << "Failed to Socket::ReceiveFrom";
				return -error;
			}
		}
	}

	int UDPSocket::SetNonBlockingMode(bool nonBlocking)
	{
		/**
		 * 게임 루프 프레임 정지를 방지하기 위해 기본적으로 non-blocking 사용
		 * blocking 모드 전환 시, 네트워크 지연이 전체 프레임 정지를 유발할 수 있음
		 */

		u_long arg = nonBlocking ? 1 : 0;
		int result = ioctlsocket(socket_, FIONBIO, &arg);

		if (result == SOCKET_ERROR)
		{
			TMT_ERR << "Failed to Socket::SetNonBlockingMode";
			return WSAGetLastError();
		}
		else
			return NO_ERROR;
	}
}