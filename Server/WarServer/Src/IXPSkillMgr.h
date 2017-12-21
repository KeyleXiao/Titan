/*******************************************************************
** 文件名: IXPSkillMgr.h
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
#include "Vector3.h"

class IXPSkillMgr : public __IWarMgr
{
public:
	// 在一定时间内检测xp技能的随机和给予
	virtual void checkPlayerXpSkillChangeInTimes() = 0;

	// 移除XP buf
	virtual void removeXPBuff(UID uid, PDBID pdbid) = 0;

};