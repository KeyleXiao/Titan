#include "stdafx.h"
#include "RoomState_Wait.h"
#include "MatchDef.h"
#include "MatchSchemeDef.h"
#include "NetMessageDef.h"
#include "IClientGlobal.h"
#include "CurrentMatchRoomInfo.h"



void RoomState_Wait::onEnter()
{
    MATCH_TRACELN(__FUNCTION__);
    //发到显示层状态
    cmd_change_roomstate cmdInfo;
    cmdInfo.nStateID = getID();
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_STATE,getID(),0,&cmdInfo, sizeof(cmdInfo));

    // MatchRoomState_Wait 状态下命令
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_START_GAME,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_PLAYER_EXIT,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_HERO_INFO,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCM_MATCH_MASTER_KICK_ROLE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_FROM_GAME_TO_ROOM,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_PRE_SELECT_HERO,this );

    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_COMPUTERPLAYER_ADD,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_MASTER_COVER_COMPUTER,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_BAN_HERO,this );
	

}

void RoomState_Wait::onLeave()
{
    MATCH_TRACELN(__FUNCTION__);

    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_START_GAME);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_PLAYER_EXIT);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_HERO_INFO);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCM_MATCH_MASTER_KICK_ROLE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_FROM_GAME_TO_ROOM);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_PRE_SELECT_HERO);

    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_COMPUTERPLAYER_ADD);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_MASTER_COVER_COMPUTER);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_BAN_HERO);
    


}

RoomState_Wait::RoomState_Wait( CurrentMatchRoomInfo* pRoom )
    :BaseRoomState(pRoom, MatchRoomState_Wait)
{

}

RoomState_Wait::~RoomState_Wait()
{
}

void RoomState_Wait::onServerMsg(SNetMsgHead* pHead, void* pData, size_t stLen)
{

    switch ( MAKEWORD(pHead->byKeyModule, pHead->byKeyAction) )
    {
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ROOM_ROLE):
        {
            onMsgRoomRole(pHead, pData, stLen);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ROOM_OB_ROLE):
        {
            onMsgRoomOBRole(pHead, pData, stLen);
        }
        break;
        
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_LEGENDCUP_WIN_MSG_INFO):
        {
            onMsgLegendCupWinMsgInfo(pHead, pData, stLen);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ROOM_START_GAME_FAIL):
        {
            onMsgRoomStartGameFail(pHead, pData, stLen);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_KICK_OUT_PLAYER):
        {
            onMsgKickOutPlayer(pHead, pData, stLen);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_POS_CHANGE):
        {
            onMsgPosChange(pHead, pData, stLen);
        }
        break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_OB_POS_CHANGE):
		{
			onMsgObPosChange(pHead, pData, stLen);
		}
		break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_HERO_CHANGE):
        {
            onMsgPreHeroChange(pHead, pData, stLen);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_COMPUTER_HERO_CHANGE):
        {
            onMsgComputerHeroChange(pHead, pData, stLen);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_COMPUTER_HERO_CAN_SELECT):
        {
            onMsgComputerHeroCanSelect(pHead, pData, stLen);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_SEND_ROOM_CAMP_COMPUTER_BANHEROS):
        {
            onMsgCampHeroCanSelect(pHead, pData, stLen);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_GLOBLE_COMPUTER_BAN_HERO_LIST):		// 电脑玩家全局禁用列表
        {
            onMsgComputerGlobleBanHeroList(pHead, pData, stLen);
        }
        break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_MATCH_LEGENDCUP_WAIT_COUNTDOWN):
		{
			onMsgLegendCupWaitCountdown(pHead, pData, stLen);
		}
		break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_CHANGE_AI_ID_SUCCESS):
        {
            onMsgChangeAIIDSuccess(pHead, pData, stLen);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ENTER_ROOM_OHTER_PRE_SELECT):
        {
            onMsgPreSelectList(pHead, pData, stLen);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_BAN_HERO_COUNT_DOWN):
        {
            onMsgBanHeroCountDown(pHead, pData, stLen);
        }
        break;

    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_POS_BAN_HERO_FLAG):
        {
            onMsgBanHeroFlag(pHead, pData, stLen);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_POS_BROAD_SINGLE_BAN_HERO):
        {
            onMsgBroadSingleBanHero(pHead, pData, stLen);
        }
        break;
    

    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_POS_ALL_ALREADY_BAN_HERO):
        {
            onMsgAlreadyBanHeroInfo(pHead, pData, stLen);
        }
        break;
        
    default:
        { 
            BaseRoomState::onServerMsg(pHead, pData, stLen);
        }
    }
}


void RoomState_Wait::onMsgPreSelectList(SNetMsgHead* head, void* data, size_t len)
{

    if (len < sizeof(SMsgHeroChange))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }

    obuf obViewdata;
    int* pHead = (int*)data;
    cmd_pre_select_head headData;
    headData.nCount = *pHead;
    obViewdata.push_back(&headData, sizeof(headData));

    SMsgHeroChange* pMsg = (SMsgHeroChange*)(pHead+1);
    for (int n = 0; n < headData.nCount; ++n)
    {
        SRoomRoleBase* pRole = m_pRoom->getRoleByPos( pMsg->Pos );
        if (Invalid_MatchRoomPos == pRole->nIndex)
        {
            ErrorLn(__FUNCTION__": Invalid_MatchRoomPos == pRole->m_Pos Pos= " << pMsg->Pos);
            return;
        }
        pRole->dwHeroID = pMsg->dwHeroID;

        cmd_match_room_hero_change cmdMatchRoomHeroChange;
        cmdMatchRoomHeroChange.nCamp			= m_pRoom->getPlayerCamp( pRole->nIndex );
        cmdMatchRoomHeroChange.nPlayerSlotID	= m_pRoom->getPlayerCampPos( pRole->nIndex );
        cmdMatchRoomHeroChange.nHeroID			= (int)pRole->dwHeroID;
        cmdMatchRoomHeroChange.nHeroStarLv		= pMsg->nHeroStarLv;
        cmdMatchRoomHeroChange.nHeroStaticLv	= pMsg->nHeroStaticLv;
        cmdMatchRoomHeroChange.bServerRand		= pMsg->bIsServerRand ? 1 : 0;
        obViewdata.push_back(&cmdMatchRoomHeroChange, sizeof(cmdMatchRoomHeroChange));

        pMsg++;

    }
    

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_PRE_HERO_CHANGE_LIST, 0, 0, obViewdata.data(), obViewdata.size());
}


void RoomState_Wait::onMsgPreHeroChange( SNetMsgHead* head, void* data, size_t len )
{
    if (len < sizeof(SMsgHeroChange))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgHeroChange* pMsg = (SMsgHeroChange*)data;
    data = (void*)(pMsg + 1);
    len -= sizeof(*pMsg);

    SRoomRoleBase* pRole = m_pRoom->getRoleByPos( pMsg->Pos );
    if (pRole == NULL)
    {
        ErrorLn(__FUNCTION__": pRole == NULL Pos="<<pMsg->Pos);
        return;
    }

    if (Invalid_MatchRoomPos == pRole->nIndex)
    {
        ErrorLn(__FUNCTION__": Invalid_MatchRoomPos == pRole->m_Pos Pos= " << pMsg->Pos);
        return;
    }
    pRole->dwHeroID = pMsg->dwHeroID;

    cmd_match_room_hero_change cmdMatchRoomHeroChange;
    cmdMatchRoomHeroChange.nCamp			= m_pRoom->getPlayerCamp( pRole->nIndex );
    cmdMatchRoomHeroChange.nPlayerSlotID	= m_pRoom->getPlayerCampPos( pRole->nIndex );
    cmdMatchRoomHeroChange.nHeroID			= (int)pRole->dwHeroID;
    cmdMatchRoomHeroChange.nHeroStarLv		= pMsg->nHeroStarLv;
    cmdMatchRoomHeroChange.nHeroStaticLv	= pMsg->nHeroStaticLv;
	cmdMatchRoomHeroChange.bServerRand		= pMsg->bIsServerRand ? 1 : 0;

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_HERO_CHANGE, 0, 0, &cmdMatchRoomHeroChange, sizeof(cmdMatchRoomHeroChange));
}

void RoomState_Wait::onMsgPosChange( SNetMsgHead* head, void* data, size_t len )
{

    if (len < sizeof(SMsgPosChange_OC))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgPosChange_OC* pMsgPosChange = (SMsgPosChange_OC*)data;

    SRoomRoleBase* pRole = m_pRoom->getRoleByPos( pMsgPosChange->m_Pos[0] );
    if (Invalid_MatchRoomPos == pRole->nIndex)
    {
        ErrorLn(__FUNCTION__": Invalid_MatchRoomPos == pRole->m_Pos pMsgPosChange->m_Pos[0]= " << pMsgPosChange->m_Pos[0]);
        return;
    }

    SMsgSelfEnterRoomData* pCurRoomData = m_pRoom->getRoomData();
    if (pCurRoomData == NULL)
    {
        ErrorLn(__FUNCTION__": pCurRoomData == NULL!!");
        return;
    }

    if (pRole->nIndex == pCurRoomData->m_MasterPos)
    {// 是房主换位置
        pCurRoomData->m_MasterPos = pMsgPosChange->m_Pos[1];
    }	
    pRole->nIndex = pMsgPosChange->m_Pos[1];

    int nSelf = 0;
    if (pCurRoomData->m_SelfPos == pMsgPosChange->m_Pos[0])
    {// 是自己换位置
        nSelf = 1;
        pCurRoomData->m_SelfPos = pRole->nIndex;
        //刷新下自己的ban人标志
        m_pRoom->updateSelfCanBanHero();
    }
    m_pRoom->swapPos(pMsgPosChange->m_Pos[0], pMsgPosChange->m_Pos[1]);
    cmd_match_room_hero_pos_change cmdMatchRoomHeroPosChange;
    cmdMatchRoomHeroPosChange.nCampFrom	= m_pRoom->getPlayerCamp( pMsgPosChange->m_Pos[0] );
    cmdMatchRoomHeroPosChange.nCampTo	= m_pRoom->getPlayerCamp( pMsgPosChange->m_Pos[1] );
    cmdMatchRoomHeroPosChange.nPlayerSlotIDFrom	= m_pRoom->getPlayerCampPos( pMsgPosChange->m_Pos[0] );
    cmdMatchRoomHeroPosChange.nPlayerSlotIDTo	= m_pRoom->getPlayerCampPos( pMsgPosChange->m_Pos[1] );
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE, nSelf, 0, &cmdMatchRoomHeroPosChange, sizeof(cmdMatchRoomHeroPosChange));
    m_pRoom->changeSummonerSkillChangePos(pMsgPosChange->m_Pos[1]);

}

void RoomState_Wait::onMsgObPosChange( SNetMsgHead* head, void* data, size_t len )
{
	if (data == NULL || len < sizeof(SMsgOBPosChange_OC))
	{
		ErrorLn(__FUNCTION__":  len= " << len);
		return;
	}
	SMsgOBPosChange_OC* pMsgObPosChange = (SMsgOBPosChange_OC*)data;

	SMsgSelfEnterRoomData* pCurRoomData = m_pRoom->getRoomData();
	if (pCurRoomData == NULL)
	{
		ErrorLn(__FUNCTION__": pCurRoomData == NULL!!");
		return;
	}

	SRoomRoleBase *pSrcRole = NULL, *pDestRole = NULL;
	if (pMsgObPosChange->bFromIsOBPos)
	{
		pSrcRole = m_pRoom->getObRoleByPos(pMsgObPosChange->uFromPos);
		pDestRole = m_pRoom->getRoleByPos(pMsgObPosChange->uDesPos);
	}
	else
	{
		pSrcRole = m_pRoom->getRoleByPos(pMsgObPosChange->uFromPos);
		pDestRole = m_pRoom->getObRoleByPos(pMsgObPosChange->uDesPos);
	}
	if (pSrcRole == NULL)
	{
		ErrorLn(__FUNCTION__": pRole == NULL");
		return;
	}
	pSrcRole->bIsOBPos = pMsgObPosChange->bDesIsOBpos;
	pSrcRole->nIndex = pMsgObPosChange->uDesPos;

	int nSelf = 0;
	if (pCurRoomData->m_bIsOBEnter == pMsgObPosChange->bFromIsOBPos && pCurRoomData->m_SelfPos == pMsgObPosChange->uFromPos)
	{
		nSelf = 1;
		pCurRoomData->m_bIsOBEnter = pSrcRole->bIsOBPos;
		pCurRoomData->m_SelfPos = pSrcRole->nIndex;
		//刷新下自己的ban人标志
		m_pRoom->updateSelfCanBanHero();
	}
	std::swap(*pSrcRole, *pDestRole);

	cmd_match_room_ob_pos_change_oc cmdPosChange;
	cmdPosChange.bSrcIsOBPos	= pMsgObPosChange->bFromIsOBPos;
	cmdPosChange.nSrcPos		= pMsgObPosChange->uFromPos;
	cmdPosChange.bDesIsOBPos	= pMsgObPosChange->bDesIsOBpos;
	cmdPosChange.nDesPos		= pMsgObPosChange->uDesPos;
	cmdPosChange.nActorID		= pDestRole->dwPDBID;
	cmdPosChange.nHeadID		= 0;
	cmdPosChange.nSex			= pDestRole->bySex;
	strcpy_s(cmdPosChange.szPlayerName, sizeof(cmdPosChange.szPlayerName), pDestRole->szName);
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE_OB, nSelf, 0, &cmdPosChange, sizeof(cmdPosChange));
	
	if (pMsgObPosChange->bDesIsOBpos <= 0)
	{
		m_pRoom->changeSummonerSkillChangePos(pMsgObPosChange->uDesPos);
	}
}


void RoomState_Wait::onMsgKickOutPlayer( SNetMsgHead* head, void* data, size_t len )
{

    if (len < sizeof(SMsgKickOutPlayer_OC))
    {
        ErrorLn(__FUNCTION__": len= " << len);
        return;
    }
    SMsgKickOutPlayer_OC* pSMsgKickOutPlayer = (SMsgKickOutPlayer_OC*)data;
    m_pRoom->playerExit(pSMsgKickOutPlayer->m_nIsOb, pSMsgKickOutPlayer->m_Pos, GVIEWCM_MATCH_MASTER_KICK_ROLE);
}

void RoomState_Wait::onMsgRoomStartGameFail( SNetMsgHead* head, void* data, size_t len )
{

    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
    if (nullptr == pRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << m_pRoom->getRoomData()->m_RoomID);
        return;
    }
    if (MatchRoomState_Wait != pRoom->byState)
    {
        ErrorLn(__FUNCTION__": MatchRoomState_Wait != pRoom->m_byState RoomID= " << m_pRoom->getRoomData()->m_RoomID<<",byState= " << pRoom->byState);
        return;
    }

    m_pRoom->toState(MatchRoomState_Wait);
}


void RoomState_Wait::onMsgRoomOBRole(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(SRoomRoleBase))
    {
        ErrorLn(__FUNCTION__": len= " << len);
        return;
    }
    SRoomRoleBase* pMsgRoomRole = (SRoomRoleBase*)data;
    m_pRoom->SetOBRoomRole(pMsgRoomRole->nIndex, *pMsgRoomRole);
    cmd_match_room_ob_data cmdMatchRoomObData;
    m_pRoom->MakeObDataCmd(pMsgRoomRole, cmdMatchRoomObData);
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_ROOM_OTHER_OB, 0, 0, &cmdMatchRoomObData, sizeof(cmdMatchRoomObData));
}

void RoomState_Wait::onMsgRoomRole( SNetMsgHead* head, void* data, size_t len )
{

    if (len < sizeof(SRoomRoleBase))
    {
        ErrorLn(__FUNCTION__": len= " << len);
        return;
    }
    SRoomRoleBase* pMsgRoomRole = (SRoomRoleBase*)data;
    m_pRoom->SetRoomRole(pMsgRoomRole->nIndex, *pMsgRoomRole);
    cmd_match_room_role_data cmdMatchRoomRoleData;
    m_pRoom->MakeRoleDataCmd(pMsgRoomRole, cmdMatchRoomRoleData);
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_ROOM_DATA, 0, 0, &cmdMatchRoomRoleData, sizeof(cmdMatchRoomRoleData));

    ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
    if (pSpellFactory == NULL)
    {
        return;
    }

    cmd_Summoner_Skill_Change cmdSummonerInfo;
    cmdSummonerInfo.nPos = pMsgRoomRole->nIndex;
    cmdSummonerInfo.nCamp = m_pRoom->getPlayerCamp(pMsgRoomRole);
    cmdSummonerInfo.pDbid = pMsgRoomRole->dwPDBID;

    for (int i = 0; i < SUMMONER_SKILL_COUNT; ++i )
    {

        SPELL_DATA *pSpellData = pSpellFactory->getSpellData(pMsgRoomRole->nSummonserSkill[i]);
        if (pSpellData != NULL)
        {
            cmdSummonerInfo.nSummonerSkill[i] = pSpellData->nIcon;
        }
    }

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_SUMMONER_BROAD_SKILL,0,0,&cmdSummonerInfo, sizeof(cmdSummonerInfo));
}

void RoomState_Wait::onMsgLegendCupWaitCountdown( SNetMsgHead* head, void* data, size_t len )
{
	if (len < sizeof(SMsgLegendCup_WaitCountdown))
	{
		ErrorLn(__FUNCTION__": len= " << len);
		return;
	}
	SMsgLegendCup_WaitCountdown* pRecvData = (SMsgLegendCup_WaitCountdown*)data;

	cmd_legendcup_room_wait_countdown cmdLegendCupRoomWaitCountdown;
	cmdLegendCupRoomWaitCountdown.nCountdown = pRecvData->nCountdown;

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_LEGENDCUP_ROOM_WAIT_COUNTDOWN,0,0,&cmdLegendCupRoomWaitCountdown, sizeof(cmdLegendCupRoomWaitCountdown));
}

void RoomState_Wait::onMsgChangeAIIDSuccess( SNetMsgHead* head, void* data, size_t len )
{
    if (len < sizeof(SMsgChangeAISuccess_SO))
    {
        ErrorLn(__FUNCTION__": len= " << len);
        return;
    }
    SMsgChangeAISuccess_SO* pRecvData = (SMsgChangeAISuccess_SO*)data;
    
    m_pRoom->changeComputerAIID(pRecvData->dwComputerID, pRecvData->nAIID);
}




bool RoomState_Wait::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{

    switch ( eventID )
    {
    case GVIEWCMD_MATCH_START_GAME:
        {
            onGVIEWCMD_MATCH_START_GAME(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_MATCH_ROOM_MASTER_COVER_COMPUTER:
        {
            onGVIEWCMD_MATCH_MASTER_COVER_COMPUTER(nParam, strParam, ptrParam);
        }
        break;
        
    case GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE:
        {
            onGVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_MATCH_ROOM_PLAYER_EXIT:
        {
            onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_MATCH_ROOM_HERO_INFO:
        {
            onGVIEWCMD_MATCH_ROOM_HERO_INFO(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCM_MATCH_MASTER_KICK_ROLE:
        {
            onGVIEWCM_MATCH_MASTER_KICK_ROLE(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_MATCH_FROM_GAME_TO_ROOM:
        {
            onGVIEWCMD_MATCH_FROM_GAME_TO_ROOM(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA:
        {
            onGVIEWCMD_MATCH_FROM_GETHREO_DES(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_MATCH_COMPUTERPLAYER_ADD:   // 添加假人
        {
            if (m_pRoom->isMaster() && m_pRoom->getRoomType() != HallRoomType_LegendCup)
            {
                onGVIEWCMD_MATCH_COMPUTERPLAYER_ADD(nParam, strParam, ptrParam);
            }
        }
        break;

    case GVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE:   // 移除假人
        {
            if (m_pRoom->isMaster())
            {
                onGVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE(nParam, strParam, ptrParam);
            }

        }
        break; 

    case GVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE:   // 假人选择英雄
        {
            if (m_pRoom->isMaster())
            {
                onGVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE(nParam, strParam, ptrParam);
            }
        }
        break;
    case GVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE:   // 假人选择英雄AI
        {
            if (m_pRoom->isMaster())
            {
                onGVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE(nParam, strParam, ptrParam);
            }
        }
        break;
    case GVIEWCMD_MATCH_ROOM_PRE_SELECT_HERO:            // 优先预选的英雄
        {

            onGVIEWCMD_MATCH_ROOM_PRE_SELECT_HERO(nParam, strParam, ptrParam);

        }
        break;
    case GVIEWCMD_MATCH_ROOM_BAN_HERO:
        {
            onGVIEWCMD_MATCH_ROOM_BAN_HERO(nParam, strParam, ptrParam);
        }
        break;

    default:
        break;
    }
    return true;
}

void RoomState_Wait::onGVIEWCMD_MATCH_FROM_GAME_TO_ROOM( int nParam, const char * strParam, void * ptrParam )
{
    m_pRoom->showRoom();
}

void RoomState_Wait::onGVIEWCM_MATCH_MASTER_KICK_ROLE( int nParam, const char * strParam, void * ptrParam )
{
    if (nullptr == ptrParam)
    {
        ErrorLn(__FUNCTION__": nullptr == ptrParam " );
        return;
    }

   IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
    if (nullptr == pRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pRoom m_RoomData.m_RoomID= " << m_pRoom->getRoomData()->m_RoomID);
        return;
    }
    if ( !m_pRoom->isMaster() )
    {
        WarningLn(__FUNCTION__": is not Master()");
        return;
    }

    cmd_match_room_hero_exit* pCmd = (cmd_match_room_hero_exit*)ptrParam;
    uchar Pos = pCmd->nPos;

	SRoomRoleBase* pRole = NULL;
	if (pCmd->bIsOb)
	{
		pRole = m_pRoom->getObRoleByPos( Pos );
	}
	else
	{
		pRole = m_pRoom->getRoleByPos( Pos );
	}
    if (Invalid_MatchRoomPos == pRole->nIndex)
    {
        WarningLn(__FUNCTION__": Invalid_MatchRoomPos == pRole->m_Pos");
        return;
    }

    SSchemeMatchRoom* pSchemeMatchRoom = m_pRoom->getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom m_RoomData.m_RoomID= " << m_pRoom->getRoomData()->m_RoomID);
        return;
    }
    if ( !pSchemeMatchRoom->m_RoomStateKick[ pRoom->byState ] )
    {
        MATCH_TRACELN(__FUNCTION__": !pSchemeMatchRoom->m_RoomStateKick[ pRoom->m_byState ] pRoom->m_byState= " << pRoom->byState);
        return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_KICK_OUT_PLAYER;						// 子消息代码  EMRoomMessageCode

    obuf256 ob;
    ob << MsgHead<<outData;

    SMsgKickOutPlayer MsgInfo;
    MsgInfo.byPos = Pos;
	MsgInfo.nIsOb = pCmd->bIsOb;
    ob << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}


void RoomState_Wait::onGVIEWCMD_MATCH_ROOM_HERO_INFO( int nParam, const char * strParam, void * ptrParam )
{

    cmd_match_room_hero_Info cmd;
    cmd.nHeroID = nParam;
    strcpy_s(cmd.szHeroCamp, sizeof(cmd.szHeroCamp), a2utf8("阵营未知"));
    strcpy_s(cmd.szHeroGank, sizeof(cmd.szHeroGank), a2utf8("定位未知"));
    strcpy_s(cmd.szHeroName, sizeof(cmd.szHeroName), a2utf8("无名"));
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_HERO_INFO, 0, 0, &cmd, sizeof(cmd));

    int HeroID = nParam;

    ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
    if (pSpellFactory == NULL)
    {
        return;
    }
    // 取得技能槽位信息
    SPELL_SLOT *sTempSlot = g_EHelper.getSpellSlot(HeroID);
    if (sTempSlot == NULL)
    {
        return;
    }
    for (int i = 0;i < sTempSlot->nCount; ++i)
    {
        //组装信息到显示层
        cmd_choose_hero_sync cmdChooseHero;
        cmdChooseHero.HeroID = HeroID;		//英雄ID
        cmdChooseHero.byIndex =i;			//槽位


        strcpy_s(cmdChooseHero.szImage, sizeof(cmdChooseHero.szImage), sTempSlot->szImage);
        strcpy_s(cmdChooseHero.szVocation, sizeof(cmdChooseHero.szVocation), sTempSlot->szImage);
        strcpy_s(cmdChooseHero.szLocate, sizeof(cmdChooseHero.szLocate), sTempSlot->szLocate);
        cmdChooseHero.nHp = sTempSlot->nHp;										// 生命
        cmdChooseHero.nPA = sTempSlot->nPA;										// 物理攻击
        cmdChooseHero.nMA = sTempSlot->nMA;										// 魔法攻击
        cmdChooseHero.nOperateFactor = sTempSlot->nOperateFactor;				// 操作系数难度

        int nSpellID = sTempSlot->nSpellID[i];

        SPELL_DATA * pSpellData = pSpellFactory->getSpellData(nSpellID);
        if (pSpellData == NULL)
        {
            continue;
        }
        memcpy(&(cmdChooseHero.SpellData), pSpellData, sizeof(cmdChooseHero.SpellData));
        //发送到客户端
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_HERO_SKILL,sizeof(cmdChooseHero), 0, &cmdChooseHero, sizeof(cmdChooseHero));
    }
}



void RoomState_Wait::onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT( int nParam, const char * strParam, void * ptrParam )
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
        TraceLn(__FUNCTION__": !pSchemeMatchRoom->m_RoomStateExit[ pRoom->m_byState ] pRoom->m_byState= " << pRoom->byState);
        return;
    }


    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_EXIT_ROOM;						// 子消息代码  EMRoomMessageCode

    obuf256 ob;
    ob << MsgHead<<outData;

    net_senddata( ob.data(), ob.size() );
}


void RoomState_Wait::onGVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE( int nParam,const char * strParam,void * ptrParam )
{
    if (nullptr == ptrParam)
    {
        ErrorLn(__FUNCTION__": nullptr == ptrParam");
        return;
    }

    cmd_match_room_ob_pos_change* pCmd = (cmd_match_room_ob_pos_change*)ptrParam;

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_POS_CHANGE;						// 子消息代码  EMRoomMessageCode

    SMsgPosChange MsgInfo;
	MsgInfo.bSelfIsOBPos = pCmd->bSelfIsOBPos;
	MsgInfo.idActor	= pCmd->idActor;
	MsgInfo.bDesIsOBPos = pCmd->bDesIsOBPos;
	MsgInfo.byPos = pCmd->nPos;

    obuf256 ob;
    ob << MsgHead<<outData << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}


void RoomState_Wait::onGVIEWCMD_MATCH_MASTER_COVER_COMPUTER(int nParam,const char * strParam,void * ptrParam)
{
    SMsgSelfEnterRoomData* pRoomData = m_pRoom->getRoomData();
    if (pRoomData == NULL)
    {
        ErrorLn(__FUNCTION__": pRoomData == NULL");
        return;
    }

    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( pRoomData->m_RoomID );
    if (nullptr == pRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << pRoomData->m_RoomID);
        return;
    }
    SSchemeMatchRoom* pSchemeMatchRoom = m_pRoom->getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom ");
        return;
    }

    if (pRoomData->m_SelfPos != pRoomData->m_MasterPos)
    {
        ErrorLn(__FUNCTION__": !MasterPos RoomID= " << pRoomData->m_RoomID);
        return;
    }
    if (MatchRoomState_Wait != pRoom->byState)
    {
        ErrorLn(__FUNCTION__": MatchRoomState_Wait != RoomState RoomID= " << pRoomData->m_RoomID<<" byState= " << pRoom->byState);
        return;
    }

    uint32 u32RoleCount = 0;
    for (int i = 1; i <= pSchemeMatchRoom->m_CampCount; ++i)
    {
        uint32 u32CampRoleCount = m_pRoom->getCampRoleCount(i, pSchemeMatchRoom);
        if (u32CampRoleCount < pSchemeMatchRoom->m_StartSelectHeroNeedCampRoleMin)
        {
            TraceLn(__FUNCTION__": failed! u32CampRoleCount < HeroNeedCampRoleMin");
            return;
        }
        u32RoleCount += u32CampRoleCount;
    }
    if (u32RoleCount < pSchemeMatchRoom->m_StartSelectHeroNeedRoleMin)
    {
        TraceLn(__FUNCTION__": failed! u32RoleCount < HeroNeedRoleMin");
        return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_MASTER_COVER_COMPUTER;						// 子消息代码  EMRoomMessageCode

    obuf256 ob;
    ob << MsgHead<<outData;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_Wait::onGVIEWCMD_MATCH_START_GAME( int nParam,const char * strParam,void * ptrParam )
{
    SMsgSelfEnterRoomData* pRoomData = m_pRoom->getRoomData();
    if (pRoomData == NULL)
    {
        ErrorLn(__FUNCTION__": pRoomData == NULL");
        return;
    }
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( pRoomData->m_RoomID );
    if (nullptr == pRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pRoom m_RoomData.m_RoomID= " << pRoomData->m_RoomID);
        return;
    }
    SSchemeMatchRoom* pSchemeMatchRoom = m_pRoom->getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom ");
        return;
    }

    if (pRoomData->m_SelfPos != pRoomData->m_MasterPos)
    {
        ErrorLn(__FUNCTION__": SelfPos != MasterPos RoomID= " << pRoomData->m_RoomID);
        return;
    }
    if (MatchRoomState_Wait != pRoom->byState)
    {
        ErrorLn(__FUNCTION__": MatchRoomState_Wait != byState RoomID= " << pRoomData->m_RoomID<<" byState= " << pRoom->byState);
        return;
    }

    uint32 u32RoleCount = 0;
    for (int i = 1; i <= pSchemeMatchRoom->m_CampCount; ++i)
    {
        uint32 u32CampRoleCount = m_pRoom->getCampRoleCount(i, pSchemeMatchRoom);
        if (u32CampRoleCount < pSchemeMatchRoom->m_StartSelectHeroNeedCampRoleMin)
        {
            TraceLn(__FUNCTION__": failed! RoleCount < HeroNeedCampRoleMin");
            return;
        }
        u32RoleCount += u32CampRoleCount;
    }
    if (u32RoleCount < pSchemeMatchRoom->m_StartSelectHeroNeedRoleMin)
    {
        TraceLn(__FUNCTION__": failed! RoleCount < StartSelectHeroNeedRoleMin");
        return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_START_GAME;						// 子消息代码  EMRoomMessageCode

    obuf256 ob;
    ob << MsgHead<<outData;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_Wait::onGVIEWCMD_MATCH_FROM_GETHREO_DES( int nParam, const char * strParam, void * ptrParam )
{
    if (nullptr == ptrParam)
    {
        ErrorLn(__FUNCTION__": nullptr == ptrParam " );
        return;
    }

    IClientEntity * pMainEntity = gClientGlobal-> getHero();
    if(pMainEntity == NULL)
    {
        return ;
    }

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        return ;
    }

    ISchemeSpellSlot* pSpellSlot =pSchemeCenter->getSchemeSpellSlot();
    if(pSpellSlot == NULL)
    {
        return ;

    }

    ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
    if(pSpellFactory== NULL )
    {
        return;
    }

    cmd_match_room_Mouse_HeroArea* pCmd = (cmd_match_room_Mouse_HeroArea*)ptrParam;
    int nHeroID = pCmd->nHeroVocationID;
    SPELL_SLOT* pSpellSlotData=pSpellSlot->getSpellSlot(nHeroID);
    if(pSpellSlotData==NULL)
    {
        return;
    }

    obuf obdata;
    cmd_match_room_hero_skill_desc_count nSkillDescCount;
	nSkillDescCount.nHeroID = nHeroID;
    nSkillDescCount.nSkillDescCount=SPELL_SLOT_SHOW_COUNT;
    obdata.push_back(&nSkillDescCount,sizeof(cmd_match_room_hero_skill_desc_count));
    for (int i=0;i<SPELL_SLOT_SHOW_COUNT;i++)
    {
        int nSpellID= pSpellSlotData->nSpellID[i];
        SPELL_DATA * pSpellData = pSpellFactory->getSpellData(nSpellID);
        if(pSpellData==NULL)
        {
            continue;
        }

        cmd_match_room_hero_skill_desc cmdHeroSkillDesc;
        cmdHeroSkillDesc.nSpellIconID=pSpellData->nIcon;
        cmdHeroSkillDesc.nSlotID=i;
        sstrcpyn(cmdHeroSkillDesc.heroSkillDes, pSpellSlotData->nSpellDesc[i], sizeof(cmdHeroSkillDesc.heroSkillDes));
        obdata.push_back(&cmdHeroSkillDesc, sizeof(cmd_match_room_hero_skill_desc ));
    }

    pMainEntity->sendCommandToEntityView(ENTITY_TOVIEW_MATCH_ROOM_SHOW_SKILL_DESC,0,0,obdata.data(), obdata.size());
}

void RoomState_Wait::onGVIEWCMD_MATCH_COMPUTERPLAYER_ADD(int nParam, const char * strParam, void * ptrParam)
{
    cmd_computerplayer_add * input = (cmd_computerplayer_add *)ptrParam;
    if (input == NULL)
    {
        ErrorLn(__FUNCTION__":wrong data !")
            return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_ROOM_COMPUTERPLAYER_ADD;						// 子消息代码  EMRoomMessageCode

    // 消息信息结构
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
    if(pRoom ==NULL)
    {
        return;
    }
    uint32 u32CampPlayerCount	= pMatchClient->getCampPlayerCountMax( pRoom->dwMatchTypeID );

    SMsg_ComputerPlayerAdd MsgInfo;
    // 转换成服务位置
    MsgInfo.nPos = input->nPos + u32CampPlayerCount* input->nCamp;

    obuf256 ob;
    ob << MsgHead<<outData<<MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_Wait::onGVIEWCMD_MATCH_COMPUTERPLAYER_REMOVE(int nParam, const char * strParam, void * ptrParam)
{
    cmd_computerplayer_remove * input = (cmd_computerplayer_remove *)ptrParam;
    if (input == NULL)
    {
        ErrorLn(__FUNCTION__":wrong data !")
            return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_ROOM_COMPUTERPLAYER_REMOVE;						// 子消息代码  EMRoomMessageCode

    // 消息信息结构
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
    if(pRoom == NULL)
    {
        return;
    }

    uint32 u32CampPlayerCount	= pMatchClient->getCampPlayerCountMax( pRoom->dwMatchTypeID );

    SMsg_ComputerPlayerRemove MsgInfo;
    MsgInfo.nPos = input->nPos + u32CampPlayerCount * (input->nCamp);

    obuf256 ob;
    ob << MsgHead<<outData<<MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_Wait::onGVIEWCMD_MATCH_COMPUTERPLAYER_HEROCHANGE(int nParam, const char * strParam, void * ptrParam)
{
    cmd_computerplayer_herochange * input = (cmd_computerplayer_herochange *)ptrParam;

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_ROOM_COMPUTERPLAYER_HEROCHANGE;						// 子消息代码  EMRoomMessageCode

    // 消息信息结构
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
    if(pRoom == NULL)
    {
        return;
    }

    uint32 u32CampPlayerCount	= pMatchClient->getCampPlayerCountMax( pRoom->dwMatchTypeID );

    SMsg_ComputerPlayerHeroChange MsgInfo;
    // 转换服务位置
    MsgInfo.nPos = input->nPos + u32CampPlayerCount*input->nCamp ;
    MsgInfo.nHeroID = input->nHeroID;                                                   // 改变的英雄ID

    obuf256 ob;
    ob << MsgHead<<outData<<MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_Wait::onMsgComputerHeroChange(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(SMsgComputerHeroChange))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgComputerHeroChange* pMsg = (SMsgComputerHeroChange*)data;
    data = (void*)(pMsg + 1);
    len -= sizeof(*pMsg);

    SRoomRoleBase* pRole = m_pRoom->getRoleByPos( pMsg->m_Pos );
    if (Invalid_MatchRoomPos == pRole->nIndex)
    {
        ErrorLn(__FUNCTION__": Invalid_MatchRoomPos == pRole->m_Pos Pos= " << pMsg->m_Pos);
        return;
    }
    pRole->dwHeroID = pMsg->m_HeroID;
    pRole->nAIID    = pMsg->m_AIID;
    sstrcpyn(pRole->szName, pMsg->strPlayerName,sizeof(pRole->szName));
	pRole->nHeroXPSkillID = pMsg->m_XpID;
    

    cmd_match_room_computer_hero_change cmdInfo;
    cmdInfo.nCamp			= m_pRoom->getPlayerCamp( pRole->nIndex );
    cmdInfo.nPlayerSlotID	= m_pRoom->getPlayerCampPos( pRole->nIndex );
    cmdInfo.nHeroID			= (int)pRole->dwHeroID;
    cmdInfo.nAIID           = pRole->nAIID;
    sstrcpyn(cmdInfo.szHeroName, pMsg->strPlayerName,sizeof(cmdInfo.szHeroName));
    
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CHANGE, 0, 0, &cmdInfo, sizeof(cmdInfo));
}



void RoomState_Wait::onMsgCampHeroCanSelect(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(SMsgCampComputerBanHeros))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgCampComputerBanHeros *pMsg = (SMsgCampComputerBanHeros *)data;
    int *pData = (int *)(pMsg+1);
    for (int i = 0; i < pMsg->nCount; ++i)
    {
        cmd_match_room_computer_hero_can_select cmdInfo;
        cmdInfo.nCamp = pMsg->nCamp;
        cmdInfo.bCanSelect = 0;
        cmdInfo.nHeroID = *pData;		//状态
        pData++;
        // 禁用或者是解锁可选择英雄
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CAN_SELECT, 0, 0, &cmdInfo, sizeof(cmdInfo));
    }
    
}


void RoomState_Wait::onMsgComputerHeroCanSelect(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(SMsgComputerCanSelect))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgComputerCanSelect *pMsg = (SMsgComputerCanSelect *)data;

    cmd_match_room_computer_hero_can_select cmdInfo;
    cmdInfo.nCamp = pMsg->nCamp;
    cmdInfo.bCanSelect =  (pMsg->bCanSelect == true) ? 1: 0;
    cmdInfo.nHeroID = pMsg->m_HeroID;		//状态


    // 禁用或者是解锁可选择英雄
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CAN_SELECT, 0, 0, &cmdInfo, sizeof(cmdInfo));
}


void RoomState_Wait::release()
{

}

void RoomState_Wait::onMsgComputerGlobleBanHeroList(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(SMsgGlobleBanHeroList))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgGlobleBanHeroList *pMsgHead = (SMsgGlobleBanHeroList *)data;
    int nNum = pMsgHead->nCount;

    if(len != sizeof(int)*(pMsgHead->nCount) + sizeof(SMsgGlobleBanHeroList))
    {
        return;
    }

    int *pHeroFirst = (int *)(pMsgHead + 1);
    obuf obDate;
    cmd_count cmdCount;
    cmdCount.nCount = nNum;
    obDate.push_back(&cmdCount, sizeof(cmd_count));

    for (int i = 0; i < nNum; ++i)
    { 
        //组装信息到显示层
        int HeroID = *pHeroFirst;
        obDate.push_back(&HeroID, sizeof(int) );

        ++pHeroFirst;
    }
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_COMPUTER_RULE_BAN_HERO_LIST, obDate.size(), 0, obDate.data(), obDate.size());
}

void RoomState_Wait::onGVIEWCMD_MATCH_COMPUTERPLAYER_HEROAI_CHANGE(int nParam, const char * strParam, void * ptrParam)
{
    cmd_computerplayer_heroaichange * input = (cmd_computerplayer_heroaichange *)ptrParam;

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_ROOM_COMPUTERPLAYER_HEROAICHANGE;						// 子消息代码  EMRoomMessageCode

    // 消息信息结构
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
    if(pRoom == NULL)
    {
        return;
    }
    uint32 u32CampPlayerCount	= pMatchClient->getCampPlayerCountMax( pRoom->dwMatchTypeID );

    SMsg_ComputerPlayerHeroAIChange MsgInfo;
    // 改变位置信息
    MsgInfo.nPos = input->nPos + u32CampPlayerCount*input->nCamp ;
    MsgInfo.nHeroID = input->nHeroID;                                                   // 英雄ID
    MsgInfo.nAIID = input->nAIID;                                                       // AIID

    obuf256 ob;
    ob << MsgHead<<outData<<MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_Wait::onGVIEWCMD_MATCH_ROOM_PRE_SELECT_HERO( int nParam, const char * strParam,void * ptrParam )
{
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    SMsgCreateRoom_OC* pRoom = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
    if (nullptr == pRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pRoom RoomID= " << m_pRoom->getRoomData()->m_RoomID);
        return;
    }

    SSchemeMatchRoom* pSchemeMatchRoom = m_pRoom->getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom RoomID= " << m_pRoom->getRoomData()->m_RoomID);
        return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_PRE_SELECT_HERO;						// 子消息代码  EMRoomMessageCode

    SMsgSelectHero MsgInfo;
    MsgInfo.nHeroID = static_cast<int>( nParam );

    obuf256 ob;
    ob << MsgHead << outData << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_Wait::restoreState()
{
    return;
}

void RoomState_Wait::onMsgBanHeroFlag(SNetMsgHead* head, void* data, size_t len)
{
    // 通知客户端显示禁用按钮
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_CAN_BAN_HERO_FLAG, 1, 0, 0, 0);
}

void RoomState_Wait::onMsgBanHeroCountDown(SNetMsgHead* head, void* data, size_t len)
{
    // ban选倒计时时间显示
    if (data == NULL || len == 0)
    {
        return;
    }

    if (len != sizeof(int))
    {
        ErrorLn(__FUNCTION__": -- error len!");
        return;
    }

    int* pCountDownTime = (int *)data;
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_CAN_BAN_HERO_COUNT_DOWN, *pCountDownTime, 0, 0, 0);
}

void RoomState_Wait::onGVIEWCMD_MATCH_ROOM_BAN_HERO(int nParam, const char * strParam,void * ptrParam)
{
    cmd_ban_hero * input = (cmd_ban_hero *)ptrParam;

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_POS_BAN_HERO;						// 子消息代码  EMRoomMessageCode

    SMsgPosBanHero_CS MsgInfo;
    // ban选英雄
    MsgInfo.nHeroID = input->nHeroID;                               // 英雄ID

    obuf256 ob;
    ob << MsgHead << outData << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_Wait::onMsgBroadSingleBanHero(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(SMsgBroadBanHead_OC))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgBroadBanHead_OC* pMsg = (SMsgBroadBanHead_OC*)data;
    int nRightLen =pMsg->nCount * sizeof(SMsgBroadBanSub_OC) + sizeof(SMsgBroadBanHead_OC);
    if (nRightLen != len)
    {
        ErrorLn(__FUNCTION__": error len! right rightlen = "<<nRightLen<<" curLen="<<len);
        return;
    }

    // 下发到显示层
    obuf obViewdata;
    cmd_room_banInfo_head cmdData;
    cmdData.nCount = pMsg->nCount;
    cmdData.nCampMaxPlayerCount = pMsg->nCampMaxPlayer;
    obViewdata.push_back(&cmdData, sizeof(cmdData));

    // 排行条目数据结构
    SMsgBroadBanSub_OC *pData = (SMsgBroadBanSub_OC *)(pMsg+1);
    for (int i = 0; i< pMsg->nCount;  i++)
    {
        // 显示层数据
        cmd_room_banInfo_sub cmdInfo;
        cmdInfo.nPos = pData->nPos;
        cmdInfo.nHeroID  = pData->nHeroID;

        obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_SINGLE_BAN_HERO, 0, 0, obViewdata.data(), obViewdata.size());
}



void RoomState_Wait::onMsgAlreadyBanHeroInfo(SNetMsgHead* head, void* data, size_t len)
{
    if (len < sizeof(SMsgBroadBanHead_OC))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgBroadBanHead_OC* pMsg = (SMsgBroadBanHead_OC*)data;
    int nRightLen =pMsg->nCount * sizeof(SMsgBroadBanSub_OC) + sizeof(SMsgBroadBanHead_OC);
    if (nRightLen != len)
    {
        ErrorLn(__FUNCTION__": error len! right rightlen = "<<nRightLen<<" curLen="<<len);
        return;
    }

    // 下发到显示层
    obuf obViewdata;
    cmd_room_banInfo_head cmdData;
    cmdData.nCount = pMsg->nCount;
    cmdData.nCampMaxPlayerCount = pMsg->nCampMaxPlayer;
    obViewdata.push_back(&cmdData, sizeof(cmdData));

    // 排行条目数据结构
    SMsgBroadBanSub_OC *pData = (SMsgBroadBanSub_OC *)(pMsg+1);
    for (int i = 0; i< pMsg->nCount;  i++)
    {
        // 显示层数据
        cmd_room_banInfo_sub cmdInfo;
        cmdInfo.nPos = pData->nPos;
        cmdInfo.nHeroID  = pData->nHeroID;

        obViewdata.push_back(&cmdInfo, sizeof(cmdInfo));
        pData++; 
    }

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_ALL_BAN_HERO, 0, 0, obViewdata.data(), obViewdata.size());
}

void RoomState_Wait::onMsgLegendCupWinMsgInfo(SNetMsgHead* head, void* data, size_t len)
{
    if (data == NULL)
    {
        return;
    }

    if (len < sizeof(SMsg_LegendCupWinMsgInfo))
    {
        return;
    }

    SMsg_LegendCupWinMsgInfo *pMsg = (SMsg_LegendCupWinMsgInfo*)data;
    cmd_war_end_legendcup_info cmd;
    cmd.eRoundType  = pMsg->eRoundType;
    cmd.bRoundLastNode  = pMsg->bRoundLastNode;
    cmd.bIsValid    = 1;
    cmd.bIsOnlineCup = pMsg->bIsOnlineCup;
    cmd.bIsLastRound = pMsg->bIsLastRound;
    cmd.bNodeEnd    = pMsg->bNodeEnd;
    cmd.bWin        = pMsg->bWin;
    cmd.dwNextStartInterval= pMsg->dwNextStartInterval;
    cmd.dwNextRoundStartTime= pMsg->dwNextRoundStartTime;

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_WIN_LEGENDCUP_MSGINFO, 0, 0, &cmd, sizeof(cmd));
}
