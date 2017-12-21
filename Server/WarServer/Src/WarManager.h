/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 陈俊涛
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场管理信息
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: ljw
** 日  期: 
** 描  述: 代码重构
********************************************************************/
#pragma once

#include "StdAfx.h"
#include "IWarManager.h"
#include "fast_id_map.h"
#include "EntityDef.h"

#define MAX_WAR_COUNT MAX_SCENE_COUNT

class CWarManager:public IWarManager
{
public:
    CWarManager():m_mutex(0),m_nAllocID(0){m_mapWarID2Service.clear(); }

    // 创建战场
    virtual int CreateWar(Room_Create_War_Context& context);

    // 销毁战场
    virtual void DestroyWar(int nWarID);

    // 战场service退出回调
    virtual void OnWarDestroied(int nWarID);

    // 获取战场service
    virtual SERVICE_PTR GetWar(int nWarID);

    virtual void onStop();

    virtual void release();

	virtual void onWarCreate(int nWarID, SERVICE_PTR pContainer);

private:
    volatile	LONG           m_mutex;
    volatile    LONG           m_nAllocID;

    typedef map<int,SERVICE_PTR> T_WARID_TO_WARSERVICE;

    T_WARID_TO_WARSERVICE m_mapWarID2Service;    // 创建完成的 战场ID->战场service

};