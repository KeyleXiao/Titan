/*******************************************************************
** 文件名:	IMovePart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	9/9/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "EventDef.h"
#include "IClientGlobal.h"
#include "buffer.h"
#include "IRenderView.h"
#include "NetMessageDef.h"
#include "IClientEntity.h"
#include "TimeSyncClient.h"
#include "MovePart.h"
#include "PlayerRole.h"
#include "IEntityProperty.h"
#include "IAOIPart.h"
#include "MovePath.h"
#include "IPathFindService.h"
#include "IPathFindManager.h"
#include "GameViewDef.h"
#include "ISchemeCenter.h"

//////////////////////////////////////////////////////////////////////////
MovePart::MovePart()
    : m_pMaster(0)
    , m_nSession(0)
    , m_dwLastSynPos2ViewTick(0)
    , m_uReportLocSerial(0)
	, m_mode(0)
    , m_bRobotMode(false)
    , m_nCreatureState(0)
    , m_wSendPkgSeq(0)
    , m_wSendKeyPkgSeq(INVLAID_PKG_SEQ)
    , m_wRcvPkgSeq(INVLAID_PKG_SEQ)
{
    m_eBehaviourType = BEHAVIOUR_TYPE_NONE;
    m_Behaviours[BEHAVIOUR_TYPE_NONE] = 0;
    m_Behaviours[BEHAVIOUR_TYPE_WALK] = WalkBehaviour::create();
    m_Behaviours[BEHAVIOUR_TYPE_JUMP] = JumpBehaviour::create();
    m_Behaviours[BEHAVIOUR_TYPE_SHIFT] = ShiftBehaviour::create();
}

bool MovePart::onLoad( IEntity* pEntity)
{
	m_pMaster = pEntity;

    UID uid = m_pMaster->getUID();
    gClientGlobal->getEventEngine()->Subscibe(this, EVENT_ENTITY_ENTER_VIEWPORT, UID_2_TYPE(uid), uid, __FILE__);
    gClientGlobal->getEventEngine()->Subscibe(this, EVENT_ENTITY_LEAVE_VIEWPORT, UID_2_TYPE(uid), uid, __FILE__);
    gClientGlobal->getEventEngine()->Subscibe(this, EVENT_ENTITY_DIE, UID_2_TYPE(uid), uid, __FILE__);

	return true;
}

void MovePart::release()
{
    gClientGlobal->getTimerAxis()->KillTimer(this);
    if(m_pMaster)
    {
        UID uid = m_pMaster->getUID();
        IEventEngine * pEventEngine = gClientGlobal->getEventEngine();
        if(pEventEngine)
        {
            BYTE type = UID_2_TYPE(uid);
            pEventEngine->UnSubscibe(this, EVENT_ENTITY_ENTER_VIEWPORT, type, uid);
            pEventEngine->UnSubscibe(this, EVENT_ENTITY_LEAVE_VIEWPORT, type, uid);
            pEventEngine->UnSubscibe(this, EVENT_ENTITY_DIE, type, uid);
        }

    }

    for(int i = 0; i < BEHAVIOUR_TYPE_MAX; ++i)
    {
        if(m_Behaviours[i])
        {
            m_Behaviours[i]->release();
            m_Behaviours[i] = 0;
        }
    }

    delete this;
}

bool MovePart::deSerialize( rkt::obuf & out,int flag )
{
    return true;
}

bool MovePart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{     
    DWORD dwTick = 0;
    int nPathPointsCount = 0;

    Move3 move;
    in >> move >> m_nCreatureState >> m_eBehaviourType;

    if(m_Behaviours[m_eBehaviourType] != NULL)
    {
        in >> m_Behaviours[m_eBehaviourType]->getCtx();
    }

    updateMoveState(move, false);

	return true;
}

void MovePart::onSyncPostion( DWORD dwTickcount, const Move3 & move )
{
    updateMoveState(const_cast<Move3&>(move));
}

void MovePart::onForceMove( const cmd_force_move & move )
{
    if ( !isCreature(m_pMaster) )
    {
        return;
    }

    m_pMaster->sendCommandToEntityView( ENTITY_TOVIEW_CHARGE, 0, 0, (void*)&move, sizeof(move) );
}

// 向目标移动
void MovePart::onMovePos(const cmd_creature_move_pos &movePos)
{
	if (m_pMaster == NULL)
	{
		return;
	}
	
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_MOVE_POS, 0, 0, (void*)&movePos, sizeof(movePos));
}

Move3 MovePart::getMoveContext()
{
    Move3 move;
    if(m_pMaster == NULL)
        return move;

    IEntityProperty* pProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
    if(pProperty == NULL)
        return move;
    
    void* data = NULL;
    int nLen = 0;
    pProperty->getProperty_custom(PROPERTY_LOCATION, data, nLen);

    if(nLen != sizeof(move.vPosition))
    {
        ErrorLn(__FILE__ << __LINE__);
        return move;
    }
    memcpy(&move.vPosition, data, nLen);

    nLen = 0;
    pProperty->getProperty_custom(PROPERTY_ANGLE, data, nLen);

    if(nLen != sizeof(move.vRotation))
    {
        ErrorLn(__FILE__ << __LINE__);
        return move;
    }
    memcpy(&move.vRotation, data, nLen);

    nLen = 0;
    pProperty->getProperty_custom(PROPERTY_VELOCITY, data, nLen);

    if(nLen != sizeof(move.vVelocity))
    {
        ErrorLn(__FILE__ << __LINE__);
        return move;
    }
    memcpy(&move.vVelocity, data, nLen);

    return move;
}

bool MovePart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch( msgId )
	{
	case SC_MSG_ENTITY_MOVE:						// 服务端通告实体移动
		{
            // 实体移动
            if(data != NULL && len >= sizeof(msg_creature_move_sync))
            {
               // 发给显示层进行移动 todo.
               onMsgMove(data, len);
            }
		}
		break;

	case SC_MSG_ENTITY_TRANSPORT:
		{
			if ( data==0 || len<sizeof(msg_creature_transport_sync) )
			{
				ErrorLn("SC_MSG_ENTITY_THANSPORT data error!len="<<len);
			}
			else

			{
				onMsgTransport(data, len);
			}
		}
		break;

	case SC_MSG_ENTITY_CTRL:        // 服务器广播非主角控制(动作、技能、etc)
		{
			if ( data==0 || len!=sizeof(msg_creature_act_sync) )
			{
				ErrorLn("SC_MSG_ENTITY_CTRL data error!len="<<len);
			}
			else
			{
				onMsgCtrl(*(msg_creature_act_sync*)data);
			}
		}
		break;
    case SC_MSG_REPORT_ENTITY_LOC_LATER:
        {
            if ( data==0 || len!=sizeof(msg_request_report_loc_later) )
            {
                ErrorLn("SC_MSG_REPORT_ENTITY_LOC_LATER data error!len="<<len);
            }
            else
            {
                onMsgReportLocLater(*(msg_request_report_loc_later*)data);
            }
        }
        break;
    case SC_MSG_FORBIT_MOVE_INITIATIVELY:
        {
            if ( data==0 || len!=sizeof(bool) )
            {
                ErrorLn("SC_MSG_FORBIT_MOVE_INITIATIVELY data error!len="<<len);
                return false;
            }

            m_bRobotMode = *((bool*)data);
            gameview_set_robot_mode cmd;
            if(m_bRobotMode)
            {
                cmd.nEnable = 1;
            }
            gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_SET_ROBOT_MODE,0,0,&cmd,sizeof(cmd) );
        }
        break;
	default:
		return false;
	}

	return true;
}


bool MovePart::onCommand( int cmdid,void * data,size_t len )
{
	switch( cmdid )
	{
	case ENTITY_CMD_SET_POS:						// 显示层发给逻辑层设定人物行走
		{
			if ( data==0 || len!=sizeof(cmd_creature_rigidbody_sync) )
			{
				ErrorLn("ENTITY_CMD_POSITION_SYNC data error!len="<<len);
			}
			else
			{
				onCmdMove(*(cmd_creature_rigidbody_sync*)data);
			}
			return true;
		}

	case ENTITY_CMD_SET_CTRL:						// 显示层发给逻辑层人物发出操作指令
		{
			if ( data==0 || len!=sizeof(cmd_creature_act_sync) )
			{
				ErrorLn("ENTITY_CMD_SET_CTRL data error!len="<<len);
			}
			else
			{
				onCmdCtrl(*(cmd_creature_act_sync*)data);
			}
			return true;
		}
		
	case ENTITY_CMD_LANDING_SPEED:		// 实体下落
		{
			if ( data==0 || len!=sizeof(cmd_creature_act_landspeed) )
			{
				ErrorLn("ENTITY_CMD_LANDING_SPEED data error!len="<<len);
			}
			else
			{
				onCmdLand(*(cmd_creature_act_landspeed*)data);
			}
            return true;
		}

    case ENTITY_CMD_SET_FORMTYPE:
        {
            if ( data==0 || len!=sizeof(cmd_creature_act_set_formtype) )
            {
                ErrorLn("ENTITY_CMD_SET_FORMTYPE data error!len="<<len);
            }
            else
            {
                onCmdSetFormType(*(cmd_creature_act_set_formtype*)data);
            }
		    return true;
        }
        
    case ENTITY_CMD_GUIDE_ROTATECAMERA:
        {
			int nCameIndex = len;
			onCmdTurnCamera(nCameIndex);
            return true;
        }

	default:
		return false;
	}
}

// 切换操作模式(假人，影响假人后退攻击时朝向)  
void MovePart::setOperateMode(int nMode, Vector3& vTarget)
{
    m_mode = nMode;
    m_target = vTarget;
}

void MovePart::setRobotMode(bool bSet)
{
    m_bRobotMode = bSet;
}

/** 显示层发出操纵玩家行走的指令
@name				: 
@param				: 位置,角度,速度等信息
@return
*/
void MovePart::onCmdMove(cmd_creature_rigidbody_sync& sync_pos)
{
    IEntityProperty* pProperty = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
    if (!m_pMaster->isHero() && 
        !(isActor(m_pMaster) && pProperty != NULL && pProperty->getProperty_Integer(PROPERTY_AI_SCHEME_ID) > 0))
    {
        ErrorLn("This entity is not hero or ai, but send move message to server!");
        return;
    }

    Move3 move;
    move.vPosition.x = sync_pos.fPosition_x;
    move.vPosition.y = sync_pos.fPosition_y;
    move.vPosition.z = sync_pos.fPosition_z;
    move.vRotation.x = sync_pos.fRotation_x;
    move.vRotation.y = sync_pos.fRotation_y;
    move.vRotation.z = sync_pos.fRotation_z;
    move.vVelocity.x = sync_pos.fVelocity_x;
    move.vVelocity.y = sync_pos.fVelocity_y;
    move.vVelocity.z = sync_pos.fVelocity_z; 

    // 只更新逻辑层
    updateMoveState(move, false);

    if(isActor(m_pMaster) && pProperty->getProperty_Integer(PROPERTY_AI_SCHEME_ID) > 0)
    {
        if(sync_pos.nSyn2Server != 0)
        {
            reportAILoc();
        }
        sync_pos.nSyn2Server = 0;
    }

    if(sync_pos.nSyn2Server !=0 )
    {
        TRACE_AOI("send sync message:x="<<move_data.move.vPosition.x<<",y="<<move_data.move.vPosition.y<<",z="<<move_data.move.vPosition.z);

        SNetMsgHead head;
        head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
        head.byDestEndPoint = MSG_ENDPOINT_SCENE;
        head.byKeyModule    = MSG_MODULEID_ENTITY;
        head.byKeyAction    = CS_MSG_CREATURE_SYNC;

        SMsgEntityHead entityHead;
        entityHead.uidMaster = m_pMaster->getUID();
        entityHead.byPartID = PART_MOVE;

        obuf256 oDataPack;
        oDataPack << head << entityHead;

        bool bIsKeyPkg = false;
        ISchemeEntity* pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
        bool bOpenOptimize = (pSchemeEntity->getEntityXml()->nOpenMovePkgOptimize != 0); 

        // 压入是否开启优化字段
        BYTE byOpenOptimize = bOpenOptimize ? 1 : 0;
        oDataPack << byOpenOptimize;

        if(bOpenOptimize) // 客户端开启了移动包优化
        {
            msg_loc_sync_pkg_optimize_head optimizeHead;

            if(m_wSendPkgSeq == INVLAID_PKG_SEQ)
            {
                m_wSendPkgSeq++;
            }

            if(sync_pos.nSyn2Server != MoveSyncFlag::TO_BROADCAST)
            {
                if(m_wSendPkgSeq - m_wSendKeyPkgSeq > 10)
                {
                    // 10个连续的非关键包？ 必须要插一个关键包
                    // 关键包 设置序号 (关键包之间不会乱序 不需要设置前置包)
                    optimizeHead.wPkgSeq = m_wSendPkgSeq;
                    optimizeHead.wPrecondPkgSeq = m_wSendPkgSeq; 

                    // 设置当前关键包
                    m_wSendKeyPkgSeq = m_wSendPkgSeq;
                    ++m_wSendPkgSeq;
                    bIsKeyPkg = true;
                }
                else
                {
                    // 非关键包 设置序号 以及前置包序号
                    optimizeHead.wPkgSeq = m_wSendPkgSeq++;  
                    optimizeHead.wPrecondPkgSeq = m_wSendKeyPkgSeq;
                    bIsKeyPkg = false;
                } 
            }
            else
            {
                // 关键包 设置序号 (关键包之间不会乱序 不需要设置前置包)
                optimizeHead.wPkgSeq = m_wSendPkgSeq;
                optimizeHead.wPrecondPkgSeq = m_wSendPkgSeq; 

                // 设置当前关键包
                m_wSendKeyPkgSeq = m_wSendPkgSeq;
                ++m_wSendPkgSeq;

                bIsKeyPkg = true;
            }

            oDataPack << optimizeHead; 
        }

        msg_creature_move_sync move_data;
        move_data.nCreaturState = sync_pos.nCreatureState;
        move_data.dwTick = gClientGlobal->getTimeSyncer()->GetTick();
        move_data.nPosX = sync_pos.fPosition_x * 10;
        move_data.nPosY = sync_pos.fPosition_y * 10;
        move_data.nPosZ = sync_pos.fPosition_z * 10;
        move_data.nRotX = sync_pos.fRotation_x * 10;
        move_data.nRotY = sync_pos.fRotation_y * 10;
        move_data.nRotZ = sync_pos.fRotation_z * 10;
        move_data.nVelocityX = sync_pos.fVelocity_x * 10;
        move_data.nVelocityY = sync_pos.fVelocity_y * 10;
        move_data.nVelocityZ = sync_pos.fVelocity_z * 10;
        move_data.direction = (int)sync_pos.nDirection; /*== TO_BROADCAST ? 1 : 0;*/
        move_data.nBehaviour = BEHAVIOUR_TYPE_WALK;

        obuf data;
        data << move_data;

        oDataPack.push_back(data.data(),data.size());

        if(bOpenOptimize)
        {
            if(bIsKeyPkg)
            {
                // 启动停止包不能丢
                net_senddata(oDataPack.data(), oDataPack.size());
            }
            else
            {
                net_senddata_flag(oDataPack.data(), oDataPack.size(), NO_ACK);
            }
        }
        else
        {
            net_senddata(oDataPack.data(), oDataPack.size());
        }
    }
}

void MovePart::onCmdCtrl(cmd_creature_act_sync& data)
{
    if(!m_pMaster->isHero())
    {
        return;
    }

	msg_creature_act_sync msg;
	msg.move.vPosition.x = data.pos.fPosition_x;    // 角色位置
	msg.move.vPosition.y = data.pos.fPosition_y;    // 角色位置
	msg.move.vPosition.z = data.pos.fPosition_z;    // 角色位置
	msg.move.vRotation.x = data.pos.fRotation_x;    // 角色朝向绕x轴旋转的角度
	msg.move.vRotation.y = data.pos.fRotation_y;    // 角色朝向绕y轴旋转的角度
	msg.move.vRotation.z = data.pos.fRotation_z;    // 角色朝向绕z轴旋转的角度
	msg.move.vVelocity.x = data.pos.fVelocity_x;    // 角色速度
	msg.move.vVelocity.y = data.pos.fVelocity_y;    // 角色速度
	msg.move.vVelocity.z = data.pos.fVelocity_z;    // 角色速度

    if(_isnan(msg.move.vPosition.x) || _isnan(msg.move.vPosition.z))
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        __debugbreak();
        return;
    }

    updateMoveState(msg.move, false);

	msg.state.nPostureID = data.state.nPostureID;   // 当前的状态
	msg.state.bIsMoving = data.state.bIsMoving;     // 是否运动中 (C#的bool是4个字节)
	msg.state.fForward = data.state.fForward;       // 向前状态控制
	msg.state.fLeft = data.state.fLeft;             // 方向
	msg.state.bJump = data.state.bJump;             // 是否正在跳跃 (C#的bool是4个字节)
	msg.state.bFall = data.state.bFall;             // 是否正在下落 (C#的bool是4个字节)

	msg.fCtrParam.x = data.fCtrParam_x;             // 操作参数,比如技能方向
	msg.fCtrParam.y = data.fCtrParam_y;             // 操作参数,比如技能方向
	msg.fCtrParam.z = data.fCtrParam_z;             // 操作参数,比如技能方向
	msg.nCtrID = data.nCtrID;                       // 操作码 ICreaturePostureState.cs::ActID
	msg.nCtrlParam = data.nCtrParam;                // 操作参数
	msg.nCtrlParam2 = data.nCtrParam2;              // 操作参数2

	SendMessageToEntityScene(0, PART_MOVE, CS_MSG_CREATURE_CTRL,  (char*)&msg, sizeof(msg));
}

void MovePart::onCmdLand(cmd_creature_act_landspeed& data)
{
    // 非主角不让发这个命令到服务器
    if (!m_pMaster || !m_pMaster->isHero())
    {
        return;
    }

	msg_creature_act_land sendData;
	sendData.fValue = data.fValue;

	SendMessageToEntityScene(0, PART_MOVE, CS_MSG_CREATURE_LAND, &sendData, sizeof(sendData));
}

void MovePart::onCmdSetFormType(cmd_creature_act_set_formtype& data)
{
    // 非主角不让发这个命令到服务器
    if (!m_pMaster || !m_pMaster->isHero())
    {
        return;
    }

    msg_creature_act_set_formtype sendData;
    sendData.nValue = data.nValue;

    SendMessageToEntityScene(0, PART_MOVE, CS_MSG_ENTITY_SET_FORMTYPE, &sendData, sizeof(sendData));
}

// 触发转镜头
void MovePart::onCmdTurnCamera(int nCameIndex)
{
    // 非主角不让发这个命令到服务器
    if (!m_pMaster || !m_pMaster->isHero())
    {
        return;
    }
    if (nCameIndex < 0 || nCameIndex >= CAMERA_BUFF_COUNT)
    {
        ErrorLn("Turn Camera parameter is invalid, param=" << nCameIndex);
        return;
    }

    SendMessageToEntityScene(0, PART_MOVE, CS_MSG_ENTITY_TURN_CAMERA, &nCameIndex, sizeof(nCameIndex));
}

// 处理服务器移动包
void MovePart::onMsgMove(void* data, int len)
{    
    DWORD dwCurTick = gClientGlobal->getTimeSyncer()->GetTick();

    // 是否开启优化字段
    BYTE byOpenOptimize = *((BYTE*)data);
    data = (BYTE*)data + 1;
    len -= 1;

    msg_loc_sync_pkg_optimize_head* pOptimizeHead = NULL;
    msg_creature_move_sync* pMsg = NULL;
    if(byOpenOptimize)
    {
        if(len < sizeof(msg_loc_sync_pkg_optimize_head) + sizeof(msg_creature_move_sync))
        {
            ErrorLn(__FUNCTION__ << " 1, msg format error, len=" << len);
            return;
        }

        pOptimizeHead = (msg_loc_sync_pkg_optimize_head*)data;
        pMsg = (msg_creature_move_sync*)(pOptimizeHead + 1); 
    }
    else
    {
        if(len < sizeof(msg_creature_move_sync))
        {
            ErrorLn(__FUNCTION__ << " 2, msg format error, len=" << len);
            return;
        }
        pMsg = (msg_creature_move_sync*)data;
    }

    /* 位置相关的同步 统一加序号，并且有两种包：一种是不能丢弃的关键包，一种是可丢弃的普通包。
       PS:不加序号的话 这两种包可能乱序到达 这是不允许的
       加了序号后，可以识别包是否过期，但是不应该允许关键包过期的情况出现
       避免关键包过期：普通包中都有前置关键包,收到时检查前置关键包还没收到的话，直接丢弃这个普通包。
    */
    if(pOptimizeHead != NULL) // 服务器开启了移动包优化
    {
        if(isDropPkg(pOptimizeHead->wPkgSeq, pOptimizeHead->wPrecondPkgSeq))
        {
            return;
        }

        // 更新当前的序列号
        m_wRcvPkgSeq = pOptimizeHead->wPkgSeq; 
    }

    if(pMsg->nBehaviour >= BEHAVIOUR_TYPE_MAX)
    {
        ErrorLn("move type error," << pMsg->nBehaviour);
        return;
    }

    Behaviour* pBehaviour = m_Behaviours[pMsg->nBehaviour];
    if(pBehaviour != NULL)
    {
        rkt::ibuffer move_data( pMsg+1,len-sizeof(msg_creature_move_sync) );
        move_data >> pBehaviour->getCtx();
    }

    Move3 move;
    move.vPosition.x = (float)pMsg->nPosX / (float)10;
    move.vPosition.y = (float)pMsg->nPosY / (float)10;
    move.vPosition.z = (float)pMsg->nPosZ / (float)10;
    move.vRotation.x = (float)pMsg->nRotX / (float)10;
    move.vRotation.y = (float)pMsg->nRotY / (float)10;
    move.vRotation.z = (float)pMsg->nRotZ / (float)10;
    move.vVelocity.x = (float)pMsg->nVelocityX / (float)10;
    move.vVelocity.y = (float)pMsg->nVelocityY / (float)10;
    move.vVelocity.z = (float)pMsg->nVelocityZ / (float)10;

    m_nCreatureState = pMsg->nCreaturState;
    updateMoveState(move);
    gotoBehaviour((BEHAVIOURTYPE)pMsg->nBehaviour);
}

void MovePart::onMsgCtrl(msg_creature_act_sync& data)
{
	if(m_pMaster->isHero() && data.nCtrID != Jump)
	{
		return;
	}

	cmd_creature_act_sync cmd /*= {0}*/;
	cmd.pos.fPosition_x = data.move.vPosition.x;
	cmd.pos.fPosition_y = data.move.vPosition.y;
	cmd.pos.fPosition_z = data.move.vPosition.z;
	cmd.pos.fRotation_x = data.move.vRotation.x;
	cmd.pos.fRotation_y = data.move.vRotation.y;
	cmd.pos.fRotation_z = data.move.vRotation.z;
	cmd.pos.fVelocity_x = data.move.vVelocity.x;
	cmd.pos.fVelocity_y = data.move.vVelocity.y;
	cmd.pos.fVelocity_z = data.move.vVelocity.z;

	cmd.state.nPostureID = data.state.nPostureID;
	cmd.state.bIsMoving = data.state.bIsMoving;
	cmd.state.fForward = data.state.fForward;
	cmd.state.fLeft = data.state.fLeft;
	cmd.state.bJump = data.state.bJump;
	cmd.state.bFall = data.state.bFall;

	cmd.fCtrParam_x = data.fCtrParam.x;
	cmd.fCtrParam_y = data.fCtrParam.y;
	cmd.fCtrParam_z = data.fCtrParam.z;
	cmd.nCtrID = data.nCtrID;
	cmd.nCtrParam = data.nCtrlParam;
	cmd.nCtrParam2 = data.nCtrlParam2;

	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SYNC_CTRL, sizeof(cmd), 0, &cmd, sizeof(cmd));
}

void MovePart::onMsgTransport(void* data, int len)
{
    msg_loc_sync_pkg_optimize_head* pOptimizeHead = NULL;
    msg_creature_transport_sync* pMsg = NULL;
    if(len == sizeof(msg_creature_transport_sync))
    {
        pMsg = (msg_creature_transport_sync*)data;
    }
    else if(len == sizeof(msg_loc_sync_pkg_optimize_head) + sizeof(msg_creature_transport_sync))
    {
        pOptimizeHead = (msg_loc_sync_pkg_optimize_head*)data;
        pMsg = (msg_creature_transport_sync*)(pOptimizeHead + 1);
    }
    else
    {
        ErrorLn(__FUNCTION__ << __LINE__ << ", len=" << len);
        return;
    }
    
    /* 位置相关的同步 统一加序号，并且有两种包：一种是不能丢弃的关键包，一种是可丢弃的普通包。
       PS:不加序号的话 这两种包可能乱序到达 这是不允许的
       加了序号后，可以识别包是否过期，但是不应该允许关键包过期的情况出现
       避免关键包过期：普通包中都有前置关键包,收到时检查前置关键包还没收到的话，直接丢弃这个普通包。
    */
    if(pOptimizeHead != NULL) // 服务器开启了移动包优化
    {
        if(isDropPkg(pOptimizeHead->wPkgSeq, pOptimizeHead->wPrecondPkgSeq))
        {
            return;
        }

        // 更新当前的序列号
        m_wRcvPkgSeq = pOptimizeHead->wPkgSeq; 
    }

    gotoBehaviour(BEHAVIOUR_TYPE_NONE);
    updateMoveState(pMsg->ptDes);

	cmd_creature_transport fs;
	fs.nDestMapID = pMsg->nMapDst; 
	fs.nSrcMapID = pMsg->nSceneSrc;
	fs.fPosition_x = pMsg->ptDes.vPosition.x;
	fs.fPosition_y = pMsg->ptDes.vPosition.y;
	fs.fPosition_z = pMsg->ptDes.vPosition.z;
	fs.fRotation_x = pMsg->ptDes.vRotation.x;
	fs.fRotation_y = pMsg->ptDes.vRotation.y;
	fs.fRotation_z = pMsg->ptDes.vRotation.z;
    fs.nReason = pMsg->nReason;
    fs.bUseAngle = 1;
    if(pMsg->nReason == TRANSPORT_REASON_FLASH)
    {
        fs.bUseAngle = 0;
    }

	m_pMaster->sendCommandToEntityView( ENTITY_TOVIEW_SYNC_TRANSPORT,sizeof(fs),0,&fs, sizeof(fs));
}

void MovePart::onMsgReportLocLater(msg_request_report_loc_later& data)
{
    m_uReportLocSerial = data.uSerial;
}

void MovePart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
    case EVENT_ENTITY_ENTER_VIEWPORT:
        {
            if(m_Behaviours[m_eBehaviourType] != NULL)
            {
                BEHAVIOURTYPE eType = m_eBehaviourType; 
                m_eBehaviourType = BEHAVIOUR_TYPE_NONE;
                gotoBehaviour(eType);
                update();
            }
        }
        break;
    case EVENT_ENTITY_LEAVE_VIEWPORT:
        {
            gotoBehaviour(BEHAVIOUR_TYPE_NONE);
        }
        break;
    case EVENT_ENTITY_DIE:
        {
            gotoBehaviour(BEHAVIOUR_TYPE_NONE);
        }
        break;
    }
}

void MovePart::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case Timer_Move_Update:
        {
            update();
        }
        break;
    }
}

void MovePart::reportAILoc()
{
    IEntityProperty* pProperty = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
    if(!(isActor(m_pMaster) && pProperty != NULL && pProperty->getProperty_Integer(PROPERTY_AI_SCHEME_ID) > 0))
    {
        ErrorLn("This entity is not ai, but send move message to server!");
        return;
    }

    msg_report_ai_loc msg;
    msg.uSerial = m_uReportLocSerial;
    msg.moveState = getMoveContext();
    SendMessageToEntityScene(m_pMaster->getUID(), PART_MOVE, CS_MSG_REPORT_AI_LOC, (char*)&msg, sizeof(msg));
}

#define INSIGHT_UPDATE_INTERVAL 32
#define OUTSIGHT_UPDATE_INTERVAL 200

void MovePart::update()
{
    Behaviour* pBehaviour = m_Behaviours[m_eBehaviourType];
    if(!pBehaviour)
    {
        return;
    }

    // 追踪目标冲锋的话 要取目标当前的位置
    Vector3* pTargetLoc = NULL;
    Vector3 targetLoc;
    if(m_eBehaviourType == BEHAVIOUR_TYPE_SHIFT)
    {
        ShiftCtx& ctx = (ShiftCtx&)pBehaviour->getCtx();
        if(ctx.shiftMode == shift_mode_forward_entity)
        {
            IClientEntity* pEntity = gClientGlobal->getEntityFactory()->getEntity(ctx.uidTarget);
            if(pEntity != NULL)
            {
                targetLoc = pEntity->getLocation();
                pTargetLoc = &targetLoc;
            }
        }
    }

    // 更新行为
    DWORD dwCurTick = gClientGlobal->getTimeSyncer()->GetTick();
    pBehaviour->update(dwCurTick, pTargetLoc);

    // 当前移动状态
    Move3 currentMove = pBehaviour->getCtx().move; 
    if(pBehaviour->getCtx().bKeepLookDir)
    {
        currentMove.vRotation = getMoveContext().vRotation;
    }

    // 更新当前移动状态
    updateMoveState(currentMove);

    // 行为自然结束了
    if(pBehaviour->getCtx().eState != BEHAVIOUR_STATE_RUNNING)
    {
        gotoBehaviour(BEHAVIOUR_TYPE_NONE);
    }
}

bool MovePart::gotoBehaviour(BEHAVIOURTYPE eType)
{
    Behaviour* pBehaviour = m_Behaviours[eType]; 

    if(pBehaviour != NULL && !pBehaviour->run())
    {
        WarningLn(__FUNCTION__ << __LINE__ << ", failed");
        return false;
    }

    BEHAVIOURTYPE eOld = m_eBehaviourType;
    m_eBehaviourType = eType;

    // 是否需要清理旧的行为
    if(m_eBehaviourType != eOld && m_Behaviours[eOld] != NULL)
    {
        m_Behaviours[eOld]->getCtx().clear();
    }

    // 是否需要启动移动定时器[冲锋是显示层模拟 逻辑层不关心]
    if(m_eBehaviourType != BEHAVIOUR_TYPE_SHIFT && eOld == BEHAVIOUR_TYPE_NONE && m_eBehaviourType != BEHAVIOUR_TYPE_NONE)
    {
        gClientGlobal->getTimerAxis()->SetTimer(Timer_Move_Update, 32, this, INFINITY_CALL, __FUNCTION__);
    }

    // 是否需要停止移动定时器
    if(eOld != BEHAVIOUR_TYPE_SHIFT && eOld != BEHAVIOUR_TYPE_NONE && m_eBehaviourType == BEHAVIOUR_TYPE_NONE)
    {
        gClientGlobal->getTimerAxis()->KillTimer(Timer_Move_Update, this);
    }

    // 如果是主角 并且冲锋状态有改变 发出事件
    if(m_pMaster->isHero() 
        && ((eOld == BEHAVIOUR_TYPE_SHIFT) != (m_eBehaviourType == BEHAVIOUR_TYPE_SHIFT)))
    {
        event_self_shift_state_change event;
        event.bShift = (m_eBehaviourType == BEHAVIOUR_TYPE_SHIFT); 
        gClientGlobal->getEventEngine()->FireExecute(EVENT_SELF_SHIFT_STATE_CHANGE, TYPE_PLAYER_ROLE, m_pMaster->getUID(), (LPCSTR)&event, sizeof(event));
    }

    switch(m_eBehaviourType)
    {
    case BEHAVIOUR_TYPE_WALK:
        {
            // 通知显示层播放走路动画
            if(m_pMaster != NULL)
            {
                cmd_entity_set_animation cmdSetAnimation;
                char* pszAnimationName = "Run";
                memcpy(cmdSetAnimation.szAnimationName, pszAnimationName, strlen(pszAnimationName));
                char* pszPropertyName = "isRun";
                memcpy(cmdSetAnimation.szAnimationBool, pszPropertyName, strlen(pszPropertyName));
                cmdSetAnimation.bEnable = 1;
                m_pMaster->sendCommandToEntityView( ENTITY_TOVIEW_ENTITY_SET_ANIMATION, sizeof(cmdSetAnimation), 0, &cmdSetAnimation, sizeof(cmdSetAnimation));
            }
        }
        break;
    case BEHAVIOUR_TYPE_JUMP:
        {
            // 要通知显示层一个ctrl 命令 要不然客户端表现不正确
            msg_creature_act_sync msg;
            msg.move = pBehaviour->getCtx().move;
            msg.move.vVelocity.y = 0;
            msg.state.bJump = 1;
            msg.nCtrID = 5/*ActID.Jump*/;

            onMsgCtrl(msg);
        }
        break;
    case BEHAVIOUR_TYPE_SHIFT:
        {
            // 组装冲锋消息 通知显示层
            ShiftCtx& ctx = (ShiftCtx&)pBehaviour->getCtx();

            // 距离
            Vector3 spaceLocation = ctx.targetLoc - m_pMaster->getLocation();
            float fSpaceLocation = spaceLocation.getLength();
            // 如果速度为0，默认赋值
            if (equalto0(ctx.fStartSpeed))
            {
                ctx.fStartSpeed = 10.0f;
            }
            float fTime = fSpaceLocation/ctx.fStartSpeed * 1000;
            if (equalto0(fTime))
            {
                // 时间太短改为100毫秒
                fTime = 100;
            }

            cmd_force_move cmd;
            cmd.uidOperator = m_pMaster->getUID();
            cmd.uidTarget = ctx.uidTarget;
            cmd.fPosition_x = ctx.targetLoc.x;
            cmd.fPosition_y = ctx.targetLoc.y;
            cmd.fPosition_z = ctx.targetLoc.z;
            cmd.fSpeed = ctx.fStartSpeed;
            cmd.fTime = fTime;
            cmd.fHeight = ctx.fHeight;
            cmd.nMoveType = (MOVE_TYPE)ctx.nAnimation;
            cmd.nLockTarget = ctx.shiftMode == shift_mode_forward_entity ? 1 : 0;
            cmd.nLockTime = ctx.maxTime;
            cmd.fLocationDistance = ctx.fCloseDistance;
            cmd.nPassive = ctx.bPassive ? 1 : 0;
            cmd.fMaxTime = ctx.maxTime;

            onForceMove(cmd);
        }
        break;
    case BEHAVIOUR_TYPE_NONE:
        {
            if(eOld == BEHAVIOUR_TYPE_WALK)
            {
                // 通知显示层停止走路动画
                if(m_pMaster != NULL)
                {
                    cmd_entity_set_animation cmdSetAnimation;
                    char* pszAnimationName = "Run";
                    memcpy(cmdSetAnimation.szAnimationName, pszAnimationName, strlen(pszAnimationName));
                    char* pszPropertyName = "isRun";
                    memcpy(cmdSetAnimation.szAnimationBool, pszPropertyName, strlen(pszPropertyName));
                    cmdSetAnimation.bEnable = 0;
                    m_pMaster->sendCommandToEntityView( ENTITY_TOVIEW_ENTITY_SET_ANIMATION, sizeof(cmdSetAnimation), 0, &cmdSetAnimation, sizeof(cmdSetAnimation));
                }
            }

            if(eOld == BEHAVIOUR_TYPE_JUMP)
            {
                // 通知显示层停止跳跃动画 todo.
            }
        }
        break;

    default: break;
    }

    return true;
}

void MovePart::updateMoveState(Move3& move, bool bNotifyView)
{
    if(m_pMaster == NULL)
        return;

    IEntityProperty* pProperty = (IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
    if(pProperty == NULL)
        return;

    pProperty->setProperty_custom(PROPERTY_LOCATION, &move.vPosition, sizeof(Vector3));
    pProperty->setProperty_custom(PROPERTY_ANGLE, &move.vRotation, sizeof(Vector3));
    pProperty->setProperty_custom(PROPERTY_VELOCITY, &move.vVelocity, sizeof(Vector3));

    // 更新到显示层
    if(bNotifyView)
    {
        syncMoveState2View(gClientGlobal->getTimeSyncer()->GetTick(), move);
    }
}

/** 同步非主角位置到显示层
*/
void MovePart::syncMoveState2View( DWORD dwTickcount, const Move3& move )
{
    if (m_pMaster->isHero() && !m_bRobotMode)
    {
        return;
    }

    cmd_creature_rigidbody_sync pos/* = {0}*/;
    pos.nCreatureState = m_nCreatureState;
    pos.nDelayTick = 0; //gClientGlobal->getTimeSyncer()->GetTick() - dwTickcount; 看代码角色不同步很有可能是对时系统不同步。暂时排除这个影响。
    pos.fPosition_x = move.vPosition.x;
    pos.fPosition_y = move.vPosition.y;
    pos.fPosition_z = move.vPosition.z;
    pos.fRotation_x = move.vRotation.x;
    pos.fRotation_y = move.vRotation.y;
    pos.fRotation_z = move.vRotation.z;
    pos.fVelocity_x = move.vVelocity.x;
    pos.fVelocity_y = move.vVelocity.y;
    pos.fVelocity_z = move.vVelocity.z;

    m_pMaster->sendCommandToEntityView( ENTITY_TOVIEW_SYNC_POS, sizeof(pos), 0, &pos, sizeof(pos));
}

// 是否丢弃包
bool MovePart::isDropPkg(WORD wPkgSeq, WORD wPkgPrecondSeq)
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

        // 检测前置包条件
        if(wPkgPrecondSeq != INVLAID_PKG_SEQ)
        {
            WORD wSub = m_wRcvPkgSeq - wPkgPrecondSeq; // 与最近收到的包的序列号之差 可以处理回绕1-65535 = 2
            if(wSub > 10000) // 最近10000个包内无前置包<==>前置包没有收到
            {
                return true;
            }
        }

        // 检测是否过期
        WORD wSub = wPkgSeq - m_wRcvPkgSeq; // 与最近收到的包的序列号之差 可以处理回绕1-65535 = 2
        if(wSub > 10000)
        {
            return true;
        }

        return false;
    }
}

void MovePart::restore()
{

}
