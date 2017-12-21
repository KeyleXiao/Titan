/*******************************************************************
** 文件名: IMonsterDiedExecMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/15
** 版  本: 1.0
** 描  述: 战场怪物死亡处理接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"

struct sEntityDieInfo;

class IMonsterDiedExecMgr : public __IWarMgr
{
public:
	// 怪物死亡
	virtual void onMonsterDied(sEntityDieInfo* pEntityDieInfo) = 0;
};