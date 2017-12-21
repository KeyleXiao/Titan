#pragma once

#include "IMonsterEvent.h"
#include "IMonsterExec.h"

class CMonsEventMutexProtectBase: public IWarMonsterEvent
{
public:
    CMonsEventMutexProtectBase(void);
    ~CMonsEventMutexProtectBase(void);

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

    // 记录互斥基地保护信息 索引 值
    struct SProtectInfo
    {
        bool bHaveExist;
        int nEffectIndex;
        int nParam;
        SProtectInfo()
        {
            //bHaveExist = false;
            //nEffectIndex = 0;
            //nParam = 0;
            memset(this, 0, sizeof(*this));
        }
    };

    typedef map<int,SProtectInfo> MAP_RecProtect;

    // 保护的索引 互斥保护基地数量记录
    map<int,MAP_RecProtect>  m_mapNeedProtectRec;

    IWar* m_pWar;

    IMonsterExec* m_pMonsterExec;
};

