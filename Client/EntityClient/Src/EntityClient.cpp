// EntityClient.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "EntityClient.h"
#include "EntityDef.h"
#include "IAOIPart.h"
#include "EventDef.h"
#include "ConfigIni.h"
#include "IDiePart.h"
#include "EntityMessageCache.h"

EntityClient * g_pEntityClient = NULL;



//extern std::map<int32,IGameViewEventHandler *>   g_GameViewEventMap;
//extern IGameViewStruct g_GameViewEntry ;
//////////////////////////////////////////////////////////////////////////

/** 构造函数
*/
EntityClient::EntityClient()
{
}

/** 创建技能系统服务
*/
bool EntityClient::Create()
{
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_ENTITY, this);
#ifdef PERFORMANCE_CLIENT_TEST
    m_MsgStat.create("[msg]entityid", ConfigIni::getInstance().isAccumulate(), ConfigIni::getInstance().getMsgInterVal(), ConfigIni::getInstance().getMsgMinVal());
    m_nMsgFlag = ConfigIni::getInstance().getMsgFlag();
#endif

	// 初始化实体消息缓存管理
	if (!CEntityMessageCache::getInstance().Init())
	{
		return false;
	}

	return true;
}

/** 释放技能系统服务
*/
bool EntityClient::Release()
{
	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_ENTITY,this);
	if (g_pEntityClient != NULL)
	{
        g_pEntityClient->GetEntityFactory()->destroyAllEntity();
	}
	g_pEntityClient = NULL;
    
	m_EnemySet.clear();

	// 释放实体消息缓存管理
	CEntityMessageCache::getInstance().UnInit();

	delete this;
	return true;
}

/**
@name   :获取实体工厂接口
*/
IEntityFactory* EntityClient::GetEntityFactory()
{
	return &m_EntityFactory;
}

struct EntityPosAngleInfo
{
public:
	int id;
	float pos[3];
	float dir[3];
};

/** 接受实体显示层发出的事件
@param entityID   : 实体ID
@param eventID    : 事件ID,具体定义见(EntityViewDef.h)
@param arg1       : 时间参数1
@param arg2       : 时间参数2
@return           : 是否接收成功
*/
bool EntityClient::onEntityEvent( ENTITY_ID entityID,int eventID,int nParam,const char * strParam,void * ptrParam )
{
	if (entityID==-1)
	{
		EntityPosAngleInfo* pInfo=(EntityPosAngleInfo*)ptrParam;
		int num=nParam/sizeof(EntityPosAngleInfo);
		for(int i=0;i<num;i++)
		{
			IClientEntity * pEntity = m_EntityFactory.getEntityByView(pInfo->id);
			if (pEntity)
			{
				pEntity->sendCommand( eventID,&pInfo->pos[0],sizeof(float)*6);
			}

			pInfo++;
		}
		return true;
	}
    else
	{
		IClientEntity * pEntity = m_EntityFactory.getEntityByView(entityID);
		if (!pEntity )
		{
            ErrorLn(__FUNCTION__": not found entity, entityID="<<entityID << ", event="<< eventID);
			return false;
		}

		pEntity->sendCommand( eventID,ptrParam,nParam);
		return true;
	}
}

/** 接受显示层发出的事件
@param entityID   : 实体ID
@param eventID : 事件ID,具体定义见(GameViewDef.h)
@param arg1    : 时间参数1
@param arg2    : 时间参数2
@return        : 是否接收成功
*/
bool EntityClient::onGameViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam )
{
	if ( m_GameViewEventMap.find(eventID) != m_GameViewEventMap.end() )
	{
		return m_GameViewEventMap[eventID]->onViewEvent( eventID,nParam,strParam,ptrParam );
	}

	WarningLn( "onGameViewEvent call failed! can not find eventID=" << eventID );
	return false;
}

/**
@name   :订阅显示层事件处理器
*/
bool EntityClient::registerGameViewEventHandler( int32 eventID, IGameViewEventHandler * pHandler)
{
	m_GameViewEventMap[eventID] = pHandler;

	return true;
}

/**
@name   :取消订阅显示层事件处理器
*/
bool EntityClient::unregisterGameViewEventHandler( int32 eventID)
{
	m_GameViewEventMap.erase(eventID);

	return true;
}

/////////IMessageHandler/////////////////////////////////////////////////////////////////
/** 消息处理
@param head : 消息头，通常为一个消息包的前4个字节
@param data : 消息的实际内容，除去了消息头的
@param len  : 内容长度
*/
void EntityClient::onMessage( SNetMsgHead* head, void* data, size_t len)
{
	if (head==NULL)
	{
		ErrorLn("entity client receive error message!");
		return;
	}

	BYTE msgId = head->byKeyAction;
	BYTE moduleId = head->byKeyModule;
	if (data == NULL || len < sizeof(SMsgEntityHead))
	{
		ErrorLn("Message length is wrong! moudle="<< moduleId <<",message="<< msgId <<",len="<< len);
		return;
	}

	// 取得实体消息头
	SMsgEntityHead* pEntityHead = (SMsgEntityHead*)data;
	void *buff = (void*)(pEntityHead+1);
	len -= sizeof(SMsgEntityHead);

	UID uidMaster = pEntityHead->uidMaster;
	BYTE partId = pEntityHead->byPartID;

	if (partId > ENTITY_PART_MAX)
	{
		ErrorLn("This is a wrong message!moudle="<< moduleId <<",message="<< msgId <<",action="<< partId);
		return;
	}

    // 实体全局消息处理
    if ( partId==ENTITY_PART_MAX )
    {
        ibuffer in(buff,len);
        onGlobalEntity( msgId,uidMaster,in );
    }
    // 具体到某个实体处理消息
    else
    {
#ifdef PERFORMANCE_CLIENT_TEST
        bool bCanAdd = false;
        bool bActor = (TYPE_PLAYER_ROLE == UID_2_TYPE(uidMaster));
        // 监控自己角色的命令
        if( bActor && (m_nMsgFlag & PERF_CHECK_ENTITY_CMD_SELF) )
        {
            IClientEntity * pHero = gClientGlobal->getHero();
            bCanAdd = (pHero && pHero->getUID() == uidMaster) ;
        }
        else if( bActor && (m_nMsgFlag & PERF_CHECK_ENTITY_CMD_OTHERHERO) )
        {
            bCanAdd = true;
        }
        else if( (m_nMsgFlag & PERF_CHECK_ENTITY_CMD_MONSTER) && (TYPE_MONSTER == UID_2_TYPE(uidMaster)))
        {
            bCanAdd = true;
        }

        if(bCanAdd)
        {
            int key = (partId* 1000 + msgId);
            m_MsgStat.push(uidMaster, key);
        } 
#endif
		//////////////////////////////////////////////////////////////////////////
		// 处理消息
        IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidMaster);
        if ( pEntity!=NULL )
        {
			pEntity->handMessage( head, pEntityHead, partId, msgId, buff, len );
		}
		else
		{
			// 没有找到实体，则缓存起该消息，等实体创建好后重新执行该消息
			CEntityMessageCache::getInstance().AddEntityMessage(head, pEntityHead, (char*)buff, len);
		}
    }
}

bool EntityClient::isHaveEnemyId( UID uId , bool bIsRemove)
{
    std::set<UID>::iterator result = m_EnemySet.find(uId);
    if ( result != m_EnemySet.end())
    {
        if ( bIsRemove )
        {
            m_EnemySet.erase(result);
        }
        return true;
    }
       
    return false;
}

void EntityClient::onGlobalEntity( BYTE msgId,UID uidMaster,ibuffer & in )
{
    switch ( msgId )
    {
    case SC_MSG_ENTITY_CREATE:      // 创建实体
        {
            onCreateEntity(uidMaster, in);
        }		
        break;

    case SC_MSG_ENTITY_DESTROY:
        {
            onDestroyEntity(uidMaster, in);
        }
        break;

    case SC_MSG_ENTITY_APPEAR:      // 实现进入视野
        {
            OnEntityAppear(uidMaster, in);
        }
        break;

    case SC_MSG_ENTITY_DISAPPEAR:       // 实现进入视野
        {
            OnEntityDisappear(uidMaster, in);
        }
        break;
    case SC_MSG_ENTITY_STEALTH_CHANGE: // 隐形状态改变,透明处理(如果是眼的话，不用透明了 todo.)
        {
            UID uid;
            bool bStealing;
            in >> uid >> bStealing;
            OnEntityStealthChange(uid, bStealing);
        }
        break;

    case SC_MSG_ENTITY_SWITCH_STATE:    // 状态切换
        {
            IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidMaster);
            if ( pEntity!=NULL && (pEntity->getTypeMask()&MASK_CREATURE) )
            {
                IClientEntity * pClientEntity = (IClientEntity*)pEntity;
                SEntiyExportShadowContext shadowData;
                int state;
                in >> shadowData >> state;
                if (in)
                    pClientEntity->changeState( state );
            }
        }
        break;

    default:
        ErrorLn( "EntityClient::onGlobalEntity unhandled msgId=" << msgId );
        break;
    }
}

IClientEntity * EntityClient::onCreateEntity(UID uidEntity, ibuffer & in)
{
	SEntiyExportShadowContext shadowData;
	in >> shadowData;
	
	if (!in) return NULL;

	IClientEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidEntity);
    if ( pEntity==NULL )
    {
		pEntity = gClientGlobal->getEntityFactory()->createEntity( uidEntity, shadowData, in.current(), in.space());
    }
	
	return pEntity;
}

void EntityClient::onDestroyEntity(UID uidEntity, ibuffer & in)
{
	UID uid;
	in >> uid;
	if (!in) return;

    TRACE_ENTITY( "EntityClient::onDestroyEntity uid="<<uidEntity );
    gClientGlobal->getEntityFactory()->destroyEntity(uidEntity);
}

// 实体进入视野
void EntityClient::OnEntityAppear(UID uidEntity, ibuffer & in)
{
    SEntiyExportShadowContext shadowData;
    in >> shadowData;

    if (!in)
	{
		return;
	}
	// 取得主角对象
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

    IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidEntity);
    if ( pEntity == NULL)
    {
		return;
	}
    if (!pEntity->onSerialize(uidEntity, in,shadowData.byFlag ) )
    {
		return;
	}

	IEntityProperty* pEntityProperty = reinterpret_cast<IEntityProperty *>(pEntity->getEntityPart(PART_PROPERTY));
	if (pEntityProperty == NULL)
	{
		return;
	}

    // 进出视野设置UID和SCENEID
    //pEntityProperty->setProperty_custom( PROPERTY_UID,&uidEntity,sizeof(uidEntity) );
    //int nSceneID = UID_2_SCENE(uidEntity);
    //pEntityProperty->setProperty_custom( PROPERTY_SCENEID,&nSceneID,sizeof(nSceneID) );

	// 判断当不是隐形广播怪时才通知显示层渲染模型
	if (!((shadowData.byType == TYPE_MONSTER) && ((pEntityProperty->getProperty_Integer(PROPERTY_SEX)) == MONSTER_TYPE_WILD_MONITOR)))
	{
		pEntity->updateView( true, true );

        // 假如需要隐藏尸体 通知显示层把尸体隐藏
        IDiePart* pDiePart = (IDiePart*)pEntity->getEntityPart(PART_DIE);
        if(pDiePart->isHideBody())
        {
            cmd_creature_hide_body hideBody;
            hideBody.nHideBody = 1;
            // 设置隐藏尸体
            pEntity->sendCommandToEntityView(ENTITY_TOVIEW_HIDE_BODY, 0, 0, &hideBody, sizeof(hideBody));
        }
	}

    if ( shadowData.byType == TYPE_PLAYER_ROLE )
    {
        m_EnemySet.insert(uidEntity);
    }
	
	event_entity_enter_viewport enterViewPort;
	enterViewPort.uidMe = pClientEntity->getUID();
	enterViewPort.uidEnter = uidEntity;
	// 进入视野事件
	pEntity->fireEvent(EVENT_ENTITY_ENTER_VIEWPORT, &enterViewPort, sizeof(enterViewPort));
}

// 实体离开视野
void EntityClient::OnEntityDisappear(UID uidEntity, ibuffer & in)
{
	// 取得主角对象
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

    // 主角消失(尸鬼)
    /*
    if(uidEntity == pClientEntity->getUID())
    {
        IEntityProperty *pEntityProperty = (IEntityProperty *)pClientEntity->getEntityPart(PART_PROPERTY);
        if(pEntityProperty != NULL)
        {
            pEntityProperty->setProperty_Integer(PROPERTY_MASK, MASKFLAG_HIDE_FRIEND);
        }
        return;
    }
    */

    //TRACE_ENTITY( "EntityClient::OnEntityDisappear uid="<<uidEntity );
    IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidEntity);
    if (pEntity == NULL)
    {
		return;
	}

//     EntityViewItem item;
//     if(((EntityBase*)pEntity)->getBasicViewInfo(item))
//     {
//         SMinimapIcon* pIconInfo = (SMinimapIcon*)gClientGlobal->getSchemeCenter()->getSchemeMinimap()->GetMinimapIconInfo(item.nMinimapIcon);
//         if(pIconInfo != NULL && pIconInfo->nIsTower)
//         {
//             return;
//         }
//     }

    pEntity->updateView( false,false);

    if (TYPE_PLAYER_ROLE == UID_2_TYPE(uidEntity) && !m_EnemySet.empty())
    {
        isHaveEnemyId(uidEntity, true);
    }
    

	event_entity_leave_viewport leaveViewPort;
	leaveViewPort.uidMe = pClientEntity->getUID();
	leaveViewPort.uidLeave = uidEntity;
	// 离开视野事件
	pEntity->fireEvent(EVENT_ENTITY_LEAVE_VIEWPORT, &leaveViewPort, sizeof(leaveViewPort));
}

// 实体进出草丛（以服务器为准，透明处理）
void EntityClient::OnEntityForestChange(UID uidEntity, int nForestID)
{
    IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidEntity);
    if ( pEntity )
    {
        // 设置透明度
        int nAlpha = 100;
        if(nForestID >= 0)
        {
            nAlpha = 50;
        }
        pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SET_ALPHA, nAlpha, 0, 0, 0);
    }
}

// 实体隐形、取消隐形（透明处理）
void EntityClient::OnEntityStealthChange(UID uidEntity, bool bStealing)
{
    IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uidEntity);
    if ( pEntity )
    {
        // 设置透明度
        int nAlpha = 100;
        if(bStealing)
        {
            nAlpha = 50;
        }
        pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SET_ALPHA, nAlpha, 0, 0, 0);
    }
}

//////////////////////////////////////////////////////////////////////////
/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT IEntityClient * CreateEntityClient(void)
{
	if(g_pEntityClient != NULL)
	{
		return g_pEntityClient;
	}

	// 实例化服务
	EntityClient * pEntityClient = new EntityClient();
	if(pEntityClient == NULL || !pEntityClient->Create())
	{
		SAFE_RELEASE(pEntityClient);

		return NULL;
	}

	g_pEntityClient = pEntityClient;

	return g_pEntityClient;
}