/*******************************************************************
** 文件名: IHeroGenicMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/30
** 版  本: 1.0
** 描  述: 英雄基因玩法接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"

class IHeroGenicMgr : public __IWarMgr
{
public:
	// 获得新的英雄基因
	virtual void onNewGenic(UID uid, int nAddHeroID, int nAddSkinID, const char* szDeadName) = 0;

	// 切换职业
	virtual void changeVocation(UID uid, int nSlotID) = 0;
};