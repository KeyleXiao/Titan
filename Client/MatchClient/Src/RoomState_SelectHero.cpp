#include "stdafx.h"
#include "RoomState_SelectHero.h"
#include "MatchDef.h"
#include "ITimerService.h"
#include "MatchSchemeDef.h"
#include "NetMessageDef.h"
#include "IClientGlobal.h"
#include "CurrentMatchRoomInfo.h"
#include "IBankPart.h"


// 选英雄倒计时 TimerID
static uint32 const scu32TimerID_SelectHero = 1;

void RoomState_SelectHero::onEnter()
{		
	MATCH_TRACELN(__FUNCTION__);
	//发到显示层状态
    cmd_change_roomstate cmdInfo;
    cmdInfo.nStateID = getID();
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_STATE,getID(),0,&cmdInfo, sizeof(cmdInfo));
	//MatchRoomState_SelectHero 状态下命令
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_HERO_CHANGE,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_PLAYER_EXIT,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_CONFIRM_HERO,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ROOM_HERO_INFO,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA,this );

}

void RoomState_SelectHero::onLeave()
{
	MATCH_TRACELN(__FUNCTION__);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_HERO_CHANGE);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_PLAYER_EXIT);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_CONFIRM_HERO);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_MATCH_ROOM_HERO_INFO);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA);

}

RoomState_SelectHero::RoomState_SelectHero( CurrentMatchRoomInfo* pRoom )
	:BaseRoomState(pRoom, MatchRoomState_SelectHero)
{

}

RoomState_SelectHero::~RoomState_SelectHero()
{
}

void RoomState_SelectHero::onServerMsg(SNetMsgHead* pHead, void* pData, size_t stLen)
{
	MATCH_TRACELN(__FUNCTION__);

	switch ( MAKEWORD(pHead->byKeyModule, pHead->byKeyAction) )
	{
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ROOM_START_SELECT_HERO):
		{
			onMsgRoomStartSelectHero(pHead, pData, stLen);
		}
		break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_START_ROOM_RULE):
		{
			onMsgStartRoomRule(pHead, pData, stLen);
		}
		break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_HERO_CHANGE):
		{
			onMsgHeroChange(pHead, pData, stLen);
		}
		break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_COMPUTER_HERO_CHANGE):
        {
            onMsgComputerHeroChange(pHead, pData, stLen);
        }
        break;

	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_CONFIRM_HERO):
		{
			onMsgConfirmHero(pHead, pData, stLen);
		}
		break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ROOM_RULE_BAN_HERO):	// 房间规则：禁用英雄
		{
			onMsgRoomRuleBanHero(pHead, pData, stLen);
		}
		break;
			
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_ROOM_RULE_SELECT_HERO): // 房间规则：选择英雄
		{
			onMsgRoomRuleSelectHero(pHead, pData, stLen);
		}
		break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_BAN_HERO_LIST):		// 房间规则：禁用英雄列表
		{
			onMsgBanHeroList(pHead, pData, stLen);
		}
		break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_HERO_CAN_SELECT):		// 房间规则：禁用英雄列表
		{
			onMsgHeroCanSelect(pHead, pData, stLen);
		}
		break;
	case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_GLOBLE_BAN_HERO_LIST):		// 全局禁用列表
		{
			onMsgGlobleBanHeroList(pHead, pData, stLen);
		}
		break;
    case MAKEWORD(MSG_MODULEID_MATCH,OC_MSG_POS_BROAD_ALL_BAN_HERO):
        {
            onMsgAllBanHeroInfo(pHead, pData, stLen);
        }
        break;
	default:
		{ 
			BaseRoomState::onServerMsg(pHead, pData, stLen);
		}
	}
}

void RoomState_SelectHero::onMsgAllBanHeroInfo(SNetMsgHead* head, void* data, size_t len)
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


void RoomState_SelectHero::onMsgConfirmHero( SNetMsgHead* head, void* pData, size_t stLen )
{
	SOC_MSG_CONFIRM_HERO* pSOC_MSG_CONFIRM_HERO;
	if ( !getMsg( pSOC_MSG_CONFIRM_HERO, pData, stLen ) )
	{
		ErrorLn(__FUNCTION__": failed! !getMsg");
		return;
	}

	cmd_match_room_confirm_hero cmdMatchRoomConfirmHero;
	cmdMatchRoomConfirmHero.nCamp			= m_pRoom->getPlayerCamp( pSOC_MSG_CONFIRM_HERO->m_Pos );
	cmdMatchRoomConfirmHero.nPlayerSlotID	= m_pRoom->getPlayerCampPos( pSOC_MSG_CONFIRM_HERO->m_Pos );
	cmdMatchRoomConfirmHero.nSelf			= (pSOC_MSG_CONFIRM_HERO->m_Pos == m_pRoom->getRoomData()->m_SelfPos) ? 1 : 0;

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_CONFIRM_HERO, 0, nullptr, &cmdMatchRoomConfirmHero, sizeof(cmdMatchRoomConfirmHero));
}

void RoomState_SelectHero::onMsgHeroChange( SNetMsgHead* head, void* data, size_t len )
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


void RoomState_SelectHero::onMsgComputerHeroChange(SNetMsgHead* head, void* data, size_t len)
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
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_COMPUTER_HERO_CHANGE, 0, 0, &cmdInfo, sizeof(cmdInfo));
}

void RoomState_SelectHero::onMsgRoomStartSelectHero( SNetMsgHead* head, void* data, size_t len )
{
    if (len != sizeof(DWORD))
    {
        ErrorLn(__FUNCTION__": len error!");
        return;
    }
    IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
    if (pMatchClient == NULL)
    {
        ErrorLn(__FUNCTION__": pMatchClient == NULL!");
        return;
    }
	SMsgCreateRoom_OC* pRoomData = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
	if (nullptr == pRoomData)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoomData RoomID=" << m_pRoom->getRoomData()->m_RoomID);
		return;
	}
	SSchemeMatchRoom* pSchemeMatchRoom = m_pRoom->getSchemeMatchRoom();
	if (nullptr == pSchemeMatchRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom RoomID=" << m_pRoom->getRoomData()->m_RoomID);
		return;
	}

	MATCH_TRACELN(__FUNCTION__": m_RoomData.m_MasterPos=" << m_pRoom->getRoomData()->m_MasterPos<<" m_SelfPos=" << m_pRoom->getRoomData()->m_SelfPos);

	if (m_pRoom->getRoomData()->m_SelfPos == m_pRoom->getRoomData()->m_MasterPos)
	{// 房主
		MATCH_TRACELN(__FUNCTION__": is master");
		if (MatchRoomState_SelectHero != pRoomData->byState)
		{
			ErrorLn(__FUNCTION__": MatchRoomState_SelectHero != pRoomData->m_byState byState= " << pRoomData->byState);
			return;
		}
	}
	else
	{
		MATCH_TRACELN(__FUNCTION__": is member");
		if (MatchRoomState_SelectHero != pRoomData->byState)
		{
			ErrorLn(__FUNCTION__": MatchRoomState_SelectHero != pRoomData->m_byState pRoomData->m_byState= " << pRoomData->byState);
			return;
		}
	}
	
    DWORD* pSelectHeroTime = (DWORD*)data;
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_START_SELECT_HERO, *pSelectHeroTime, 0, 0, 0);
}


bool RoomState_SelectHero::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{
	switch ( eventID )
	{
	case GVIEWCMD_MATCH_ROOM_HERO_CHANGE:
		{
			onGVIEWCMD_MATCH_ROOM_HERO_CHANGE(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_MATCH_ROOM_PLAYER_EXIT:
		{
			onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_MATCH_ROOM_CONFIRM_HERO:
		{
			onGVIEWCMD_MATCH_ROOM_CONFIRM_HERO(nParam, strParam, ptrParam);
		}
		break;
	case GVIEWCMD_MATCH_ROOM_HERO_INFO:
		{
			onGVIEWCMD_MATCH_ROOM_HERO_INFO(nParam, strParam, ptrParam);
		}
		break;
    case GVIEWCM_MATCH_MOUSE_LEFT_HERO_AREA:
        {
           onGVIEWCMD_MATCH_FROM_GETHREO_DES(nParam, strParam, ptrParam);
        }
        break;    
	}
	return true;
}

void RoomState_SelectHero::onGVIEWCMD_MATCH_ROOM_HERO_INFO( int nParam, const char * strParam, void * ptrParam )
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



void RoomState_SelectHero::onGVIEWCMD_MATCH_ROOM_CONFIRM_HERO( int nParam, const char * strParam, void * ptrParam )
{

	obuf256 ob;
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
	MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_CONFIRM_SELECT_HERO;						// 子消息代码  EMRoomMessageCode

	ob << MsgHead<<outData;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_SelectHero::onGVIEWCMD_MATCH_ROOM_PLAYER_EXIT( int nParam, const char * strParam, void * ptrParam )
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


void RoomState_SelectHero::onGVIEWCMD_MATCH_ROOM_HERO_CHANGE( int nParam, const char * strParam,void * ptrParam )
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

	
	SNetMsgHead MsgHead;
	MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
	MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_SELECT_HERO;						// 子消息代码  EMRoomMessageCode

    obuf256 ob;
	ob << MsgHead<<outData;

	SMsgSelectHero MsgInfo;
	MsgInfo.nHeroID = static_cast<int>( nParam );
	ob << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void RoomState_SelectHero::onMsgRoomRuleBanHero(SNetMsgHead* head, void* data, size_t len)
{

	if (len < sizeof(SMsgRuleBanHero))
	{
		ErrorLn(__FUNCTION__":  len= " << len);
		return;
	}
	SMsgRuleBanHero *pMsgHead = (SMsgRuleBanHero *)data;
	int nPosNum = pMsgHead->nCanBanPosCount;

	if(len != sizeof(SMsgRulePos)*pMsgHead->nCanBanPosCount + sizeof(SMsgRuleBanHero))
	{
		return;
	}

	SMsgRulePos *pPosFirst = (SMsgRulePos *)(pMsgHead + 1);

	obuf obData;
	cmd_match_room_ban_hero_count cmdCount;
	cmdCount.nCount = nPosNum;
	obData.push_back(&cmdCount, sizeof(cmd_match_room_ban_hero_count ));

	for (int i = 0; i < nPosNum; ++i)
	{ 
		//组装信息到显示层
		cmd_match_room_rule_ban_hero cmdInfo;
		cmdInfo.nStepID =  pMsgHead->nStepID;
		cmdInfo.nRuleType = pMsgHead->nRuleType;		//状态
		cmdInfo.nTimeInterval = pMsgHead->nTimeInterval; // 流程持续时间
		cmdInfo.bIsSelf = (m_pRoom->getRoomData()->m_SelfPos == pPosFirst->m_Pos)? 1: 0;
		cmdInfo.nCamp = m_pRoom->getPlayerCamp(pPosFirst->m_Pos);
		cmdInfo.nPos = m_pRoom->getPlayerCampPos(pPosFirst->m_Pos);

		obData.push_back(&cmdInfo, sizeof( cmd_match_room_rule_ban_hero));

		//gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_RULE_BAN_HERO, 0, 0, &cmdInfo, sizeof(cmdInfo));
		++pPosFirst;
	}
	// 由于第5个参数传不过去，  用第二个参数代替长度
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_RULE_BAN_HERO, obData.size(), 0, obData.data(), obData.size());
}

void RoomState_SelectHero::onMsgRoomRuleSelectHero(SNetMsgHead* head, void* data, size_t len)
{
	if (len < sizeof(SMsgRuleSelectHero))
	{
		ErrorLn(__FUNCTION__":  len= " << len);
		return;
	}
	SMsgRuleSelectHero *pMsgHead = (SMsgRuleSelectHero *)data;
	int nPosNum = pMsgHead->nCanSelectCount;

	if(len != sizeof(SMsgRulePos)*pMsgHead->nCanSelectCount + sizeof(SMsgRuleBanHero))
	{
		return;
	}

	SMsgRulePos *pPosFirst = (SMsgRulePos *)(pMsgHead + 1);

	obuf obData;
	cmd_count cmdCount;
	cmdCount.nCount = nPosNum;
	obData.push_back(&cmdCount, sizeof(cmdCount));
	for (int i = 0; i < nPosNum; ++i)
	{ 
		//组装信息到显示层
		cmd_match_room_rule_ban_hero cmdInfo;
		cmdInfo.nStepID =  pMsgHead->nStepID;
		cmdInfo.nRuleType = pMsgHead->nRuleType;		//状态
		cmdInfo.nTimeInterval = pMsgHead->nTimeInterval; // 流程持续时间
		cmdInfo.bIsSelf = (m_pRoom->getRoomData()->m_SelfPos == pPosFirst->m_Pos)? 1: 0;
		cmdInfo.nCamp = m_pRoom->getPlayerCamp(pPosFirst->m_Pos);
		cmdInfo.nPos = m_pRoom->getPlayerCampPos(pPosFirst->m_Pos);
		obData.push_back(&cmdInfo, sizeof(cmdInfo));
		++pPosFirst;
	}
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_RULE_SELECT_HERO,0,0, obData.data(), obData.size() );
}

void RoomState_SelectHero::onMsgBanHeroList(SNetMsgHead* head, void* data, size_t len)
{

	if (len < sizeof(SMsgRuleBanHeroList))
	{
		ErrorLn(__FUNCTION__":  len= " << len);
		return;
	}
	SMsgRuleBanHeroList *pMsgHead = (SMsgRuleBanHeroList *)data;
	int nNum = pMsgHead->nCount;

	if(len != sizeof(int)*pMsgHead->nCount + sizeof(SMsgRuleBanHeroList))
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
		// 禁用英雄列表
		++pHeroFirst;
	}
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_RULE_BAN_HERO_LIST, obDate.size(), 0, obDate.data(), obDate.size());
}

void RoomState_SelectHero::onMsgStartRoomRule(SNetMsgHead* head, void* data, size_t len)
{

	IMatchClient *pMatchClient = gClientGlobal->getMatchClient();
	SMsgCreateRoom_OC* pRoomData = pMatchClient->getRoomData( m_pRoom->getRoomData()->m_RoomID );
	if (nullptr == pRoomData)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoomData RoomID=" << m_pRoom->getRoomData()->m_RoomID);
		return;
	}

	if (m_pRoom->getRoomData()->m_SelfPos == m_pRoom->getRoomData()->m_MasterPos)
	{// 房主
		if (MatchRoomState_SelectHero != pRoomData->byState)
		{
			MATCH_ERRORLN(__FUNCTION__": MatchRoomState_SelectHero != pRoomData->m_byState byState= " << pRoomData->byState);
			return;
		}
	}
	else
	{
		if (MatchRoomState_SelectHero != pRoomData->byState)
		{
			ErrorLn(__FUNCTION__": MatchRoomState_SelectHero != pRoomData->m_byState pRoomData->m_byState= " << pRoomData->byState);
			return;
		}
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_START_ROOM_RULE, 0, 0, 0, 0);
}

void RoomState_SelectHero::onMsgHeroCanSelect(SNetMsgHead* head, void* data, size_t len)
{
   
    if (len < sizeof(SMsgCanSelect))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }

    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return;
    }

    IBankPart *pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
    if (pBankPart == NULL)
    {
        return;
    }

	SMsgCanSelect *pMsg = (SMsgCanSelect *)data;

    //还得判定自己能否选择
    if (pMsg->bCanSelect == true)
    {
        if (!pBankPart->isSystemHero(pMsg->m_HeroID))
        {
            pMsg->bCanSelect = false;
        }
    }
	
	cmd_match_room_hero_can_select cmdInfo;
	cmdInfo.bCanSelect =  (pMsg->bCanSelect == true) ? 1: 0;
	cmdInfo.nHeroID = pMsg->m_HeroID;		//状态

	
	// 禁用或者是解锁可选择英雄
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_HERO_CAN_SELECT, 0, 0, &cmdInfo, sizeof(cmdInfo));

}

void RoomState_SelectHero::onMsgGlobleBanHeroList(SNetMsgHead* head, void* data, size_t len)
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
		// 禁用英雄列表
		//gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_RULE_BAN_HERO_LIST, HeroID, 0, 0, 0);
		++pHeroFirst;
	}
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ROOM_GLOBLE_BAN_HERO_LIST, obDate.size(), 0, obDate.data(), obDate.size());
}

void RoomState_SelectHero::onGVIEWCMD_SUMMONER_SKILL_RETURN(int nParam, const char * strParam, void * ptrParam)
{
    cmd_SmSillInfo_Return* pCmdInfo = (cmd_SmSillInfo_Return*)ptrParam;
    //TODO 将召唤师的技能id发送到服务器， 服务器设置id并存储起来
}

void RoomState_SelectHero::onGVIEWCMD_MATCH_FROM_GETHREO_DES( int nParam, const char * strParam, void * ptrParam )
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

void RoomState_SelectHero::release()
{
    return;
}

void RoomState_SelectHero::restoreState()
{
    return;
}
