#pragma once
#include "IReplayRunnable.h"
#include "ViewRedisData.h"


class Replay;
struct STime;
typedef	std::vector<ViewRedisData::BattleInfo>	VecBattleInfo;

struct LoadBattleInfoTask : public IReplayRunnable<Replay>
{
	string&			m_prepKey;		// Ԥ����Ϣkey
	STime&			m_timeEnd;		// ����ʱ��

	string&			strKey;		// ս��key

	long			lLength;
	VecBattleInfo	vecInfo;	// ս��List�Ǵ��ұ�����ģ����Ҫ����߶�ȡ



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