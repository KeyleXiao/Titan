#pragma once
#include "ViewRedisData.h"
#include "LoadBattleInfoTask.h"
#include "ViewNode.h"
#include "ViewMsgDef_Client.h"
#include "STime.h"


struct ClientUser;
struct ViewParam;
using ViewRedisData::BattleInfo;
using ViewRedisData::BattlePrepInfo;

class Replay
{
	friend struct LoadPrepInfoTask;
	friend struct LoadBattleInfoTask;
public:
	Replay()
		: m_rID(0)
		, m_lReaded(0)
		, m_bStart(false)
		, m_bFinished(false)
		, m_nRefCount(0)
		, m_pTask(nullptr)
	{}

	~Replay()
	{
		safeDelete(m_pTask);
	}

	bool Start();
	// 加入一个观战者
	void Insert(ClientUser* pClientUser, const ViewParam& sViewParam);
	// 移除观战者
	void Remove(PlayerID dwPlayerID);
	// 每隔N毫秒，由ReplayContainer来调用
	void HeartBeat();

	// 引用计数 +1
	void IncRef() { m_nRefCount += 1; }
	// 引用计数 -1
	void DecRef() { m_nRefCount -= 1; }
	// 引用计数是否为0
	bool IsRefEmpty() { return m_nRefCount == 0; }

private:
	// 发送战报给所有观战者
	void BroadCastBattleInfo();

public:
	ReplayID	m_rID;		// 战报ID

private:
	typedef	std::vector<SMsgView_VC_BattleInfo>	Vec_VC_BattleInfo;

	string					m_prepKey;		// 预备key
	SMsgView_VC_BattlePrep	m_battlePrepMsg;// 预备消息
	STime					m_timeBegin;	// 开始时间
	STime					m_timeEnd;		// 结束时间

	string					m_strKey;		// 战报对应的Key
	Vec_VC_BattleInfo		m_vecBattleInfo;// 战报消息vector

	LoadBattleInfoTask*		m_pTask;	// 循环使用的读取任务
	long					m_lReaded;	// 已经读完的数目

	int			m_nRefCount;	// 引用计数（必须为0，此对象才能被移除。）
	bool		m_bStart;		// 战场是否已经启动(必须为true时，玩家才可读取战报信息)
	bool		m_bFinished;	// 战场是否已经结束（结束了，就不用再去读战报了）

	typedef std::map<PlayerID, ViewNode>	UserMap;
	UserMap		m_UserMap;	// 观战者map
	
};