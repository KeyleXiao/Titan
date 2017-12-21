/*******************************************************************
** 文件名:	EntityBase.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体基类实现
********************************************************************/

#include "stdafx.h"
#include "EntityBase.h"
#include "MovePart.h"
#include "AOIPart.h"
#include "MonsterProperty.h"
#include "IClientGlobal.h"
#include "EntityFactory.h"
#include "EventDef.h"
#include "IRenderView.h"
#include "ISpell.h"
#include "DiePart.h"
#include "CommonStructDef.h"
#include "ISchemeCenter.h"
#include "IFreezeClient.h"
#include "IBuffClient.h"
#include "ISlotLogicPart.h"
#include "SlotLogicPart.h"
#include "PlayerBankPart.h"
#include "PlayerKillPart.h"
#include "IWarClient.h"
#include "ICamp.h"
#include "EntityMessageCache.h"

#include <strstream>
using namespace std;

struct SCmdRouteTable
{
    BYTE table[ENTITY_CMD_MAX];

    SCmdRouteTable()
    {
        memset(table, ENTITY_PART_MAX, sizeof(table));
    }
};

static SCmdRouteTable g_cmdRoute; 

///////////////////////////////////////////////////////////////////////////////////
EntityBase::EntityBase()
	: m_viewID(INVALID_ENTITY_ID)
    , m_bOnView(false)
    , m_byteEntityType(0)
{
	memset( m_pPartList,0,sizeof(m_pPartList) ); 

	// 当前控制状态
	m_byCurControlState = ENTITY_STATE_UNKNOW;
	// 当前状态
	m_pCurrentState = 0;
}

///////////////////////////////////////////////////////////////////////////////////
bool EntityBase::create( UID uid,SEntiyExportShadowContext &shadowData,void * context,int len )
{
    m_byteEntityType = shadowData.byType;

    for ( int i=0;i<ENTITY_PART_MAX;i++ )
    {
        m_pPartList[i] = createEntityPart((ENTITY_PART)i);
    }

    // 服务器没有开启七龙珠功能
    if( ((shadowData.byReserve & 0x1) != 0x1) )
    {
        // 强行关闭七龙珠
        if ( m_pPartList[PART_DRAGONBALL]!=0 )
        {
            m_pPartList[PART_DRAGONBALL]->release();
            m_pPartList[PART_DRAGONBALL] = 0;
        }
    }

    // 设置实体UID和位置属性
    IEntityProperty * pProperty = getProperty();
    if(pProperty)
    {
        pProperty->setProperty_custom( PROPERTY_UID,&uid,sizeof(uid) );
        pProperty->setProperty_custom( PROPERTY_LOCATION,&shadowData.loc,sizeof(shadowData.loc) );

        int nSceneID = UID_2_SCENE(uid);
        pProperty->setProperty_custom( PROPERTY_SCENEID,&nSceneID,sizeof(nSceneID) );
    }

    //////////////////////////////////////////////////////////////////////////
    if ( !fireVote( EVENT_ENTITY_CREATE,&uid,sizeof(uid) ) )
    {
        return false;
    }

    // 序列化
    int flag = shadowData.byFlag;
    ibuffer in(context,len);
    if ( onSerialize(uid, in,flag)==false )
    {
        ErrorLn("entity onSerialize failed! flag="<<flag );
        return false;
    }	

    // 初始化各部件数据
    for ( int i=0;i<ENTITY_PART_MAX;++i )
    {
        IEntityPart * part = m_pPartList[i];
        if ( part && !part->onLoad(this) )
        {
            ErrorLn( "part->onLoad failed! partID:"<<i );
            return false;
        }
    }

	Vector3 v3Angle(0.0f, shadowData.fAngleY, 0.0f);
	getProperty()->setProperty_custom( PROPERTY_ANGLE,&v3Angle,sizeof(Vector3) );

    // 初始化各状态
    m_EntityStandState.create(this);
    m_EntityDieState.create(this);
	m_EntityFightState.create(this);
    // 转到设定状态
    changeState( shadowData.byCurState );

	// 发出实体创建事件
	fireEvent( EVENT_ENTITY_CREATE,&uid,sizeof(uid) );
	return true;
}

bool EntityBase::createView( int flag )
{
	EntityViewItem item;

    if ( !getBasicViewInfo(item) )
    {
        ErrorLn( "EntityBase::createView getBasicViewInfo failed!" );
        return false;
    }	

    //// 暂时设置皮肤ID到属性中，从服务器上取
    //getProperty()->setProperty_Integer( PROPERTY_SKIN,item.nSkinID );

	getRender()->createEntityView( item );
    m_viewID = item.EntityID;
    if ( m_viewID==INVALID_ENTITY_ID )
    {
        ErrorLn( "EntityBase::createView invalid viewID=" << m_viewID );
        return false;
    }
	
    // 发出实体显示创建事件
    UID uid = getUID();
    fireEvent( EVENT_ENTITY_CREATE_VIEW,&uid,sizeof(uid) );

	// 运行缓存的消息
	CEntityMessageCache::getInstance().RunMessage(uid);

	// 输出信息
	string strOutput;
	switch(item.EntityType)
	{
	case ENTITY_TYPE::TYPE_PLAYER_ROLE: { strOutput=( item.byIsHero? "Marster":"Player"); } break;
	case ENTITY_TYPE::TYPE_MONSTER:		{ strOutput="Monster"; } break;
	case ENTITY_TYPE::TYPE_TANK:		{ strOutput="Tank"; } break;
	default:							{ strOutput="Player"; } break;
	}

#if 0
    ostrstream buffer;
    buffer <<  strOutput.c_str()<< " Create:name="<<getName()<<",id="<<getIntProperty(PROPERTY_ID)<<",uid="<<uid<<",viewid="<< m_viewID <<",HeroID="<<item.nHeroID <<",SkinID="<<item.nSkinID<< std::ends;

    if(item.EntityType == ENTITY_TYPE::TYPE_PLAYER_ROLE) {
        if ( item.byIsHero==1 )	{
            ENTITY_EMPHASISLN( buffer.str());
        } else {
            ENTITY_WARNINGLN( buffer.str() );
        }
    } else {
        //ENTITY_TRACELN( buffer.str() );
    }
#endif

    return true;
}

/** 更新实体显示层的参数
@name				: 
@param				: 
@return
*/
void EntityBase::updateView( bool bOnView ,bool bUseTranSport)
{
    m_bOnView = bOnView;

    // 更新实体位置
    Vector3 loc = getLocation();
    Vector3 angle = getAngle();
    //TRACE_ENTITY("angle= "<<angle.x<<", "<<angle.y<<", "<<angle.z);

    cmd_creature_transport pos;

    pos.fPosition_x = loc.x;
    pos.fPosition_y = loc.y;
    pos.fPosition_z = loc.z;

    pos.fRotation_x = angle.x;
    pos.fRotation_y = angle.y;
    pos.fRotation_z = angle.z;
    pos.bUseAngle = 1;
    pos.nReason = TRANSPORT_REASON_APPEAR;

    if(bUseTranSport)
    {
        sendCommandToEntityView(ENTITY_TOVIEW_SYNC_TRANSPORT,sizeof(pos),0,&pos, sizeof(pos));
        //TRACE_ENTITY( "updateView set pos="<<loc.x<<", "<<loc.y<<", "<<loc.z );
    }

    // 不在视野的话显示层参数都不用更新了
    if ( !m_bOnView )
    {
        // 发送实体离开视野消息
        sendCommandToEntityView(ENTITY_TOVIEW_ENTITY_LEAVE_VIEW,0,0,NULL, 0);
        return;
    }

    // 更新所有属性
    getProperty()->updateView(PROPERTY_MAX);

    // 更新阵营
    int nCampFlag = isHero()? CampFlag_Self : Camp::getCampFlag( gClientGlobal->getHero(),this );

    sendCommandToEntityView(ENTITY_TOVIEW_SYNC_BLOODSHOW,nCampFlag,0,0,0 );
    // 发送实体进入视野消息
    sendCommandToEntityView(ENTITY_TOVIEW_ENTITY_ENTER_VIEW,0,0,NULL, 0);
}

/** 改变当前状态 
@ nState: 最新的状态 ENTITY_STATE
@return  
*/
bool EntityBase::changeState( int nState )
{
	if ( nState<=ENTITY_STATE_UNKNOW || nState>=ENTITY_STATE_MAXID )
	{
		return false;
	}

	// 状态没变化
	if ( nState==getState() )
	{
		return false;
	}

	// 停止当前状态
	if ( m_pCurrentState!=NULL )
	{
		m_pCurrentState->end();
	}
	
	switch ( nState )
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
	m_pCurrentState->start();

	// 更新状态值
	m_byCurControlState = nState;

	UID uid = getUID();
    BYTE type  = UID_2_TYPE(uid);

	// 发送状态变化事件
	event_entity_change_state eventData;
	eventData.uid = uid;
	eventData.nState = nState;
	gClientGlobal->getEventEngine()->FireExecute(EVENT_ENTITY_CHANGE_STATE, type, uid, (LPCSTR)&eventData, sizeof(eventData));

	// 主角通知显示层
	if (isHero())
	{
		sendCommandToEntityView(ENTITY_TOVIEW_SWITCH_STATE, nState, 0, NULL, 0);
	}

	//ENTITY_TRACELN("Entity change state,uid="<< getUID()<<", state="<< nState);

	return true;
}

// 取得当前状态 
int EntityBase::getState(void)
{
	return m_byCurControlState;
}

// 发送命令到显示层实体,用于封装getRenderView()->sendEntityCommand
void EntityBase::sendCommandToEntityView(int cmdID, int nParam, const char* strParam, void* ptrParam, int len)
{
	// 取得显示层接口
	IRenderViewProxy* pRenderView = gClientGlobal->getRenderView();
	if(!pRenderView)
	{
		return;
	}

#ifdef PERFORMANCE_CLIENT_TEST
    if(cmdID == ENTITY_TOVIEW_UPDATE_INTEGERPROPERTY)
    {
        IEntityFactory * pFactory = gClientGlobal->getEntityFactory();
        if(pFactory)
        {
            pFactory->onUpdateView(this, cmdID, nParam, strParam, ptrParam, len);
        }
    }
#endif

    //TraceLn(__FUNCTION__": view="<<m_viewID << ", cmd="<<cmdID);
	// 发送到显示层
	pRenderView->sendEntityCommand(getUID(), m_byteEntityType, cmdID, nParam, strParam, ptrParam, len );
}

///////////////////////////////////////////////////////////////////////////////////
UID EntityBase::getUID() const
{
	void * data = 0;
	int len = 0;
	if ( getProperty()->getProperty_custom(PROPERTY_UID,data,len) )
	{
		return *(UID*)data;
	}

	return INVALID_UID;
}

bool EntityBase::isHero() const
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////////
const char * EntityBase::getName() const
{
	return getProperty()->getProperty_String(PROPERTY_NAME);
}

///////////////////////////////////////////////////////////////////////////////////
int EntityBase::getMapID() const
{
	return getProperty()->getProperty_Integer(PROPERTY_SCENEID);
}

///////////////////////////////////////////////////////////////////////////////////
Vector3 EntityBase::getLocation() const 
{
	void * data = 0;
	int len = 0;
	if ( getProperty()->getProperty_custom(PROPERTY_LOCATION,data,len) )
	{
		return *(Vector3*)data;
	}

	return Vector3(0.0f,0.0f,0.0f);
}

Vector3 EntityBase::getAngle() const 
{
	void * data = 0;
	int len = 0;
	if ( getProperty()->getProperty_custom(PROPERTY_ANGLE,data,len) )
	{
		return *(Vector3*)data;
	}

	return Vector3(0.0f,0.0f,0.0f);
}

///////////////////////////////////////////////////////////////////////////////////
IEntityPart * EntityBase::getEntityPart(ENTITY_PART id) 
{
	if ( id>=0 && id<ENTITY_PART_MAX )
	{
		return m_pPartList[id];
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
void EntityBase::sendCommand( int cmdid,void * data,int32 len )
{
    BYTE& byPart = g_cmdRoute.table[cmdid]; 
    if( byPart!=ENTITY_PART_MAX )
    {
        m_pPartList[byPart]->onCommand(cmdid,data,len);
    }
    else
    {
        for ( int id=0;id<ENTITY_PART_MAX;++id )
        {
            if ( m_pPartList[id] )
            {
                if ( m_pPartList[id]->onCommand(cmdid,data,len) )
                {
                    byPart = id; 
                    return;
                }
            }
        }
    }
};

// 处理实体消息
void EntityBase::handMessage(SNetMsgHead* head, void * pEntityHead, BYTE byPartID,int msgId,void * data,int len )
{
	if ( data==NULL || len<0 )
	{
		ErrorLn("Message length about entity is wrong!moudle="<< msgId <<",len="<< len);
		return;
	}

	if ( byPartID>ENTITY_PART_MAX )
	{
		ErrorLn("This is a wrong message about monster!moudle="<< msgId <<",action="<< byPartID);
		return;
	}

	// 状态过滤
	if ( m_pCurrentState && !m_pCurrentState->onMessage(byPartID,msgId,data,len) )
	{
		return;
	}

	if (byPartID < ENTITY_PART_MAX)
	{
		// 由实体来处理
		IEntityPart * pEntityPart = m_pPartList[byPartID];
		if ( pEntityPart==NULL )
		{
			// 默认为实体来处理了
			ErrorLn("Message about EntityBase don't handle! message="<< msgId);
			return;
		}

		pEntityPart->onMessage(pEntityHead, msgId,data,len );
	}
	else
	{
		ErrorLn( "EntityBase::handMessage byPartID=" << byPartID );
	}
}

void EntityBase::fireEvent( int eventID,void *data,int len )
{
	UID  uid   = getUID();
	BYTE type  = UID_2_TYPE(uid);

	gClientGlobal->getEventEngine()->FireExecute( eventID,type,uid,(LPCSTR)data,len );
}

bool EntityBase::fireVote( int eventID,void *data,int len )
{
	UID  uid   = getUID();
	BYTE type  = UID_2_TYPE(uid);

	return gClientGlobal->getEventEngine()->FireVote( eventID,type,uid,(LPCSTR)data,len );
}

bool EntityBase::onSerialize(UID uid, rkt::ibuffer & in,int flag )
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

        IEntityPart * part = getEntityPart((ENTITY_PART)part_id);
        if ( part )
        {
            if ( !part->onSerialize(uid, ibuffer(in.current(), len),flag) )
            {
                ErrorLn(__FUNCTION__ << " failed, part=" << part_id);
                bResult = false;
            }
        }
        else
        {
            ErrorLn(__FUNCTION__ << " failed! Can not find part id"<<part_id);
            bResult = false;
        }

        in.offset(len);
    }

    return bResult;
}

bool EntityBase::deSerialize( rkt::obuf & out,int flag )
{
    bool bResult = true;
    for ( int id=0;id<ENTITY_PART_MAX;++id )
    {
        if ( m_pPartList[id] )
        {
            out << (short)id;                                      // 部件id

            int len_offset = out.size();     
            int len = 0;
            out << len;                                            // 数据长度

            int begin = out.size();                                
            if ( !m_pPartList[id]->deSerialize(out,flag) )         // 数据
            {
                ErrorLn(__FUNCTION__ << ", part=" << id);
                bResult = false;
            }
            len = out.size() - begin;

            *((int*)(out.data() + len_offset)) = len;              // 保存长度
        }
    }

    return bResult;
}

/** 根据部件ID创建一个新的部件
@name				: 
@param				: 部件ID
@return
*/
IEntityPart * EntityBase::createEntityPart( ENTITY_PART id )
{
    switch ( id )
    {
    case PART_PROPERTY:         // 属性部件
        return NULL;
    case PART_MOVE:             // 移动部件
        return new MovePart;
    case PART_AOI:              // AOI
        return new AOIPart;
    case PART_DIE:              // 死亡部件
        return new DiePart;
    case PART_SLOTLOGIC:        // 槽位部件
        return new CSlotLogicPart;
    case PART_SPELL:            // 技能部件
        {
            ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
            return ( pSpellFactory!=NULL? pSpellFactory->createSpellPart() : NULL );
        }
    case PART_FREEZE:           // 冷却部件
        {
            IFreezeClient *pFreezeClient = gClientGlobal->getFreezeClient();
            return ( pFreezeClient!=NULL? pFreezeClient->CreatePart() : NULL );
        }
    case PART_BUFF:             // Buff部件
        {
            IBuffClient *pBuffClient = gClientGlobal->getBuffClient();
            return ( pBuffClient!=NULL? pBuffClient->CreatePart() : NULL );
        }
    case PART_PK:
        {
            return new PlayerKillPart;
        }      

    default:
        return NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////
void EntityBase::destroy(EntityDestroyReason eReason, void* pParam)
{
	UID uid = getUID();
	event_entity_destroy entityDestroy;
	entityDestroy.uid = uid;
	// 发出实体销毁
	fireEvent(EVENT_ENTITY_DESTROY, &entityDestroy, sizeof(entityDestroy));

    // 通知显示层销毁
    if ( m_viewID!=INVALID_ENTITY_ID )
    {
        if(eReason == DESTROY_REASON_NORMAL)
        {
            // 直接销毁
            getRender()->destroyEntityView(m_viewID, m_byteEntityType);
        }
        else
        {
            // 需要播放完死亡动画后自动销毁
            sendCommandToEntityView(ENTITY_TOVIEW_ENTITY_DEAD, 0, 0, pParam, sizeof(cmd_creature_die));
        }
        m_viewID = INVALID_ENTITY_ID;
    }
    
    for ( int i=ENTITY_PART_MAX-1;i>=0;--i )
    {
        if ( m_pPartList[i]!=0 )
        {
            m_pPartList[i]->restore();
        }
    }

    for ( int i=ENTITY_PART_MAX-1;i>=0;--i )
    {
        if ( m_pPartList[i]!=0 )
        {
            m_pPartList[i]->release();
            m_pPartList[i] = 0;
        }
    }

    delete this;
}
