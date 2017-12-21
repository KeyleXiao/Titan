/*******************************************************************
** 文件名: IDevilKingMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/29
** 版  本: 1.0
** 描  述: 魔王玩法接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"

class IDevilKingMgr : public __IWarMgr
{
public:
	// 有玩家死亡
	virtual void onPlayerDie() = 0;
	// 有玩家击杀了人
	virtual void onPlayerKillPerson(UID uidMurder, UID uidDead) = 0;
};