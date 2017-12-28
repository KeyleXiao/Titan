/*******************************************************************
** 文件名: WarLogRecord.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/10/18
** 版  本: 1.0
** 描  述: 战场日志记录
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once
#include "ILogRecordMgr.h"
class IWar;
struct SWarForceEndInfo;

using namespace War;

class CWarLogRecord: public ILogRecordMgr, public TimerHandler, public IDBRetSink
{
	struct SHeroAddExpOrTalentLog
	{
		int nAddTalent[ETALENTDROPTYPE_MAX];     // 增加天赋
		int nAddExp[EEXPDROPTYPE_MAX];           // 增加经验
		int nOldTotalTalent;					 // 旧的总天赋点
		int nOldTotalExp;						 // 旧的总经验

		SHeroAddExpOrTalentLog()
		{
			memset(this, 0, sizeof(*this));
		}
	};

public:
	CWarLogRecord(IWar* pWar);
	~CWarLogRecord();

public:
	//////////////////////TimerHandler///////////////////
	virtual void OnTimer(unsigned long dwTimerID);

	//////////////////////IDBRetSink///////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

public:
	////////////////////////ILogRecordMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 填充比赛记录log
	virtual void fillWarActorLog(bool bIsEnd);

	// 战场记录的日志批处理
	virtual bool batchSaveWarActorLog(bool bIsEnd);

	// 记录增加金钱和经验的日志
	virtual void recordAddExpOrTalentLog(bool bForceSave = false);

	// 将增加经验记录到容器中
	virtual void setAddExpToMap(PDBID pDbid, int nAddExp, EExpDropType type);

	// 将增加金钱记录到容器中
	virtual void setAddTalentToMap(PDBID pDbid, int nAddTalent, ETalentDropType type);

	// 记录英雄皮肤使用次数数据
	virtual void saveHeroSkinUseCount();

	// 记录本局被禁英雄列表到数据库（用于后台英雄禁用次数统计）
	virtual void saveBanHeroListToDB();

	// 保存匹配分
	virtual void saveMatchTypeRankScoreAndGrade();

	// 保存英雄胜利失败
	virtual void recordHeroWinOrDefeat();

	////////////////////////ILogRecordMgr end/////////////////////////
private:

	// 战场
	IWar*	m_pWar;

	// 后台记录信息
	DWORD                                       m_dwLastSaveLogTick;    // 上次保存日记时间
	std::map<PDBID, DBREQ_PARAM_WAR_ACTOR_INFO> m_mapWarActorLog;       // 比赛记录map(场中记录也有)
	std::map<PDBID, DBREQ_PARAM_WAR_ACTOR_INFO> m_mapWarActorLogEnd;    // 比赛记录map(只有结束的记录)
	// 英雄金币经验来源统计打印log
	std::map<PDBID, SHeroAddExpOrTalentLog>          m_mapHeroAddExpOrTalentLog;
};