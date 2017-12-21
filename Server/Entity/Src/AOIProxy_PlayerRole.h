/*******************************************************************
** 文件名:	AOIProxy_PlayerRole.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家角色的AOI Proxy
********************************************************************/

#pragma once

#include "NetMessageDef.h"
#include "IGameSceneService.h"
#include "IPlayerRole.h"
#include "buffer.h"
#include "EntityHelper.h"
#include "AOIProxy_Base.h"
#include "Simple_Atom_Lock.h"

// 角色视野
class AOIProxy_PlayerRole : public AOIProxy_Base 
{
	__IPlayerRole * m_pRole;
public:
	AOIProxy_PlayerRole( __IPlayerRole * pHost) : AOIProxy_Base(pHost),m_pRole(pHost)
	{
	}

	// 处理广播
	virtual void onBroadcast( BROADCAST_ID id,AOI_Proxy * pSrc,PACKAGE_PTR package )
	{
        int nFlag = 0;
        if(id == BROADCAST_TYPE_COUNT)
        {
            nFlag = SEND_DATA_FLAG::NO_ACK;
        }
        send2Client(m_uid, package, nFlag);
		//m_pRole->send2Client(package);
	}
};
