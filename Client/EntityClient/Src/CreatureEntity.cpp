/*******************************************************************
** 文件名:	CreatureEntity.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	7/8/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "IClientGlobal.h"
#include "CreatureEntity.h"
#include "IEntityView.h"
#include "EntityFactory.h"
#include "IGameView.h"

bool CreatureEntity::create( ACTOR_INFO * pInfo )
{
	m_nUID    = pInfo->nActorID;
	m_nViewID = getEntityViewEntry().create(1);

	getEntityViewEntry().sendCommand(m_nViewID,ENTITY_CMD_SHOW_BLOOD,1,1);
	getEntityViewEntry().sendCommand(m_nViewID,ENTITY_CMD_SET_NAME,getIntPtrString(pInfo->szName),1);
	getEntityViewEntry().sendCommand(m_nViewID,ENTITY_CMD_SET_BLOOD,pInfo->nBlood,100);
	return true;
}

bool CreatureEntity::onEvent( int32 eventID,int32 param1,int32 param2 )
{
	switch(eventID)
	{
	case ENTITY_CMD_POSITION_SYNC:
		{
			if ( param2!=sizeof(msg_creature_rigidbody_sync) )
			{
				ErrorLn("事件参数错误!len=" << param2 << " event=" << eventID);
				return false;
			}

			msg_creature_rigidbody_sync * data = (msg_creature_rigidbody_sync*)param1;
			data->nTick = getSyncTickCount();
			data->nActorID = m_nUID;

			obuf ob;
			fillData(ob,MSG_ENDPOINT_CLIENT,MSG_ENDPOINT_ZONE,MSG_MODULEID_GAME,CS_MSG_CREATURE_SYNC,data,sizeof(msg_creature_rigidbody_sync));
			gClientGlobal->getNetConnection()->SendData(ob.data(), ob.size());
			break;
		}
	case ENTITY_CMD_CTRL_SYNC:
		{
			if ( param2!=sizeof(msg_creature_act_sync) )
			{
				ErrorLn("事件参数错误!len=" << param2 << " event=" << eventID);
				return false;
			}

			msg_creature_act_sync * data = (msg_creature_act_sync*)param1;
			data->pos.nTick = getSyncTickCount();
			data->pos.nActorID = m_nUID;

			obuf ob;
			fillData(ob,MSG_ENDPOINT_CLIENT,MSG_ENDPOINT_ZONE,MSG_MODULEID_GAME,CS_MSG_CREATURE_SYNC,data,sizeof(msg_creature_act_sync));
			gClientGlobal->getNetConnection()->SendData(ob.data(), ob.size());
			break;
		}
	default:
		break;
	}

	return true;
}

void CreatureEntity::handleMsg( int32 nMsgID,void * data,int32 len )
{
	if ( gClientGlobal->getEntityFactory()->getHero()->getID()==getID() )
	{
		int x = 0;
	}
	if (nMsgID == CS_MSG_CREATURE_SYNC)
	{
		if ( len==sizeof(msg_creature_rigidbody_sync) )
		{
			getEntityViewEntry().sendCommand(m_nViewID,ENTITY_CMD_POSITION_SYNC,(int32)data,len );
		}else if ( len==sizeof(msg_creature_act_sync) )
		{
			getEntityViewEntry().sendCommand(m_nViewID,ENTITY_CMD_CTRL_SYNC,(int32)data,len );
		}	
	}
}

void CreatureEntity::release()
{
	if ( m_nViewID!=0 )
	{
		getEntityViewEntry().destroy(m_nViewID);
		m_nViewID = 0;
	}

	delete this;
}