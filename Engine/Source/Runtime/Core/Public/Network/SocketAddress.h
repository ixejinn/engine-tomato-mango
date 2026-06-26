#ifndef MANGO_SOCKET_ADDRESS_H
#define MANGO_SOCKET_ADDRESS_H

#include <WinSock2.h>
#include <string>
#include <cstdint>

namespace tomato
{
	/**
	* IPv4 기반 sockaddr 래퍼 클래스
	* - 내부적으로 sockaddr / sockaddr_in 구조를 직접 다루지 않도록 캡슐화
	* - 주소 표현을 일관되게 유지하기 위한 목적
	*/
	class SocketAddress
	{
	public:
		SocketAddress(uint32_t inAddress, uint16_t inPort)
		{
			memset(&sockAddr_, 0, sizeof(sockAddr_));
			GetAsSockAddrIn()->sin_family = AF_INET;
			GetIPv4Ref() = htonl(inAddress);
			GetAsSockAddrIn()->sin_port = htons(inPort);
		}

		// 문자열 형태 IPv4주소 초기화 (ex: "127.0.0.1")
		// inet_addr 사용 -> IPv6 미지원
		SocketAddress(const char* inAddress, uint16_t inPort)
		{
			memset(&sockAddr_, 0, sizeof(sockAddr_));
			GetAsSockAddrIn()->sin_family = AF_INET;
			GetIPv4Ref() = inet_addr(inAddress);
			GetAsSockAddrIn()->sin_port = htons(inPort);
		}

		SocketAddress(const sockaddr& inSockAddr)
		{
			memcpy(&sockAddr_, &inSockAddr, sizeof(sockaddr));
		}

		SocketAddress()
		{
			memset(&sockAddr_, 0, sizeof(sockAddr_));
			GetAsSockAddrIn()->sin_family = AF_INET;
			GetIPv4Ref() = INADDR_ANY;
			GetAsSockAddrIn()->sin_port = 0; //automatically allocated
		}

		bool operator==(const SocketAddress& other) const
		{
			return (sockAddr_.sa_family == AF_INET &&
				GetAsSockAddrIn()->sin_port == other.GetAsSockAddrIn()->sin_port) &&
				(GetIPv4Ref() == other.GetIPv4Ref());
		}

		// unordered_map / unordered_set 사용을 위한 해시 생성
		// IPv4 / Port / AddressFamily 조합 기반
		size_t GetHash() const
		{
			return (GetIPv4Ref()) |
				((static_cast<uint32_t>(GetAsSockAddrIn()->sin_port)) << 13) |
				sockAddr_.sa_family;
		}

		void SetPort(uint16_t inPort) { GetAsSockAddrIn()->sin_port = htons(inPort); }

		uint32_t			GetSize()			const { return sizeof(sockaddr); }
		std::string			ToString()			const;
		uint32_t			GetIPv4()			const { return ::ntohl(GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
		uint16_t			GetPort()			const { return ntohs(GetAsSockAddrIn()->sin_port); }

	private:
		friend class UDPSocket;
		friend class TCPSocket;

		sockaddr sockAddr_;

		// sockaddr 내부 IPv4 필드에 대한 직접 참조
		// reinterpret_cast 사용 -> 구조체 레이아웃 변경 시 위험
		uint32_t& GetIPv4Ref() { return *reinterpret_cast<uint32_t*>(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
		const uint32_t& GetIPv4Ref()		const { return *reinterpret_cast<const uint32_t*>(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }

		sockaddr_in* GetAsSockAddrIn() { return reinterpret_cast<sockaddr_in*>(&sockAddr_); }
		const sockaddr_in* GetAsSockAddrIn()	const { return reinterpret_cast<const sockaddr_in*>(&sockAddr_); }

	};
}

namespace std
{
	// SocketAddress를 unordered_map의 key로 사용하기 위한 hash 특수화
	template<> struct hash< tomato::SocketAddress >
	{
		size_t operator()(const tomato::SocketAddress& inAddress) const
		{
			return inAddress.GetHash();
		}
	};
}
#endif // !MANGO_SOCKET_ADDRESS_H