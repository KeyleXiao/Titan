/*******************************************************************
** 文件名:	DiePart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	实体死亡部件

********************************************************************/

#include "stdafx.h"
#include "DiePart.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "IEntityProperty.h"
#include "EntityViewDef.h"
#include "EventDef.h"
#include "CommonStructDef.h"
#include "TimeSyncClient.h"
#include "EffectDef.h"
#include "EntityFactory.h"
#include "WarDef.h"
using namespace EFFECT_CLIENT;
#define  MONSTERDIE_INNER_EFFECT(typeName,obj,pContext)  gClientGlobal->getEffectFactory()->DoInnerEffect( #typeName,&obj,sizeof(obj),pContext)

////////////////////* DiePart */////////////////////////////////////////////
DiePart::DiePart()
{
    m_serState = ENTITY_STATE_UNKNOW;
    m_dwDeadTick = 0;;	// 死亡时刻
    m_nReliveTick = 0;
	m_pMaster = NULL;
	m_SObstacleNode = NULL;
    m_bHideBody = false;
}

DiePart::~DiePart()
{

}

////////////////////IEntityPart/////////////////////////////////////////////
bool DiePart::onLoad( IEntity* pEntity)
{
    if(pEntity == NULL)
    {
        return false;
    }

	m_pMaster = pEntity;

	UID uid = m_pMaster->getUID();
	gClientGlobal->getEventEngine()->Subscibe(this, EVENT_ENTITY_ENTER_VIEWPORT, UID_2_TYPE(uid), uid, __FILE__);

	return true;
}

void DiePart::release()
{
	if (m_pMaster != NULL)
	{
		UID uid = m_pMaster->getUID();
		gClientGlobal->getEventEngine()->UnSubscibe(this, EVENT_ENTITY_ENTER_VIEWPORT, UID_2_TYPE(uid), uid);
	}

	delete this;
}

short DiePart::getPartID()
{
	return PART_DIE;
}

IEntity * DiePart::getHost()
{
	return m_pMaster;
}

bool DiePart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch( msgId )
	{
	case SC_MSG_ENTITY_RELIVE:			// 实体复活
		{
			onRelive(data, len);
		}
		break;
	case SC_MSG_ENTITY_DIE:
		{
			onDie(data, len);
		}
		break;
	default:
		{
			ErrorLn("die have a message not handle, message="<< msgId);
			return false;
		}

	}

	return true;
}

// 显示层的命令处理
bool DiePart::onCommand( int cmdid,void * data,size_t len )
{
    switch(cmdid)
    {
    case ENTITY_CMD_LOAD_COMPLETED:
        {
            if (m_pMaster) 
            {
                m_pMaster->fireEvent(EVENT_ENTITY_VIEW_LOADED, NULL, 0);
            }

            onCommand_viewObjectLoadCompleted();  // 显示层加载完成
            return true;
        }
        break;
    default:
        break;
    }

	return false;
}

void DiePart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_ENTER_VIEWPORT:
	{
		if (m_pMaster == NULL)
			return;

		// 非主角进入视野时，要通知显示层状态
		if (m_serState == ENTITY_STATE_DIE && !m_pMaster->isHero() && isActor(m_pMaster))
		{
			DWORD dwSyncTick = gClientGlobal->getTimeSyncer()->GetTick();
			cmd_creature_die cmd;
			cmd.nDeadInterval = (dwSyncTick > m_dwDeadTick ? dwSyncTick - m_dwDeadTick : 0);
			cmd.nInterval = m_nReliveTick;
			cmd.uMurderID = 0;
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ENTITY_DEAD, 0, 0, &cmd, sizeof(cmd));
		}

		if (m_serState != ENTITY_STATE_DIE)
		{
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RELIVE, 0, 0, 0, 0);
		}
	}
	break;
	default:
		break;
	}
}

////////////////////ISerializable/////////////////////////////////////////////
bool DiePart::onSerialize(UID uid, rkt::ibuffer & in,int flag )
{
    // 当前实体类型
    BYTE nType = UID_2_TYPE(uid);

    // 当前状态
    in >> m_serState;

    // 死亡时加入死亡时间
    if(m_serState == ENTITY_STATE_DIE)
    {
        BYTE nDeadType = 0;
        in >> nDeadType;
        
        // 人物下发死亡和复活时间
        if(nType == TYPE_PLAYER_ROLE)
        {
            in>> m_dwDeadTick >> m_nReliveTick ;
        }

        updateHideBody(nDeadType);
    }

    // 是否为动态障碍信息
    bool isObstacle =false;
    in >> isObstacle;

    // 动态障碍模块处理
    if( isObstacle )
    {
        SObstacleNode node;
        in>>node;

        updateDynamicObstacleMsg(node);
    }


    // 原来的模块处理
    if(m_pMaster == NULL)
    {
        return true;
    }
    IClientEntity* pEntity = (IClientEntity*)m_pMaster;
    int nOldState = pEntity->getState(); 

    // 角色进入视野，打印日志
    if(isActor(m_pMaster))
    {
        WarningLn("" << pEntity->getName() << " appear, oldState=" << nOldState << " curState=" << m_serState);
    }

	pEntity->changeState(m_serState);

	return true;
}

bool DiePart::deSerialize( rkt::obuf & out,int flag )
{
	return true;
}

////////////////////DiePart/////////////////////////////////////////////
bool DiePart::isHideBody()
{
    if(m_pMaster == NULL || !isCreature(m_pMaster))
    {
        return false;
    }	

    IClientEntity* pEntity = (IClientEntity*)m_pMaster;
    if(pEntity->getState() != ENTITY_STATE_DIE)
    {
        return false;
    }

    return m_bHideBody;
}

void DiePart::updateHideBody(int nDeadType)
{
    if(m_pMaster == NULL || !isCreature(m_pMaster))
    {
        return;
    }

    if(nDeadType < 0 || nDeadType >= DeadEffect_Max)
    {
        ErrorLn(__FUNCTION__ << __LINE__ << ", nDeadType out of range");
        return;
    }

    ISchemeDieLighting *pSchemeDieLighting = gClientGlobal->getSchemeCenter()->getSchemeDieLighting();
    if (pSchemeDieLighting == NULL)
    {
        return;
    }
    // 职业ID
    int nVocation = ((IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY))->getProperty_Integer(PROPERTY_VOCATION);

    SDieLighting *pDieLighting = pSchemeDieLighting->getDieLighting(nVocation, TYPE_PLAYER_ROLE);
    if (pDieLighting == NULL)
    {
        return;
    }

    m_bHideBody = (pDieLighting->nHideBody[nDeadType] != 0);
}

void DiePart::onRelive(void * data, int32 len)
{
	if (m_pMaster == 0)
	{
		return;
	}
	if(!isCreature(m_pMaster))
	{
		return;
	}	

    IClientEntity* pEntity = (IClientEntity*)m_pMaster;
    if(isHideBody() && ((EntityBase*)m_pMaster)->isOnView())
    {
        pEntity->updateView(true, false);
    }

	// 复活时站立
	pEntity->changeState(ENTITY_STATE_STAND);

    // 复活事件
    event_actor_relive relive;
    m_pMaster->fireEvent(EVENT_ACTOR_RELIVE, &relive, sizeof(relive));
	
	if(pEntity->isHero())
	{
		EmphasisLn(__FUNCTION__": name="<< pEntity->getName())
	}
	// 通知显示层	
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_RELIVE, 0, 0, 0, 0);
}


void DiePart::onDie(void * data, int32 len)
{
	if (!m_pMaster)
	{
		return;
	}
	if (len != sizeof(msg_entity_state_info))
	{
		return;
	}

    if(!isCreature(m_pMaster))
    {
        return;
    }
    IClientEntity* pEntity = (IClientEntity*)m_pMaster;

	// 死亡状态
	pEntity->changeState(ENTITY_STATE_DIE);	

	msg_entity_state_info *pState = (msg_entity_state_info*)data;
    updateHideBody(pState->nDeadType);

	cmd_creature_die cmd;
	// 发最终复活时间（同步服务器的时间为基准）
	DWORD dwSynTick = gClientGlobal->getTimeSyncer()->GetTick();
    cmd.nDeadInterval = 0;
	cmd.nInterval = (pState->nData > dwSynTick) ? ((pState->nData-dwSynTick)/1000)*1000 : 0;  // pState->nData;	
    cmd.uMurderID = pState->uidOperator;
	cmd.isNoRelive = pState->bNoRelive ? 1 : 0;
	if(pEntity->isHero() && cmd.nInterval > 0)
	{
		EmphasisLn(__FUNCTION__": name="<< pEntity->getName()<<": counter="<<cmd.nInterval);
	}
	
    // 移除动态障碍
    removeDynamicObstacle();

	if(isMonster(m_pMaster))
	{

        // 基地特殊处理
        IEntityProperty* pTartgetPropPart = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
        if(pTartgetPropPart)
        {
            int nSubType = pTartgetPropPart->getProperty_Integer(PROPERTY_VOCATION);
            if( nSubType == MONSTER_SUB_TYPE_BASE  )
            {
                IClientEntity *pClientEntity = gClientGlobal->getHero();
                if (pClientEntity != NULL)
                {
                    // 发出死亡事件
                    event_entity_die die;
                    die.uidOperator = pState->uidOperator;
                    die.uidTarget = m_pMaster->getUID();
                    die.nDeadType = pState->nDeadType;
                    die.vTargetEndLoc = pState->v;
                    die.vTurnTargetTile = Vector3(0,0,0);
                    // 死亡事件
                    pClientEntity->fireEvent(EVENT_ENTITY_DIE, (char *)&die, sizeof(die));
                }
            }
        }

        //怪物死亡销毁实体
		gClientGlobal->getEntityFactory()->destroyEntity(pEntity->getUID(), DESTROY_REASON_DIE, &cmd);
	}
    else
    {
        // 发出死亡事件
        event_entity_die event_data;
        m_pMaster->fireEvent(EVENT_ENTITY_DIE, &event_data, sizeof(event_data));

        // 只通知显示层死亡 不销毁
        m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ENTITY_DEAD, 0, 0, &cmd, sizeof(cmd));
    }
}

// 显示层加载完成
void DiePart::onCommand_viewObjectLoadCompleted(void)
{
    if(m_pMaster == NULL || !m_pMaster->isHero())
    {
        return;
    }

    IClientEntity* pEntity = (IClientEntity*)m_pMaster;
    pEntity->changeState(m_serState);

    // 非主角进出视野时，要通知显示层状态
    if(m_serState == ENTITY_STATE_DIE)
    {
        DWORD dwSyncTick = gClientGlobal->getTimeSyncer()->GetTick();
        cmd_creature_die cmd;
        cmd.nDeadInterval   = (dwSyncTick > m_dwDeadTick ? dwSyncTick-m_dwDeadTick : 0);
        cmd.nInterval       = m_nReliveTick;
        cmd.uMurderID       = 0;
        m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_ENTITY_DEAD, 0, 0, &cmd, sizeof(cmd));
    }

    Vector3 loc = m_pMaster->getLocation();
    WarningLn("Master View Object Loaded: uid="<<m_pMaster->getUID() <<", name="<<m_pMaster->getName() <<", x="<< loc.x <<",y="<< loc.y<<",z="<< loc.z );
}


static bool CheckTheSameObstacle(const SObstacleNode& nodeA,const SObstacleNode& nodeB)
{
	if(abs(nodeA.data[0] - nodeB.data[0])>=0.01f)return false;
	if(abs(nodeA.data[1] - nodeB.data[1])>=0.01f)return false;
	if(abs(nodeA.data[2] - nodeB.data[2])>=0.01f)return false;
	if(abs(nodeA.data[3] - nodeB.data[3])>=0.01f)return false;
	if(abs(nodeA.data[4] - nodeB.data[4])>=0.01f)return false;
	if(abs(nodeA.data[5] - nodeB.data[5])>=0.01f)return false;
	return true;
}

void DiePart::updateDynamicObstacleMsg(const SObstacleNode& node){

	if(m_SObstacleNode == NULL){
		m_SObstacleNode = (SObstacleNode*)malloc(sizeof(SObstacleNode));
		m_SObstacleNode->angle = node.angle;
		for(int i=0;i<6;i++)
			m_SObstacleNode->data[i] = node.data[i];
		m_SObstacleNode->EntityUid = node.EntityUid;
		m_SObstacleNode->isPreload = node.isPreload;
		m_SObstacleNode->nCamp = node.nCamp;
		m_SObstacleNode->nFlag = node.nFlag;
		m_SObstacleNode->type = node.type;
		addDynamicObstacle(*m_SObstacleNode);
	}
	else
	{
		//这里不应该进来：动态障碍不该移动，要移动也应该自己手动删除，手动添加0.0
		
		if(!CheckTheSameObstacle(*m_SObstacleNode,node))
		{
			removeDynamicObstacle();
			m_SObstacleNode->angle = node.angle;
			for(int i=0;i<6;i++)
			m_SObstacleNode->data[i] = node.data[i];
			m_SObstacleNode->EntityUid = node.EntityUid;
			m_SObstacleNode->isPreload = node.isPreload;
			m_SObstacleNode->nCamp = node.nCamp;
			m_SObstacleNode->nFlag = node.nFlag;
			m_SObstacleNode->type = node.type;
			addDynamicObstacle(*m_SObstacleNode);
		}
	}

}

void DiePart::addDynamicObstacle(SObstacleNode& node)
{
	//return;//动态障碍暂时屏蔽 2016-06-08（李界华）
	IPathFindService* pPathFindService = gClientGlobal->getPathFindManager()->getPathFindService();
	if(!pPathFindService ||!(pPathFindService->hasCHF()))
	{
		return;
	}
    // uid无效返回，不创建
    if(node.EntityUid == 0 || node.EntityUid == INVALID_UID)
    {
        return;
    }
	pPathFindService->addDynamicObstacle(node.EntityUid,&node,node.nCamp);
    // 以下是给unity传递顶点信息，画出动态障碍;给cs发信息
    cmd_DynamicObstacle_Info cmdDynamicObstacleInfo;
    cmdDynamicObstacleInfo.uidTarget	=  node.EntityUid;
	cmdDynamicObstacleInfo.type = node.type;
    cmdDynamicObstacleInfo.nIsAdd=1;
    for(int i=0;i<6;i++)
    {
		cmdDynamicObstacleInfo.nDatas[i]=node.data[i];
    }
    //判断obstacleNode的阵营信息，检查是否需要绘制碰撞体
	//node.nFlag....
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_DRAW_DYNAMICOBSTACLE, 0, 0, &cmdDynamicObstacleInfo, sizeof(cmdDynamicObstacleInfo));

}

void DiePart::removeDynamicObstacle()
{
    if(!IsObstacle())
    {
        return;
    }
	IPathFindService* pPathFindService = gClientGlobal->getPathFindManager()->getPathFindService();
	if(!pPathFindService)
	{
		return;
	}
    UID uidEntity=m_pMaster->getUID();
    // uid无效返回，不创建
    if(uidEntity==0||uidEntity==INVALID_UID)
    {
        return;
    }
    // 移除动态障碍
	pPathFindService->removeObstacle(uidEntity); 

    // 以下是给unity传递信息,删除动态障碍图
    cmd_DynamicObstacle_Info cmdDynamicObstacleInfo;
    cmdDynamicObstacleInfo.uidTarget	=  uidEntity;
    cmdDynamicObstacleInfo.nIsAdd=0;
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_DRAW_DYNAMICOBSTACLE, 0, 0, &cmdDynamicObstacleInfo, sizeof(cmdDynamicObstacleInfo));

}

// 是否有动态障碍
bool DiePart::IsObstacle()
{
	if(m_SObstacleNode!=NULL){
		return (m_SObstacleNode->EntityUid > 0);
	}
	return false;
}

void DiePart::restore()
{

}
