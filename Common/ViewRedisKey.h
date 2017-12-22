#pragma once
#include <string>
#include "ViewDef_.h"

using std::string;

struct ViewRedisKey
{
	static inline string BuildReplayKey(ReplayID rID)
	{
		return "" + rID;
	}
};