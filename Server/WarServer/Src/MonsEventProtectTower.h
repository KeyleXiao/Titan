#pragma once

#include "IMonsterEvent.h"
#include "IMonsterExec.h"

class CMonsEventProtectTower: public IWarMonsterEvent
{
public:
    CMonsEventProtectTower(void);
    ~CMonsEventProtectTower(void);

    // 初始化
    virtual bool Init(IWar*pWar, IMonsterExec* pMonsterExec);

    // 释放
    virtual bool release();

    // 更新事件数据
    virtual bool updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true);

    // 处理事件
    virtual bool sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true);

    // 处理事件
    virtual bool handleSlefMonsterEvent( SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn = true);
private:
	IWar* m_pWar;

    IMonsterExec* m_pMonsterExec;
};

