#include "stdafx.h"
#include "RoomState_ReadyGo.h"
#include "MatchDef.h"
#include "ITimerService.h"
#include "MatchSchemeDef.h"
#include "NetMessageDef.h"
#include "IClientGlobal.h"
#include "CurrentMatchRoomInfo.h"

void RoomState_ReadyGo::onEnter()
{
	MATCH_TRACELN(__FUNCTION__);
	//发到显示层状态
    cmd_change_roomstate cmdInfo;
    cmdInfo.nStateID = getID();
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_STATE,getID(),0,&cmdInfo, sizeof(cmdInfo));

	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_PLAYER_EXIT,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN,this );
}

void RoomState_ReadyGo::onLeave()
{
	MATCH_TRACELN(__FUNCTION__);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_PLAYER_EXIT);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN);
}

RoomState_ReadyGo::RoomState_ReadyGo( CurrentMatchRoomInfo* pRoom )
	:BaseRoomState(pRoom, MatchRoomState_ReadyGo)
{

}

RoomState_ReadyGo::~RoomState_ReadyGo()
{

}


void RoomState_ReadyGo::onServerMsg(SNetMsgHead* pHead, void* pData, size_t stLen)
{

	switch ( MAKEWORD(pHead->byKeyModule, pHead->byKeyAction) )
	{
	case MAKEWORD(MSG_MODULEID_MATCH, OC_MSG_ALL_CONFIRM_HERO):
		{
			onMsgAllConfirmHero(pHead, pData, stLen);
		}
		break;
	case MAKEWORD(MSG_MODULEID_MATCH, OC_MSG_HERO_SKIN_CHANGE):
		{
			onMsgHeroSkinChange(pHead, pData, stLen);
		}
		break;
    case MAKEWORD(MSG_MODULEID_MATCH, OC_MSG_SYN_ROOM_ALL_XP_SKILLID):
        {
            // 收集房间所玩家XP技能ID 通知给房间玩家
            onMsgSynAllHeroXPSkill(pHead, pData, stLen);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_BROADCAST_ROOM_HERO_WIN_DATA):
        {
            onMsgHeroWinDataInfo(pHead, pData, stLen);
        }
        break;

	default:
		{ 
			BaseRoomState::onServerMsg(pHead, pData, stLen);
		}
	}
}
void RoomState_ReadyGo::onMsgSynAllHeroXPSkill(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(SMsgSynXPSkillHead_OC))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgSynXPSkillHead_OC* pHead = (SMsgSynXPSkillHead_OC*)data;

    if (len != sizeof(SMsgSynXPSkillHead_OC) + pHead->nCount * sizeof(SMsgSynXPSkillInfo_OC))
    {
        ErrorLn(__FUNCTION__":  error len = " << len);
        return;
    }

    SMsgSynXPSkillInfo_OC *pData = (SMsgSynXPSkillInfo_OC *)(pHead +1);
    for (int i = 0; i < pHead->nCount; ++i)
    {
        SRoomRoleBase* pInfo = m_pRoom->getPlayerRoomBaseInfo(pData->dwActorID);
        if (pInfo == NULL )
        {
            continue;
        }

        pInfo->nHeroXPSkillID = pData->nHeroXPSkillID;
        pData++;
    }

}

void RoomState_ReadyGo::onMsgHeroWinDataInfo(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(int))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    int* pCount = (int*)data;
    if (len != sizeof(int) + *pCount * sizeof(SMsg_BroadcastHeroWinData))
    {
        ErrorLn(__FUNCTION__":  error len = " << len);
        return;
    }

    SMsg_BroadcastHeroWinData *pData = (SMsg_BroadcastHeroWinData *)(pCount +1);
    for (int i = 0; i < *pCount; ++i)
    {
        SRoomRoleBase* pInfo = m_pRoom->getPlayerRoomBaseInfo(pData->dwActorID);
        if (pInfo == NULL )
        {
            continue;
        }

        pInfo->fWinRate     = pData->fWinRate;                               // 所选英雄胜率
        pInfo->nMatchNum    = pData->nMatchNum;                              // 所选英雄选择的场次
        pInfo->nRankGrade   = pData->nRankGrade;                             // 当前段位等级
        pData++;
    }
}

void RoomState_ReadyGo::onMsgAllConfirmHero( SNetMsgHead* head, void* data, size_t len )
{
	SSchemeMatchRoom* pSchemeMatchRoom = m_pRoom->getSchemeMatchRoom();
	if (nullptr == pSchemeMatchRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom RoomID= " << m_pRoom->getRoomData()->m_RoomID);
		return;
	}

	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID);
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << m_pRoom->getRoomData()->m_RoomID);
		return;
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_ALL_CONFIRM_HERO, pSchemeMatchRoom->m_ReadyGoTime, nullptr, 0, 0);
}

//社会服发下来消息
void RoomState_ReadyGo::onMsgHeroSkinChange( SNetMsgHead* head, void* data, size_t len )
{
	if (len < sizeof(SMsgHeroSkinChange))
	{
		ErrorLn(__FUNCTION__":  len= " << len);
		return;
	}
	SMsgHeroSkinChange* pMsg = (SMsgHeroSkinChange*)data;

	SRoomRoleBase* pRole = m_pRoom->getRoleByPos( pMsg->Pos );
	if (Invalid_MatchRoomPos == pRole->nIndex)
	{
		ErrorLn(__FUNCTION__": Invalid_MatchRoomPos == pRole->m_Pos Pos= " << pMsg->Pos);
		return;
	}

	if (pRole->dwHeroID != pMsg->dwHeroID)
	{
		return;
	}

	pRole->dwSkinID = pMsg->dwSkinID;
	
}


bool RoomState_ReadyGo::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{

	switch ( eventID )
	{
	case GVIEWCMD_MATCH_ROOM_PLAYER_EXIT:
		{
			onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN:
		{
			onGVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN(nParam, strParam, ptrParam);
		}
		break;
	}
	return true;
}

void RoomState_ReadyGo::onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT(int nParam, const char * strParam, void * ptrParam)
{

	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom m_RoomData.m_RoomID= " << m_pRoom->getRoomData()->m_RoomID);
		return;
	}
	SSchemeMatchRoom* pSchemeMatchRoom = m_pRoom->getSchemeMatchRoom();
	if (nullptr == pSchemeMatchRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom m_RoomData.m_RoomID= " << m_pRoom->getRoomData()->m_RoomID);
		return;
	}
	if ( !pSchemeMatchRoom->m_RoomStateExit[ pRoom->byState ] )
	{
		ErrorLn(__FUNCTION__": !pSchemeMatchRoom->m_RoomStateExit[ pRoom->m_byState ] pRoom->m_byState= " << pRoom->byState);
		return;
	}

	obuf256 ob;
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
	MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_EXIT_ROOM;						// 子消息代码  EMRoomMessageCode

	ob << MsgHead<<outData;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_ReadyGo::onGVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN( int nParam, const char * strParam,void * ptrParam )
{
	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoom m_RoomData.m_RoomID= " << m_pRoom->getRoomData()->m_RoomID);
		return;
	}

	SSchemeMatchRoom* pSchemeMatchRoom = m_pRoom->getSchemeMatchRoom();
	if (nullptr == pSchemeMatchRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom RoomID= " << m_pRoom->getRoomData()->m_RoomID);
		return;
	}

	cmd_matchroom_selectheroskin* pCmd = (cmd_matchroom_selectheroskin*)ptrParam;

	obuf256 ob;
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
	MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
	MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

	SMsgMatchDataSubMsg outData;
	outData.dwMsgCode	= CS_MSG_SELECT_HERO_SKIN;						// 子消息代码  EMRoomMessageCode

	ob << MsgHead<<outData;
	SMsgSelectHeroSkin MsgInfo;
	MsgInfo.dwHeroID = pCmd->nHeroID;
	MsgInfo.dwSkinID = pCmd->nSkinID;
	ob << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_ReadyGo::release()
{
    return;
}

void RoomState_ReadyGo::restoreState()
{
    return;
}