#pragma once

#include "IMonsterEvent.h"
#include "IMonsterExec.h"

class CMonsEventUltimateSoldiers: public IWarMonsterEvent
{
public:
    CMonsEventUltimateSoldiers(void);
    ~CMonsEventUltimateSoldiers(void);

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

    struct SUltimateSoldiersInfo
	{
		bool bIsDestroy;
		int nIndex;
		SUltimateSoldiersInfo()
		{
			//bIsDestroy = false;
			//nIndex = 0;
            memset(this, 0, sizeof(*this));
		}
	};

    // key位阵容  value为超级兵数据
	map<int, map<int, SUltimateSoldiersInfo> >        m_mapUltimateSoldiers;

	IWar* m_pWar;

    IMonsterExec* m_pMonsterExec;
};

