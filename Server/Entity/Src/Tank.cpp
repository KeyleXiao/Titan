/*******************************************************************
** 文件名:	e:\Rocket\Server\EntityServer\Src\Tank.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015-2-26
** 版  本:	1.0
** 描  述:	载具
** 应  用:

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "Tank.h"
#include "EventDef.h"
#include "NetMessageDef.h"
#include "ISpell.h"
#include "TankProperty.h"
#include "IServerGlobal.h"
#include "IEntityAOI.h"
#include "IMovePart.h"
#include "IServerGlobal.h"
#include "IDiePart.h"
#include "IFreezeServer.h"
#include "IBuffServer.h"
#include "IDamageServer.h"
#include "ITankServer.h"
#include "IDragonBallPart.h"
#include "IPlayerKillPart.h"
// 取得属性接口
#define GetProperty() ((__IEntityProperty*)m_pPartList[PART_PROPERTY])
//////////////////////////////////////////////////////////////////////////
CTank::CTank()
{
	memset( m_pPartList,0,sizeof(m_pPartList) );
	m_pEventEngine = NULL;
	m_nMapID = INVALID_MAPID;

	// 当前控制状态
	m_byCurControlState = ENTITY_STATE_UNKNOW;
	// 当前状态
	m_pCurrentState = 0;

    memset(m_szName,0,sizeof(m_szName));
}

CTank::~CTank()
{
}

///////////////////////////////////////////////////////////////////////////////////
bool CTank::create(UID uid,void * pContext,int len,int flag)
{
#define TANK_CREATE_FAIL_DELETE_PARTS(errinfo)	\
	ErrorLn(errinfo);\
	for(int n=ENTITY_PART_MAX-1;n<=0;--n)\
		safeRelease(m_pPartList[n]);

	// 事件中心
	if ( m_pEventEngine ) delete m_pEventEngine;
	m_pEventEngine = new EntityEvent(uid);

	//////////////////////////////////////////////////////////////////////////
	// 属性部件
	__IEntityProperty * property = new EntityPart_TankProperty;
	m_pPartList[PART_PROPERTY] = property;

	// 视野部件
	__IEntityAOI * pAOI = CreateEntityAOIProc();
	if ( pAOI==0)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create AOI part failed!");
		return false;
	}
	m_pPartList[PART_AOI] = pAOI;

	// 移动组件
	__IMovePart * pMove = CreateMovePartProc();
	if ( pMove==0)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create Move part failed!");
		return false;
	}
	m_pPartList[PART_MOVE] = pMove;

	// 技能部件
	__ISpellPart *pSpellPart = gServerGlobal->getSpellFactory()->createSpellPart();
	if (pSpellPart == 0)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create Spell Part failed!");
		return false;
	}
	m_pPartList[PART_SPELL] = pSpellPart;

	// 冷却服务器
	IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
	if (pFreezeServer == 0)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("get Freeze Server Failed!");
		return false;
	}
	// 冷却部件
	__IFreezePart *pFreezePart = pFreezeServer->CreatePart();
	if (pFreezePart == NULL)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create pFreezePart failed!");
		return false;
	}
	m_pPartList[PART_FREEZE] = pFreezePart;

	// 死亡部件
	__IDiePart * pDiePart = CreateDiePartProc();
	if ( pDiePart==0 )
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create die part failed!");
		return false;
	}
	m_pPartList[PART_DIE] = pDiePart;

	// Buff服务器
	IBuffServer *pBuffServer = gServerGlobal->getBuffServer();
	if (pBuffServer == 0)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("get Buff Server Failed!");
		return false;
	}
	// Buff部件
	__IBuffPart *pBuffPart = pBuffServer->CreatePart();
	if ( pBuffPart==0)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create Buff part failed!");
		return false;
	}
	m_pPartList[PART_BUFF] = pBuffPart;

	// 伤害服务器
	IDamageServer *pDamageServer = gServerGlobal->getDamageServer();
	if (pDamageServer == 0)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("get Damage Server Failed!");
		return false;
	}
	// 伤害部件
	__IDamagePart *pDamagePart = pDamageServer->crateDamagePart();
	if ( pDamagePart==0 )
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create Damage part failed!");
		return false;
	}
	m_pPartList[PART_DAMAGE] = pDamagePart;

	// tank服务器
	ITankServer *pTankServer = gServerGlobal->getTankServer();
	if (pDamageServer == 0)
	{
		TANK_CREATE_FAIL_DELETE_PARTS("get Tank Server Failed!");
		return false;
	}

	// Tank部件
	__ITankBoxPart *pTankBoxPart = pTankServer->CreateTankBoxPart();
	if ( pTankBoxPart==0 )
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create Damage part failed!");
		return false;
	}
	m_pPartList[PART_TANK_BOX] = pTankBoxPart;

	// 七龙珠部件
	__IDragonBallPart * pDragonBallPart = CreateDragonBallPartProc();
	if ( pDragonBallPart==0 )
	{
		TANK_CREATE_FAIL_DELETE_PARTS("create Test part failed!");
		return false;
	}
	m_pPartList[PART_DRAGONBALL] = pDragonBallPart;

    // PK组件
    __IPlayerKillPart *pPlayerKill = CreatePlayerKillPartProc();
    if ( pPlayerKill==0 )
    {
        TANK_CREATE_FAIL_DELETE_PARTS("create pPlayerKill part failed!");
        return false;
    }
    m_pPartList[PART_PK] = pPlayerKill;

	//////////////////////////////////////////////////////////////////////////
	// 序列化
	ibuffer in(pContext,len);

	if ( onSerialize(in,flag)==false )
	{
		ErrorLn("CTank onSerialize failed! flag="<<flag);
		return false;
	}

	// 初始化各部件数据
	for (int i=PART_PROPERTY; i<ENTITY_PART_MAX; ++i)
	{
		if (m_pPartList[i] && m_pPartList[i]->onLoad(this, elogin_online) == false)
		{
			TANK_CREATE_FAIL_DELETE_PARTS("create failed! partID:"<<i);
			return false;
		}
	}

	// 站立状态
	m_EntityStandState.create(this);
	// 死亡状态
	m_EntityDieState.create(this);
	// 战斗死亡状态
	m_EntityFightState.create(this);
	// 转到站立状态
	changeState(ENTITY_STATE_STAND);

	event_entity_create context;
	context.uid = uid;
	context.flag= flag;

	getEntityEvent()->FireExecute(EVENT_ENTITY_CREATE,(LPCSTR)&context,sizeof(context));
    getEntityEvent()->FireExecute(EVENT_ENTITY_AFTER_CREATE, NULL, 0);

	// 开始初始化buff
	pBuffPart->BuildInitBuff();

	return true;
}

//////////////////////////////////////////////////////////////////////////
void CTank::release() 
{
	delete this;
}
///////////////////////////////////////////////////////////////////////////////////
void CTank::destroy()
{
	if ( m_pPartList[PART_PROPERTY]==0 )
		return;

	// 发出实体销毁事件
	UID uid = getUID();
	event_entity_destroy context;
	context.uid = uid;
    context.bBroadAOI = true;

    getEntityEvent()->FireVote(EVENT_ENTITY_DESTROY, (LPCSTR)&context,sizeof(context));
	
	for ( int i=ENTITY_PART_MAX-1;i>=0;--i )
	{
		if ( m_pPartList[i]!=0 )
		{
			m_pPartList[i]->release();
            m_pPartList[i] = 0;
		}
	}

    getEntityEvent()->FireExecute(EVENT_ENTITY_DESTROY,(LPCSTR)&context,sizeof(context));

	if ( m_pEventEngine )
	{
		delete m_pEventEngine;
		m_pEventEngine = 0;
	}

	m_nMapID = INVALID_MAPID;
	//gServerGlobal->getEntityFactory()->revertUID(uid);
}

///////////////////////////////////////////////////////////////////////////////////
UID CTank::getUID() const
{
	UID ret = INVALID_UID;
    __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty == NULL)
    {
        return ret;
    }

    if ( pProperty->getProperty_custom(PROPERTY_UID,&ret,sizeof(ret)) )
    {
        return ret;
    }

	return INVALID_UID;
}

//////////////////////////////////////////////////////////////////////////
ENTITY_TYPE CTank::getType()
{
	return TYPE_TANK;
}

///////////////////////////////////////////////////////////////////////////////////
char * CTank::getName()
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
int CTank::getSceneID() const
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
int CTank::getMapID() const
{
    __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty == NULL)
    {
        return 0;
    }

	return m_nMapID;
}

///////////////////////////////////////////////////////////////////////////////////
Vector3 CTank::getLocation() const 
{
	Vector3 ret(0.0f,0.0f,0.0f);

    __IEntityProperty* pProperty =  GetProperty(); 
    if(pProperty == NULL)
    {
        return ret;
    }

	if ( pProperty->getProperty_custom(PROPERTY_LOCATION,&ret,sizeof(ret)) )
	{
		return ret;
	}

	return ret;
}

void CTank::setLocation(Vector3 vLoc)
{
#ifdef ENTITY_CHECK_LOCATION
    if(_isnan(vLoc.x) || _isnan(vLoc.z) || abs(vLoc.x) > CHECK_LOCATION_THRESHOLD  || abs(vLoc.z) > CHECK_LOCATION_THRESHOLD )
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

Vector3 CTank::getAngle() const
{
    Vector3 ret(0.0f,0.0f,0.0f);

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
__IEntityPart * CTank::getEntityPart(ENTITY_PART id) 
{
	if ( id>=0 && id<ENTITY_PART_MAX )
	{
		return m_pPartList[id];
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
void CTank::handleMessage( void * data,int32 len )
{
	if ( data==NULL || len<sizeof(SNetMsgHead) )
	{
		ErrorLn("CTank::handleMessage failed! message length is too short! [len]" << len );
		return;
	}

	SNetMsgHead * head = (SNetMsgHead *)data;
	data = (void*)(head+1);
	len -= sizeof(SNetMsgHead);

	if ( len<sizeof(SMsgEntityHead) )
	{
		ErrorLn("CTank::handleMessage failed! message length is too short! msgid="<< head->byKeyAction );
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
		ErrorLn("tank:["<<getName()<<"] handleMessage failed! partId overflow! msgid="<< msgId );
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
			ErrorLn("tank:["<<getName()<<"] handleMessage failed! partId not instance! msgid="<< msgId );
			return;
		}

		//ENTITY_TRACELN("CTank::handleMessage() partid="<< partId <<", msg="<< msgId);
		m_pPartList[partId]->onMessage(pEntityHead, msgId, buff, len);
		return;
	}
	else
	{
		// 默认为实体来处理了
		ErrorLn("tank:["<<getName()<<"] handleMessage has a message not handle! message="<< msgId);
	}
};

/** 改变当前状态 
@ nState: 最新的状态 ENTITY_STATE
@return  
*/
bool CTank::changeState(int nState)
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
int CTank::getState(void)
{
	//return (m_pCurrentState ? m_pCurrentState->getState() : ENTITY_STATE_UNKNOW);
	return ENTITY_STATE_UNKNOW;
}

// 取得数值属性
int CTank::getIntProperty(int nPropID)
{
	__IEntityProperty * pPropertyPart = GetProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_Integer(nPropID) : 0);
}

// 设置数值属性
bool CTank::setIntProperty(int nPropID, int nValue)
{
    __IEntityProperty * pPropertyPart = GetProperty();
    return (pPropertyPart ? pPropertyPart->setProperty_Integer(nPropID, nValue) : false);
}

// 取得字符串属性
std::string CTank::getStringProperty(int nPropID)
{
	__IEntityProperty * pPropertyPart = GetProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_String(nPropID) : "");
}

// 取得来源游戏世界ID
int CTank::getFromGameWorldID()
{
	// todo
	return 0;
}

bool CTank::onSerialize( rkt::ibuffer & in,int flag )
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

bool CTank::deSerialize( rkt::obuf & out,int flag )
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



/** 清空
@param   
@param   
@return  
*/
void CTank::Clean(void)
{
	//// 结束当前状态	
	//if(m_pCreatureCurState != NULL)
	//{
	//	m_pCreatureCurState->End(NULL, 0);
	//	m_pCreatureCurState = NULL;
	//}

	//// 站立状态
	//m_CreatureStandState.Restore();	

	//// 坐下状态
	//m_CreatureSitState.Restore();

	//// 骑马状态
	//m_CreatureRideState.Restore();	

	//// 死亡状态
	//m_CreatureDieState.Restore();

	//// 取得相应的场景并移出场景
	//IGameZone * pGameZone = g_ExternalFacade.GetGameZone(GetZoneID());
	//if(pGameZone != NULL && IsInZoneTile())
	//{
	//	pGameZone->RemoveEntity(GetMapLoc(), this);
	//}

	//// 移出场景世界
	//IEntityWorld * pEntityWorld = g_ExternalFacade.GetEntityWorld();
	//if(pEntityWorld != NULL && m_bInEntityWorldFlag)
	//{
	//	pEntityWorld->Remove(this);
	//}

	//// 是否在实体世界内
	//m_bInEntityWorldFlag = false;

	//// 部件还原
	//for(int i = 0; i < ENTITYPART_ENTITY_MAXID; i++)
	//{
	//	if(m_pEntityPart[i] != NULL)
	//	{
	//		// 此处切忌不能用SAFE_RESTORE宏
	//		m_pEntityPart[i]->Restore();
	//	}		
	//}

	//// 实体类型
	//m_EntityClass.Restore();

	//// 信号槽
	//m_MessageSlot.Restore();

	//// 属性集
	//m_TankPropBank.Restore();

	//// 本载具配置信息
	//m_pTankSchemeInfo = NULL;

	//// 本实体九宫格玩家数量
	//m_nPersonQty9Grid = 0;

	//// 选中被观看实体列表，观看实体ID
	//m_ViewerUIDList.clear();

}


/** 取得载具称号
@param   
@param   
@return  
*/
LPCSTR CTank::GetTitle(void)
{
	//return m_TankPropBank.GetTitle();
	return "";
}

/** 设置载具名字
@param   
@param   
@return  
*/
void CTank::SetTitle(LPCSTR pszTitle)
{
	//m_TankPropBank.SetTitle(pszTitle);
}

/** 取得本载具配置信息
@param   
@param   
@return  
*/
STankSchemeInfo * CTank::GetSchemeInfo(void)
{
	return m_pTankSchemeInfo;
}


/** 测试上次移除的tile位置
@param   
@param   
@return  
*/
void CTank::DebugLastRemoveTile(POINT ptLastTile)
{

}


/** 导出数据库属性现场
@param   
@param   
@return  
*/
bool CTank::ExportMapContext(LPSTR pszContext, int &nLen)
{
	//return m_TankPropBank.ExportMapContext(pszContext, nLen);
	return "";
}

/** 取得扩展数据
@param   
@param   
@return  
*/
LPCSTR CTank::GetExtendData(void)
{
	//return m_TankPropBank.GetExtendData();
	return "";
}


/** 是否PK载具
@return  
*/
bool CTank::IsPKTank()
{
	if (m_pTankSchemeInfo == NULL)
	{
		return false;
	}
	return true;
}

/** PK载具是否有当前技能
@return  
*/
bool CTank:: IfHavePKTankSkill(int nSpellID)
{
	std::map<ushort, ushort>::iterator it = m_mPKSkills.find(nSpellID);
	if (it == m_mPKSkills.end() || it->second!=nSpellID)
	{
		return false;
	}

	return true;
}


/** 读配置文件，添加buf
@param   
@param   
@return  
*/
bool CTank::AddBuffFromScheme(void)
{
	if (m_pTankSchemeInfo==NULL)
	{
		ErrorLn(__FUNCTION__<<" 添加buff失败，配置表为空！");
		return false;
	}
	
	LONGLONG uID = getUID();
	return true;
}

/** 判断是否能随时跳出载具
@name				: 
@param				: 
@return	
*/
bool CTank::CanJumpOut(void)
{
	return true;
}

/** 判断是否禁用操作提示
@name				: 
@param				: 
@return
*/
bool CTank::IsForbidTip(void)
{
	return true;
}

/** 是否移动时打断进度条
@name				: 
@param				: 
@return
*/
bool CTank::IsMoveBreakProgBar(void)
{
	return true;
}


// 设置地图ID
void CTank::setMapID(int nMapID)
{
	m_nMapID = nMapID;
}

int CTank::mobaGoodsAddEffect(int nEffectID, void * pAddContext, size_t len)
{
    return 0;
}

void CTank::mobaGoodsRemoveEffect(int nID)
{
    return;
}

