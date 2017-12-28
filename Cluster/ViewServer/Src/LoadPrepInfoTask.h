#pragma once
#include "IReplayRunnable.h"
#include "ViewRedisData.h"


using namespace ViewRedisData;
struct SMsgView_VC_BattlePrep;
struct STime;

struct LoadPrepInfoTask : public IReplayRunnable<Replay>
{
	string&						m_prepKey;			// Ԥ����Ϣkey
	SMsgView_VC_BattlePrep&		m_battlePrepInfo;	// Ԥ����Ϣ
	bool						m_bReadedPrep;		// Ԥ����Ϣ�Ƿ��ȡ��

	STime&				m_timeBegin;	// ��ʼʱ��
	STime&				m_timeEnd;		// ����ʱ��


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