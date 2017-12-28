#pragma once
#include <string>
#include "ViewDef_.h"

using std::string;

namespace ViewRedisKey
{
	// ս��Key
	inline string BuildReplayKey(ReplayID rID)
	{
		return "" + rID;
	}

	// ս��Ԥ����ϢKey
	inline string BuildReplayPrepKey(ReplayID rID)
	{
		return rID + ":Prep";
	}
};