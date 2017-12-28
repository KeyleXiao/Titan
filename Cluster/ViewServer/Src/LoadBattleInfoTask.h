#pragma once
#include "IReplayRunnable.h"
#include "ViewRedisData.h"


class Replay;
struct STime;
typedef	std::vector<ViewRedisData::BattleInfo>	VecBattleInfo;

struct LoadBattleInfoTask : public IReplayRunnable<Replay>
{
	string&			m_prepKey;		// 预备信息key
	STime&			m_timeEnd;		// 结束时间

	string&			strKey;		// 战报key

	long			lLength;
	VecBattleInfo	vecInfo;	// 战报List是从右边塞入的，因此要从左边读取



	LoadBattleInfoTask(Replay& t, string& key, string& prepKey, STime& timeEnd)
		: IReplayRunnable(t)
		, strKey(key)
		, m_prepKey(prepKey)
		, m_timeEnd(timeEnd)
	{}

	virtual	bool NeedRecovery() { return false; }

protected:
	virtual void Run() override;

	virtual void Callback(Replay& tHolder) override;
};