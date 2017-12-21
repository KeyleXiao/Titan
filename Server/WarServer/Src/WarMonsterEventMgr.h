/*******************************************************************
** 文件名: WarMonsterEvent.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 秦其勇
** 日  期: 2017/3/20
** 版  本: 1.0
** 描  述: 战场怪物事件相关
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "StdAfx.h"
#include "GameDef.h"
#include "WarDef.h"
#include "SchemeDef.h"
#include "IWar.h"
#include "IMonsterEvent.h"
#include <map>

class IWar;

class CWarMonsterEventMgr
{
public:
	CWarMonsterEventMgr();
	~CWarMonsterEventMgr();

public:
	bool Init(IWar*pWar, IMonsterExec * pMonsterExec);

    void Release();

    void handleMonsReliveEvent();

    void handleMonsDieEvent(sEntityDieInfo entityDieInfo, SMonsterRefreshInfo* pRefreshInfo);

    // 处理事件
    bool handleSlefMonsterEvent( SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true);

private:
    // 战场信息
	IWar*  m_pWar;

    IMonsterExec* m_pMonsterExec;

    map<EWarInfoEvent, IWarMonsterEvent*> m_mapMonsterEvent; 
};