/*******************************************************************
** 文件名:	Monster.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/23/2014
** 版  本:	1.0
** 描  述:	

			怪物/NPC实体对象（非玩家实体对象)
********************************************************************/

#include "stdafx.h"
#include "EventDef.h"
#include "NetMessageDef.h"
#include "Monster.h"
#include "ISpell.h"
#include "MonsterProperty.h"
#include "IServerGlobal.h"
#include "IEntityAOI.h"
#include "IMovePart.h"
#include "IServerGlobal.h"
#include "IDiePart.h"
#include "IFreezeServer.h"
#include "IBuffServer.h"
#include "IDamageServer.h"
#include "IEnmityServer.h"
#include "IEntityEvent.h"
#include "IWarPart.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "IAIPart.h"
#include "IPlayerKillPart.h"

// 取得属性接口
#define GetProperty() ((__IEntityProperty*)m_pPartList[PART_PROPERTY])
//////////////////////////////////////////////////////////////////////////
Monster::Monster()
{
	memset( m_pPartList,0,sizeof(m_pPartList) );
	m_pEventEngine = NULL;

	m_nMapID = INVALID_MAPID;

	// 当前控制状态
	m_byCurControlState = ENTITY_STATE_UNKNOW;
	// 当前状态
	m_pCurrentState = 0;
    memset(m_szName,0,sizeof(m_szName));

    m_pMobaGoodsEffect = new CMobaGoodsEffect(CAST_TYPE(__IEntity *,this));
}

///////////////////////////////////////////////////////////////////////////////////
bool Monster::create(UID uid,void * pContext,int len,int flag)
{
#define MONSTER_CREATE_FAIL_DELETE_PARTS(errinfo)	\
	ErrorLn(errinfo);\
	for(int n=ENTITY_PART_MAX-1;n >=0; --n)\
		safeRelease(m_pPartList[n]);

	// 事件中心
	if ( m_pEventEngine ) delete m_pEventEngine;
	m_pEventEngine = new EntityEvent(uid);

    // 创建上下文数据
    CreateNpcContext * pCreateContext = (CreateNpcContext*)pContext;
    // 定位部件位置
    pContext = pCreateContext+1;
    len -= sizeof(CreateNpcContext);

	//////////////////////////////////////////////////////////////////////////
	// 属性部件
	__IEntityProperty * property = new EntityPart_MonsterProperty;
	m_pPartList[PART_PROPERTY] = property;

	// 视野部件
	__IEntityAOI * pAOI = CreateEntityAOIProc();
	if ( pAOI==0 )
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("create AOI part failed!");
		return false;
	}
	m_pPartList[PART_AOI] = pAOI;

	// 移动组件
	__IMovePart * pMove = CreateMovePartProc();
	if ( pMove==0 )
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("create Move part failed!");
		return false;
	}
	m_pPartList[PART_MOVE] = pMove;

	// 技能部件
	__ISpellPart *pSpellPart = gServerGlobal->getSpellFactory()->createSpellPart();
	if (pSpellPart == 0 )
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("create Spell Part failed!");
		return false;
	}
	m_pPartList[PART_SPELL] = pSpellPart;

	// 冷却服务器
	IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
	if (pFreezeServer == 0)
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("get Freeze Server Failed!");
		return false;
	}
	// 冷却部件
	__IFreezePart *pFreezePart = pFreezeServer->CreatePart();
	if (pFreezePart == NULL )
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("create pFreezePart failed!");
		return false;
	}
	m_pPartList[PART_FREEZE] = pFreezePart;

	// 死亡部件
	__IDiePart * pDiePart = CreateDiePartProc();
	if ( pDiePart==0 )
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("create die part failed!");
		return false;
	}
	m_pPartList[PART_DIE] = pDiePart;

	// Buff服务器
	IBuffServer *pBuffServer = gServerGlobal->getBuffServer();
	if (pBuffServer == 0)
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("get Buff Server Failed!");
		return false;
	}
	// Buff部件
	__IBuffPart *pBuffPart = pBuffServer->CreatePart();
	if ( pBuffPart==0 )
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("create Buff part failed!");
		return false;
	}
	m_pPartList[PART_BUFF] = pBuffPart;

	// 伤害服务器
	IDamageServer *pDamageServer = gServerGlobal->getDamageServer();
	if (pDamageServer == 0)
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("get Damage Server Failed!");
		return false;
	}
	// 伤害部件
	__IDamagePart *pDamagePart = pDamageServer->crateDamagePart();
	if ( pDamagePart==0 )
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("create Damage part failed!");
		return false;
	}
	m_pPartList[PART_DAMAGE] = pDamagePart;

	// AI服务器
    __IAIPart* pAIPart = CreateAIPartProc();
    if (pDamageServer == 0)
    {
        MONSTER_CREATE_FAIL_DELETE_PARTS("create AI part failed");
        return false;
    }
    m_pPartList[PART_AI] = pAIPart; 

	// 仇恨服务器
	IEnmityServer *pEnmityServer = gServerGlobal->getEnmityServer();
	if (pEnmityServer == 0)
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("get Enmity Server Failed!");
		return false;
	}
	// 仇恨部件
	__IEnmityPart *pEnmityPart = pEnmityServer->CreatePart();
	if ( pEnmityPart==0 )
	{
		MONSTER_CREATE_FAIL_DELETE_PARTS("create Enmity part failed!");
		return false;
	}
	//m_pPartList[PART_ENMITY] = pEnmityPart;
	//*/

    // 战场部件
    __IWarPart * pWarPart = CreateWarPartProc();
    if ( pWarPart==0 )
    {
        MONSTER_CREATE_FAIL_DELETE_PARTS("create pWarPart part failed!");
        return false;
    }
    m_pPartList[PART_WAR] = pWarPart;

    // PK组件
    __IPlayerKillPart *pPlayerKill = CreatePlayerKillPartProc();
    if ( pPlayerKill==0 )
    {
        MONSTER_CREATE_FAIL_DELETE_PARTS("create pPlayerKill part failed!");
        return false;
    }
    m_pPartList[PART_PK] = pPlayerKill;

	//////////////////////////////////////////////////////////////////////////
	// 序列化
	ibuffer in(pContext,len);

	if ( onSerialize(in,flag)==false )
	{
		ErrorLn("monster onSerialize failed! flag="<<flag);
		return false;
	}

	// 初始化各部件数据
	for (int i=PART_PROPERTY; i<ENTITY_PART_MAX; ++i)
	{
		if (m_pPartList[i] && m_pPartList[i]->onLoad(this, elogin_online) == false)
		{
			MONSTER_CREATE_FAIL_DELETE_PARTS("create failed! partID:"<<i);
			return false;
		}
	}

    m_belongEntityMgr.onload(this);

	// 站立状态
	m_EntityStandState.create(this);
	// 死亡状态
	m_EntityDieState.create(this);
	// 战斗死亡状态
	m_EntityFightState.create(this);
	// 转到站立状态
	changeState(ENTITY_STATE_STAND);

	// 发出实体创建事件
	event_entity_create context;
	context.uid = uid;
	context.flag= flag;
  
    UID uidMaster = INVALID_UID;
    property->getProperty_custom(PROPERTY_MASTER_UID,&uidMaster,sizeof(UID));

    context.uidCreator=uidMaster;

    __IEntityEvent * pEntityEvent = getEntityEvent();
    if(pEntityEvent)
    {
        pEntityEvent->FireExecute(EVENT_GAME_MONSTER_PREPARE_CREATE,(LPCSTR)&context,sizeof(context));
        pEntityEvent->FireExecute(EVENT_ENTITY_CREATE,(LPCSTR)&context,sizeof(context));
        pEntityEvent->FireExecute(EVENT_ENTITY_AFTER_CREATE, NULL, 0);
    }

    //if(UID_2_TYPE(uidMaster) == TYPE_PLAYER_ROLE)
    //{
    //    ActorHelper actorHelper(uidMaster);
    //    if(actorHelper.m_ptr)
    //    {
    //        actorHelper.m_ptr->onAddRemoveSummonEntity(uid, true);
    //    }
    //}
    // 如果是有主人的怪物，通知到主人怪物已创建好了
    if( isValidUID(uidMaster) )
    {
        EntityBelongContext bedata;
        bedata.isCreated = true;
        bedata.uid = uid;
        bedata.uidMaster = uidMaster;
        bedata.nFlag = pCreateContext->nFlag;

        entityNotifyMaster(bedata.uidMaster , bedata);
    }

 //   int nMonsterID = getIntProperty(PROPERTY_ID);
 //   int nWarID = pWarPart ? pWarPart->getWarID() : 0;
	//TraceLn( "Monster create successful.id="<< nMonsterID <<", uid="<< uid << ",sceneid=" << getSceneID() <<", WarID="<<nWarID );

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Monster::release() 
{
	// 销毁 : 因为智能指针会延迟销毁，所以有时候需要主动调用销毁函数
	//destroy();

    if (m_pMobaGoodsEffect)
    {
        m_pMobaGoodsEffect->release();
        m_pMobaGoodsEffect = NULL;
    }
    // 释放属性部件
    if(m_pPartList[PART_PROPERTY] != 0)
    {
        m_pPartList[PART_PROPERTY]->release();
        m_pPartList[PART_PROPERTY] = 0;
    }
    // 释放pk部件
    if(m_pPartList[PART_PK] != 0)
    {
        m_pPartList[PART_PK]->release();
        m_pPartList[PART_PK] = 0;
    }

    

	delete this;
    //ErrorLn("Monster::release()");
}

///////////////////////////////////////////////////////////////////////////////////
// 销毁 : 因为智能指针会延迟销毁，所以有时候需要主动调用销毁函数
void Monster::destroy()
{
    if (m_pMobaGoodsEffect)
    {
        m_pMobaGoodsEffect->releaseMobaGoodsEffect();
    }

	if ( m_pPartList[PART_PROPERTY]==0 )
		return;

	// 发出实体销毁事件
	UID uid = getUID();
    UID uidMaster = INVALID_UID;

    __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty != NULL)
    {
        pProperty->getProperty_custom(PROPERTY_MASTER_UID,&uidMaster,sizeof(UID));
    }

	event_entity_destroy context;
	context.uid         = uid;
    // 怪物死亡状态下删除不触发AOI消息
    context.bBroadAOI   = ( getState()!=ENTITY_STATE_DIE );

    getEntityEvent()->FireVote(EVENT_ENTITY_DESTROY, (LPCSTR)&context,sizeof(context));
	
    for ( int i=ENTITY_PART_MAX-1;i>=0;--i )
    {
        // 优先停止一些逻辑处理
        if ( m_pPartList[i]!=0 )
        {
            m_pPartList[i]->restore();
        }
    }

	for ( int i=ENTITY_PART_MAX-1;i>=0;--i )
	{
        // 特殊处理:销毁属性部件
        if( i==PART_PROPERTY)
        {
            __IEntityProperty* pProperty = (__IEntityProperty*)m_pPartList[i];
            if(pProperty != 0)
            {
                pProperty->destroy();
            }
            continue;
        }
        // 特殊处理：销毁pk部件 其他部件todo.
        if( i==PART_PK )
        {
            __IPlayerKillPart* pPK = (__IPlayerKillPart*)m_pPartList[i];
            if( pPK!=0 )
            {
                pPK->destroy();
            }
            continue;
        }

		if ( m_pPartList[i]!=0 )
		{
			m_pPartList[i]->release();
			m_pPartList[i] = 0;
        }
    }

    m_belongEntityMgr.restore();

    getEntityEvent()->FireExecute(EVENT_ENTITY_DESTROY,(LPCSTR)&context,sizeof(context));

	if ( m_pEventEngine )
	{
		delete m_pEventEngine;
		m_pEventEngine = 0;
	}

    //if(UID_2_TYPE(uidMaster) == TYPE_PLAYER_ROLE)
    //{
    //    ActorHelper actorHelper(uidMaster);
    //    if(actorHelper.m_ptr)
    //    {
    //        actorHelper.m_ptr->onAddRemoveSummonEntity(uid, false);
    //    }
    //}

    // 如果是有主人的怪物，通知到主人怪物已创建好了
    if( isValidUID(uidMaster) )
    {
        EntityBelongContext bedata;
        bedata.isCreated = false;
        bedata.uid = uid;
        bedata.uidMaster = uidMaster;

        entityNotifyMaster(bedata.uidMaster , bedata);
    }

	m_nMapID = INVALID_MAPID;
	//gServerGlobal->getEntityFactory()->revertUID(uid);
}

///////////////////////////////////////////////////////////////////////////////////
UID Monster::getUID() const
{
	UID ret = INVALID_UID;

   __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty == NULL)
    {
        return INVALID_UID;
    }

	if ( pProperty->getProperty_custom(PROPERTY_UID,&ret,sizeof(UID)) )
	{
		return ret;
	}

	return INVALID_UID;
}

//////////////////////////////////////////////////////////////////////////
ENTITY_TYPE Monster::getType()
{
	return TYPE_MONSTER;
}

///////////////////////////////////////////////////////////////////////////////////
char * Monster::getName()
{
   
    if(m_szName[0] == '\0')
    {
        __IEntityProperty* pProperty = GetProperty();
        if (pProperty)
        {
            sstrcpyn(m_szName,pProperty->getProperty_String(PROPERTY_NAME).c_str(),sizeof(m_szName));
        }
    }

    return m_szName;

}

///////////////////////////////////////////////////////////////////////////////////
int Monster::getSceneID() const
{
    __IEntityProperty* pProperty = GetProperty();
    if (!pProperty)
    {
        return INVALID_SCENE_ID;
    }

    int nSceneID = pProperty->getProperty_Integer(PROPERTY_SCENEID);
    if (nSceneID == 0)
    {
        UID uid = getUID();
        nSceneID = UID_2_SCENE(uid);
    }
    return nSceneID;
}

// 取得当前所在地图ID
int Monster::getMapID() const
{
	return m_nMapID;
}

///////////////////////////////////////////////////////////////////////////////////
Vector3 Monster::getLocation() const 
{
	Vector3  ret(0.0f,0.0f,0.0f);

    __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty == NULL)
    {
        return ret;
    }

	if (pProperty->getProperty_custom(PROPERTY_LOCATION,&ret,sizeof(ret)) )
	{
		return ret;
	}

	return ret;
}

void Monster::setLocation(Vector3 vLoc)
{
#ifdef ENTITY_CHECK_LOCATION
    if( _isnan(vLoc.x) || _isnan(vLoc.z) || abs(vLoc.x) > CHECK_LOCATION_THRESHOLD  || abs(vLoc.z) > CHECK_LOCATION_THRESHOLD )
    {
        __debugbreak();
    }
#endif

    __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty == NULL)
    {
        return ;
    }

	pProperty->setProperty_custom( PROPERTY_LOCATION,(void*)&vLoc,sizeof(vLoc) );
}

Vector3 Monster::getAngle() const
{
    Vector3  ret(0.0f,0.0f,0.0f);
    __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty == NULL)
    {
        return ret;
    }

    if ( pProperty->getProperty_custom(PROPERTY_ANGLE,&ret,sizeof(ret)) )
    {
        return ret;
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
__IEntityPart * Monster::getEntityPart(ENTITY_PART id) 
{
	if ( id>=0 && id<ENTITY_PART_MAX )
	{
		return m_pPartList[id];
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
void Monster::handleMessage( void * data,int32 len )
{
	if ( data==NULL || len<sizeof(SNetMsgHead) )
	{
		ErrorLn("Monster::handleMessage failed! message length is too short! [len]" << len );
		return;
	}

	SNetMsgHead * head = (SNetMsgHead *)data;
	data = (void*)(head+1);
	len -= sizeof(SNetMsgHead);

	if ( len<sizeof(SMsgEntityHead) )
	{
		ErrorLn("Monster::handleMessage failed! message length is too short! msgid="<< head->byKeyAction );
		return;
	}

	SMsgEntityHead * pEntityHead = (SMsgEntityHead *)data;
	void *buff = (void*)(pEntityHead+1);
	len -= sizeof(SMsgEntityHead);

	BYTE partId = pEntityHead->byPartID;
	BYTE msgId = head->byKeyAction;

	// 实体中的异常消息
	if (partId > ENTITY_PART_MAX)
	{
		ErrorLn("monster:["<<getName()<<"] handleMessage failed! partId overflow! msgid="<< msgId );
		return;
	}

	// 状态过滤
	if (m_pCurrentState && !m_pCurrentState->onMessage(head, pEntityHead, buff, len))
	{
		return;
	}

	// 由实体来处理
	if (partId < ENTITY_PART_MAX)
	{
		if (m_pPartList[partId]==NULL)
		{
			ErrorLn("monster:["<<getName()<<"] handleMessage failed! partId not instance! msgid="<< msgId );
			return;
		}

		//ENTITY_TRACELN("Monster::handleMessage() partid="<< partId <<", msg="<< msgId);
		m_pPartList[partId]->onMessage(pEntityHead, msgId, buff, len);
		return;
	}
	else
	{
		// 默认为实体来处理了
		ErrorLn("monster:["<<getName()<<"] handleMessage has a message not handle! message="<< msgId);
	}
};

/** 改变当前状态 
@ nState: 最新的状态 ENTITY_STATE
@return  
*/
bool Monster::changeState(int nState)
{
	if (nState <= ENTITY_STATE_UNKNOW || nState >= ENTITY_STATE_MAXID)
	{
		return false;
	}
	// 状态没变化
	if (nState == getState())
	{
		return false;
	}

	// 停止当前状态
	if(m_pCurrentState != NULL)
	{
		m_pCurrentState->end();
	}

	switch (nState)
	{
	case ENTITY_STATE_DIE:
		{
			m_pCurrentState = &m_EntityDieState;
		}
		break;
	case ENTITY_STATE_FIGHT:
		{
			m_pCurrentState = &m_EntityFightState;
		}
		break;
	default:
		{
			m_pCurrentState = &m_EntityStandState;
		}
		break;
	}
	// 启动新状态
	if(m_pCurrentState == NULL)
	{
		return false;
	}
	m_pCurrentState->start();
	// 更新状态值
    int nOldState = m_byCurControlState;
	m_byCurControlState = nState;	

	// 发送状态变化事件
	event_entity_change_state eventData;
	eventData.uid = getUID();
    eventData.nOldState = nOldState;
	eventData.nState = nState;

    __IEntityEvent * pEntityEvent = getEntityEvent();
    if(pEntityEvent)
    {
        pEntityEvent->FireExecute(EVENT_ENTITY_CHANGE_STATE,(LPCSTR)&eventData, sizeof(eventData));
        pEntityEvent->FireExecute(EVENT_GAME_ENTITY_CHANGE_STATE,(LPCSTR)&eventData, sizeof(eventData));
    }
	return true;
}

// 取得当前状态 
int Monster::getState(void)
{
	return  m_byCurControlState;
}

// 取得数值属性
int Monster::getIntProperty(int nPropID)
{
	__IEntityProperty * pPropertyPart = GetProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_Integer(nPropID) : 0);
}

// 设置数值属性
bool Monster::setIntProperty(int nPropID, int nValue)
{
    __IEntityProperty * pPropertyPart = GetProperty();
    return (pPropertyPart ? pPropertyPart->setProperty_Integer(nPropID, nValue) : false);
}

// 取得字符串属性
std::string Monster::getStringProperty(int nPropID)
{
	__IEntityProperty * pPropertyPart = GetProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_String(nPropID) : "");
}

// 取得来源游戏世界ID
int Monster::getFromGameWorldID()
{
	// todo
	return 0;
}

// 设置阵营
bool Monster::setCamp(BYTE byCamp)
{
	__IEntityProperty * pProperty = (__IEntityProperty *)getEntityPart(PART_PROPERTY);
	if (pProperty == NULL)
	{
		return false;
	}

	return pProperty->setProperty_Integer(PROPERTY_CAMP, byCamp);
}

// 设置怪物视野
bool Monster::setSight(float fEyeshot)
{
	__IEntityAOI * pAOI = (__IEntityAOI*)getEntityPart(PART_AOI);
	if ( pAOI==0 )
		return false;

	pAOI->setSight(fEyeshot);
	return true;
}

bool Monster::onSerialize( rkt::ibuffer & in,int flag )
{
    bool bResult = true;
    while( true )
    {
        short part_id = 0; int len = 0;
        in >> part_id >> len;
        if ( !in )
        {
            return ( in.current()==in.end() );
        }

        if(len == 0)
        {
            continue;
        }

        if(in.space() < len)
        {
            ErrorLn(__FUNCTION__ << ", space < len, part id=" << part_id);
            return false;
        }

        __IEntityPart * part = getEntityPart((ENTITY_PART)part_id);
        if ( part )
        {
            if ( !part->onSerialize(ibuffer(in.current(), len),flag) )
            {
                ErrorLn(__FUNCTION__ << " failed, part=" << part_id);
                bResult = false;
            }
        }
        else
        {
            ErrorLn(__FUNCTION__ << "  failed! Can not find part id"<<part_id);
            bResult = false;
        }

        in.offset(len);
    }

    return bResult;
}

bool Monster::deSerialize( rkt::obuf & out,int flag )
{
    bool bResult = true;
    for ( int id=0;id<ENTITY_PART_MAX;++id )
    {
        if ( m_pPartList[id] )
        {
            out << (short)id;                                      // 部件id

            int len_offset = out.size();     
            out << (int)0;                                            // 数据长度

            int begin = out.size();                                
            if ( !m_pPartList[id]->deSerialize(out,flag) )         // 数据
            {
                ErrorLn(__FUNCTION__ << ", part=" << id);
                bResult = false;
            }

            *((int*)(out.data() + len_offset)) = out.size() - begin;              // 保存长度
        }
    }

    return bResult;
}

// 设置地图ID
void Monster::setMapID(int nMapID)
{
	m_nMapID = nMapID;
}

// 用于通知指定阵营野怪死亡状况
void Monster::msgToSpecCampWildInfo(UID uMonsterUid, int nCamp)
{
	if (uMonsterUid == INVALID_UID)
	{
		ErrorLn("uMonsterUid is INVALID_UID !!! " << " FUNCTION: " << __FUNCTION__ << "FILE: " << __FILE__ << " LINE: " << __LINE__);
		return;
	}

	__IWarPart *pWarPart = reinterpret_cast<__IWarPart *>(m_pPartList[PART_WAR]);
	if(pWarPart == NULL)
	{
		return;
	}

	int nWarID = pWarPart->getWarID();
	SERVICE_PTR pWarContainer = gServerGlobal->getWarManager()->GetWar(nWarID);
	if(pWarContainer == NULL)
	{
		return;
	}

	IWarService* pWarService = reinterpret_cast<IWarService *>(pWarContainer->query_interface());

	pWarService->msgToSpecCampWildInfo(uMonsterUid, nCamp);
}

// 强制自杀
void Monster::forceKillSelf()
{
	// 由于这种自杀不会触发死亡事件，但要把实体死亡告知相关的模块
	__IEntityEvent * pEntityEvent = getEntityEvent();
	if (pEntityEvent)
	{
		// 发出死亡事件
		event_entity_die die;
		die.uidOperator = 0;
		die.uidTarget = getUID();

		//pEntityEvent->FireExecute(EVENT_ENTITY_KILLSELF, (LPCSTR)&die, sizeof(die));
	}	

	destroy();
}

bool Monster::setVigourRatio(int* pVigourRatio)
{
	__IEntityProperty * pProperty = (__IEntityProperty *)getEntityPart(PART_PROPERTY);
	if (pProperty == NULL)
	{
		return false;
	}

	return ((EntityPart_MonsterProperty*)pProperty)->setVigourRatio(pVigourRatio);
}

int Monster::mobaGoodsAddEffect(int nEffectID,void * pAddContext, size_t len)
{
    if (m_pMobaGoodsEffect == NULL)
    {
        return 0;
    }
    return m_pMobaGoodsEffect->mobaGoodsAddEffect(nEffectID, pAddContext, len);
}

void Monster::mobaGoodsRemoveEffect(int nID)
{
    if (m_pMobaGoodsEffect == NULL)
    {
        return;
    }
    
    m_pMobaGoodsEffect->mobaGoodsRemoveEffect(nID);

}

void Monster::releaseMobaGoodsEffect()
{
    if (m_pMobaGoodsEffect == NULL)
    {
        return;
    }
    m_pMobaGoodsEffect->releaseMobaGoodsEffect();
}
