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
#include "IServiceContainer.h"
#include "IServerGlobal.h"
#include "ISceneManager.h"
#include "IEntityProperty.h"
#include "NetMessageDef.h"
#include "EventDef.h"
#include "EntityHelper.h"
#include "IGameResService.h"
#include "ITimerService.h"
#include "IMovePart.h"
#include "IEntityAOI.h"
#include "IWarPart.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "ITimeSyncService.h"
#include "WarHelper.h"
#include "EntityLifeHelper.h"
#include "ObjectLifeControl.h"
#include "IPlayerKillPart.h"
#include "PlayerKillMessageDef.h"
#include "SpellMessageDef.h"

#ifndef OBSTACLE_RADIO
#define OBSTACLE_RADIO				1.2f		// 障碍距离阀值
#endif



////////////////////* DiePart */////////////////////////////////////////////
DiePart::DiePart()
{
    m_pMaster	= NULL;	
    memset(&m_ReliveInfo, 0, sizeof(m_ReliveInfo));
    m_nDeadType = 0;
}

DiePart::~DiePart()
{

}

////////////////////* IEntityPart */////////////////////////////////////////////
short DiePart::getPartID()
{
    return PART_DIE;
}

bool DiePart::onLoad( __IEntity* pEntity, int nLoginMode)
{
    if (!pEntity)
    {
        return false;
    }

    m_pMaster = pEntity;

    __IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
    if (pEventEngine != NULL)
    {
        // 订阅最终受到伤害事件
        pEventEngine->Subscibe(this, EVENT_ENTITY_DAMAGE_RESULT, __FUNCTION__);
    }

    return true;
}

bool DiePart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    return true;
}

__IEntity * DiePart::getHost()
{
    return m_pMaster;
}

void DiePart::release()
{
    gServerGlobal->getTimerService()->KillTimer(ETIMER_RELIVE, this);

    if (m_pMaster)
    {
        __IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
        if ( pEventEngine != NULL )
        {
            // 取消订阅最终受到伤害事件
            pEventEngine->UnSubscibe(this, EVENT_ENTITY_DAMAGE_RESULT);
        }
    }
    m_pMaster = 0;

    delete this;
}

bool DiePart::onSerialize( rkt::ibuffer & in,int flag )
{
    return true;
}

bool DiePart::deSerialize( rkt::obuf & out,int flag )
{
    // 导出到所有客户端
    if( (flag & TO_ALL_CLIENT) )
    {
        // 实体当前状态
        BYTE nCurState = ENTITY_STATE_STAND;
        if(m_pMaster != NULL)
        {
            nCurState = m_pMaster->getState(); 
        }
        out << nCurState;

        // 对不用类型实体压不同数据
        switch(m_pMaster->getType())
        {
        case TYPE_MONSTER:  // 怪物
            {
                // 死亡时加入死亡时间
                if(nCurState == ENTITY_STATE_DIE)
                {
                    out << m_nDeadType;
                }

                // 是否为动态障碍
                bool isObstacle = (m_SObstacleNode.EntityUid != 0);
                out << isObstacle;

                // 压入障碍数据
                if(isObstacle)
                {
                    out << m_SObstacleNode;
                }
            }
            break;

        case TYPE_PLAYER_ROLE:
            {
                // 死亡时加入死亡时间
                if(nCurState == ENTITY_STATE_DIE)
                {
                    out  << m_nDeadType << m_ReliveInfo.dwOperateTick << m_ReliveInfo.nActiveInterval;
                }
            }
            break;

        default: break;
        }
    }

    return true;
}

////////////////////* IEventExecuteSink */////////////////////////////////////////////
void DiePart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{		
    switch (wEventID)
    {
    case EVENT_ENTITY_DAMAGE_RESULT:		// 最终受到伤害
        {
            onEventDamageResult(bSrcType, dwSrcID, pszContext, nLen);
        }
        break;

    default:
        break;
    }
}

void DiePart::onDie(UID uidMurder, int nDeadType, int nDiePKType)
{
    if (m_pMaster == 0)
    {
        return;
    }

    __IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
    if(pWarPart == NULL)
    {
        return;
    }

    m_pMaster->changeState(ENTITY_STATE_DIE);
    m_nDeadType = nDeadType;

    int nWarID = pWarPart->getWarID();
    // 静态地图
    if (nWarID == INVALID_WAR_ID)
    {
        memset(&m_ReliveInfo, 0, sizeof(m_ReliveInfo));
        int nUidMy = m_pMaster->getUID();
        int nSceneID = UID_2_SCENE(nUidMy);

        //// 取复活点配置
        Vector3 ptReborn;
        float fAngleY = 0;
        int nMapID = gServerGlobal->getSceneManager()->sceneIDToMapID(nSceneID);
        int nCampID = m_pMaster->getIntProperty(PROPERTY_CAMP);
        SMapLandSchemeInfo* pInfo = gServerGlobal->getSchemeCenter()->getSchemeMapLand()->GetMapLandSchemeInfo(nMapID, EGM_RPG, nCampID);
        if(pInfo != NULL && !pInfo->vecBornPos.empty())
        {
            int nIndex = rand() % pInfo->vecBornPos.size();
            const SPosWithAngle& pos = pInfo->vecBornPos[nIndex];

            ptReborn = pos.ptPos;
            fAngleY = pos.fAngleY;
        }
        else
        {
            // 没有配置则原地复活
            ptReborn = m_pMaster->getLocation();
            Vector3 vAngles = m_pMaster->getAngle();
            fAngleY = vAngles.y;
        }

        m_ReliveInfo.nSceneID = nSceneID;
        m_ReliveInfo.x = ptReborn.x;
        m_ReliveInfo.y = ptReborn.y;
        m_ReliveInfo.z = ptReborn.z;
        m_ReliveInfo.fAngleY = fAngleY;
        m_ReliveInfo.nActiveInterval = 3000;// 死亡激活复活操作时间长 ms
        m_ReliveInfo.dwOperateTick = gServerGlobal->getTimeSyncService()->GetTick(); 	// 当前操作的时间
		m_ReliveInfo.nDiePKType = nDiePKType;

		__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
		if (pEntityEvent != NULL)
		{
			pEntityEvent->FireExecute(EVENT_ENTITY_BEFORE_RELIVE, (char *)&m_ReliveInfo, sizeof(m_ReliveInfo));
		}
    }
    else
    {
        WarHelper warHelper(nWarID);
        if(warHelper.m_ptr)
        {
            Vector3 v = m_pMaster->getLocation();
            UID uDiedUID =m_pMaster->getUID();
            int nCamp = m_pMaster->getIntProperty(PROPERTY_CAMP);
            int nMurderCamp = getProperty_Integer(uidMurder,PROPERTY_CAMP);

            sEntityDieInfo entityDieInfo;
            entityDieInfo.uDiedPropertyID   = m_pMaster->getIntProperty(PROPERTY_ID);        // 怪物ID或者人物PDBID
            entityDieInfo.uDiedUID          = uDiedUID;                                          // 死亡的UID
            entityDieInfo.nDiedCamp         = nCamp;                                             // 死亡玩家的阵营
            entityDieInfo.nDiedVocation     = m_pMaster->getIntProperty(PROPERTY_VOCATION);//getProperty_Integer(uDiedUID,PROPERTY_VOCATION);            // 职业
            entityDieInfo.nLevel            = m_pMaster->getIntProperty(PROPERTY_LEVEL);

            entityDieInfo.uMurderPropertyID = getProperty_Integer(uidMurder,PROPERTY_ID);         // 凶手的怪物ID或者人物PDBID
            entityDieInfo.uMurderUID        = uidMurder;                                          // 凶手的UID
            entityDieInfo.nMurderCamp       = nMurderCamp;                                                  // 死亡玩家的阵营
            entityDieInfo.nMurderVocation   = getProperty_Integer(uidMurder,PROPERTY_VOCATION);   // 击杀者职业
            entityDieInfo.x                 = v.x;                                                          // 死亡位置
            entityDieInfo.y                 = v.y;                                                          // 死亡位置
            entityDieInfo.z                 = v.z;                                                          // 死亡位置
            sstrcpyn(entityDieInfo.szDiedName,getProperty_String(uDiedUID, PROPERTY_NAME).data(),sizeof(entityDieInfo.szDiedName));
            sstrcpyn(entityDieInfo.szMurderName,getProperty_String(uidMurder, PROPERTY_NAME).data(),sizeof(entityDieInfo.szMurderName));

            // 执行怪物死亡逻辑后直接返回
            if (!isActor(m_pMaster->getUID()))
            {
                warHelper.m_ptr->onEntityDie(entityDieInfo);
            }
            else
            {
                // 人物时，需要返回复活信息
                memset(&m_ReliveInfo, 0, sizeof(m_ReliveInfo));
                warHelper.m_ptr->onEntityDieEx(entityDieInfo, m_ReliveInfo);

				if (m_ReliveInfo.bNoRelive == false)
				{
					m_ReliveInfo.nDiePKType = nDiePKType;

					if (m_ReliveInfo.nSceneID == 0)
					{
						WarningLn("Die[" << m_pMaster->getName() << "], relive sceneid=0, System auto reset scene by uid");

						UID uid = m_pMaster->getUID();
						m_ReliveInfo.nSceneID = UID_2_SCENE(uid);
					}
					m_ReliveInfo.dwOperateTick = gServerGlobal->getTimeSyncService()->GetTick();

					ENTITY_EMPHASISLN("Die[" << m_pMaster->getName() << "], relive sceneid=" << m_ReliveInfo.nSceneID
						<< ",x=" << m_ReliveInfo.x << ",y=" << m_ReliveInfo.y << ",z=" << m_ReliveInfo.z << ",Interval=" << m_ReliveInfo.nActiveInterval);

					// 有复活时间的转到效果中去调整复活时间
					if (m_ReliveInfo.nActiveInterval > 0)
					{
						__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
						if (pEntityEvent != NULL)
						{
							pEntityEvent->FireExecute(EVENT_ENTITY_BEFORE_RELIVE, (char *)&m_ReliveInfo, sizeof(m_ReliveInfo));
						}
					}
					else if (m_ReliveInfo.nActiveInterval < 0) // 复活时间有异常报下
					{
						ErrorLn(__FUNCTION__": Player ActiveInterval is negative! Please check!!! name=" << m_pMaster->getName() << ", ActiveInterval=" << m_ReliveInfo.nActiveInterval);
					}
				}
            }
        } 
    }

    // 广播消息给客户端
    msg_entity_state_info info;
    info.byState = ENTITY_STATE_DIE;
    info.v = Vector3(m_ReliveInfo.x, m_ReliveInfo.y, m_ReliveInfo.z);
    // 发最终复活时间（同步服务器的时间为基准）
    info.nData = (gServerGlobal->getTimeSyncService()->GetTick() + m_ReliveInfo.nActiveInterval);
    info.uidOperator = uidMurder;
    info.nDeadType = nDeadType;
	info.bNoRelive = m_ReliveInfo.bNoRelive;
    broadcast(SC_MSG_ENTITY_DIE, (LPCSTR)&info, sizeof(info));

    if(isActor(m_pMaster->getUID()) && m_ReliveInfo.bNoRelive == false)
    {
        // 没有配置复活时间刻度将直接复活
        if (m_ReliveInfo.nActiveInterval <= 0)
        {
            ErrorLn("actor transport to loc(" << m_ReliveInfo.x << "," << m_ReliveInfo.y << "," << m_ReliveInfo.z << "), name=" << m_pMaster->getName());
            relive();
        }
        else	
        {
            gServerGlobal->getTimerService()->SetTimer(ETIMER_RELIVE, m_ReliveInfo.nActiveInterval, this);
        }
    }
}

// 处理最终伤害事件
void DiePart::onEventDamageResult(BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    assert(nLen == sizeof(event_entity_damage));
    if (nLen != sizeof(event_entity_damage))
    {
        return;
    }

    if (m_pMaster == NULL)
    {
        return;
    }

    event_entity_damage * pevent = (event_entity_damage*)pszContext;
    if (pevent->bDead)
    {
        __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
        if (pEntityEvent == NULL)
            return;

        // 发出死亡事件
        event_entity_die die;
        die.nSpellID = pevent->nID;
        die.uidOperator = pevent->uidOperator;
        die.uidTarget = pevent->uidTarget;
        die.nSpellEffectType = pevent->nSpellEffectType;
        die.nDeadType = pevent->nDeadType;
		
		__IPlayerKillPart * pPKPart = (__IPlayerKillPart *)m_pMaster->getEntityPart(PART_PK);
		int nPKType = PK_INVALID;
		if (pPKPart != NULL)
		{
			nPKType = pPKPart->getPKType();
		}

        // 现在针对怪物
        if (UID_2_TYPE(die.uidTarget) == TYPE_MONSTER)
        {
            ObjectLifeControl<MonsterLife> * pControl = new ObjectLifeControl<MonsterLife>();
            MonsterLife *pMonster =	pControl->Attach( new MonsterLife(die.uidTarget), NULL, NULL, &MonsterLife::destoryCallBack);
            die.ptrRef = (IRefObj*)pControl;

            // 死亡事件
            if (pEntityEvent->FireVoteExecute(EVENT_ENTITY_DIE, (char *)&die, sizeof(die)))
            {
                pEntityEvent->FireExecute(EVENT_GAME_ENTITY_DIE, (char *)&die, sizeof(die));  
                pMonster->setDieDestroy(true);
            }

            if (pControl)
            {
                pControl->Release();
            }
        }
        else
        {
            pEntityEvent->FireVoteExecute(EVENT_ENTITY_DIE, (char *)&die, sizeof(die));
            pEntityEvent->FireExecute(EVENT_GAME_ENTITY_DIE, (char *)&die, sizeof(die));
        }

        // 直接通知凶手的技能部件
        {
            if(pevent->nID != 0)
            {
                obuf256 data;

                SNetMsgHead head;
                head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
                head.byDestEndPoint = MSG_ENDPOINT_SCENE;
                head.byKeyModule = MSG_MODULEID_ENTITY;
                head.byKeyAction = SPELL_MSG_KILL_ENTITY;

                // 消息头
                SMsgEntityHead entityHead;
                entityHead.uidMaster = pevent->uidOperator;
                entityHead.byPartID = PART_SPELL;

                data << head << entityHead;
                data.push_back(&die, sizeof(event_entity_die));

                // 发送消息
                handleMessage(pevent->uidOperator, data.data(), (int)data.size());
            }
        }

        // 其他模块都处理完了 再发给客户端死亡。
        // 假如先发给客户端死亡，紧接着其他模块可能会下发消息造成客户端显示层出bug（如客户端反馈说死亡被技能模块的"打断动作"命令打断）。
        onDie(pevent->uidOperator, pevent->nDeadType, nPKType);

//         TraceLn(m_pMaster->getName() << "["<< die.uidTarget <<"] is dead.murder="<< die.uidOperator 
//             <<",spell="<< die.nSpellID <<",SpelEffT="<< die.nSpellEffectType <<",DeadT="<< die.nDeadType
//             <<",DamageT="<< pevent->nDamageType<<",AttT="<<pevent->nAttackResultType<<",rebound="<<pevent->bRebound
//        );
    }
}

void DiePart::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case ETIMER_RELIVE:
        {
            // 关闭定时器
            gServerGlobal->getTimerService()->KillTimer(ETIMER_RELIVE, this);

            // 执行复活处理
            relive();
        }
        break;
    }
}

/** 取得当前状态
@return  
*/
bool DiePart::relive()
{
    if (!m_pMaster)
    {
        return false;
    }

	__IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
	if (pWarPart == NULL)
	{
		return false;
	}

	int nWarID = pWarPart->getWarID();

    // 死亡状态
    if (m_pMaster->getState() != ENTITY_STATE_DIE)
    {
        return false;
    }

    // 取得实体属性部件
    __IEntityProperty * pProperty = (__IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
    if (!pProperty)
    {
        return false;
    }	

    Vector3 ptDes = getReliveLoc();

    Vector3 ptSrc= m_pMaster->getLocation();
    //ENTITY_EMPHASISLN(__FUNCTION__": " << m_pMaster->getName().c_str() << ": Src[x=" << ptSrc.x << ",y=" << ptSrc.y << ",z=" << ptSrc.z << "],Dest[x=" << ptDes.x << ",y=" << ptDes.y << ",z=" << ptDes.z << "]");

    // 先将人物传送到复活点
    __IMovePart* pMovePart = (__IMovePart*)m_pMaster->getEntityPart(PART_MOVE);
    if (pMovePart == NULL)
        return false;
    Move3 move;
    move.vPosition = ptDes;
    move.vRotation.y = m_ReliveInfo.fAngleY;
    pMovePart->stopAt(move, 0, TRANSPORT_REASON_REBORN);

    // 转到站立状态
    m_pMaster->changeState(ENTITY_STATE_STAND);	

	// 设置延后到复活才能设置的属性
	for (std::vector<PropToSet>::iterator it = m_vecPropToSet.begin(); it != m_vecPropToSet.end(); ++it)
	{
		pProperty->setProperty_Integer(it->nPropID, it->nValue);
	}
	m_vecPropToSet.clear();

    // 设置生物实体满血
    pProperty->setProperty_Integer(PROPERTY_HP, pProperty->getProperty_Integer(PROPERTY_MAX_HP));
    // 设置生物实体满蓝
    pProperty->setProperty_Integer(PROPERTY_MP, pProperty->getProperty_Integer(PROPERTY_MAX_MP));
    // 所在地图(这个m_ReliveInfo.nSceneID有时候取不到会是0)
    // pProperty->setProperty_Integer(PROPERTY_SCENEID, m_ReliveInfo.nSceneID);

    ENTITY_EMPHASISLN("onRelive["<< m_pMaster->getName() <<"]ID="<<m_pMaster->getUID()<<", SceneID="<<m_ReliveInfo.nSceneID);

    // 广播消息给客户端
    msg_entity_state_info info;
    info.byState = ENTITY_STATE_DIE;
    info.v = Vector3(m_ReliveInfo.x, m_ReliveInfo.y, m_ReliveInfo.z);
    info.nData = 0;

    broadcast(SC_MSG_ENTITY_RELIVE, (LPCSTR)&info, sizeof(info));

    // 清空复活数据
    memset(&m_ReliveInfo, 0, sizeof(m_ReliveInfo));

    // 发送复活事件
    __IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent != NULL)
    {
        pEntityEvent->FireExecute(EVENT_ENTITY_RELIVE, NULL, 0);
    }

	WarHelper warHelper(nWarID);
	if (warHelper.m_ptr != NULL)
	{
		warHelper.m_ptr->onEntityRelive(m_pMaster->getUID());
	}
    return true;
}

/** 取得复活信息
@return  
*/
bool DiePart::getReliveInfo(int &nSceneID, float &fLocX, float &fLocY, float &fLocZ, float &fAngleY)
{
    nSceneID    = m_ReliveInfo.nSceneID;    // 复活到目标场景ID
    fLocX       = m_ReliveInfo.x;           // 复活X坐标
    fLocY       = m_ReliveInfo.y;           // 复活Y坐标
    fLocZ       = m_ReliveInfo.z;           // 复活Z坐标
    fAngleY     = m_ReliveInfo.fAngleY;     // 复活绕向Y朝向角度

    return true;
}

void DiePart::setIntPropertyOnRelive(int nProperty, int nValue)
{
	if (m_pMaster == NULL)
		return;

	// 当前不是死亡状态 直接设置
	if (m_pMaster->getState() != ENTITY_STATE_DIE)
	{
		m_pMaster->setIntProperty(nProperty, nValue);
		return;
	}

	// 当前是死亡状态 需要缓存
	PropToSet prop;
	prop.nPropID = nProperty;
	prop.nValue = nValue;
	m_vecPropToSet.push_back(prop);
}

// 找一个空位复活点
Vector3	DiePart::getReliveLoc()
{
    // 容易随机到地图外 导致原地复活。所以暂时直接读配置随机出的复活点
    return Vector3(m_ReliveInfo.x,m_ReliveInfo.y,m_ReliveInfo.z);

    static Vector3 s_OffsetPt[5] = {
        Vector3(0,0,0), 
        Vector3(OBSTACLE_RADIO*2,0,0), 
        Vector3(-OBSTACLE_RADIO*2,0,0), 
        Vector3(0,0,-OBSTACLE_RADIO*2), 
        Vector3(0,0,OBSTACLE_RADIO*2)
    };
    // 找一个空位复活点
    Vector3 vc(m_ReliveInfo.x,m_ReliveInfo.y,m_ReliveInfo.z);
    Vector3 ptDes = vc;
    map<UID, Vector3> mapFindEntity;	
    int nUidMy = m_pMaster->getUID();
    //int nCamp = getProperty_Integer(nUidMy, PROPERTY_CAMP);
    int nSceneID = UID_2_SCENE(nUidMy);
    SceneServiceHelper sceneHelper(nSceneID);
    if ( sceneHelper.m_ptr != 0 )
    {
        AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
        int num = sceneHelper.m_ptr->k_nearest( vc, OBSTACLE_RADIO*3, pProxyArray, MAX_INTEREST_OBJECT, CAMP_MASK_ALL, true);
        if ( num > 0 )
        {
            for (int i=0; i<num; ++i)
            {
				UID uidTemp = pProxyArray[i]->m_uid;
				if (UID_2_TYPE(uidTemp) == TYPE_PLAYER_ROLE && nUidMy != uidTemp)
                {
                    mapFindEntity[uidTemp] = pProxyArray[i]->m_Loc;
                }
            }
        }
    }	
    for (int i=0; i<5; ++i)
    {
        bool bEmptyLoc = true;
        ptDes = vc + s_OffsetPt[i];
        map<UID, Vector3>::iterator iter = mapFindEntity.begin();
        for (; iter != mapFindEntity.end(); ++iter)
        {
            Vector3 vctDis = iter->second - ptDes;
            if ( (vctDis.x*vctDis.x + vctDis.z*vctDis.z) <= OBSTACLE_RADIO*OBSTACLE_RADIO)
            {
                bEmptyLoc = false;
                break;
            }
        }	
        if (bEmptyLoc)
        {
            break;
        }
    }
    return ptDes;
}

/** 广播死亡复活信息
@return  
*/
bool DiePart::broadcast(BYTE byAction, LPCSTR pszContext, size_t nLen)
{
    if (!m_pMaster)
    {
        return false;
    }

    int nSceneID = m_pMaster->getSceneID();
    SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(nSceneID);
    if ( pContainer==0 )
    {
        ErrorLn(__FUNCTION__": can not found the scene!id=" << nSceneID <<",name="<<m_pMaster->getName() );
        return false;
    }

    IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
    if ( pScene==0 )
    {
        ErrorLn(__FUNCTION__": can not found the scene!id=" << nSceneID << ",name=" << m_pMaster->getName());
        return false;
    }

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = byAction;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
    entityHead.byPartID = PART_DIE;				// 部件ID，ENTITY_PART_MAX时将由实体来处理

    obuf256 data;
    data << head << entityHead;
    data.push_back(pszContext, nLen);

    if(m_pMaster->getType() == TYPE_MONSTER)
    {
        pScene->broadcast_all(BROADCAST_DO_ACTION, string(data.data(),data.size()), CAMP_MASK_ALL);
    }
    else
    {
        pScene->broadcast( m_pMaster->getLocation(),0,BROADCAST_DO_ACTION,string(data.data(),data.size()) );
    }
    return true;
}

void  DiePart::receiveDynamicObstacleInfo(void * pObstacleInfo, size_t nLen)
{
	if(pObstacleInfo == NULL || nLen != sizeof(m_SObstacleNode))
		return;

	memcpy(&m_SObstacleNode, pObstacleInfo, nLen);
}

void DiePart::restore()
{

}

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __IDiePart * CreateDiePart()
{
    return new DiePart;
}