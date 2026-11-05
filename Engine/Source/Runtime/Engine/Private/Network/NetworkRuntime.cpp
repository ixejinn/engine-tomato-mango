#include "Network/NetworkRuntime.h"

namespace tomato
{
	bool NetworkRuntime::ShoudRollback(uint64_t currT, int64_t& outLateT)
	{
		if (!client_) return false; // alone 모드일 때

		auto lateT = client_->GameNet().GetConfirmedTick();
		if (currT > lateT && currT - lateT <= ROLLBACK_WINDOW)
		{
			outLateT = lateT;
			return true;
		}
		return false;
	}
}