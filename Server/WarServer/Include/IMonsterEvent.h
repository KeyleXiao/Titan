/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 陈俊涛
** 日  期: 2015/3/12
** 版  本: 1.0
** 描  述: 战场 怪物处理的接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "StdAfx.h"
#include "IWar.h"
#include <vector>
#include <list>
using namespace std;


struct SMonsterReliveInfoEX
{
    int nWarInfoIndex;
    int nCamp;
    bool bMonsterEmpty;
};

struct SMonsterDieInfoEX
{
    int nWarInfoIndex;
    int nCamp;
};

class IWarMonsterEvent
{
public:
    // 初始化
    virtual bool Init(IWar*pWar, IMonsterExec* pMonsterExec) = 0;

    // 释放
    virtual bool release() = 0;

    // 更新事件数据
    virtual bool updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true) = 0;

    // 处理事件
    virtual bool sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true) = 0;

    // 处理事件
    virtual bool handleSlefMonsterEvent( SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true) = 0;
};