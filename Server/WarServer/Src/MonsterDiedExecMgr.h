/*******************************************************************
** 文件名: RandomRefreshMons.h
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

#include "IMonsterDiedExecMgr.h"

class IWar;

class CMonsterDiedExecMgr : public IMonsterDiedExecMgr
{
public:
	CMonsterDiedExecMgr(IWar* pWar);
	~CMonsterDiedExecMgr();

	////////////////////////////////////////IMonsterDiedExecMgr Start////////////////////////////
public:
	// 初始化
	virtual bool Init(int nMatchType);

	// 释放
	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 怪物死亡
	virtual void onMonsterDied(sEntityDieInfo* pEntityDieInfo);

	////////////////////////////////////////IMonsterDiedExecMgr End////////////////////////////

private:
	IWar* m_pWar;

	int m_nWarType;
};