#ifndef MANGO_NETCOMMON_H
#define MANGO_NETCOMMON_H

#include <typeinfo>
#include <array>

namespace tomato
{
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
#endif // !MANGO_NETCOMMON_H
