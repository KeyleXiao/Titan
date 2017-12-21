/*******************************************************************
** 文件名: IWarDropMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/5
** 版  本: 1.0
** 描  述: 战场子功能接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"

class IWarDropMgr : public __IWarMgr
{
public:
	// 增加天赋点
	virtual void addPersonTalentValue(UID realMurderID, UID uMurderID, int nMurderCamp, EWarTalentType TalentType, Vector3 vLoc, int nDistans, UID nMonsterID, int nEXP = 0, int nAdd = 0, float fDecayFactor = 0.0f, ETalentDropType talentDropType = ETALENTDROPTYPE_SOLIDER) = 0;

	// 增加经验值
	virtual bool addHeroEXP(EWarAddEXPType AddExpType, UID MurderID, int nMurderCamp, Vector3 vLoc, int nDis, int nAddValue = 0, EExpDropType expDropType = EEXPDROPTYPE_SOLIDER) = 0;

	// 添加buff
	virtual bool addHeroBuffValue(EWarBuffType AddBuffType, UID MurderID, int nMurderCamp, Vector3 vLoc, int nDis, int nBuffValue = 0, int nLevel = 0) = 0;

	// 处理玩家死亡
	virtual void dealPersonDie(sEntityDieInfo entityDieInfo, bool bBcastTips = true) = 0;

	// 设置整场战斗的最高不死连杀和时间连杀
	virtual void setKillMaxInfo(SWPerson* pPerson) = 0;
};