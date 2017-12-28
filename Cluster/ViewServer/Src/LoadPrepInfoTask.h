#pragma once
#include "IReplayRunnable.h"
#include "ViewRedisData.h"


using namespace ViewRedisData;
struct SMsgView_VC_BattlePrep;
struct STime;

struct LoadPrepInfoTask : public IReplayRunnable<Replay>
{
	string&						m_prepKey;			// 预备信息key
	SMsgView_VC_BattlePrep&		m_battlePrepInfo;	// 预备信息
	bool						m_bReadedPrep;		// 预备信息是否读取完

	STime&				m_timeBegin;	// 开始时间
	STime&				m_timeEnd;		// 结束时间


	LoadPrepInfoTask(Replay& t, string& prepKey, SMsgView_VC_BattlePrep& battlePrepInfo, STime& timeBegin, STime& timeEnd)
		: IReplayRunnable(t)
		, m_prepKey(prepKey)
		, m_battlePrepInfo(battlePrepInfo)
		, m_timeBegin(timeBegin)
		, m_timeEnd(timeEnd)
		, m_bReadedPrep(false)
	{}

	virtual	bool NeedRecovery();
	virtual bool NeedSpawnAgain();

protected:
	virtual void Run() override;

	virtual void Callback(Replay& tHolder) override;
};