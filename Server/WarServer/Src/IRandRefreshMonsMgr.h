/*******************************************************************
** 文件名: IRandRefreshMons.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/15
** 版  本: 1.0
** 描  述: 战场随机刷怪接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"

class IRandRefreshMonsMgr : public __IWarMgr
{
public:
	// 停止怪物AI
	virtual void stopAI() = 0;

	// 释放怪物
	virtual void releaseMonsterRes() = 0;
};