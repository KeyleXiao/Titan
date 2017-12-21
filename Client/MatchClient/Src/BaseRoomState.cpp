#include "stdafx.h"
#include "BaseRoomState.h"
#include "CurrentMatchRoomInfo.h"
#include "MatchDef.h"
#include "MatchSchemeDef.h"
#include "NetMessageDef.h"
#include "IState.h"
#include "ISpell.h"
#include "IMatchClient.h"
#include "GameViewDef.h"
#include "IClientEntity.h"
 

BaseRoomState::BaseRoomState(CurrentMatchRoomInfo* pRoom, uint32 u32ID)
	:IState(u32ID)
	,m_pRoom( pRoom )
{
	MATCH_TRACELN("BaseRoomState::BaseRoomState() ID=" << getID());
}

BaseRoomState::~BaseRoomState()
{
	MATCH_TRACELN("BaseRoomState::~BaseRoomState() ID=" << getID());
}
 
 
void BaseRoomState::onServerMsg(SNetMsgHead* pHead, void* pData, size_t stLen)
{
	switch (MAKEWORD(pHead->byKeyModule, pHead->byKeyAction) )
	{
	// 社会服发下来的消息
	case MAKEWORD(MSG_MODULEID_MATCH, OC_MSG_MASTER_CHANGE):              // 房主改变
		{
			onMsgChangeMaster(pHead, pData, stLen);
		}
		break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_EXIT_ROOM_HALL_ROOM):        // 离开房间后获得的大厅房间数据
		{
			onMsgExitRoomToHall(pHead, pData, stLen);
		}
		break;

	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_KICK_OUT_ROOM_HALL_ROOM):    // 被踢到大厅后获得的大厅房间数据
		{
			onMsgKickOutRoomToHall(pHead, pData, stLen);
		}
		break;

	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_LOAD_SCENE):                 // 加载场景
		{
			onMsgLoadScene(pHead, pData, stLen);
		}
		break;

	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_INSIDE_ROOM_STATE):          // 房间内状态改变
		{
			onMsgChangeRoomState(pHead, pData, stLen);
		}
		break;

	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_RETURN_TO_ROOM):             // 重新回到房间
		{
			onMsgReturnToRoom(pHead, pData, stLen);
		}
		break;

	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_EXIT_ROOM):                  // 离开房间
		{
			onMsgExitRoom(pHead, pData, stLen);
		}
		break;

    //case MAKEWORD(MSG_MODULEID_MATCH,SC_MSG_ROOM_SUMMONER_SKILL):      // 更新召唤师技能
    //    {
    //        onMsgUpdateSummonerSkillInfo(pHead, pData, stLen );
    //    }
    //    break;
    case MAKEWORD(MSG_MODULEID_MATCH,SC_MSG_ROOM_SUMMONER_SKILL_INROOM):   // 房间内更新召唤师技能
        {
            onMsgUpdateSummonerSkillInfoInRoom(pHead, pData, stLen);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ROOM_ROLE_COUNT):               // 房间人数同步
        {
            // 房间内不用响应
        }
        break;

	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_EXIT_HALL_FROM_LEGENDCUPROOM):
		{
			onMsgExitHallFromLegendRoom(pHead, pData, stLen);
		}
		break;

    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_EXIT_HALL_FROM_MATCHROOM):        // 离开撮合房间
        {
            onMsgExitHallFromMatchRoom(pHead, pData, stLen);
        }
        break;

	default:
		{
			ErrorLn(__FUNCTION__": a message not handle, msg= " << pHead->byKeyAction <<", state_name="<< (m_pRoom ? getStateTypeName(m_pRoom->getState()->getID()) : "") );
		}
        break;
	}
}

void BaseRoomState::onMsgExitRoom( SNetMsgHead* head, void* data, size_t len )
{
	if (len < sizeof(SMsgExitRoom_OC))
	{
		ErrorLn("Exit room message is not match ! len="<< len <<", id="<< (m_pRoom ? m_pRoom->getRoomData()->m_RoomID : 0) );
		return;
	}

	SMsgExitRoom_OC* pMsg = (SMsgExitRoom_OC*)data;
    if(m_pRoom)
    {
	    m_pRoom->playerExit(pMsg->m_OBPos, pMsg->m_Pos, GVIEWCMD_MATCH_ROOM_PLAYER_EXIT);
    }
}

//void BaseRoomState::onMsgUpdateSummonerSkillInfo(SNetMsgHead* head, void* data, size_t len)
//{
//    if ( data== NULL || len != sizeof(SMsgSummoner_Skill))
//    {
//        ErrorLn("Update summoner skill info: pData==NULL or length is error, len="<<len);
//        return;
//    }
//
//    SMsgSummoner_Skill* pMsg = (SMsgSummoner_Skill*)data;
//    if(m_pRoom)
//    {
//        m_pRoom->setPDbidSummonerSKill(pMsg);
//    }
//}

void BaseRoomState::onMsgUpdateSummonerSkillInfoInRoom(SNetMsgHead* pHead, void* pData, size_t len)
{
    if(m_pRoom == NULL)
    {
        ErrorLn("Update summoner skill info in room: room object is not exist!" );
        return;
    }
    if ( pData== NULL || len < sizeof(SMsgSummoner_Skill))
    {
        ErrorLn("Update summoner skill info in room: pData==NULL or length is error, len="<< len <<", id="<< m_pRoom->getRoomData()->m_RoomID );
        return;
    }

    SMsgSummoner_Skill* pMsg = (SMsgSummoner_Skill*)pData;

    SRoomRoleBase* pInfo = m_pRoom->getPlayerRoomBaseInfo(pMsg->pDbid);
    if (pInfo == NULL )
    {
        ErrorLn("not exist room data for actor: pInfo == NULL, dbid="<< pMsg->pDbid);
        return;
    }

    m_pRoom->updateSummonerSkillInfo(pInfo->nIndex, pMsg->nSummonerSkill, SUMMONER_SKILL_COUNT);

    cmd_Summoner_Skill_Change cmdSummonerInfo;
    ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
    if (pSpellFactory == NULL)
    {
        return;
    }

    cmdSummonerInfo.nPos = pInfo->nIndex;
    cmdSummonerInfo.nCamp = m_pRoom->getPlayerCamp(pInfo);
    cmdSummonerInfo.pDbid = pMsg->pDbid;
    for ( int i = 0; i < SUMMONER_SKILL_COUNT; ++i )
    {

        SPELL_DATA *pSpellData = pSpellFactory->getSpellData(pMsg->nSummonerSkill[i]);
        if (pSpellData != NULL)
        {
            cmdSummonerInfo.nSummonerSkill[i] = pSpellData->nIcon;
        }
    }

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_SUMMONER_BROAD_SKILL,0,0,&cmdSummonerInfo, sizeof(cmdSummonerInfo));
}

void BaseRoomState::showSummonerSkillInfo(SMsgSummoner_Skill* pMsgInfo)
{

}

void BaseRoomState::onMsgReturnToRoom(SNetMsgHead* head, void* data, size_t len)
{
	m_pRoom->inital( data,len, false );
}


void BaseRoomState::onMsgChangeRoomState( SNetMsgHead* head, void* data, size_t len )
{
	if (len < sizeof(msg_change_roomState))
	{
		ErrorLn("Change room state message is not match! len="<< len);
		return;
	}
	msg_change_roomState* pMsg = (msg_change_roomState*)data;

    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoomData = pMatchClient->getRoomData( pMsg->dwRoomID);
	if (nullptr == pRoomData)
	{
        ErrorLn("it's not exists room data when change room state! id="<< pMsg->dwRoomID);
		return;
	}

	if (pMsg->nState < MatchRoomState_Wait  || pMsg->nState > MatchRoomState_Fighting)
	{
        ErrorLn("state data is invalid when change room state! id="<< pMsg->dwRoomID <<", state="<< pMsg->nState);
		return;
	}

	pRoomData->byState = pMsg->nState;

	m_pRoom->toState(pRoomData->byState);
}



void BaseRoomState::onMsgLoadScene( SNetMsgHead* head, void* data, size_t len )
{
	MATCH_TRACELN(__FUNCTION__);

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE, 0, 0, 0,0 );

	if (len < sizeof(SMsgLoadScene_SC))
	{
		ErrorLn(__FUNCTION__": len= " << len);
		return;
	}
	SMsgLoadScene_SC* pMsg = (SMsgLoadScene_SC*)data;
	data = (void*)(pMsg + 1);
	len -= sizeof(SMsgLoadScene_SC);
	MATCH_TRACELN(__FUNCTION__": pMsgLoadScene->m_SceneID =  " << pMsg->dwMapID);

    gClientGlobal->getEntityFactory()->destroyAllEntity();
	m_pRoom->loadSenece(pMsg->dwMapID, Vector3());
}


void BaseRoomState::onMsgKickOutRoomToHall( SNetMsgHead* head, void* data, size_t len )
{
	MATCH_TRACELN(__FUNCTION__);

	m_pRoom->getRoomData()->m_SelfPos = Invalid_MatchRoomPos;		
	gClientGlobal->gotoState( ESS_GAME,data,len );

	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	if (pMatchClient != NULL)
	{
		pMatchClient->enterHall(data, len);
	}
}


void BaseRoomState::onMsgExitRoomToHall( SNetMsgHead* head, void* data, size_t len )
{
	MATCH_TRACELN(__FUNCTION__);

	m_pRoom->getRoomData()->m_SelfPos = Invalid_MatchRoomPos;

	gClientGlobal->gotoState( ESS_GAME,data,len );

	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	if (pMatchClient != NULL)
	{
		pMatchClient->enterHall(data, len);
	}
}

void BaseRoomState::onMsgExitHallFromLegendRoom( SNetMsgHead* head, void* data, size_t len )
{
	MATCH_TRACELN(__FUNCTION__);

	m_pRoom->getRoomData()->m_SelfPos = Invalid_MatchRoomPos;

	gClientGlobal->gotoState( ESS_GAME,data,len );

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_SCENE, 0, 0, 0, 0);
}


void BaseRoomState::onMsgExitHallFromMatchRoom(SNetMsgHead* head, void* data, size_t len)
{
    MATCH_TRACELN(__FUNCTION__);

    m_pRoom->getRoomData()->m_SelfPos = Invalid_MatchRoomPos;

    gClientGlobal->gotoState( ESS_GAME,data,len );

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_SCENE, 0, 0, 0, 0);
}


void BaseRoomState::onMsgChangeMaster( SNetMsgHead* head, void* data, size_t len )
{
	MATCH_TRACELN(__FUNCTION__);

	if (len < sizeof(SMsgMasterChange))
	{
		ErrorLn(__FUNCTION__": len= " << len);
		return;
	}
	SMsgMasterChange* pMsgMasterChange = (SMsgMasterChange*)data;
	data = (void*)(pMsgMasterChange + 1);
	len -= sizeof(*pMsgMasterChange);

	SRoomRoleBase* pRole = m_pRoom->getRoleByPos( pMsgMasterChange->m_Pos );
	if (Invalid_MatchRoomPos == pRole->nIndex)
	{
		ErrorLn(__FUNCTION__": Invalid_MatchRoomPos == pRole->m_Pos pMsgMasterChange->m_Pos= " << pMsgMasterChange->m_Pos);
		return;
	}
	//if (m_pRoom->getRoomData()->m_MasterPos == pMsgMasterChange->m_Pos)
	//{
	//	ErrorLn(__FUNCTION__": m_RoomData.m_MasterPos == pMsgMasterChange->m_Pos pMsgMasterChange->m_Pos= " << pMsgMasterChange->m_Pos);
	//	return;
	//}

	cmd_match_room_master_change cmdMatchRoomMasterChange;
	cmdMatchRoomMasterChange.nCampOld			= m_pRoom->getPlayerCamp( m_pRoom->getRoomData()->m_MasterPos );
	cmdMatchRoomMasterChange.nPlayerSlotIDOld	= m_pRoom->getPlayerCampPos( m_pRoom->getRoomData()->m_MasterPos );

	m_pRoom->getRoomData()->m_MasterPos = pMsgMasterChange->m_Pos;

	cmdMatchRoomMasterChange.nSlef			= m_pRoom->isMaster(m_pRoom->getRoomData()->m_SelfPos) ? 1 : 0;
	cmdMatchRoomMasterChange.nCamp			= m_pRoom->getPlayerCamp( pMsgMasterChange->m_Pos );
	cmdMatchRoomMasterChange.nPlayerSlotID	= m_pRoom->getPlayerCampPos( pMsgMasterChange->m_Pos );
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_MASTER_CHANGE, 0, 0, &cmdMatchRoomMasterChange, sizeof(cmdMatchRoomMasterChange));
}



bool BaseRoomState::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{
	MATCH_WARNINGLN("BaseRoomState::onServerMsg unknown eventID= " << eventID << " RoomState() ="<<getStateTypeName(getID()));
	return false;
}

inline const char* BaseRoomState::getStateTypeName(uint32 nType)
{
	static char* szTypeName[MatchRoomStates] = 
	{
		// 开始游戏前，等待
		"MatchRoomState_Wait", 	
		// 选人
		"MatchRoomState_SelectHero", 	
		
		// 全部玩家确认英雄 开始倒计时中
		"MatchRoomState_ReadyGo",

        // 战斗中
        "MatchRoomState_Fighting",	
	};

	if (nType > MatchRoomStates)
	{
		return "MatchRoomStates";
	}
	return szTypeName[nType];
}


