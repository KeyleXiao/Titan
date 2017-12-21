/*******************************************************************
** 文件名:	E:\speed\Server\MatchSceneServer\Src\MatchSceneService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	1/2/2016	
** 版  本:	1.0
** 描  述:	
动态障碍 代理
********************************************************************/

#pragma once

using namespace rkt;
using namespace std;
#include "math.h"

#include "IPathFindService.h"

#include "EventDef.h"
#include "EntityDef.h"
#include "PathFindDef.h"

#include "EntityHelper.h"

//#include "PathFindHelper.h"
//struct dtTileObstacle;

class DynamicObstacleService_Proxy : public IEventExecuteSink
{
public:

    // 订阅创建和销毁事件
    virtual bool  start(int nSceneID,IPathFindService*PathFindService);
    // 接受消息
    virtual void  OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    void release();

public:

    // 构造函数
    DynamicObstacleService_Proxy();

    // 析构函数
    ~DynamicObstacleService_Proxy();

private:
    void onObStacleNotify(UID uidMaster, bool bCreated);

#ifdef PATH_FIND_DEBUG
	void GetNavMeshBroadcast();
#endif

	void addDynamicObstacleBroadcast(UID uid, dtTileObstacle& obstacles);
	void removeDynamicObstacleBroadcast(ObstacleRef obstacleRef);
	// 给客户端发添加障碍物消息
	void sceneBroadcastAll(BROADCAST_ID id, string & msg, int nCampMask = CAMP_MASK_ALL);
private:    
    IPathFindService*  m_pService;      // 寻路服务指针	
    int                m_nSceneID;      // 场景id
};

