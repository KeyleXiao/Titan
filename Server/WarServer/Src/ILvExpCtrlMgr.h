/*******************************************************************
** 文件名: ILvExpCtrlMgr.h
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

class ILvExpCtrlMgr : public __IWarMgr
{
public:
	// 控制等级经验算法  野怪和小兵
	virtual void controlLvExp(EExpDropType expDropType, int nPlayerLv, int nCamp, Vector3 vLoc, int nDistance, int& nAdd, bool bIsUseShareCoeff = false) = 0;

	// 控制等级经验算法  英雄
	virtual void controlLvExp(int nPlayerLv, int nDiePlayerLv, int nAroundPlayerCount, int& nAdd) = 0;
};