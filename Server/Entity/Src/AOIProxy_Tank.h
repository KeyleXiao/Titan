/*******************************************************************
** 文件名:	AOIProxy_Tank.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	12/24/2014
** 版  本:	1.0
** 描  述:	

			载具/Tank的AOI Proxy
********************************************************************/

#pragma once

#include "NetMessageDef.h"
#include "IGameSceneService.h"
#include "IPlayerRole.h"
#include "buffer.h"
#include "AOIProxy_Monster.h"


// 怪物视野
class AOIProxy_Tank : public AOIProxy_Base 
{
public:
	AOIProxy_Tank( __IEntity * pHost) : AOIProxy_Base(pHost)
	{
	}

    ////////////////////以下只是为了方便通知电脑AI视野情况用/////////////
    // 进出自己的视野
    virtual void onEntityInOutSight(AOI_Proxy* pEntity, bool bInSight)
    {

    }

    // 通知移动
    virtual void onEntityMove(AOI_Proxy* pEntity)
    {

    }

    ////////////////////以上只是为了方便通知电脑AI视野情况用/////////////
};