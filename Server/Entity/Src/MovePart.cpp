/*******************************************************************
** 文件名:	MovePart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	rjc
** 日  期:	4/23/2015
** 版  本:	1.0
** 描  述:	
modifyied by ljw at 2015.8.18
移动部件
********************************************************************/

#include "stdafx.h"
#include "MovePart.h"
#include "EventDef.h"
#include "IServerGlobal.h"
#include "ISceneManager.h"
#include "NetMessageDef.h"
#include "TimeSyncServer.h"
#include "IEntityProperty.h"
#include "IPlayerRole.h"
#include "EntityAOI.h"
#include "IEntityEvent.h"
#include "IPathFindManager.h"
#include "ITimeSyncService.h"
#include "ISchemeCenter.h"
#include "IBuffServer.h"
#include "IEntityAOI.h"
#include "EntityHelper.h"
#include "IAIPart.h"
#include "IPathFindService.h"
#include "PathFindHelper.h"
#include <cmath>
#include "DamageDef.h"
#include "ExternalHelper.h"
#include "HeightFieldMgr.h"
#include "IWingPart.h"
#include "IWarPart.h"

// 是否打开客户端移动地形检测
//#define OPEN_CLIENT_TERRIAN_CHECK  

#define MONSTER_SPEED_TIMES             100     // 移动速度配置放大倍数

#define ACTOR_MOVE_UPDATE_INTERVAL 64
#define NPC_MOVE_UPATE_INTERVAL 256
#define SHIFT_UPDATE_INTERVAL 32
#define MOVE_ACCELERATE_INTERVAL 500
//////////////////////////////////////////////////////////////////////////
CMovePart::CMovePart()
    : m_pMaster(0)
    , m_fSpeed(0)
    , m_bCanMove(true)
    , m_dwKeepDirEndTick(0)
    , m_dwLastUpdatePosTick(0)
    , m_dwLastLostLocationTick(0)
    , m_uLostLocationSerialNo(0)
    , m_bEnterStrictMoveCheck(false)
    , m_bForbitMoveInitiatively(false)
    , m_nFindPathFailTimes(0)
    , m_bCanPassBarrier(false)
    , m_nCreatureState(0)
	, m_dwRcvLastMoveMsgTick(0)
    , m_dwRigidityEndTick(0)
    , m_bHandlingDropablePkg(false)
    , m_wRcvPkgSeq(INVLAID_PKG_SEQ)
    , m_wSendPkgSeq(0)
    , m_wSendKeyPkgSeq(INVLAID_PKG_SEQ)
    
{
}

short CMovePart::getPartID()
{
    return PART_MOVE;
}

__IEntity * CMovePart::getHost()
{
    return m_pMaster;
}

bool CMovePart::onSerialize( rkt::ibuffer & in,int flag )
{
    return true;
}

bool CMovePart::deSerialize( rkt::obuf & out,int flag )
{
    if ( (flag&TO_CLIENT_OTHER) || (flag&TO_CLIENT_SELF) )
    {
        out << getMoveState() << m_nCreatureState << m_eBehaviour;

        if(m_Behaviours[m_eBehaviour])
        {
            BehaviourCtx& ctx = m_Behaviours[m_eBehaviour]->getCtx(); 
            out << ctx;
        }
    }
    return true;
}

bool CMovePart::onLoad( __IEntity* pEntity, int nLoginMode)
{
    if(!pEntity)
    {
        return false;
    }
    m_pMaster = pEntity;

    m_eBehaviour = BEHAVIOUR_TYPE_NONE;
    m_Behaviours[BEHAVIOUR_TYPE_NONE] = 0;
    m_Behaviours[BEHAVIOUR_TYPE_WALK] = WalkBehaviour::create();
    m_Behaviours[BEHAVIOUR_TYPE_JUMP] = JumpBehaviour::create();
    m_Behaviours[BEHAVIOUR_TYPE_SHIFT] = ShiftBehaviour::create();

    if(m_pMaster->getType()== ENTITY_TYPE::TYPE_MONSTER)
    {
        ISchemeMonster* pSchemeMonstor = gServerGlobal->getSchemeCenter()->getSchemeMonster();
        if (pSchemeMonstor == NULL)
        {
            return false;
        }
        __IEntityProperty * property = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
        if ( property == NULL )
        {
            return false;
        }

        int nMonsterID = property->getProperty_Integer( PROPERTY_ID );
        SMonsterScheme * pSchemeInfo = pSchemeMonstor->getMonsterShemeInfo(nMonsterID);
        if (pSchemeInfo != NULL)
        {
            m_fSpeed  = (float)property->getProperty_Integer(PROPERTY_SD) / MONSTER_SPEED_TIMES;
            m_bCanMove = (pSchemeInfo->nMonsterMask & MASKFLAG_NOT_MOVE) == 0;
        }
    }

    __IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent)
    {
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CREATE,__FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (ENTITYEVENT_KEY)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_SD), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DIE, __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PREPARE_CAST_SPELL, __FUNCTION__);
    }

    SceneLogicEventHelper helper(m_pMaster->getSceneID());
    if(helper.m_ptr)
    {
         helper.m_ptr->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_OBSTACLE_CREATE, TYPE_MONSTER, 0, __FUNCTION__);
         helper.m_ptr->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_OBSTACLE_DESTROY, TYPE_MONSTER, 0, __FUNCTION__);
    }

    // 玩家或者召唤物订阅一下障碍事件(结界只挡英雄和召唤物)
    if((m_pMaster->getType() == TYPE_PLAYER_ROLE || !isInvalidUID(m_pMaster->getIntProperty(PROPERTY_MASTER_UID))) && helper.m_ptr)
    {
        helper.m_ptr->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_BLOCK_BARRIER_CREATE, SOURCE_TYPE_SCENE, 0, __FUNCTION__);
        helper.m_ptr->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_BLOCK_BARRIER_DESTROY, SOURCE_TYPE_SCENE, 0, __FUNCTION__);
    }

	// 玩家和假人
	if (m_pMaster->getType() == TYPE_PLAYER_ROLE)
	{
		gServerGlobal->getTimerService()->SetTimer(MOVE_ACCELERATE_TIMER, MOVE_ACCELERATE_INTERVAL, this, INFINITY_CALL, __FUNCTION__);
		pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CHANGE_STATE, __FUNCTION__);
	}

    return true;
}

bool CMovePart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch(msgId)
    {
    case CS_MSG_CREATURE_SYNC:  //  客户端移动
        {
            if (m_pMaster->getType() != TYPE_PLAYER_ROLE)
            {
                ErrorLn("npc can't handle CS_MSG_CREATURE_SYNC");
                return false;
            }

            if(len < sizeof(msg_creature_move_sync))
            {
                return false;
            }

            bool bResult = handleClientMove(data, len);
            m_bHandlingDropablePkg = false;
            
            return bResult;
        }
        break;

    case CS_MSG_CREATURE_CTRL:  // 处理客户端控制请求
        {
            if (m_pMaster->getType() != TYPE_PLAYER_ROLE)
            {
                ErrorLn("monster can't handle CS_MSG_CREATURE_CTRL");
                return false; 
            }

            return handleClientCtrl(data, len);
        }
        break;

    case CS_MSG_CREATURE_LAND:	// 客户端实体着陆
        {
            if (m_pMaster->getType() != TYPE_PLAYER_ROLE)
            {
                ErrorLn("monster can't handle CS_MSG_CREATURE_LAND");
                return false;
            }

            if ( len != sizeof(msg_creature_act_land) )
            {
                return false;
            }

            handleCommandLand((msg_creature_act_land*)data);
        }
        break;

    case CS_MSG_ENTITY_SET_FORMTYPE:    // 玩家设置立足点类型
        {
            if ( len != sizeof(msg_creature_act_set_formtype) )
            {
                return false;
            }
            handleCommandSetFormType((msg_creature_act_set_formtype*)data);
        }
        break;

    case CS_MSG_ENTITY_TURN_CAMERA:
        {
            handleCommandTurnCamera(data, len);
        }
        break;

    case CS_MSG_REPORT_AI_LOC:
        {
            if(len != sizeof(msg_report_ai_loc))
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }

            if(((SMsgEntityHead*)pEntityHead)->uidMaster == m_pMaster->getUID())
            {
                msg_report_ai_loc* pMsg = (msg_report_ai_loc*)data;

                if(!isLostLocation() || pMsg->uSerial != m_uLostLocationSerialNo)
                {
                    return false;
                }

                // 校验假人位置
                if(/*!verifyClientMove(pMsg->moveState.vPosition) || */checkPos(pMsg->moveState.vPosition) != CHECK_RESULT_OK)
                {
                    WarningLn(__FUNCTION__ << __LINE__ << ", checkMove failed..." << "pos(" << pMsg->moveState.vPosition.x << "," << pMsg->moveState.vPosition.y << "," << pMsg->moveState.vPosition.z << ")");
                    return false;
                }
                
                changeMoveState(pMsg->moveState);
              
                m_dwLastLostLocationTick = 0;
            }
            else
            {
                // 客户端上报假人强制位移(动作决定位移)
                SNetMsgHead head;
                head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
                head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
                head.byKeyModule   = MSG_MODULEID_ENTITY;	
                head.byKeyAction   = CS_MSG_REPORT_AI_LOC;

                // 实体类统一用子头来用于转向部件
                SMsgEntityHead entityHead;
                entityHead.uidMaster = ((SMsgEntityHead*)pEntityHead)->uidMaster;			
                entityHead.byPartID = PART_MOVE;

                obuf256 buf;
                buf << head << entityHead;
                buf.push_back(data, len);

				// 转发给假人的MovePart,因为假人是收不到客户端包的，只能通过真人代发
                handleMessage(((SMsgEntityHead*)pEntityHead)->uidMaster,buf.data(), buf.size());
            }
        }
        break;
    case SS_MSG_FORBID_MOVE_INITIATIVELY:
        {
            if(data == NULL || len < sizeof(bool))
                return false;

            m_bForbitMoveInitiatively = *((bool*)data);

            // 通知客户端
            if(m_pMaster->getType() == TYPE_PLAYER_ROLE && ((__IPlayerRole*)m_pMaster)->getClientID() != INVALID_CLIENT)
            {
                SNetMsgHead head;
                head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
                head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
                head.byKeyModule   = MSG_MODULEID_ENTITY;	
                head.byKeyAction   = SC_MSG_FORBIT_MOVE_INITIATIVELY;

                // 实体类统一用子头来用于转向部件
                SMsgEntityHead entityHead;
                entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
                entityHead.byPartID = PART_MOVE;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

                obuf256 data;
                data << head << entityHead << m_bForbitMoveInitiatively;

                ((__IPlayerRole*)m_pMaster)->send2Client(data.data(), data.size());
            }
        }
        break;

    default:
        {
            return false;
        }
    }

    return true;
}

bool  CMovePart::setSpeed(float speed)
{
    float fDiff = m_fSpeed - speed;

    if(fDiff > -0.001 && fDiff < 0.001)
    {
        return true;
    }

	m_fSpeed = speed;

    // 冲锋等暂时不支持过程中改变速度
    if(m_eBehaviour != BEHAVIOUR_TYPE_WALK)
        return true;

    if(m_Behaviours[m_eBehaviour] != NULL && m_Behaviours[m_eBehaviour]->isRunning())
    {
        m_Behaviours[m_eBehaviour]->adjustCtx(gServerGlobal->getTimeSyncService()->GetTick());
        m_Behaviours[m_eBehaviour]->getCtx().fStartSpeed = m_fSpeed;

        // 重新广播一下移动
        gotoBehaviour(m_eBehaviour);
    }

    return true;
}

// 移动到某点 cmd
bool CMovePart::moveTo(Vector3 &loc, bool bInitiatively)
{
    if(getTickCountEx() < m_dwRigidityEndTick)
    {
        return false;
    }

	if (!canMove(bInitiatively))
		return false;

	Move3 currentMove = getMoveState();
	if (currentMove.vPosition.checkDistance(loc, 0.02))
		return false;

	std::vector<Vector3> path;

    // 调用寻路接口
    int nSceneID = m_pMaster->getSceneID();
    if (gServerGlobal->getPathFindManager()->GetPathFromMapID(
        m_pMaster->getMapID(), nSceneID,
        currentMove.vPosition, loc, path,
        CAMP_MASK(m_pMaster->getIntProperty(PROPERTY_CAMP)) << 12) && !path.empty())
    {
        if (!path[0].checkDistance(currentMove.vPosition, 1))
            path.insert(path.begin(), currentMove.vPosition);
    }
    else
    {
        TraceLn("npc path not found, from(" << currentMove.vPosition.x << "," << currentMove.vPosition.y << "," << currentMove.vPosition.z << "), to(" << loc.x << "," << loc.y << "," << loc.z << ")");
    }

	return execMove(path);
}

// 移动
bool CMovePart::move(Vector3* pPointList, int nPointCount, bool bInitiatively)
{
    if(m_pMaster == NULL || nPointCount <= 0)
    {
        return false;
    }

    // 第一个路点和自己的距离比较大 说明AI服务那边寻的路用的是比较旧的起点 本次移动忽略
    // 有个假人原地复活问题 是因为回城复活后又用了旧的路点，直接更新了回去!
    Vector3 loc = m_pMaster->getLocation();
    float fDiff = loc.getDistance(pPointList[0]);
    if(fDiff >= 2)
    {
        return false;
    }

    if(getTickCountEx() < m_dwRigidityEndTick)
    {
        return false;
    }

	if (!canMove(bInitiatively))
    {
        return false;
    }

	std::vector<Vector3> path;
    if(fDiff > 0.5)
    {
        // 起点为自己的当前位置
        // 假如有误差时直接更新 可能会造成拉扯一下？
        path.push_back(loc);
    }

	for (int i = 0; i < nPointCount; ++i)
	{
		path.push_back(pPointList[i]);
	}

	return execMove(path);
}

bool CMovePart::canMove(bool bInitiatively)
{
    if (m_pMaster == NULL)
    {
        return false;
    }

	if (!m_bCanMove)
	{
		return false;
	}

    if(m_pMaster->getState() == ENTITY_STATE_DIE)
    {
        return false;
    }

	if (m_bForbitMoveInitiatively && bInitiatively)
	{
		WarningLn(__FUNCTION__ << "entity can't move initiatively..., [" << m_pMaster->getName() << "]");
		return false;
	}

    __IAIPart* pAIPart = (__IAIPart*)m_pMaster->getEntityPart(PART_AI);
	if (pAIPart != NULL && pAIPart->isAICharacter() && isLostLocation())
	{
		return false;
	}

	if (m_eBehaviour == BEHAVIOUR_TYPE_JUMP || m_eBehaviour == BEHAVIOUR_TYPE_SHIFT)
	{
		return false;
	}

	if (m_fSpeed < 0.0001f)
	{
		m_fSpeed = m_pMaster->getIntProperty(PROPERTY_SD);
		m_fSpeed /= MONSTER_SPEED_TIMES;
	}

	if (m_fSpeed < 0.0001f)
	{
		return false;
	}

	Move3 currentMove = getMoveState();

	// 投票是否可以移动
	event_entity_move context;
	context.uid = m_pMaster->getUID();
	context.oldLoc = currentMove.vPosition;
	context.curState = BEHAVIOUR_TYPE_WALK;
	if (!m_pMaster->getEntityEvent()->FireVote(EVENT_ENTITY_MOVE, (LPCSTR)&context, sizeof(context)))
	{
		return false;
	}

	return true;
}

// 移动
bool CMovePart::execMove(std::vector<Vector3> vecPath)
{
	if (vecPath.size() < 2)
	{
		++m_nFindPathFailTimes;

		if (m_nFindPathFailTimes >= 10)
		{
			event_got_stuck event;
			m_pMaster->getEntityEvent()->FireExecute(EVENT_GOT_STUCK, (LPCSTR)&event, sizeof(event));
			m_nFindPathFailTimes = 0;
		}
		return false;
	}

	m_nFindPathFailTimes = 0;

	// 设置走路参数
	WalkCtx& walkCtx = (WalkCtx&)m_Behaviours[BEHAVIOUR_TYPE_WALK]->getCtx();
	walkCtx.dwStartTick = gServerGlobal->getTimeSyncService()->GetTick();
	walkCtx.fStartSpeed = m_fSpeed;
	walkCtx.bKeepLookDir = false;
	walkCtx.vecPoint = vecPath;
	walkCtx.move = getMoveState();

	gotoBehaviour(BEHAVIOUR_TYPE_WALK);

	return true;
}

bool CMovePart::jumpTo(Vector3& loc, bool bAutoCheckGround, float fGroundY)
{
    if(m_pMaster == NULL)
    {
        return false;
    }

    if(m_pMaster->getState() == ENTITY_STATE_DIE)
    {
        return false;
    }

    if(m_fSpeed < 0.0001f)
    {
        m_fSpeed = m_pMaster->getIntProperty(PROPERTY_SD); 
        m_fSpeed /= MONSTER_SPEED_TIMES;
    }

	if (m_eBehaviour == BEHAVIOUR_TYPE_JUMP || m_eBehaviour == BEHAVIOUR_TYPE_SHIFT)
	{
		return false;
	}

    // 设置参数
    Move3 move = getMoveState();
    JumpCtx& ctx = (JumpCtx&)m_Behaviours[BEHAVIOUR_TYPE_JUMP]->getCtx();
    ctx.clear();
    ctx.dwStartTick = gServerGlobal->getTimeSyncService()->GetTick();
    ctx.fStartSpeed = m_fSpeed;
    ctx.startLoc = move.vPosition;
    ctx.targetLoc = loc; 
    ctx.move = move;

    m_bAutoCheckGround = bAutoCheckGround;
    m_fGroundY = fGroundY;

    gotoBehaviour(BEHAVIOUR_TYPE_JUMP);

    return true;
}

// 冲锋   
bool CMovePart::shift(void *pData, int nDataLen)
{   
    if(m_pMaster == NULL)
    {
        return false;
    }

    if(m_pMaster->getState() == ENTITY_STATE_DIE)
    {
        return false;
    }

    if(pData == NULL || nDataLen != sizeof(ShiftCtx))
    {
        return false;
    }
    ShiftCtx* pCtx = (ShiftCtx*)pData;

    // 是否为玩家
    bool isPlayerRole = m_pMaster->getType() == TYPE_PLAYER_ROLE;

    // 当前正在进行冲锋
    if(m_eBehaviour == BEHAVIOUR_TYPE_SHIFT && m_Behaviours[BEHAVIOUR_TYPE_SHIFT]->isRunning())
    {
        ShiftCtx& ctx = (ShiftCtx&)m_Behaviours[BEHAVIOUR_TYPE_SHIFT]->getCtx();
        
        // 当前冲锋优先级高 拒绝新的冲锋
        if(ctx.nPriority > pCtx->nPriority)
        {
            return false;
        }

        // 优先级一样 当前冲锋的技能晚 拒绝新的冲锋
        if(ctx.nPriority == pCtx->nPriority)
        {
            if(ctx.dwSpellStartTick > pCtx->dwSpellStartTick)
            {
                if (isPlayerRole)
                {
                    TraceLn("actor[" << m_pMaster->getName() << "] keep shift...");
                }
                return false;
            }
            if(isPlayerRole)
            {
                TraceLn("actor[" << m_pMaster->getName() << "] shift break by new shift...");
            }
        }
    } 

    Move3 move = getMoveState();

    // 投票是否可以冲锋
    event_entity_move context;
    context.uid = m_pMaster->getUID();
    context.oldLoc = move.vPosition;
    context.newLoc = pCtx->startLoc;
    context.curState = BEHAVIOUR_TYPE_SHIFT;
    context.nShiftPriority = pCtx->nPriority;
    context.bShiftPassive = pCtx->bPassive;
    if(!m_pMaster->getEntityEvent()->FireVote(EVENT_ENTITY_MOVE, (LPCSTR)&context, sizeof(context)))
    {
        return false;
    }

    pCtx->startLoc = move.vPosition;

	if(pCtx->shiftMode == shift_mode_forward_loc)
	{
        // 非抛物线冲锋 调整仰角
        if(!pCtx->bUseHeight)
        {
            Vector3 dir = pCtx->targetLoc - pCtx->startLoc;
            float fDistance = dir.getLength();
            Vector3 dirNormal = dir * (1 / fDistance);

            int nMapID = m_pMaster->getMapID();

            // 从地表下面冲锋的话 先调整到地表上面。
            // 有个bug:从高地的斜坡下往上冲 假如客户端给的目标点是在高地上的 
            //       导致冲锋的路径在折角处完全在地下面很深的地方，会被按照撞墙处理冲不上去。
            //       所以这儿根据坡的斜率调整仰角 让这类冲锋不至于冲不上去
            if(dirNormal.y > 0.15)
            {
                // 特殊处理高地斜坡上冲锋
                Vector3 ptNext = pCtx->startLoc + dirNormal * 0.5;
                float fOriY = ptNext.y;
                if(gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(nMapID, ptNext, 1, 0) && ptNext.y > fOriY + 0.1)
                {
                    // 沿着坡的方向冲
                    Vector3 locStart = pCtx->startLoc; 
                    // 高度场误差有0.x 所以求坡的方向时也用高度场求出来的高度调整起点
                    gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(nMapID, locStart, 1, 0);
                    // 抬高一点
                    Vector3 dirNewNormal = (ptNext - locStart).normalize();
                    if(dirNewNormal.y > dirNormal.y)
                    {
                        dirNormal = dirNewNormal;
                        pCtx->targetLoc = pCtx->startLoc + dirNormal * fDistance;
                        dir = pCtx->targetLoc - pCtx->startLoc;
                    }
                }
            }

            // 冲锋有悬空点需要检测仰角 避免冲到山上
            Vector3 ptCheck = pCtx->startLoc + dirNormal * 2;
            float fOriY = ptCheck.y;
            bool bCheckAngle = gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(nMapID, ptCheck, 0, 100) && fOriY > ptCheck.y + 0.5;

            // 冲锋仰角调整 0或者90度表示无限制(往下冲锋调整角度暂时屏蔽 正下方冲锋感觉是合理的)
            if (bCheckAngle && pCtx->targetLoc.y > move.vPosition.y && pCtx->nMaxUpAngle > 0 && pCtx->nMaxUpAngle < 90)
            {
                Vector3 locProjected = pCtx->targetLoc;
                locProjected.y = move.vPosition.y;

                float fXZEdgeLen = (locProjected - move.vPosition).getLength();
                if (fXZEdgeLen < 0.001)
                {
                    return false;
                }

                float fMaxTan = tan(pCtx->nMaxUpAngle * 3.14 / 180);
                if (abs(pCtx->targetLoc.y - locProjected.y) / fXZEdgeLen > fMaxTan)
                {
                    float fDistance = (pCtx->targetLoc - move.vPosition).getLength();

                    float fUpDownDis = fMaxTan * fXZEdgeLen;
                    pCtx->targetLoc.y = locProjected.y + ((pCtx->targetLoc.y > move.vPosition.y) ? fUpDownDis : -fUpDownDis);

                    pCtx->targetLoc = move.vPosition + (pCtx->targetLoc - move.vPosition).normalize() * fDistance;
                }
            }
        }
        else
        {
            // 抛物线 目标点可能高度比配置还高 把目标点调整到和起点一样高
            if(pCtx->targetLoc.y > pCtx->startLoc.y + pCtx->fHeight)
            {
                pCtx->targetLoc.y = pCtx->startLoc.y; 
            }
        }
	}

    ShiftCtx& ctx = (ShiftCtx&)m_Behaviours[BEHAVIOUR_TYPE_SHIFT]->getCtx(); 
    ctx = *pCtx;
    ctx.move = move;
    ctx.move.vPosition = ctx.startLoc;

    gotoBehaviour(BEHAVIOUR_TYPE_SHIFT);

    return true;
}

// 检测两点之间障碍(这个接口暂时只有AI翻滚判断时用到)
bool CMovePart::canPass(Vector3 locFrom, Vector3 locTo, float fAgentHeight)
{
	Vector3 line = locTo - locFrom;
	int nDistance = line.getLength();

	for (int i = 0; i < nDistance; ++i)
	{
		Vector3 ptCheck = locFrom + line * ((float)i / (float)nDistance);
		if (isHitBarrier(locFrom, ptCheck) || !gServerGlobal->getHeightFieldMgr()->canPass(m_pMaster->getMapID(), ptCheck))
		{
			return false;
		}
	}

	return true;
}

// 设置是否可以通过结界
void CMovePart::setCanPassBarrier(bool bCanPass)
{
    m_bCanPassBarrier = bCanPass;
}

// 当前的移动状态
Behaviour* CMovePart::getCurrentBehaviour()
{
    return m_Behaviours[m_eBehaviour];
}

// 旋转朝向
void CMovePart::rotateTo(Vector3 dirNew)
{
    if(!m_bCanMove)
    {
        return;
    }

    Move3 move = getMoveState();
    move.vRotation = dirNew.toEulerAngles();

    setMoveContext(move);

    m_dwKeepDirEndTick = getTickCountEx() + 200;
}

// 停止移动 cmd
bool CMovePart::stopMoving(bool bInitiatively)
{
    if(m_bForbitMoveInitiatively && bInitiatively)
    {
        WarningLn(__FUNCTION__ << "entity can't move initiatively..., [" << m_pMaster->getName() << "]");
        return false;
    }

    if(isLostLocation())
    {
        return false;
    }

    // 冲锋优先级暂定最高 只可能自然停止或者被其他冲锋打断
    if(m_eBehaviour == BEHAVIOUR_TYPE_SHIFT)
    {
        return false;
    }

    executeStopmoving();
    return true;
}

// 传送命令立即执行，被别的包冲刷掉可能表现有问题。
bool CMovePart::stopAt(Move3& move, int nSceneID, int nReason)
{
    __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent == NULL)
    {
        return false;
    }

    int nCurSceneID = m_pMaster->getSceneID();
    if(nSceneID == 0)
    {
        nSceneID = nCurSceneID;
    }

    Vector3 oldLoc = m_pMaster->getLocation();

    if(nReason != TRANSPORT_REASON_BACKCITY 
        && nReason != TRANSPORT_REASON_REBORN
        && nReason != TRANSPORT_REASON_SET_POS)
    {
        // 闪现校验障碍
        Vector3 flash = move.vPosition - oldLoc;
        int nFlashDistance = (move.vPosition - oldLoc).getLength();
        Vector3 ptPrevCheck = oldLoc;   
        for(int i = 0; i < nFlashDistance; ++i)
        {
            Vector3 ptCheck = oldLoc + flash * ((float)i / (float)nFlashDistance);  
            CheckResult eResult = checkPos(ptCheck);

            if(eResult != CHECK_RESULT_OK)
            {
                move.vPosition = ptPrevCheck;
                break;
            }
            else
            {
                ptPrevCheck = ptCheck;
            }
        }
    }
    else
    {
        // 普通传送贴地
        gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(m_pMaster->getMapID(), move.vPosition, 50, 50);
    }

    event_entity_transport transport;
    transport.oldScene = nCurSceneID;
    transport.oldLoc = oldLoc;
    transport.newScene = nSceneID;
    transport.newMove = move;
    transport.nReason = nReason;

    // 先投票，通过才能传送
//     if (!pEntityEvent->FireVote(EVENT_ENTITY_TRANSPORT,(LPCSTR)&transport,sizeof(transport)))
//     {
//         return false;
//     }

    // 发起传送事件
    pEntityEvent->FireExecute(EVENT_ENTITY_TRANSPORT,(LPCSTR)&transport,sizeof(transport));
    // 更新位置
    setMoveContext(move);
    // 转为停止
    gotoBehaviour(BEHAVIOUR_TYPE_NONE);

    m_bEnterStrictMoveCheck = true;
    m_dwStrictCheckTimeOut = getTickCountEx() + 400;

    return true;
}

void  CMovePart::executeStopmoving()
{
    if(m_eBehaviour == BEHAVIOUR_TYPE_NONE)
        return;

    gotoBehaviour(BEHAVIOUR_TYPE_NONE);
}

bool CMovePart::changeMoveState(Move3& move, bool bNotifySelf, bool bVerify)
{
    if(bVerify)
    {
        // 冲锋过程中技能那边假如同步位置过来的话 会把冲锋冲掉
        if (m_eBehaviour == BEHAVIOUR_TYPE_SHIFT || m_eBehaviour == BEHAVIOUR_TYPE_JUMP)
        {
            return false;
        }

        float fOffset = 0;
        if(!verifyClientMove(move.vPosition, &fOffset))
        {
            Vector3 locServer = m_pMaster->getLocation();
            TraceLn(__FUNCTION__ << ", location verify failed, actor=" << m_pMaster->getName() << ",serverpos(" << locServer.x << "," << locServer.y << "," << locServer.z << "), client loc(" << move.vPosition.x << "," << move.vPosition.y << "," << move.vPosition.z << ")");
            return false;
        }
    }

    m_dwLastLostLocationTick = 0;
    
    int nCheckMask = CHECK_MASK_DYNAMIC_BARRIER; 
    if(bVerify)
    {
       // bVerify为true表示需要校验一下 比如客户端使用技能时向服务器同步位置时的位置校验
#ifdef OPEN_CLIENT_TERRIAN_CHECK
        nCheckMask |= CHECK_MASK_TERRAIN; 
#endif // OPEN_CLIENT_TERRIAN_CHECK
    }
    else
    {
        // bVerify为false一般是服务器内部的某些效果设置位置
        nCheckMask |= CHECK_MASK_TERRAIN; 
    }

    if(checkPos(move.vPosition, nCheckMask) == CHECK_RESULT_OK)
    {
        setMoveContext(move);
    }
    gotoBehaviour(BEHAVIOUR_TYPE_NONE, bNotifySelf);

//     if(bNotifySelf)
//         correctClientPos();

    return true;
}

float CMovePart::getSpeed()
{
    return m_fSpeed;
}

bool  CMovePart::isMoving()
{
    Behaviour* pBehaviour = m_Behaviours[m_eBehaviour];
    
    return pBehaviour != NULL && pBehaviour->isRunning();
}

// 是否正在转向
bool CMovePart::isRotating()
{
    return getTickCountEx() < m_dwKeepDirEndTick;
}

Vector3 CMovePart::getMovingTarget()
{
	Vector3 ptTarget;
    Behaviour* pBehaviour = m_Behaviours[m_eBehaviour];
    if(pBehaviour == NULL)
    {
        return ptTarget;
    }

    if(pBehaviour->getTargetPoint(ptTarget))
        return ptTarget;

    return ptTarget;
}

void CMovePart::OnTimer( unsigned long dwTimerID )
{
    switch (dwTimerID)
    {
    case MOVE_UPDATE_TIMER:
        {
            update();
        }
        break;
	case MOVE_ACCELERATE_TIMER:
	{
		updateSpeedup();
	}
	break;
    default:
        break;
    }
}

// 部件更新
void CMovePart::update()
{    
    Behaviour* pBehaviour = m_Behaviours[m_eBehaviour];
    if(!pBehaviour)
    {
        WarningLn(__FUNCTION__ << ", current move state is null");
        return;
    }

    // 当前时刻
    DWORD dwNowTick = gServerGlobal->getTimeSyncService()->GetTick(); 

    // 上次update到本次update时间差
    int nElapseSinceLastUpdate = dwNowTick - pBehaviour->getCtx().dwLastUpdateTick;
    if(nElapseSinceLastUpdate <= 0)
    {
        return;
    }

     UID uidSelf = m_pMaster->getUID();

    // 追踪目标冲锋的话 要取目标当前的位置
    Vector3* pTargetLoc = NULL;
    Vector3 targetLoc;
    if(m_eBehaviour == BEHAVIOUR_TYPE_SHIFT 
        && ((ShiftCtx&)pBehaviour->getCtx()).shiftMode == shift_mode_forward_entity)
    {
        targetLoc = getLocation(((ShiftCtx&)pBehaviour->getCtx()).uidTarget);
        if(targetLoc != Vector3(0, 0, 0)) pTargetLoc = &targetLoc;
    }
    
    // 最终移动状态
    Move3 mvFinal;
    bool bSetMove = false;

    // 检测路点结果
    CheckResult eResult = CHECK_RESULT_OK;

    // 假人、怪物走路是寻路出来的 不需要用高度场检测地形碰撞
    int nCheckMask = CHECK_MASK_DYNAMIC_BARRIER;
    if(m_eBehaviour != BEHAVIOUR_TYPE_WALK)
    {
        nCheckMask |= CHECK_MASK_TERRAIN;
    }

    // 需要逐格检测
    // 否则update很慢时途中的地形会跳过检测（如场景边缘的气墙、地面等），导致卡外面或者掉地下面
    BEHAVIOURSTATE& refState = pBehaviour->getCtx().eState; 
    int nUpdateGrid = (m_eBehaviour == BEHAVIOUR_TYPE_SHIFT ? SHIFT_UPDATE_INTERVAL : (m_pMaster->getType() == TYPE_PLAYER_ROLE ? ACTOR_MOVE_UPDATE_INTERVAL : NPC_MOVE_UPATE_INTERVAL));
    for(DWORD dwTick = pBehaviour->getCtx().dwLastUpdateTick + nUpdateGrid; 
        dwTick < dwNowTick + 32 && refState == BEHAVIOUR_STATE_RUNNING; dwTick += nUpdateGrid)
    {
        pBehaviour->update(dwTick, pTargetLoc);

        Move3& mvSample = pBehaviour->getCtx().move;

        if(pBehaviour->getCtx().bKeepLookDir)
        {
            mvSample.vRotation = getMoveState().vRotation;
        }

        // 检查本次移动
        eResult = checkPos(mvSample.vPosition, nCheckMask);

        if(eResult != CHECK_RESULT_OK)
        {
            break;
        }

        mvFinal = mvSample;
        bSetMove = true;
    }
 
    if(bSetMove)
    {
        Vector3 locOld = m_pMaster->getLocation();
        event_entity_move context;
        context.uid = uidSelf;
        context.oldLoc = locOld;
        context.newLoc = mvFinal.vPosition;
        context.curState = m_eBehaviour;
        if(m_eBehaviour == BEHAVIOUR_TYPE_SHIFT)
        {
            ShiftCtx& shiftCtx = (ShiftCtx&)m_Behaviours[BEHAVIOUR_TYPE_SHIFT]->getCtx(); 
            context.nShiftPriority = shiftCtx.nPriority;
            context.bShiftPassive = shiftCtx.bPassive;
        }
        // 发投票事件
        if(!m_pMaster->getEntityEvent()->FireVote(EVENT_ENTITY_MOVE, (LPCSTR)&context, sizeof(context)))
        {
            // 强制同步一次位置
            correctClientPos(true);

            // 转为停止
            gotoBehaviour(BEHAVIOUR_TYPE_NONE);

            return;
        }

        // 测试发现 两点之间寻路 只要直线路径上没障碍 寻路最终返回起点终点
        // 所以假如路面不平的话 服务器需要自己贴一下地
        // 最好改一下寻路库...
        if(m_eBehaviour == BEHAVIOUR_TYPE_WALK)
        {
            gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(m_pMaster->getMapID(), mvFinal.vPosition, 5, 5);
        }

        // 设置位置
        setMoveContext(mvFinal);
    }

    // 碰撞了
    if(eResult != CHECK_RESULT_OK)
    {
        // 冲锋撞到静态的地形障碍 加buff
        if(eResult == CHECK_RESULT_HIT_TERRAIN_OBSTACLE && m_eBehaviour == BEHAVIOUR_TYPE_SHIFT)
        {
            shiftHitTerrainAddBuff();  
        }

        // 强制同步一次位置
        correctClientPos(true);

        // 冲锋完加buff
        if(m_eBehaviour == BEHAVIOUR_TYPE_SHIFT)
        {
            shiftFinishAddBuff();
        }

        // 转为停止
        gotoBehaviour(BEHAVIOUR_TYPE_NONE);

        return;
    }

    // 行为自然停止了
    if(refState != BEHAVIOUR_STATE_RUNNING)
    {
        if(pBehaviour->getType() == BEHAVIOUR_TYPE_SHIFT) 
        {
            // 冲锋完后 强制同步一下客户端位置 
            // 因为客户端冲锋是显示层自己模拟的 有时候不同步 不知道什么原因
            correctClientPos(true);

            // 冲锋完加buff
            shiftFinishAddBuff();
        }

        gotoBehaviour(BEHAVIOUR_TYPE_NONE);
        return;
    }
}

bool CMovePart::gotoBehaviour(BEHAVIOURTYPE eType, bool bNotifySelf)
{
    // 正在冲锋的话 不能不通知自己 否则客户端不清除冲锋标记会导致接下来几秒放不了技能
    // 客户端放技能时会触发changeMoveState会调这个函数 并且一般是不需要通知自己的位置的
    if(m_eBehaviour == BEHAVIOUR_TYPE_SHIFT && !bNotifySelf)
    {
        bNotifySelf = true;
    }

    Behaviour* pBehaviour = m_Behaviours[eType]; 

    if(pBehaviour != NULL && !pBehaviour->run())
    {
        return false;
    }

    BEHAVIOURTYPE eOld = m_eBehaviour;
    m_eBehaviour = eType;

    // 是否需要清理旧的行为
    if(m_eBehaviour != eOld && m_Behaviours[eOld] != NULL)
    {
        m_Behaviours[eOld]->getCtx().clear();
    }

    // 是否需要启动移动定时器
    if(eOld == BEHAVIOUR_TYPE_NONE && m_eBehaviour != BEHAVIOUR_TYPE_NONE)
    {
        int nInterval = NPC_MOVE_UPATE_INTERVAL;
        if(m_pMaster->getType() == TYPE_PLAYER_ROLE)
        {
            nInterval = ACTOR_MOVE_UPDATE_INTERVAL;
        }
        gServerGlobal->getTimerService()->SetTimer(MOVE_UPDATE_TIMER, nInterval, this, INFINITY_CALL, __FUNCTION__);
    }

    // 是否需要停止移动定时器
    if(eOld != BEHAVIOUR_TYPE_NONE && m_eBehaviour == BEHAVIOUR_TYPE_NONE)
    {
        gServerGlobal->getTimerService()->KillTimer(MOVE_UPDATE_TIMER, this);
    }

	// 从跑、跳，进入其他状态，要移除加速Buff
	if ((eOld== BEHAVIOUR_TYPE_WALK || eOld== BEHAVIOUR_TYPE_JUMP)
		&& eType!= BEHAVIOUR_TYPE_WALK 
		&& eType!= BEHAVIOUR_TYPE_JUMP)
	{
		this->removeSpeedupBuff();
	}
	if (eOld!= BEHAVIOUR_TYPE_WALK
		&& eType== BEHAVIOUR_TYPE_WALK)
	{
		this->addSpeedupBuff();
	}

    // 发送移动状态改变事件
    event_entity_move_behaviour_change synContext;
    synContext.uid = m_pMaster->getUID();
    synContext.move = getMoveState(); 
    synContext.speed = m_fSpeed;
    synContext.bNotifySelf = bNotifySelf;
    synContext.bNeedBroadcast = !m_bHandlingDropablePkg;
    synContext.nBehaviour = m_eBehaviour;
    synContext.nCreatureState = m_nCreatureState;

    m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE,(LPCSTR)&synContext,sizeof(synContext));

    if(eType == BEHAVIOUR_TYPE_NONE)
    {
        if(eOld != BEHAVIOUR_TYPE_JUMP)
        {
            // AI浮空了 先掉落下来
            if(m_pMaster->getIntProperty(PROPERTY_AI_SCHEME_ID) > 0)
            {
                Vector3 locGround = synContext.move.vPosition;
                if(gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(m_pMaster->getMapID(), locGround, 3, 100))
                {
                    if(synContext.move.vPosition.y > locGround.y + 3)
                    {
                        jumpTo(locGround, false, locGround.y);
                    }
                }
            }
        }
    }

    return true;
}

CMovePart::CheckResult CMovePart::checkPos(Vector3& loc, int nCheckMask)
{
    if(isInvalidPoint(loc))
    {
        return CHECK_RESULT_INVALID_LOC; 
    }

    // 是否超出边界
    float bmin[3], bmax[3];
    if(gServerGlobal->getHeightFieldMgr()->getMapBounds(m_pMaster->getMapID(), bmin, bmax))
    {
        if(loc.y < bmin[1])
        {
            return CHECK_RESULT_BELOW_LOWEST_HEIGHT; 
        }

        if(loc.x < bmin[0] || loc.x > bmax[0] || loc.z < bmin[2] || loc.z > bmax[2])
        {
            return CHECK_RESULT_OUT_OF_MAP_RANGE; 
        }
    }

    Vector3 locOld = m_pMaster->getLocation();
    UID uidSelf = m_pMaster->getUID(); 

    // 检测结界
    if(nCheckMask & CHECK_MASK_DYNAMIC_BARRIER)
    {
        if(isHitBarrier(locOld, loc))
            return CHECK_RESULT_HIT_DYANMIC_BARRIER;
    }

    // 检测地形障碍
    if(nCheckMask & CHECK_MASK_TERRAIN)
    {
        if(!gServerGlobal->getHeightFieldMgr()->canPass(m_pMaster->getMapID(), loc))
        {
            return CHECK_RESULT_HIT_TERRAIN_OBSTACLE; 
        }
    }

    return CHECK_RESULT_OK;
}

// 设置位置
void CMovePart::setMoveContext(Move3& move)
{
    Vector3 locOld = m_pMaster->getLocation();

    __IEntityProperty* pProperty = (__IEntityProperty*)(m_pMaster->getEntityPart(PART_PROPERTY));
    pProperty->setProperty_custom(PROPERTY_LOCATION, &(move.vPosition), sizeof(move.vPosition));
    pProperty->setProperty_custom(PROPERTY_ANGLE, &(move.vRotation), sizeof(move.vRotation));
    pProperty->setProperty_custom(PROPERTY_VELOCITY, &(move.vVelocity), sizeof(move.vVelocity));

    // 如果是actor 发场景逻辑事件
    if(m_pMaster->getType() == TYPE_PLAYER_ROLE)
    {
        UID uidSelf = m_pMaster->getUID();
        event_entity_move context; 
        context.uid = uidSelf; 
        context.oldLoc = locOld;
        context.newLoc = move.vPosition;
        __IEntityAOI* pAOI = (__IEntityAOI*)m_pMaster->getEntityPart(PART_AOI);
        if(pAOI != NULL)
        {
            AOI_PROXY_PTR pAOIProxy = pAOI->getAOIProxy();
            if(pAOIProxy != NULL)
            {
                context.nVisibleFlag = pAOIProxy->m_nVisibaleFlag;
            }
        }

        SceneLogicEventHelper helper(UID_2_SCENE(uidSelf));
        if(helper.m_ptr)
        {
            helper.m_ptr->FireExecute(EVENT_GAME_ACTOR_MOVE, TYPE_PLAYER_ROLE, uidSelf, (LPCSTR)&context,sizeof(context));
        }
    }

    m_dwLastUpdatePosTick = gServerGlobal->getTimeSyncService()->GetTick();
}

// 是否遇到障碍
bool CMovePart::isHitBarrier(Vector3 ptFrom, Vector3 ptTo)
{
    if(m_bCanPassBarrier)
    {
        return false;
    }

    for(std::list<Barrier*>::iterator it = m_listBarrier.begin(); it != m_listBarrier.end(); ++it)
    {
        // 正好落在结界上不算撞到 #5341 结界边缘碰到目标时，不要卡住目标。
        if(!(*it)->hitTest(ptFrom, ptFrom) && (*it)->hitTest(ptFrom, ptTo))
        {
            return true;
        }
    }

    return false;
}

// 校验移动位置
bool CMovePart::verifyClientMove(Vector3& vPoint, float * fOffset)
{
	DWORD dwCurTick = gServerGlobal->getTimeSyncService()->GetTick(); 
	int nElapse = dwCurTick - m_dwLastUpdatePosTick;
	if(nElapse <= 0)
	{
		nElapse = 100;   // 容差100ms
	}

    // (y轴上的位移忽略)
    Move3 move = getMoveState();
    Vector3 ptServer = move.vPosition; 
    Vector3 ptClient = vPoint; 
    float fDiffrence = (ptClient - ptServer).getLength();

    // 准备好当前移动速度
    if(m_fSpeed < 0.0001f)
    {
        m_fSpeed = m_pMaster->getIntProperty(PROPERTY_SD); 
        m_fSpeed /= MONSTER_SPEED_TIMES;
    }

    // 本次允许位移
    float fAllowOffset = 0;
    if(m_bEnterStrictMoveCheck && getTickCountEx() < m_dwStrictCheckTimeOut)
    {
        // 传送后防止被客户端旧的位置更新回去。 
        fAllowOffset = 3;
    }
    else
    {
        // 瞬移位移(有些动作决定位移的，如翻滚、被弓箭手击退)
        float fFlashOffset = 8;
        float fVelocity = move.vVelocity.getLength(); 
        if(fVelocity > 0.1)
        {
            // 如果客户端之前在移动，本次允许的最大位移
            fAllowOffset = max(fVelocity, m_fSpeed) * nElapse / 1000 * 1.5 + fFlashOffset;
        }
        else
        {
            // 如果客户端之前没移动,本次允许的最大位移
            fAllowOffset = fFlashOffset;
        }
    }

    if(fDiffrence > fAllowOffset)
    {
        TraceLn("actor=" << m_pMaster->getName() << ", pos check fail, serverPos(" << ptServer.x << "," << ptServer.y << "," << ptServer.z << "), clientPos(" << ptClient.x << "," << ptClient.y << "," << ptClient.z << "), allow offset=" << fAllowOffset);
        return false;
    }

	// 校验通过返回偏移值
	if( fOffset ) *fOffset = fDiffrence;

    return true;
}

// 是否丢弃包
bool CMovePart::isDropPkg(WORD wPkgSeq, WORD wPkgPrecondSeq, bool bCheckPrecondPkg)
{
    if(wPkgSeq == wPkgPrecondSeq)
    {
        // 收到关键包 机制上保证关键包不会过期
        return false;
    }
    else
    {
        if(m_wRcvPkgSeq == INVLAID_PKG_SEQ)
        {
            // 之前还没收到过任何包 这个包不必丢弃
            return false;
        }

        // 需要检测前置包条件
        if(bCheckPrecondPkg)
        {
            if(wPkgPrecondSeq != INVLAID_PKG_SEQ)
            {
                WORD wSub = m_wRcvPkgSeq - wPkgPrecondSeq; // 与最近收到的包的序列号之差 可以处理回绕1-65535 = 2
                if(wSub > 10000) // 最近10000个包内无前置包<==>前置包没有收到
                {
                    TraceLn("prcond pkg=" << wPkgPrecondSeq << ", last pkg=" << m_wRcvPkgSeq << ", this_pkg=" << wPkgSeq  << ",name=" << m_pMaster->getName());
                    return true;
                }
            }
        }

        // 检测是否过期
        WORD wSub = wPkgSeq - m_wRcvPkgSeq; // 与最近收到的包的序列号之差 可以处理回绕1-65535 = 2
        if(wSub > 10000)
        {
            // 内网应该不会乱序 按理不会走到这里
            WarningLn("syn pkg=" << wPkgSeq << ", last pkg=" << m_wRcvPkgSeq << ", name=" << m_pMaster->getName());
            return true;
        }

        return false;
    }
}

// 客户端同步位置
bool CMovePart::handleClientMove(void* data, int len)
{
    if(data == NULL || m_pMaster == NULL)
    {
        return false;
    }

    // 是否开启优化字段
    BYTE byOpenOptimize = *((BYTE*)data);
    data = (BYTE*)data + 1;
    len -= 1;

    msg_loc_sync_pkg_optimize_head* pOptimizeHead = NULL;
    msg_creature_move_sync* msg = NULL;
    if(byOpenOptimize)
    {
        if(len < sizeof(msg_loc_sync_pkg_optimize_head) + sizeof(msg_creature_move_sync))
        {
            ErrorLn(__FUNCTION__ << " 1, msg format error, len=" << len);
            return false;
        }

        pOptimizeHead = (msg_loc_sync_pkg_optimize_head*)data;
        msg = (msg_creature_move_sync*)(pOptimizeHead + 1); 
    }
    else
    {
        if(len < sizeof(msg_creature_move_sync))
        {
            ErrorLn(__FUNCTION__ << " 2, msg format error, len=" << len);
            return false;
        }
        msg = (msg_creature_move_sync*)data;
    }

    // 客户端开了移动包优化
    if(pOptimizeHead != NULL)
    {
        /* 位置相关的同步 统一加序号，并且有两种包：一种是不能丢弃的关键包，一种是可丢弃的普通包。
           PS:不加序号的话 这两种包可能乱序到达 这是不允许的
           加了序号后，可以识别包是否过期，但是不应该允许关键包过期的情况出现
           避免关键包过期：普通包中都有前置关键包,收到时检查前置关键包还没收到的话，直接丢弃这个普通包。
         */

        /*
           服务器之前的一波优化，导致收到网关过来的移动消息后，已经去重投递，这就不能保证关键包不能丢的机制。
           由于客户端向服务器同步位置的消息只有这一种 所以关键包和普通包意义上没区别 丢弃关键包和丢弃普通包一样
           (不同于客户端：会收到传送同步 普通移动同步。 两种包乱序不允许，且传送包不能丢!。)
           所以只保证顺序执行过期丢弃的机制没问题。
        */
        if(isDropPkg(pOptimizeHead->wPkgSeq, pOptimizeHead->wPrecondPkgSeq, false))
        {
            return false;
        } 

        // 更新当前的序列号
        m_wRcvPkgSeq = pOptimizeHead->wPkgSeq; 

        // 正在处理可以丢的包
        m_bHandlingDropablePkg = (pOptimizeHead->wPkgSeq != pOptimizeHead->wPrecondPkgSeq); 
    }
    else
    {
        m_bHandlingDropablePkg = false; // 是否需要移动包再加字段？
    }

    if(!canMove(true))
    {
        return false;
    }

	m_dwRcvLastMoveMsgTick = gServerGlobal->getTimeSyncService()->GetTick();
	int nElapse = m_dwRcvLastMoveMsgTick - msg->dwTick;
	if( nElapse < -300 || nElapse>500 )
	{
		WarningLn(__FUNCTION__ << __LINE__ << ", handleClientMove failed...serverTick=" << m_dwRcvLastMoveMsgTick << " clientTick=" << msg->dwTick << " name=" << m_pMaster->getName() );
		return false;
	}

    Move3 move;
    move.vPosition.x = (float)msg->nPosX / (float)10;
    move.vPosition.y = (float)msg->nPosY / (float)10;
    move.vPosition.z = (float)msg->nPosZ / (float)10;
    move.vRotation.x = (float)msg->nRotX / (float)10;
    move.vRotation.y = (float)msg->nRotY / (float)10;
    move.vRotation.z = (float)msg->nRotZ / (float)10;
    move.vVelocity.x = (float)msg->nVelocityX / (float)10; 
    move.vVelocity.y = (float)msg->nVelocityY / (float)10; 
    move.vVelocity.z = (float)msg->nVelocityZ / (float)10;

    // 校验本次移动
    float fOffset = 0;
    if(!verifyClientMove(move.vPosition, &fOffset))
    {
        //          WarningLn(__FUNCTION__ << ", verifyClientMove failed..." << "pos(" << msg->move.vPosition.x << "," << msg->move.vPosition.y << "," << msg->move.vPosition.z << ")"
        //              << "name=" << m_pMaster->getName() << ", mapid=" << m_pMaster->getMapID() << ", sceneid=" << m_pMaster->getSceneID());
        // 发传送消息到客户端到客户端收到传送消息这段时间 可能还会有同步包上来(或者网络上或者消息缓存里已经有比较旧位置的同步消息) 
        // 这个期间的包要加强一点校验 不然位置会被设置成传送之前的位置。这个期间收到这种包 也不应该再拉（传送）客户端，不然连着传送客户端可能会抖动。
        if(m_bEnterStrictMoveCheck && getTickCountEx() < m_dwStrictCheckTimeOut)
        {
            WarningLn("actor=" << m_pMaster->getName() << ", old loc sync msg recvd");
            return false;
        }

        gotoBehaviour(BEHAVIOUR_TYPE_NONE, false);
        correctClientPos();
        return false;
    }
    
    int nCheckMask = CHECK_MASK_DYNAMIC_BARRIER; 
#ifdef OPEN_CLIENT_TERRIAN_CHECK
    nCheckMask |= CHECK_MASK_TERRAIN; 
#endif

    // 校验位置
    CheckResult eCheckResult = checkPos(move.vPosition, nCheckMask);
    switch(eCheckResult)
    {
    case CHECK_RESULT_OK:
        {

        }
        break;
    case CHECK_RESULT_INVALID_LOC:
    case CHECK_RESULT_OUT_OF_MAP_RANGE:
    case CHECK_RESULT_HIT_DYANMIC_BARRIER:
        {
            gotoBehaviour(BEHAVIOUR_TYPE_NONE, false);
            correctClientPos();
            return false;
        }
        break;
    case CHECK_RESULT_HIT_TERRAIN_OBSTACLE:
        {
            WarningLn("client pos(" << move.vPosition.x << "," << move.vPosition.y << "," << move.vPosition.z << "), hit terrain obstacle, name=" << m_pMaster->getName());   
             
            gotoBehaviour(BEHAVIOUR_TYPE_NONE, false);
            correctClientPos(true, 2);
            return false;
        }
        break;
    case CHECK_RESULT_BELOW_LOWEST_HEIGHT:
        {
            TraceLn(__FUNCTION__ << ", actor[" << m_pMaster->getName() << "] fall2Death, y=" << move.vPosition.y);
            fall2Death();
            return false;
        }
        break;
    default:
        {
            ErrorLn(__FUNCTION__ << ", checkPos result=" << eCheckResult);
            return false;
        }
    }

    // 投票是否可以移动
	if ( fOffset>0.01 )
	{
		if (!m_pMaster->getEntityEvent()->FireVote(EVENT_ENTITY_CLIENTOPT_MOVE, NULL, 0))
		{
            Vector3 locServer = m_pMaster->getLocation();

            TraceLn("actor=" << m_pMaster->getName() << ", cant move: vote fail.., locServer(" << locServer.x << "," << locServer.y << "," << locServer.z << "), locClient(" << move.vPosition.x << "," << move.vPosition.y << "," << move.vPosition.z << ")");
            gotoBehaviour(BEHAVIOUR_TYPE_NONE, false);
            return false;
		}
	}

    m_bEnterStrictMoveCheck = false;

    // 还要测试 先忽略这个字段
    //m_nCreatureState = msg->nCreaturState;
    m_nCreatureState = -1;

    setMoveContext(move);

	if (false)
	{
		//调试日志
		auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
		int sudu = m_pMaster->getIntProperty(PROPERTY_SD);
		auto uSelfID = m_pMaster->getUID();
		TraceLn("=====Player【" << uSelfID << "】send move msg!currSpeed:" << sudu << "============ time:" << curTick << ",direction==" << msg->direction);
	}

	bool isWalking = msg->nVelocityX || msg->nVelocityY || msg->nVelocityZ;
	if (isWalking)
	{
		if (msg->direction == WALK_DIRECTION::WALK_BACK)
			removeSpeedupBuff();
		else if (msg->direction == WALK_DIRECTION::WALK_FORWARD)
			addSpeedupBuff();
	}
	else
	{
		//停下来了
		removeSpeedupBuff();
	}
	gotoBehaviour(BEHAVIOUR_TYPE_NONE, false);

    return true;
}

bool isRoleForLog(__IEntity* pMaster) {
	return false;
	bool isFindAI = false;
	bool isRole = (pMaster->getIntProperty(PROPERTY_AI_SCHEME_ID) <= 0);
	if (isFindAI && !isRole)
	{
		static const auto uFixID = pMaster->getUID();
		auto uTempID = pMaster->getUID();
		if (uFixID == uTempID)
		{
			return true;
		}
		return false;
	}
	return isRole;
}


void CMovePart::updateSpeedup()
{
	if (ENTITY_TYPE::TYPE_PLAYER_ROLE != m_pMaster->getType())
		return;
	if (m_pMaster->getIntProperty(PROPERTY_AI_SCHEME_ID) > 0)
		return;
	
	// 玩家
	if (m_dwRcvLastMoveMsgTick == 0)
		return;
	auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
	if (curTick - m_dwRcvLastMoveMsgTick == MOVE_ACCELERATE_INTERVAL * 2)
		removeSpeedupBuff();
}

void CMovePart::addSpeedupBuff()
{
	if (ENTITY_TYPE::TYPE_PLAYER_ROLE != m_pMaster->getType())
		return;
	if (m_pMaster->getState() == ENTITY_STATE_FIGHT)
		return;
	if (m_pMaster->getState() == ENTITY_STATE_DIE)
		return;

	// 是玩家 且 还没走过
	if (m_pMaster->getIntProperty(PROPERTY_AI_SCHEME_ID) <= 0
		&& m_dwRcvLastMoveMsgTick == 0)
		return;

	/// 获取配置中心接口
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (!pSchemeCenter)
		return;
	ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
		return;
	ISchemeSceneMatron *pSchemeSceneMatron = pSchemeCenter->getSchemeSceneMatron();
	if (pSchemeSceneMatron == NULL)
		return;
	//检查当前地图是否允许“越跑越快”
	if (!pSchemeSceneMatron->CanAccelerate(m_pMaster->getMapID()))
		return;

	__IBuffPart *pBuffPart = (__IBuffPart *)m_pMaster->getEntityPart(PART_BUFF);
	if (pBuffPart == NULL)
		return;
	auto pBuffIDs = pSchemeEntity->getEntityXml()->pArrWarSpeedBuffIDs;
	int bBuffLevel = pSchemeEntity->getEntityXml()->nWarSpeedBuffLevel;
	UID uSelfID = m_pMaster->getUID();

	for (size_t i = 0; i < MAX_SPEED_UP_BUFF; i++)
	{
		auto nBuffID = pBuffIDs[i];
		if (nBuffID<=0)
			continue;

		if (pBuffPart->IsExist(nBuffID, 0, uSelfID))
			continue;

		if (isRoleForLog(m_pMaster))
		{
			// 是玩家
			auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
			int sudu = m_pMaster->getIntProperty(PROPERTY_SD);
			TraceLn("+++++Player【" << uSelfID << "】,currSpeed:" << sudu << ",Add SpeedBuff" << nBuffID<<"++++++++++++++++ time:" << curTick);
		}

		bool success = pBuffPart->Add(nBuffID, bBuffLevel, uSelfID);

		if (isRoleForLog(m_pMaster))
		{
			// 是玩家
			int sudu = m_pMaster->getIntProperty(PROPERTY_SD);
			TraceLn("+++++Player【" << uSelfID << "】Speed Up " << success << ",currSpeed:" << sudu);
		}
	}
}

// 删除加速Buff
void CMovePart::removeSpeedupBuff()
{
	if (ENTITY_TYPE::TYPE_PLAYER_ROLE != m_pMaster->getType())
		return;
	// 是玩家 且 还没走过
	if (m_pMaster->getIntProperty(PROPERTY_AI_SCHEME_ID) <= 0
		&& m_dwRcvLastMoveMsgTick == 0)
		return;

	/// 获取配置中心接口
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (!pSchemeCenter)
		return;
	ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
		return;
	ISchemeSceneMatron *pSchemeSceneMatron = pSchemeCenter->getSchemeSceneMatron();
	if (pSchemeSceneMatron == NULL)
		return;
	//检查当前地图是否允许“越跑越快”
	if (!pSchemeSceneMatron->CanAccelerate(m_pMaster->getMapID()))
		return;

	__IBuffPart *pBuffPart = (__IBuffPart *)m_pMaster->getEntityPart(PART_BUFF);
	if (pBuffPart == NULL)
		return;

	auto pBuffIDs = pSchemeEntity->getEntityXml()->pArrWarSpeedBuffIDs;
	UID uSelfID = m_pMaster->getUID();
	for (size_t i = 0; i < MAX_SPEED_UP_BUFF; i++)
	{
		auto nBuffID = pBuffIDs[i];
		if (nBuffID <= 0)
			continue;

		if (isRoleForLog(m_pMaster))
		{
			int sudu = m_pMaster->getIntProperty(PROPERTY_SD);
			TraceLn("-----Player【" << m_pMaster->getUID() << "】,currSpeed:" << sudu << ",del SpeedBuff" << nBuffID << "--------");
		}

		// 删除加速Buff
		pBuffPart->Remove(nBuffID, (UID)INVALID_UID, uSelfID);

		if (isRoleForLog(m_pMaster))
		{
			// 是玩家
			int sudu = m_pMaster->getIntProperty(PROPERTY_SD);
			TraceLn("------Player【" << uSelfID << "】 del SpeedBuff,currSpeed:" << sudu);
		}
	}
}

// 客户端控制(目前只是广播了一下)
bool CMovePart::handleClientCtrl(void* data, int len)
{
    int nSceneID = m_pMaster->getSceneID();
    SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(nSceneID);
    if ( pContainer==0 )
    {
        ErrorLn("move part on command COMMAND_ENTITY_CTRL failed! nSceneID=" << nSceneID );
        return false;
    }

    IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
    if (pScene == 0)
    {
        ErrorLn("move part on command COMMAND_ENTITY_CTRL failed! pScene==0, nSceneID=" << nSceneID );
        return false;
    }

    // 停止移动
    // stopMoving();

    obuf o;
    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
    head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
    head.byKeyModule    = MSG_MODULEID_ENTITY;
    head.byKeyAction    = SC_MSG_ENTITY_CTRL;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
    entityHead.byPartID = PART_MOVE;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

    o << head << entityHead;
    o.push_back(data,len);

	// 只需要广播给周围的人就可以了
	pScene->broadcast_nearLoc(m_pMaster->getLocation(),0,BROADCAST_DO_ACTION,std::string(o.data(),o.size()), AOI_PROXY_PTR(0), false);

    return true;
}

// 客户端降落
void CMovePart::handleCommandLand(msg_creature_act_land* data)
{
    /// 获取配置中心接口
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (!pSchemeCenter)
    {
        return;
    }

    ISchemeLandBuff* pSchemeLandBuff = pSchemeCenter->getSchemeLandBuff();
    if (!pSchemeLandBuff)
    {
        return;
    }
    SBuffNode * pNode = pSchemeLandBuff->GetBuffNode((int)data->fValue);
    if (!pNode)
    {
        return;
    }
    __IBuffPart *pBuffPart = (__IBuffPart *)m_pMaster->getEntityPart(PART_BUFF);
    if (pBuffPart == NULL)
    {
        return;
    }

    // 停止移动
    //stopMoving();

    //WarningLn("CPlayRoleMovePart::handleCommandLand() name="<< m_pMaster->getName() <<", landValue="<< (int)data->fValue <<", buffid="<< pNode->dwID <<", level="<< pNode->dwLevel);
    // 给实体添加buff
    pBuffPart->Add(pNode->dwID, pNode->dwLevel, m_pMaster->getUID());
}

// 客户端设置立足点类型
void CMovePart::handleCommandSetFormType(msg_creature_act_set_formtype* data)
{
    if(m_pMaster == NULL || m_pMaster->getType() != TYPE_PLAYER_ROLE)
    {
        return;
    }
    int nNewFormType = data->nValue;
    // 不合法的类型
    if(nNewFormType < FORM_GROUND || nNewFormType >= FORM_MAX)
    {
        return;
    }
    // 立足点类型 FORM_TYPE
    int nOldFormType = m_pMaster->getIntProperty(PROPERTY_FORMTYPE);
    // 类型没有变化
    if(data->nValue == nOldFormType)
    {
        return;
    }

    // 切换成地面状态时，校验Y坐标大于5视为不合法, TODO 后面的校验到实际情境中再来补上
    if(nNewFormType == FORM_GROUND)
    {
        Vector3 loc = m_pMaster->getLocation();
        PathFindHelper helper(m_pMaster->getSceneID());
        if(helper.m_ptr)
        {
            float fHeight = .0f;
            helper.m_ptr->getHeight(&loc, fHeight);
            if(fHeight > 5.0)
            {
                return;
            }
        }
    }

    // 更新属性值
    m_pMaster->setIntProperty(PROPERTY_FORMTYPE, nNewFormType);
    //TraceLn(__FUNCTION__": "<< m_pMaster->getName()<<" SET FORM TYPE="<< nNewFormType);
}

// 客户端转镜头
void CMovePart::handleCommandTurnCamera(void * data, size_t len)
{
    if (!m_pMaster)
    {
        return;
    }
    if (!data || len < sizeof(int))
    {
        return;
    }
    int nCameraIndex = *(int*)data;
    if (nCameraIndex < 0 || nCameraIndex >= CAMERA_BUFF_COUNT)
    {
        ErrorLn(__FUNCTION__": parameter is invalid!  index="<< nCameraIndex <<", name="<< m_pMaster->getName()  );
        return;
    }

    /// 获取配置中心接口
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (!pSchemeCenter)
    {
        return;
    }
    ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
    if (pSchemeEntity == NULL)
    {
        return;
    }
    int nBuffID= pSchemeEntity->getEntityXml()->nTurnCameraBuffID[nCameraIndex];
    if ( nBuffID == 0)
    {
        return;
    }
    __IBuffPart *pBuffPart = (__IBuffPart *)m_pMaster->getEntityPart(PART_BUFF);
    if (pBuffPart == NULL)
    {
        return;
    }
    pBuffPart->Add(nBuffID, 1, m_pMaster->getUID());
}

// 取速度方向
Vector3  CMovePart::getAngle()
{
    return getMoveState().vRotation;
}

Move3 CMovePart::getMoveState()
{
    Move3 move;
    if(m_pMaster == NULL)
    {
        return move;
    }

    __IEntityProperty* pProperty = (__IEntityProperty*)(m_pMaster->getEntityPart(PART_PROPERTY));
    pProperty->getProperty_custom(PROPERTY_LOCATION, &(move.vPosition), sizeof(move.vPosition));
    pProperty->getProperty_custom(PROPERTY_ANGLE, &(move.vRotation), sizeof(move.vRotation));
    pProperty->getProperty_custom(PROPERTY_VELOCITY, &(move.vVelocity), sizeof(move.vVelocity));

    return move;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMovePart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch (wEventID)
    {
	case EVENT_ENTITY_CREATE:
	{
	}
	break;
	case EVENT_ENTITY_CHANGE_STATE:
	{
		if (pszContext == NULL || nLen < sizeof(event_entity_change_state))
		{
			ErrorLn(__FUNCTION__ << " EVENT_ENTITY_CHANGE_STATE format error");
			return;
		}

		event_entity_change_state* pEvent = (event_entity_change_state*)pszContext;
		if (ENTITY_STATE_FIGHT == pEvent->nState)
		{
			// 进入战斗状态
			this->removeSpeedupBuff();
		}
	}
	break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_SD:
        {
			if(pszContext == NULL || nLen < sizeof(event_entity_property_change))
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return;
            }
            event_entity_property_change* pEvent = (event_entity_property_change*)pszContext;
            if(pEvent->nPropertyID != PROPERTY_SD)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return;
            }

            int nSpeed = 0;
            switch(pEvent->nLen)
            {
            case sizeof(BYTE):
                nSpeed = *(BYTE *)&pEvent->data;
            case  sizeof(short):
                nSpeed = *(unsigned short *)&pEvent->data;
            case sizeof(int):
                nSpeed = *(int*)pEvent->data; 
            }

            setSpeed((float)nSpeed / MONSTER_SPEED_TIMES);
        }
        break;
    case EVENT_ENTITY_DIE:
        {
            m_dwLastLostLocationTick = 0;
			
			bool bShifting = m_eBehaviour == BEHAVIOUR_TYPE_SHIFT;
			executeStopmoving();
			if (bShifting)
			{
				correctClientPos(true);
			}
        }
        break;
    case EVENT_GAME_OBSTACLE_CREATE:
    case EVENT_GAME_OBSTACLE_DESTROY:
        {
            // 假如自己正在障碍物附近移动,重启移动
            if(isMoving())
            {
                Vector3 ptObstacle = getLocation(dwSrcID);
                Move3 move = getMoveState();

                if(move.vPosition.checkDistance(ptObstacle, 64))
                {
                    m_Behaviours[m_eBehaviour]->adjustCtx(gServerGlobal->getTimeSyncService()->GetTick());
                    gotoBehaviour(m_eBehaviour);
                }
            }
        }
        break;
    case EVENT_GAME_BLOCK_BARRIER_CREATE:
        {
            // 添加到本地障碍记录
            if(pszContext == NULL || nLen < sizeof(event_block_barrier_create))
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return;
            }

            event_block_barrier_create* pEvent = (event_block_barrier_create*)pszContext;

            // 障碍对自己无效
            if(!chooseTarget(pEvent->uidCreater, pEvent->nTargetFilter, m_pMaster->getUID()))
            {
                return;
            }

            Barrier* pBarrier = NULL;
            switch(pEvent->nBarrierShape)
            {
            case BARRIER_SHAPE_CIRCLE:
                {
                    pBarrier = Barrier_Circle::create((BarrierBlockType)pEvent->nBlockType,
                        pEvent->uidCreater, pEvent->dwSerial, pEvent->circle.locCenter, pEvent->circle.fInnerRadius, pEvent->circle.fOutterRadius);
                }
                break;
            case BARRIER_SHAPE_LINE:
                {
                    pBarrier = Barrier_Line::create((BarrierBlockType)pEvent->nBlockType,
                        pEvent->uidCreater, pEvent->dwSerial, pEvent->line.locStart, pEvent->line.locEnd, pEvent->line.fWidth, pEvent->line.fHeight);
                }
                break;
            case BARRIER_SHAPE_CYLINDER:
                {
                    pBarrier = Barrier_Cylinder::create((BarrierBlockType)pEvent->nBlockType, pEvent->uidCreater, pEvent->dwSerial, pEvent->cylinder.locCenter, pEvent->cylinder.fRadius, pEvent->cylinder.fHeight);
                }
                break;
            default:
                {
                    ErrorLn(__FUNCTION__ << __LINE__ << ", undefined shape=" << pEvent->nBarrierShape);
                    return;
                }
            }

            if(pBarrier == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return;
            }

            // 添加到本地记录
            m_listBarrier.push_back(pBarrier);
        }
        break;
    case EVENT_GAME_BLOCK_BARRIER_DESTROY:
        {
            if(pszContext == NULL || nLen < sizeof(event_block_barrier_destroy))
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return;
            }

            event_block_barrier_destroy* pEvent = (event_block_barrier_destroy*)pszContext;
            for(std::list<Barrier*>::iterator it = m_listBarrier.begin(); it != m_listBarrier.end(); )
            {
                if((*it)->m_eBlockType == pEvent->nBlockType
                    && (*it)->m_uidCreater == pEvent->uidCreater
                    && (*it)->m_dwSerial == pEvent->dwSerial)
                {
                    (*it)->release();
                    m_listBarrier.erase(it++);
                }
                else
                {
                    ++it;
                }
            }
        }
        break;

    default:
        break;
    }
}

bool CMovePart::OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
    case EVENT_ENTITY_PREPARE_CAST_SPELL:
        {
            // 正在冲锋 不允许放技能
            return m_eBehaviour != BEHAVIOUR_TYPE_SHIFT; 
        }
    }

    return true;
}

// 客户端拉回到服务器位置
void CMovePart::correctClientPos(bool bBroadcast, int nRaiseHeight)
{
    if(m_pMaster == NULL || m_pMaster->getType() != TYPE_PLAYER_ROLE)
    {
        return;
    }

    int nSceneID = m_pMaster->getSceneID();
    int nMapID = m_pMaster->getMapID();

    Move3 move = getMoveState();

    // 往上拉高也有可能卡在类似屋檐的物体里
    Vector3 loc = move.vPosition;
    loc.y += nRaiseHeight;
    if(gServerGlobal->getHeightFieldMgr()->canPass(m_pMaster->getMapID(), loc))
    {
        move.vPosition = loc;
    }
    else
    {
        WarningLn(__FUNCTION__ << __LINE__ << ", raise up will hit obstacle, actor=" << m_pMaster->getName() << ", pos(" << move.vPosition.x << "," << move.vPosition.y << "," << move.vPosition.z << ")");
    }

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = SC_MSG_ENTITY_TRANSPORT;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
    entityHead.byPartID = PART_MOVE;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

    obuf256 data;
    data << head << entityHead;

    // 是否开启移动包优化
    bool bEnableOptimize = false;
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter != NULL)
    {
        ISchemeEntity* pSchemeEntity = pSchemeCenter->getSchemeEntity();
        if(pSchemeEntity != NULL)
        {
            bEnableOptimize = (pSchemeEntity->getEntityXml()->nOpenMovePkgOptimize != 0);
        }
    }

    // 开启移动包优化
    if(bEnableOptimize)
    {
        if(m_wSendPkgSeq == INVLAID_PKG_SEQ)
        {
            m_wSendPkgSeq++;
        }

        msg_loc_sync_pkg_optimize_head optimizeHead;
        // 设置序号 以及前置包序号(关键包之间不会乱序 不需要设置前置包)
        optimizeHead.wPkgSeq = m_wSendPkgSeq; 
        optimizeHead.wPrecondPkgSeq = m_wSendPkgSeq; 

        // 设置当前关键包
        m_wSendKeyPkgSeq = m_wSendPkgSeq;
        ++m_wSendPkgSeq;

        data << optimizeHead; 
    }

    // 收到这个消息后，客户端只做瞬移就可以了
    msg_creature_transport_sync msg;
    msg.nSceneSrc = nSceneID;
    msg.ptSrc = move.vPosition;
    msg.nSceneDst = nSceneID;
    msg.nMapDst = nMapID;
    msg.ptDes = move;

    data << msg;

    if(!bBroadcast)
    {
        ((__IPlayerRole*)m_pMaster)->send2Client(data.data(), data.size());
    }
    else
    {
        __IEntityAOI* pAOIPart = (__IEntityAOI*)m_pMaster->getEntityPart(PART_AOI);
        if(pAOIPart != NULL)
        {
            SceneServiceHelper sceneHelper(nSceneID);
            if(sceneHelper.m_ptr)
                sceneHelper.m_ptr->broadcast(move.vPosition, 0, BROADCAST_MOVE, string(data.data(), data.size()), pAOIPart->getAOIProxy());
        }
    }

    m_bEnterStrictMoveCheck = true;
    m_dwStrictCheckTimeOut = getTickCountEx() + 400; 

    if(m_pMaster->getState() == ENTITY_STATE_DIE)
    {
        ErrorLn(__FUNCTION__ << ",actor=" << m_pMaster->getName());
    }

    TraceLn("crorrect client=" << m_pMaster->getName() << " to loc(" << move.vPosition.x << "," << move.vPosition.y << "," << move.vPosition.z << "), mapid=" << m_pMaster->getMapID());
}

// 摔死
void CMovePart::fall2Death()
{
    UID uid = m_pMaster->getUID();
    msg_entity_damage damage;
    damage.uidOperator = uid;
    damage.nDamageType = DAMAGE_TYPE_EFFECT;
    damage.nDeadType = DeadEffect_Normal;
    damage.nDamageHP = m_pMaster->getIntProperty(PROPERTY_MAX_HP) * 1.5;

    // 通知客户端
    obuf256 obufData;
    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = DAMAGE_MSG_DAMAGE;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = uid;				// 实体UID
    entityHead.byPartID = PART_DAMAGE;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

    obufData << head << entityHead;
    obufData.push_back(&damage,sizeof(damage));

    // 发送消息
    handleMessage(uid, obufData.data(),obufData.size());
}

// 丢失位置后 在客户端上报最新位置之前 不能移动
void CMovePart::lostLocation()
{
    __IEntityAOI* pEntityAOIPart = (__IEntityAOI*)m_pMaster->getEntityPart(PART_AOI);
    if(pEntityAOIPart == NULL)
    {
        return;
    }

    int nSceneID = m_pMaster->getSceneID();
    SceneServiceHelper helper(nSceneID);
    if(helper.m_ptr == NULL)
    {
        ErrorLn(__FUNCTION__ << __LINE__ << ", nSceneID=" << nSceneID);
        return;
    }

    // 先停止当前移动
    executeStopmoving();

    // 标记位置丢失
    m_dwLastLostLocationTick = getTickCountEx();

    // 递增当前序列号
    ++m_uLostLocationSerialNo;

    // 广播稍后客户端重新上报位置
    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = SC_MSG_REPORT_ENTITY_LOC_LATER;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
    entityHead.byPartID = PART_MOVE;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

    msg_request_report_loc_later msg;
    msg.uSerial = m_uLostLocationSerialNo;

    obuf256 data;
    data << head << entityHead << msg;
    helper.m_ptr->broadcast(getMoveState().vPosition,0,BROADCAST_MOVE,string(data.data(),data.size()), pEntityAOIPart->getAOIProxy(), false);
}

// 是否丢失位置（动作决定位移_假人）
bool CMovePart::isLostLocation()
{
    return getTickCountEx() < m_dwLastLostLocationTick + 1000 * 3;
}

void CMovePart::release()
{
    if ( m_pMaster )
    {
        __IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
        if (pEntityEvent)
        {
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CREATE);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (ENTITYEVENT_KEY)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_SD));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DIE);
            pEntityEvent->UnSubscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PREPARE_CAST_SPELL);
			if (m_pMaster->getType() == TYPE_PLAYER_ROLE)
				pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CHANGE_STATE);
        }

        SceneLogicEventHelper helper(m_pMaster->getSceneID());
        if(helper.m_ptr)
        {
            helper.m_ptr->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_OBSTACLE_CREATE, TYPE_MONSTER, 0);
            helper.m_ptr->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_OBSTACLE_DESTROY, TYPE_MONSTER, 0);
        }

        if((m_pMaster->getType() == TYPE_PLAYER_ROLE || !isInvalidUID(m_pMaster->getIntProperty(PROPERTY_MASTER_UID))) && helper.m_ptr)
        {
            helper.m_ptr->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_BLOCK_BARRIER_CREATE, SOURCE_TYPE_SCENE, 0);
            helper.m_ptr->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_BLOCK_BARRIER_DESTROY, SOURCE_TYPE_SCENE, 0);
        }
    }
    m_pMaster = 0;

    // 停止timer
    gServerGlobal->getTimerService()->KillTimer(MOVE_UPDATE_TIMER, this);
	gServerGlobal->getTimerService()->KillTimer(MOVE_ACCELERATE_TIMER, this);

    for(int i = 0; i < BEHAVIOUR_TYPE_MAX; ++i)
    {
        if(m_Behaviours[i])
        {
            m_Behaviours[i]->release();
            m_Behaviours[i] = 0;
        }
    }

    for(std::list<Barrier*>::iterator it = m_listBarrier.begin(); it != m_listBarrier.end(); ++it)
    {
        (*it)->release();
    }
    m_listBarrier.clear();

    delete this;
}

// 冲锋遇到地形障碍
void CMovePart::shiftHitTerrainAddBuff()
{
    if(m_eBehaviour != BEHAVIOUR_TYPE_SHIFT)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    Behaviour* pBehaviour = m_Behaviours[m_eBehaviour];
    if(!pBehaviour)
    {
        WarningLn(__FUNCTION__ << ", current move state is null");
        return;
    }

    ShiftCtx& shiftCtx = (ShiftCtx&)pBehaviour->getCtx(); 
    __IBuffPart* pBuffPart = NULL;
    if(shiftCtx.nHitObstacleAddBuffID != 0 && (pBuffPart = (__IBuffPart*)m_pMaster->getEntityPart(PART_BUFF)) != NULL)
    {
        pBuffPart->Add(shiftCtx.nHitObstacleAddBuffID, shiftCtx.nHitObstacleAddBuffLevel, m_pMaster->getUID());
    }
}

// 冲锋完加buff
void CMovePart::shiftFinishAddBuff()
{
    if(m_eBehaviour != BEHAVIOUR_TYPE_SHIFT)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    Behaviour* pBehaviour = m_Behaviours[m_eBehaviour];
    if(!pBehaviour)
    {
        WarningLn(__FUNCTION__ << ", current move state is null");
        return;
    }

    ShiftCtx& shiftCtx = (ShiftCtx&)pBehaviour->getCtx(); 
    if(shiftCtx.nFinishBuff_ID > 0)
    {
        AddBuff(shiftCtx.uidFinishBuff_Target, shiftCtx.nFinishBuff_ID, shiftCtx.nFinishBuff_Level, shiftCtx.uidFinishBuff_Operator);
    }
}

void CMovePart::restore()
{

}

////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __IMovePart * CreateMovePart()
{
    return new CMovePart;
}