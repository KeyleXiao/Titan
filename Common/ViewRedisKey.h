#pragma once
#include <string>
#include "ViewDef_.h"

using std::string;

namespace ViewRedisKey
{
	// 战报Key
	inline string BuildReplayKey(ReplayID rID)
	{
		return "" + rID;
	}

	// 战报预备信息Key
	inline string BuildReplayPrepKey(ReplayID rID)
	{
		return rID + ":Prep";
	}
};