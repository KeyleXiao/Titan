/*******************************************************************
** 文件名: XPSkill.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/20
** 版  本: 1.0
** 描  述: XP技能
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "GameDef.h"
#include "WarDef.h"
#include "SchemeDef.h"
#include "IDBEngine.h"
#include "IXPSkillMgr.h"
#include <map>

class IWar;


class CXPSkill : public IXPSkillMgr
{
public:
	CXPSkill(IWar* pWar);
	~CXPSkill();

public:
	////////////////////////IXPSkillMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 在一定时间内检测xp技能的随机和给予
	virtual void checkPlayerXpSkillChangeInTimes();

	// 移除XP buf
	virtual void removeXPBuff(UID uid, PDBID pdbid);

	////////////////////////IXPSkillMgr end/////////////////////////

private:
	/////////////////////////////////////随机XP技能所有者/////////////////////////////
	// 随机冷却中的xp技能给玩家
	void randomPlayerXpSkillInCool();

	// xp技能冷却完毕加上buff效果，可以使用
	void setPlayerXpSkillCoolFinishedToAddXPBuff();

	// 随机给玩家冷却中的xp，告知客户端
	void sendXpSkillRandomPlayerToClient(PDBID dwPDBID);

	// 判断是否满足获得XP的条件
	bool judgeIsCanHaveXpSkill(PDBID pDBID, int nPlayerNum);
	bool IsCanUseXpSkill(PDBID pDBID);

	// 移除buff
	void addXPBuff();

private:
	// 功能函数
	int getCanUseXPPlayerCount(int nCamp);

private:

	// 战场
	IWar*	m_pWar;

	map<PDBID, int>                             m_mapLastXPSkill;               // 上次XP技能容器
	map<PDBID, int>                             m_mapCurrentSkill;              // 本次xp技能容器
	map<PDBID, int>                             m_mapPossessdSkill;				// 已经拥有xp技能容器
	ESkillXp_AddState                           m_xpSkillAddState;              // xp技能添加状态
	int                                         m_nLastRefreshXpTick;           // 上次xp技能计数

	int m_nXPSkillBuffID;
};