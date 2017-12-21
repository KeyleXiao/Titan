/*******************************************************************
** 文件名: ILogRecordMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/5
** 版  本: 1.0
** 描  述: 战场日志-数据库记录接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"
#include "Vector3.h"

class ILogRecordMgr : public __IWarMgr
{
public:
	// 填充比赛记录log
	virtual void fillWarActorLog(bool bIsEnd) = 0;

	// 战场记录的日志批处理
	virtual bool batchSaveWarActorLog(bool bIsEnd) = 0;

	// 记录增加金钱和经验的日志
	virtual void recordAddExpOrTalentLog(bool bForceSave = false) = 0;

	// 将增加经验记录到容器中
	virtual void setAddExpToMap(PDBID pDbid, int nAddExp, EExpDropType type) = 0;

	// 将增加金钱记录到容器中
	virtual void setAddTalentToMap(PDBID pDbid, int nAddTalent, ETalentDropType type) = 0;

	// 记录英雄皮肤使用次数数据
	virtual void saveHeroSkinUseCount() = 0;

	// 记录本局被禁英雄列表到数据库（用于后台英雄禁用次数统计）
	virtual void saveBanHeroListToDB() = 0;

	// 保存匹配分
	virtual void saveMatchTypeRankScoreAndGrade() = 0;

	// 保存英雄胜利失败
	virtual void recordHeroWinOrDefeat() = 0;
};