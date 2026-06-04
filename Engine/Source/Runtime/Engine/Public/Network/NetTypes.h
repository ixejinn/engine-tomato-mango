#ifndef MANGO_NETTYPES_H
#define MANGO_NETTYPES_H

#include <cstdint>
#include <array>

namespace tomato
{
	using PlayerId = uint8_t;
	using MatchId = uint16_t;
	using RequestId = uint32_t;
	using SessionId = uint32_t;
	using ServerTimeMs = uint32_t;

	enum class NetMode
	{
		NM_Alone,
		NM_ListenServer,
		NM_Client
	};

	/// Maximum payload byte size.
/// IP fragmentation을 피하기 위해 MTU 미만으로 제한.
	constexpr uint16_t MAX_PACKET_SIZE{ 1024 };
	/// Raw byte buffer for network
	using RawBuffer = std::array<uint8_t, MAX_PACKET_SIZE>;
}

#endif // !MANGO_NETTYPES_H
