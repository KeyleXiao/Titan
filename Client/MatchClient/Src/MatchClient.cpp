#include "stdafx.h"
#include "MatchClient.h"
#include "IClientEntity.h"
#include "GameViewDef.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"
#include "IChatClient.h"
#include "ISpell.h"
#include "IStage_Manager.h"
#include "ILoginServerProtocol.h"
#include "CurrentMatchRoomInfo.h"
#include "EntityDef.h"


MatchClient* g_pMatchClient = NULL;
void MatchClient::release(void)
{
    gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_MATCH, this);

    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_EXIT_HALL);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_ENTER_HALL);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_USER_EXITGAME );
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_USER_EXIT_WAR );
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_OPEN_CREATE_ROOM);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_OPEN_CREATE_ROOM_MODES);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_CREATE_ROOM);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCM_MATCH_HALL_ROOM_DATA);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCM_MATCH_TO_CHOOSE_VOCATION);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_ENTER_ROOM);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_BEGIN_MATCH);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_CANCEL_MATCH);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_GUIDE_INFO);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_CREATE_GUIDE_ROOM);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_CONFIRM_ACCEPTCHECK);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_REFUSE_ACCEPTCHECK);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_CUSTOMER_INVITE_RESPON);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MASTER_VIEW_LOADED);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_MATCH_RECOMMEND_RANK_RESULT);
    
    

	// 全局指针NULL
	g_pMatchClient = NULL;

	delete this;
}


bool MatchClient::Create(void)
{

    IMessageDispatch* pMessageDispatch = gClientGlobal->getMessageDispatch();
    if (pMessageDispatch == NULL)
    {
        return false;
    }

    pMessageDispatch->registerMessageHandler(MSG_MODULEID_MATCH, this);
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_EXIT_HALL,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ENTER_HALL,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_USER_EXITGAME,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_USER_EXIT_WAR,this );

    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_OPEN_CREATE_ROOM,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_OPEN_CREATE_ROOM_MODES,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_CREATE_ROOM,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCM_MATCH_HALL_ROOM_DATA,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCM_MATCH_TO_CHOOSE_VOCATION,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_ENTER_ROOM,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_BEGIN_MATCH,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_CANCEL_MATCH,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_GUIDE_INFO,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_CREATE_GUIDE_ROOM,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_CONFIRM_ACCEPTCHECK,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_REFUSE_ACCEPTCHECK,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_CUSTOMER_INVITE_RESPON,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MASTER_VIEW_LOADED,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_MATCH_RECOMMEND_RANK_RESULT,this );
    
    
    

	return true;
}

bool MatchClient::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{
    switch ( eventID )
    {
    case GVIEWCMD_USER_EXITGAME:
        {
            onViewSendExitGameMsg();
        }
        break;

    case GVIEWCMD_USER_EXIT_WAR:
        {
            onViewSendExitWarMsg();
        }
        break;


    case GVIEWCMD_MATCH_OPEN_CREATE_ROOM:
        {
            onViewCmdMatchOpenCreateRoom(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_MATCH_CREATE_ROOM:
        {
            onViewCmdMatchCreateRoom(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_MATCH_GUIDE_INFO:
        {
            onViewCmdMatchGuideInfo(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_MATCH_ENTER_ROOM:
        {
            onViewCmdMatchEnterRoom(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCM_MATCH_HALL_ROOM_DATA:
        {
            onViewCmdMatchHallRoomData(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCM_MATCH_TO_CHOOSE_VOCATION:
        {
            onViewCmdMatchToChooseVocation(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_MATCH_ENTER_HALL:         // 点击游戏大厅
        {
            onViewCmdEnterHall(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_MATCH_EXIT_HALL:         // 点击离开大厅
        {
            onViewCmdExitHall(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_MATCH_BEGIN_MATCH:         // 开始撮合
        {
            onViewCmdBeginMatch(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_MATCH_CANCEL_MATCH:         // 取消撮合
        {
            onViewCmdCancelMatch(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_CREATE_GUIDE_ROOM:         // 创建引导房间
        {
            onViewCmdCreateGuideRoom(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_MATCH_CONFIRM_ACCEPTCHECK:         // 接受撮合比赛
        {
            onViewCmdAcceptMatch(nParam, strParam, ptrParam);
        }
        break;
    case GVIEWCMD_MATCH_REFUSE_ACCEPTCHECK:         // 拒绝撮合比赛
        {
            onViewCmdRefuseMatch(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_MATCH_CUSTOMER_INVITE_RESPON:         // 自定义邀请比赛
        {
            onViewCmdCustomerInviteRespon(nParam, strParam, ptrParam);
        }
        break;

    case GVIEWCMD_MASTER_VIEW_LOADED:                   // 主角创建成功
        {
            onViewMasterViewLoaded(nParam, strParam, ptrParam);
        }
        break;
	case GVIEWCMD_MATCH_RECOMMEND_RANK_RESULT:
		{
			onViewCmdMatchRecommendResult(nParam, strParam, ptrParam);
		}
        break;

    default:
        ErrorLn( __FUNCTION__": unhandled eventID=" << eventID );
        return false;
    }
        
    return true;
}

void MatchClient::onMessage(SNetMsgHead* head, void* data, size_t len)
{
    if (head->byKeyAction == OC_MSG_ROOM_CLIENT_MSSAGE_INFO)
    {
        // 公共消息
        if (data == NULL || len != sizeof(DWORD))
        {
            WarningLn(__FUNCTION__": data == NULL || len == 0");
            return;
        }
        DWORD* dwChatID = (DWORD *)data;
        gClientGlobal->getChatClient()->showSystemMessage(EMChatTipID(*dwChatID));
        return;
    }

    if (gClientGlobal->getCurState() == ESS_MATCHROOM)
    {
        // 房间消息
        CurrentMatchRoomInfo::getInstance().onMatchMessage(head, data, len);
    }
    else
    {
        switch ( head->byKeyAction )
        {
        case OC_MSG_CREATE_ROOM:
            {
                onMsgCreateRoom(head, data, len);
            }
            break;
        case OC_MSG_ROOM:
            {
                onMsgRoom(head, data, len);
            }
            break;
        case OC_MSG_ENTER_ROOM:
            {
                onMsgEnterRoom(head, data, len);
            }
            break;
        case OC_MSG_OB_ENTER_ROOM:
            {
                onMsgOBEnterRoom(head, data, len);
            }
            break;
        case OC_MSG_ROOM_ROLE_COUNT:
            {
                onMsgRoomRoleCount(head, data, len);
            }
            break;
        case OC_MSG_OUTSIDE_ROOM_STATE:
            {
                onMsgRoomState(head, data, len);
            }
            break;
        case OC_MSG_RELEASE_ROOM:
            {
                onMsgReleaseRoom(head, data, len);
            }
            break;
        case OC_MSG_EXIT_HALL:
            {
                onMsgExitHall(head, data, len);
            }
            break;
        case OC_MSG_ENTER_HALL:
            {
                onMSEnterHall(head, data, len);
            }
            break;
        case OC_MSG_EXIT_ROOM_HALL_ROOM:        // 离开房间后获得的大厅房间数据
            {
                onMsgExitRoomToHall(head, data, len);
            }
            break;
		case OC_MSG_EXIT_HALL_FROM_LEGENDCUPROOM:
			{
				onMsgExitHallFromLegendCupRoom(head, data, len);
			}
			break;
        case SC_MSG_CANCEL_MATCH:        
            {
                onMsgCancelMatch(head, data, len);
            }
            break;
        case SC_MSG_IS_MATCHING:        
            {
                onMsgIsMatching(head, data, len);
            }
            break;
        case SC_MSG_MATCH_OK:        
            {
                onMsgMatchOK(head, data, len);
            }
            break;
        case SC_MSG_MATCH_FAIL:        
            {
                onMsgMatchFail(head, data, len);
            }
            break;

        case OC_MSG_MATCH_BROAD_WAITACCEPT:        
            {
                onMsgWaitAccept(head, data, len);
            }
            break;

        case OC_MSG_MATCH_CUR_MATCHPLAYER_COUT:        
            {
                onMsgMatchPlayerCountInfo(head, data, len);
            }
            break;
            
        case OC_MSG_MATCH_BROAD_OTHER_ACCEPTMATCH:        
            {
                onMsgBroadOtherAcceptmatch(head, data, len);
            }
            break;

        case SC_MSG_MATCH_REFUSE:        
            {
                onMsgGetRefuseMatch(head, data, len);
            }
            break;

        case OC_MSG_MATCH_SLEF_PUNISH_TIMEINFO:        
            {
                onMsgSelfPunishInfo(head, data, len);
            }
            break;

        case SC_MSG_MATCH_REQUEST_PUNISH_TIMEINFO:        
            {
                onMsgMatchRequestPunishInfo(head, data, len);
            }
            break;
        case OC_MSG_MATCH_DEL_SLEF_PUNISH_TIMEINFO:        
            {
                onMsgDelPunishInfo(head, data, len);
            }
            break;
        case OC_MSG_MATCH_OPEN_OR_CLOSE:        
            {
                onMsgMatchCloseOrOpen(head, data, len);
            }
            break;
		case OC_MSG_MATCH_RECOMMEND_RANK:
			{
				onMsgMatchRecommendRank(head, data, len);
			}
			break;
        default:
            {
                ErrorLn(__FUNCTION__": unknown head->byKeyAction= " << head->byKeyAction);
            }
            break;
        }
    }
}


void MatchClient::onMsgGetRefuseMatch(SNetMsgHead* head, void* data, size_t len)
{
    // 比赛被拒绝了
    m_nMatchType = MatchType_None;
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_HIDE_ACCEPT_CHECK, 0, 0, 0, 0);
}

void MatchClient::onMsgWaitAccept(SNetMsgHead* head, void* data, size_t len)
{
    if(data == NULL)
    {
        ErrorLn(__FUNCTION__": data == NULL!");
        return;
    }
    //撮合请求 接受 or 拒绝
    if (len != sizeof(SMsgMatchAcceptCheck))
    {
        ErrorLn(__FUNCTION__": length error");
        return;
    }
    SMsgMatchAcceptCheck *pData = (SMsgMatchAcceptCheck *)data;
    
    // 通知客户端显示排队相关
    cmd_wait_accept_wnd cmdData;
    cmdData.dwAcceptCheckID = pData->dwAcceptCheckID;
    cmdData.byMatchType     = pData->byMatchType;
    cmdData.nWaitTime       = pData->nWaitTime;

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCH_SHOW_ACCEPT_CHECK,0,0,&cmdData, sizeof(cmdData));

}


void MatchClient::onMsgMatchPlayerCountInfo(SNetMsgHead* head, void* data, size_t len)
{
    if(data == NULL)
    {
        ErrorLn(__FUNCTION__": data == NULL!");
        return;
    }
    //撮合请求 接受 or 拒绝
    if (len != sizeof(SMsgMatchCurPlayerCount))
    {
        ErrorLn(__FUNCTION__": length error");
        return;
    }
    SMsgMatchCurPlayerCount *pData = (SMsgMatchCurPlayerCount *)data;

    // 通知客户端显示排队相关
    cmd_match_cur_player_count cmdData;
    cmdData.nMaxCount = pData->nMaxCount;
    cmdData.nCurCount = pData->nCurCount;
    cmdData.nLeftTime = pData->nLeftTime;
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCH_CUR_MATCH_COUNT_INFO,0,0,&cmdData, sizeof(cmdData));
}

void MatchClient::onMsgMatchRequestPunishInfo(SNetMsgHead* head, void* data, size_t len)
{
    if(data == NULL)
    {
        ErrorLn(__FUNCTION__": data == NULL!");
        return;
    }
    //惩罚信息
    if (len < sizeof(SMsgRequestPunishInfoHead_OC))
    {
        ErrorLn(__FUNCTION__": length error");
        return;
    }

    
    SMsgRequestPunishInfoHead_OC *pHeadData = (SMsgRequestPunishInfoHead_OC *)data;
    int rightLen = pHeadData->nCount * sizeof(SMsgRequestPunishInfo_OC) + sizeof(SMsgRequestPunishInfoHead_OC);
    if (len != rightLen)
    {
        ErrorLn(__FUNCTION__": length error! len="<<len<<" rightLen="<<rightLen);
        return;
    }
    obuf obViewData;
    cmd_match_request_punish_head viewhead;
    viewhead.nCount = pHeadData->nCount;
    viewhead.bIsTeamRequest = pHeadData->bIsTeamRequest;
    obViewData.push_back(&viewhead,sizeof(viewhead));

    SMsgRequestPunishInfo_OC *pData = (SMsgRequestPunishInfo_OC *)(pHeadData + 1);
    for (int i = 0; i < pHeadData->nCount; ++i)
    {
        cmd_match_request_punish_info subInfo;
        subInfo.byPunishType = pData->ePunishType;
        subInfo.nLeftTime   = pData->dwLeftTime;
        sstrcpyn(subInfo.szName, pData->szName, sizeof(subInfo.szName));
        
        obViewData.push_back(&subInfo,sizeof(subInfo));
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCH_REQUEST_PUNISHINFO,0,0,obViewData.data(),obViewData.size());
}


void MatchClient::onMsgDelPunishInfo(SNetMsgHead* head, void* data, size_t len)
{
    if(data == NULL)
    {
        ErrorLn(__FUNCTION__": data == NULL!");
        return;
    }
    //惩罚信息
    if (len < sizeof(SMsgDelPunishInfo_OC))
    {
        ErrorLn(__FUNCTION__": length error");
        return;
    }


    SMsgDelPunishInfo_OC *pData = (SMsgDelPunishInfo_OC *)data;
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_DELETE_PUNISH_TYPE_INFO, pData->byPunishType,0,0,0);
}


void MatchClient::onMsgMatchCloseOrOpen(SNetMsgHead* head, void* data, size_t len)
{
    if(data == NULL)
    {
        ErrorLn(__FUNCTION__": data == NULL!");
        return;
    }
    //惩罚信息
    if (len < sizeof(SMsgMatchOpenOrClose))
    {
        ErrorLn(__FUNCTION__": length error");
        return;
    }


    SMsgMatchOpenOrClose *pData = (SMsgMatchOpenOrClose *)data;
    cmd_match_close_or_open cmd;
    cmd.bActorLogin = pData->bActorLogin == true? 1: 0;
    cmd.bOpen = pData->bOpen == true? 1: 0;
    cmd.nMatchType = pData->nMatchType;
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCH_OPEN_OR_CLOSE, 0,0,&cmd,sizeof(cmd));
}

void MatchClient::onMsgMatchRecommendRank(SNetMsgHead* head, void* data, size_t len)
{
	gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCH_RECOMMEND_RANK, 0, 0, NULL, 0);
}

void MatchClient::onViewCmdMatchRecommendResult( int nParam, const char * strParam, void * ptrParam )
{
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_SCENE;
	head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_MATCH;
	head.byKeyAction    = MSG_MATCH_DATE_MODULEMSG;

	SMsgMatchDataSubMsg outData;
	outData.dwMsgCode	= CS_MSG_MATCH_RECOMMEND_RESULT;	    // 子消息代码
	outData.dwUserData1	= nParam;								// 用户数据1
	outData.dwUserData2	= 0;						            // 用户数据2
	outData.dwUserData3	= 0;						            // 用户数据3

	obuf256 ob;
	ob <<head<<outData;

	net_senddata( ob.data(), ob.size() );
}



void MatchClient::onMsgSelfPunishInfo(SNetMsgHead* head, void* data, size_t len)
{
    if(data == NULL)
    {
        ErrorLn(__FUNCTION__": data == NULL!");
        return;
    }
    //惩罚信息
    if (len < sizeof(SMsgSelfPunishInfoHead_OC))
    {
        ErrorLn(__FUNCTION__": length error");
        return;
    }
    
    SMsgSelfPunishInfoHead_OC *pHeadData = (SMsgSelfPunishInfoHead_OC *)data;
    int rightLen = pHeadData->nCount * sizeof(SMsgSelfPunishInfo_OC) + sizeof(SMsgSelfPunishInfoHead_OC);
    if (len != rightLen)
    {
        ErrorLn(__FUNCTION__": length error! len="<<len<<" rightLen="<<rightLen);
        return;
    }

    obuf obViewData;
    cmd_self_punish_head viewhead;
    viewhead.nCount = pHeadData->nCount;
    obViewData.push_back(&viewhead,sizeof(viewhead));

    SMsgSelfPunishInfo_OC *pData = (SMsgSelfPunishInfo_OC *)(pHeadData + 1);
    for (int i = 0; i < pHeadData->nCount; ++i)
    {
        cmd_self_punish_info subInfo;
        subInfo.byPunishType = pData->ePunishType;
        subInfo.nLeftTime   = pData->dwLeftTime;

        obViewData.push_back(&subInfo,sizeof(subInfo));

        pData++;
    }

    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCH_SELF_PUNISHINFO,0,0,obViewData.data(),obViewData.size());

}

void MatchClient::onMsgBroadOtherAcceptmatch(SNetMsgHead* head, void* data, size_t len)
{

    // 广播其他人接收比赛信息信息
    //gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_MATCH_ACCEPT_CHECK, 0, 0, 0, 0);
}

void MatchClient::showHall()
{
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_EXIT_ROOM_ENTER_HALL, 0, 0, 0, 0);
    size_t stCount = m_mapRooms.size();
    TraceLn("MatchClient::showHall m_mapRooms.size()= " << stCount);

    obuf obData;
    cmd_match_hall_rooms_head cmdHead;
    cmdHead.nRoomCount = m_mapRooms.size();
    obData<<cmdHead;
    MAP_Rooms::iterator itor = m_mapRooms.begin();
    for (;itor != m_mapRooms.end(); ++itor)
    {
        SMsgCreateRoom_OC& roomData = itor->second;
        cmd_match_hall_add_room cmdMatchHallAddRoom;
        make(&roomData, cmdMatchHallAddRoom);
        obData<<cmdMatchHallAddRoom;
    }
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_HALL_ADD_ROOM, 0, 0, obData.data(), obData.size());
}



void MatchClient::onMatchMessage( SNetMsgHead* head, void* data, size_t len )
{
    TraceLn(__FUNCTION__": len= " << len<<"head->byKeyAction =" <<head->byKeyAction );

    switch ( head->byKeyAction )
    {


    default:
        {
            ErrorLn(__FUNCTION__": unknown head->byKeyAction= " << head->byKeyAction);
        }
    }
}

void MatchClient::onMsgExitRoomToHall( SNetMsgHead* head, void* data, size_t len )
{    
}

void MatchClient::onMsgCreateRoom( SNetMsgHead* head, void* data, size_t len )
{
    TraceLn(__FUNCTION__": len= " << len);

    if (len < sizeof(SMsgCreateRoom))
    {
        ErrorLn(__FUNCTION__": len= " << len);
        return;
    }
    SMsgCreateRoom* pMsg = (SMsgCreateRoom*)data;

	data = (void*)(pMsg + 1);
	len -= sizeof(SMsgCreateRoom);
	if ( !pMsg->m_Result )
	{
		// todo:pt 通知view 创建房间失败
		TraceLn(__FUNCTION__": Result= " << pMsg->m_Result);
		return;
	}

	if (len < sizeof(SMsgSelfEnterRoomData))
	{
		ErrorLn(__FUNCTION__": len= " << len);
		return;
	}
	SMsgSelfEnterRoomData* pMsgEx = (SMsgSelfEnterRoomData*)data;
	SMsgCreateRoom_OC* pRoomData = getRoomData( pMsgEx->m_RoomID );
	if  (nullptr == pRoomData)
	{
		ErrorLn(__FUNCTION__": nullptr == pRoomData pMsgCreateRoom_Ex->m_RoomID= " << pMsgEx->m_RoomID);
		return;
	}

	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
		return;
	ISchemeMatchRoom * pMatchRoom = pCenter->getSchemeMatchRoom();
	if (pMatchRoom == NULL)
		return;
	SSchemeMatchRoom * pSchemeRoom = pMatchRoom->getSchemeMatchRoomByMatchTypeID(pRoomData->dwMatchTypeID);
	if (pSchemeRoom == NULL)
		return;
	
    obuf obData;
    cmd_count cmdCount;
    cmdCount.nCount = SUMMONER_SKILL_COUNT;
    obData << cmdCount;

    ISchemeSummonerSkill* pInfo = gClientGlobal->getSchemeCenter()->getSchemeSummonerSkill();
    if( pInfo == NULL )
    {
        return;
    }

    ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
    if (pSpellFactory == NULL)
    {
        return;
    }

    for (int i = 0; i < SUMMONER_SKILL_COUNT;++i )
    {
        int nSummonerSkillId = pMsg->m_SkillId[i];
        SSummonerSkillInfo * pSummonerInfo=pInfo->getSummonerSkillInfo(nSummonerSkillId);
        if(pSummonerInfo==NULL)
        {
            continue;
        }
        cmd_SummonerSkill_Info cmdSkillInfo;
        cmdSkillInfo.nIndex = i;
        cmdSkillInfo.nSkillId = nSummonerSkillId;
        SPELL_DATA *pSpellData = pSpellFactory->getSpellData(cmdSkillInfo.nSkillId);
        if (pSpellData != NULL)
        {
            cmdSkillInfo.nIconId = pSpellData->nIcon;
        }

        strcpy_s(cmdSkillInfo.summonerSkillDesc, sizeof(cmdSkillInfo.summonerSkillDesc),pSummonerInfo->stDesc );
        obData << cmdSkillInfo;
    }
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCH_SUMMONER_DEFAULT_SKILL,0,0,obData.data(),obData.size());
    // 将召唤师的技能发送过去
    sendAllSummonerSkillInf(pRoomData->nModeID);



    {
        obuf256 obData;
        obData << (*pMsgEx);
        gClientGlobal->gotoState( ESS_MATCHROOM,obData.data(),obData.size() );
        CurrentMatchRoomInfo::getInstance().inital(obData.data(),obData.size(), pRoomData->bGuidRoom);
        cmd_send_war_typeid cmdData;
        cmdData.nConfigWarID = pRoomData->nConfigWarID;
        cmdData.nMatchTypeID = pRoomData->dwMatchTypeID;
        gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_PLAY_SEND_WAR_TYPEID,pRoomData->nConfigWarID,0,&cmdData,sizeof(cmdData));
    }
}

void MatchClient::sendAllSummonerSkillInf(int nWarMode)
{
    ISchemeSummonerSkill* pInfo = gClientGlobal->getSchemeCenter()->getSchemeSummonerSkill();
    if( pInfo == NULL )
    {
        return;
    }
    ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
    if (pSpellFactory == NULL)
    {
        return;
    }

    // 将所有技能信息发送到显示层
    std::map<int, SSummonerSkillInfo>* pSkillMap = pInfo->getSummonerSkillByWarMode(nWarMode);
    if ( pSkillMap == NULL )
    {
        return;
    }

    // 临时定义的当前等级
    int nPlayerLv = 1;
    std::map<int, SSummonerSkillInfo>::iterator itBegin = pSkillMap->begin();

    obuf obSmData;

    cmd_count cmdSmCount;
    cmdSmCount.nCount = pSkillMap->size();
    obSmData << cmdSmCount;
    for (; itBegin != pSkillMap->end(); ++ itBegin )
    {
        SSummonerSkillInfo skillInfo = itBegin->second;
        cmd_SmAllSkillInfo cmdSkillInfo;
        cmdSkillInfo.nId = skillInfo.nId;

        SPELL_DATA *pSpellData = pSpellFactory->getSpellData(cmdSkillInfo.nId);
        if (pSpellData != NULL)
        {
            cmdSkillInfo.nIconId = pSpellData->nIcon;
        }

        cmdSkillInfo.nIsUnlock = skillInfo.nLv <= nPlayerLv ? 1:0;
        strcpy_s(cmdSkillInfo.summonerSkillDesc, sizeof(cmdSkillInfo.summonerSkillDesc),skillInfo.stDesc );
        obSmData << cmdSkillInfo;
    }

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_SUMMONER_ALL_SKILL, 0, 0, obSmData.data(), obSmData.size() );

}

void MatchClient::onMsgOBEnterRoom(SNetMsgHead* head, void* data, size_t len)
{
    TraceLn(__FUNCTION__": len= " << len);

    if (len < sizeof(SMsgSelfEnterRoomData))
    {
        ErrorLn(__FUNCTION__": SMsgEnterRoom len= " << len);
        return;
    }

    SMsgSelfEnterRoomData* pEnterRoom_Ex = (SMsgSelfEnterRoomData*)data;
    if ( !pEnterRoom_Ex->m_Result )
    {
        // todo:pt 通知view 进入房间失败
        TraceLn(__FUNCTION__": pMsgEnterRoom->m_Result= " << pEnterRoom_Ex->m_Result);
        return;
    }

    gClientGlobal->gotoState( ESS_MATCHROOM,data,len );
    CurrentMatchRoomInfo::getInstance().inital(data,len, false);

    SMsgCreateRoom_OC* pRoomData = getRoomData( pEnterRoom_Ex->m_RoomID );
    if (pRoomData == NULL)
    {
        ErrorLn(__FUNCTION__": m_RoomID= " << pEnterRoom_Ex->m_RoomID);
        return;
    }
    cmd_send_war_typeid cmdData;
    cmdData.nConfigWarID = pRoomData->nConfigWarID;
    cmdData.nMatchTypeID = pRoomData->dwMatchTypeID;
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_PLAY_SEND_WAR_TYPEID,pRoomData->nConfigWarID,0,&cmdData,sizeof(cmdData));
}


void MatchClient::onMsgEnterRoom( SNetMsgHead* head, void* data, size_t len )
{
    TraceLn(__FUNCTION__": len= " << len);

    if (len < sizeof(SMsgSelfEnterRoomData))
    {
        ErrorLn(__FUNCTION__": SMsgEnterRoom len= " << len);
        return;
    }

    SMsgSelfEnterRoomData* pEnterRoom_Ex = (SMsgSelfEnterRoomData*)data;
    if ( !pEnterRoom_Ex->m_Result )
    {
        // todo:pt 通知view 进入房间失败
        ErrorLn(__FUNCTION__": pMsgEnterRoom->m_Result= " << pEnterRoom_Ex->m_Result);
        return;
    }

	SMsgCreateRoom_OC* pRoomData = getRoomData( pEnterRoom_Ex->m_RoomID );
	if (pRoomData == NULL)
	{
		ErrorLn(__FUNCTION__": m_RoomID= " << pEnterRoom_Ex->m_RoomID);
		return;
	}
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
		return;
	ISchemeMatchRoom * pMatchRoom = pCenter->getSchemeMatchRoom();
	if (pMatchRoom == NULL)
		return;
	SSchemeMatchRoom * pSchemeRoom = pMatchRoom->getSchemeMatchRoomByMatchTypeID(pRoomData->dwMatchTypeID);
	if (pSchemeRoom == NULL)
		return;

    // 处理召唤师信息
    ISchemeSummonerSkill* pInfo = gClientGlobal->getSchemeCenter()->getSchemeSummonerSkill();
    if( pInfo == NULL )
    {
        return;
    }
    ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
    if (pSpellFactory == NULL)
    {
        return;
    }

    obuf obData;
    cmd_count cmdCount;
    cmdCount.nCount = SUMMONER_SKILL_COUNT;
    obData << cmdCount;
    for (int i = 0; i < SUMMONER_SKILL_COUNT;++i )
    {
        int nSummonerSkillId=pEnterRoom_Ex->m_SummonerSKill[i];
        SSummonerSkillInfo * pSummonerInfo=pInfo->getSummonerSkillInfo(nSummonerSkillId);
        if(pSummonerInfo==NULL)
        {
            continue;
        }
        cmd_SummonerSkill_Info cmdSkillInfo;
        cmdSkillInfo.nIndex = i;
        cmdSkillInfo.nSkillId = nSummonerSkillId;

        SPELL_DATA *pSpellData = pSpellFactory->getSpellData(cmdSkillInfo.nSkillId );
        if (pSpellData != NULL)
        {
            cmdSkillInfo.nIconId = pSpellData->nIcon;
        }

        strcpy_s(cmdSkillInfo.summonerSkillDesc, sizeof(cmdSkillInfo.summonerSkillDesc),pSummonerInfo->stDesc );
        obData << cmdSkillInfo;
    }
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_MATCH_SUMMONER_DEFAULT_SKILL,0,0,obData.data(),obData.size());

    // 将召唤师的技能发送过去
    sendAllSummonerSkillInf(pSchemeRoom->m_nModeID);

    gClientGlobal->gotoState( ESS_MATCHROOM,data,len );
    CurrentMatchRoomInfo::getInstance().inital(data,len, false);


    cmd_send_war_typeid cmdData;
    cmdData.nConfigWarID = pRoomData->nConfigWarID;
    cmdData.nMatchTypeID = pRoomData->dwMatchTypeID;
    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_PLAY_SEND_WAR_TYPEID,pRoomData->nConfigWarID,0,&cmdData,sizeof(cmdData));

}

void MatchClient::onViewCmdMatchGuideInfo(int nParam,const char * strParam,void * ptrParam)
{
    if (ptrParam == nullptr)
    {
        ErrorLn(__FUNCTION__": ptrParam == nullptr");
        return;
    }

    cmd_guide_info* pCmd = static_cast<cmd_guide_info*>( ptrParam );

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_UPDATE_GUIDE_INFO;						// 子消息代码  EMRoomMessageCode

    SMsg_UpdateGuideInfo MsgInfo;
    MsgInfo.nGuidID = pCmd->nGuideID;

    obuf256 ob;
    ob << MsgHead<<outData << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdMatchCreateRoom( int nParam, const char * strParam,void * ptrParam )
{
    TraceLn(__FUNCTION__);
    if (ptrParam == nullptr)
    {
        ErrorLn(__FUNCTION__": ptrParam == nullptr");
        return;
    }

    cmd_match_create_room* pCmd = static_cast<cmd_match_create_room*>( ptrParam );
    ISchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return;
    }
    SSchemeMatchRoom* pMatchRoom = pSchemeMatchRoom->getSchemeMatchRoomByMatchTypeID(pCmd->nMatchType);
    if (pMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__": pMatchRoom == NULL");
        return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_CREATE_ROOM;						// 子消息代码  EMRoomMessageCode

    SMsgCreateRoom_CS MsgInfo;
    MsgInfo.dwMatchType = pCmd->nMatchType;
    strcpy_s(MsgInfo.szRoomName, sizeof(MsgInfo.szRoomName), pCmd->szName);
    if (pCmd->bHavePassWord > 0)
    {
        MsgInfo.bHavePassword = true;
        strcpy_s(MsgInfo.szPassword, sizeof(MsgInfo.szPassword), pCmd->szPassword);
    }
	MsgInfo.bPermitOb = pCmd->byPermitOb;

    obuf256 ob;
    ob << MsgHead<<outData << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdCreateGuideRoom( int nParam, const char * strParam,void * ptrParam )
{
    TraceLn(__FUNCTION__);
    if (ptrParam == nullptr)
    {
        ErrorLn(__FUNCTION__": ptrParam == nullptr");
        return;
    }

    cmd_match_create_guide_room* pCmd = static_cast<cmd_match_create_guide_room*>( ptrParam );
    ISchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return;
    }
    SSchemeMatchRoom* pMatchRoom = pSchemeMatchRoom->getSchemeMatchRoomByMatchTypeID(pCmd->nMatchType);
    if (pMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__": pMatchRoom == NULL");
        return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_CREATE_GUIDE_ROOM;						// 子消息代码  EMRoomMessageCode

    SMsgCreateGuideRoom MsgInfo;
    MsgInfo.dwMatchType = pCmd->nMatchType;
    MsgInfo.nGuideHeroID = pCmd->nGuidHeroID;

    obuf256 ob;
    ob << MsgHead<<outData << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdMatchEnterRoom( int nParam,const char * strParam,void * ptrParam )
{
    TraceLn(__FUNCTION__": nParam= " << nParam);
    if (ptrParam == nullptr)
    {
        ErrorLn(__FUNCTION__": ptrParam == nullptr");
        return;
    }

    

    cmd_match_enter_room* pCmd = static_cast<cmd_match_enter_room*>( ptrParam );

    DWORD RoomID = pCmd->nRoomID;
    SMsgCreateRoom_OC* pRoom = getRoomData( RoomID );
    if (nullptr == pRoom)
    {
        TraceLn(__FUNCTION__": nullptr == pRoom RoomID= " << RoomID);
        gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_MATCH_ROOM_CANT_FIND);
        return;
    }
    if (pRoom->byRoleCount >= getPlayerCountMax(pRoom->dwMatchTypeID))
    {
        gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_MATCH_ROOM_IS_FULL);
        return;
    }

    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoomByMatchType(pRoom->dwMatchTypeID);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return;
    }
    if ( !pSchemeMatchRoom->m_RoomStateEnter[ pRoom->byState ] )
    {
        gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_MATCH_ROOM_STATE_CANT_JOIN);
        return;
    }

    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_ENTER_ROOM;						// 子消息代码  EMRoomMessageCode

    SMsgEnterRoom MsgInfo;
    MsgInfo.dwRoomID = RoomID;
    if (pRoom->bHavePassword)
    {
        strcpy_s(MsgInfo.szPassword, sizeof(MsgInfo.szPassword), pCmd->szPassword);
    }

    obuf256 ob;
    ob << MsgHead<<outData << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdMatchOpenCreateRoom( int nParam,const char * strParam,void * ptrParam )
{
    TraceLn(__FUNCTION__": nParam= " << nParam);

    ISchemeMatchRoom* pSchemeMatchRoom = gClientGlobal->getSchemeCenter()->getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return;
    }

    // 打开创建房间界面
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_OPEN_CREATE_ROOM, 0, 0, 0, 0);
}

void MatchClient::onMsgRoom( SNetMsgHead* head, void* data, size_t len )
{

    if (len < sizeof(SMsgCreateRoom_OC))
    {
        ErrorLn("MatchClient::onMsgRoom  len= " << len);
        return;
    }
    SMsgCreateRoom_OC* pRoomData = (SMsgCreateRoom_OC*)data;
    data = (void*)(pRoomData + 1);
    len -= sizeof(SMsgCreateRoom_OC);
    addRoom( pRoomData );

    bool bShowBefore = isShowInHall(pRoomData);
    pRoomData->byState = pRoomData->byState;

    //设置大厅显示状态
    if ( isShowInHall(pRoomData) )
    { 
        // 要添加显示出来
        syncAddRoom(pRoomData);
    }
    else
    {
        // 现在是隐藏状态
        if (bShowBefore)
        {
            // 显示->隐藏
            syncRemoveRoom( pRoomData->dwRoomID );
        }
    }
}

SMsgCreateRoom_OC* MatchClient::getRoomData(DWORD RoomID)
{
    MAP_Rooms::iterator itor = m_mapRooms.find( RoomID );
    if (itor != m_mapRooms.end())
    {
        return &(itor->second);
    }

    return nullptr;
}

void MatchClient::onMsgRoomRoleCount( SNetMsgHead* head, void* data, size_t len )
{
    if (len < sizeof(SMsgRoomRoleCount))
    {
        ErrorLn(__FUNCTION__":  len= " << len);
        return;
    }
    SMsgRoomRoleCount* pMsgRoomRoleCount = (SMsgRoomRoleCount*)data;
    data = (void*)(pMsgRoomRoleCount + 1);
    len -= sizeof(SMsgRoomRoleCount);

    SMsgCreateRoom_OC* pRoomData = getRoomData( pMsgRoomRoleCount->m_RoomID);
    if (nullptr == pRoomData)
    {
        ErrorLn(__FUNCTION__": nullptr == pRoomData pMsgRoomRoleCount->m_RoomID= " << pMsgRoomRoleCount->m_RoomID);
        return;
    }
    pRoomData->byRoleCount = pMsgRoomRoleCount->m_RoleCount;

    cmd_match_hall_add_room cmdMatchHallAddRoom;
    make(pRoomData, cmdMatchHallAddRoom);
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_HALL_ROOM_FRESH, 0, 0, &cmdMatchHallAddRoom, sizeof(cmdMatchHallAddRoom));
}

SMapSchemeInfo* MatchClient::getSchemeMap( DWORD MapID )
{
    ISchemeMapInfo* pSchemeMapInfo = gClientGlobal->getSchemeCenter()->getSchemeMapInfo();
    if (nullptr == pSchemeMapInfo)
    {
        ErrorLn("MatchClient::getSchemeMap nullptr pSchemeMapInfo");
        return nullptr;
    }
    return pSchemeMapInfo->GetMapSchemeInfo( MapID );
}

void MatchClient::make( SMsgCreateRoom_OC* pRoom, cmd_match_hall_add_room& cmd )
{
    SMsgCreateRoom_OC& roomData = *pRoom;

    SMapSchemeInfo* pSchemeMap = getSchemeMap( roomData.dwMapID );
    if (nullptr == pSchemeMap)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMap roomData.m_MapID=" << roomData.dwMapID);
        return;
    }
    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoomByMatchType(roomData.dwMatchTypeID);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom dwMatchTypeID = "<<roomData.dwMatchTypeID);
        return;
    }
    cmd.bIsMatchRoom = roomData.bIsMatchRoom == true ? 1 : 0;
    cmd.nRoomIndex		= roomData.dwRoomID;
    cmd.nCurPlayerNum	= roomData.byRoleCount;
    cmd.nMaxPlayerNum	= (pSchemeMatchRoom->m_CampRoleMax * pSchemeMatchRoom->m_CampCount);
	cmd.nMaxObNum		= pSchemeMatchRoom->m_nMaxOBCount;
    cmd.nMapID			= roomData.dwMapID;
    cmd.nRoomMode		= roomData.nModeID;
    cmd.nConfigWarID    = roomData.nConfigWarID;
    cmd.bIsDelayJoin    = pSchemeMatchRoom->m_bCanDeLayJoinRoom == true? 1: 0;
    cmd.nDelayTime      = pSchemeMatchRoom->m_nDeLayJoinTime;
    cmd.bLock           = roomData.bHavePassword;
	cmd.nRoomType		= roomData.nRoomType;
	cmd.bOnlineLegendCup = roomData.bOnlineLegendCup == true ? 1 : 0;
    cmd.nMatchTypeID    = roomData.dwMatchTypeID;
	cmd.bPermitOb		= roomData.bPermitOB == true? 1: 0;
    strcpy_s(cmd.szRoomName, sizeof(cmd.szRoomName), roomData.szRoomName);
    strcpy_s(cmd.szMapDes, sizeof(cmd.szMapDes), getTextInfo( pSchemeMatchRoom->m_DescTextInfoID ) );
    strcpy_s(cmd.szMode, sizeof(cmd.szMode), getModeName( roomData.nModeID));
    strcpy_s(cmd.szMapName, sizeof(cmd.szMapName), pSchemeMap->szName);
    strcpy_s(cmd.szState, sizeof(cmd.szState), getStateName( roomData.byState ));

}

uint32 MatchClient::getPlayerCountMax( DWORD dwMatchType)
{
    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoomByMatchType(dwMatchType);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom dwMatchType=" << dwMatchType);
        return 0;
    }
    return (pSchemeMatchRoom->m_CampRoleMax * pSchemeMatchRoom->m_CampCount);
}


void MatchClient::onMsgRoomState( SNetMsgHead* head, void* data, size_t len )
{
    if (len < sizeof(msg_change_roomState))
    {
        ErrorLn(__FUNCTION__": len= " << len);
        return;
    }

    msg_change_roomState* pMsg = (msg_change_roomState*)data;
    SMsgCreateRoom_OC* pRoomData = getRoomData( pMsg->dwRoomID);
    if (nullptr == pRoomData)
    {
        return;
    }

    bool bShowBefore = isShowInHall(pRoomData);
    pRoomData->byState = pMsg->nState;

    //设置大厅显示状态
    if ( isShowInHall(pRoomData) )
    {
        // 现在是显示状态 
        if (bShowBefore)
        {
            // 之前就显示着
            syncFreshRoom(pRoomData);
        }
        else
        {
            // 要添加显示出来
            syncAddRoom(pRoomData);
        }
    }
    else
    {
        // 现在是隐藏状态
        if (bShowBefore)
        {
            // 显示->隐藏
            syncRemoveRoom( pRoomData->dwRoomID );
        }
    }
}

BYTE MatchClient::getCampCount( DWORD dwMatchType)
{
    TraceLn(__FUNCTION__": dwMatchType=" << dwMatchType);
    SSchemeMatchRoom*  pSchemeMatchRoom = getSchemeMatchRoomByMatchType(dwMatchType);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return 0;
    }

    return  pSchemeMatchRoom->m_CampCount;
}

BYTE MatchClient::getCampCount( SMsgCreateRoom_OC* pRoom )
{
    if (nullptr == pRoom)
    {
        TraceLn(__FUNCTION__": nullptr == pRoom ");
        return 0;
    }
    MATCH_TRACELN(__FUNCTION__": pRoom->m_RoomID= " << pRoom->dwRoomID);
    return getCampCount( pRoom->dwMatchTypeID);
}

rkt::uint32 MatchClient::getCampPlayerCountMax( DWORD dwMatchType)
{
    SSchemeMatchRoom*  pSchemeMatchRoom = getSchemeMatchRoomByMatchType(dwMatchType);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return 0;
    }

    return  pSchemeMatchRoom->m_CampRoleMax;
}

rkt::uint32	MatchClient::getObCountMax(DWORD dwMatchType)
{
	SSchemeMatchRoom*  pSchemeMatchRoom = getSchemeMatchRoomByMatchType(dwMatchType);
	if (nullptr == pSchemeMatchRoom)
	{
		ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
		return 0;
	}

	return  pSchemeMatchRoom->m_nMaxOBCount;
}

rkt::uint32 MatchClient::getCampPlayerCountMax( SMsgCreateRoom_OC* pRoom )
{
    if (nullptr == pRoom)
    {
        TraceLn(__FUNCTION__": nullptr == pRoom ");
        return 0;
    }
    MATCH_TRACELN(__FUNCTION__": pRoom->m_RoomID= " << pRoom->dwRoomID);
    return getCampPlayerCountMax( pRoom->dwMatchTypeID );
}

void MatchClient::onMsgReleaseRoom( SNetMsgHead* head, void* data, size_t len )
{
    if (len < sizeof(SMsgReleaseRoom))
    {
        ErrorLn(__FUNCTION__": len= " << len);
        return;
    }
    SMsgReleaseRoom* pMsgReleaseRoom = (SMsgReleaseRoom*)data;
    data = (void*)(pMsgReleaseRoom + 1);
    len -= sizeof(*pMsgReleaseRoom);

    SMsgCreateRoom_OC* pRoom = getRoomData( pMsgReleaseRoom->m_RoomID);
    if (nullptr == pRoom)
    {
        TraceLn(__FUNCTION__": nullptr == pRoom pMsgReleaseRoom->m_RoomID= " << pMsgReleaseRoom->m_RoomID);
        return;
    }

    syncRemoveRoom( pRoom->dwRoomID);
    removeRoom( pRoom );
}

void MatchClient::removeRoom( SMsgCreateRoom_OC* pRoom )
{
    TraceLn(__FUNCTION__": roomID"<<pRoom->dwRoomID);
    m_mapRooms.erase( pRoom->dwRoomID);
}

void MatchClient::initFromRoom( void* pData,size_t stLen )
{
    // 新的大厅数据
    addHallRoom(pData, stLen);
}


void MatchClient::addHallRoom( void* pData, size_t stLen )
{
    m_mapRooms.clear();
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_HALL_REMOVE_ALL_ROOM, 0, 0, 0, 0);

    if (stLen < sizeof(SMsgHallRoom))
    {
        ErrorLn(__FUNCTION__": SMsgHallRoom stLen= " << stLen);
        return;
    }
    SMsgHallRoom* pMsgHallRoom = (SMsgHallRoom*)pData;
    pData = (void*)(pMsgHallRoom + 1);
    stLen -= sizeof(*pMsgHallRoom);

    TraceLn(__FUNCTION__": pMsgHallRoom->m_RoomCount =  " << pMsgHallRoom->m_RoomCount);
    if ( pMsgHallRoom->m_RoomCount <= 0 )
    {
        return;
    }

    if (stLen < (sizeof(SMsgCreateRoom_OC) * pMsgHallRoom->m_RoomCount))
    {
        ErrorLn(__FUNCTION__": SMsgCreateRoom_OC stLen= " << stLen);
        return;
    }
    SMsgCreateRoom_OC* pSMsgRoom = nullptr;
    for (size_t i = 0; i < pMsgHallRoom->m_RoomCount; ++i)
    {
        pSMsgRoom = (SMsgCreateRoom_OC*)pData;
        pData = (void*)(pSMsgRoom + 1);
        stLen -= sizeof(SMsgCreateRoom_OC);
        TraceLn(__FUNCTION__": i=" << i << " pSMsgRoom->m_RoomID=" << pSMsgRoom->dwRoomID << " m_RoomName=" << pSMsgRoom->szRoomName);
        addRoom( pSMsgRoom );
    }
}

ISchemeTextInfo* MatchClient::getSchemeTextInfo()
{

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeCenter");
        return nullptr;
    }

    return pSchemeCenter->getSchemeTextInfo();
}

ISchemeMatchRoom* MatchClient::getSchemeMatchRoom()
{
    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeCenter");
        return nullptr;
    }

    return pSchemeCenter->getSchemeMatchRoom();
}

SSchemeMatchRoom* MatchClient::getSchemeMatchRoomByMatchType(DWORD dwMatchType)
{
    ISchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoom();
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return nullptr;
    }
    return pSchemeMatchRoom->getSchemeMatchRoomByMatchTypeID(dwMatchType);
}

char const* MatchClient::getModeName( uchar ModeID )
{
    return getTextInfo(ModeID + 100);
}

char const* MatchClient::getTextInfo( int nID )
{
    ISchemeTextInfo* pISchemeTextInfo = getSchemeTextInfo();
    if (nullptr == pISchemeTextInfo)
    {
        ErrorLn("MatchClient::make nullptr == pISchemeTextInfo");
        return nullptr;
    }
    return  pISchemeTextInfo->getText( nID ).c_str();
}

char const* MatchClient::getStateName( int nState )
{
    return getTextInfo(nState + 500);
}

char const* MatchClient::getMapDesc( DWORD dwMatchType)
{
    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoomByMatchType( dwMatchType);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeMatchRoom! " );
        return false;
    }

    return getTextInfo( pSchemeMatchRoom->m_DescTextInfoID );
}

bool MatchClient::isShowInHall( SMsgCreateRoom_OC* pRoomData )
{
    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoomByMatchType(pRoomData->dwMatchTypeID);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeMatchRoom! " );
        return false;
    }
    return pSchemeMatchRoom->m_RoomStateShow[ pRoomData->byState ];
}

void MatchClient::syncRemoveRoom( DWORD RoomID )
{
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_HALL_REMOVE_ROOM, RoomID, 0, 0, 0);
}

void MatchClient::addRoom( SMsgCreateRoom_OC* pRoomData )
{
    m_mapRooms.insert( make_pair(pRoomData->dwRoomID, *pRoomData) );
}

void MatchClient::syncFreshRoom( SMsgCreateRoom_OC* pRoomData )
{
    cmd_match_hall_add_room cmdMatchHallAddRoom;
    make(pRoomData, cmdMatchHallAddRoom);
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_HALL_ROOM_FRESH, 0, 0, &cmdMatchHallAddRoom, sizeof(cmdMatchHallAddRoom));
}

void MatchClient::syncAddRoom( SMsgCreateRoom_OC* pRoomData )
{
    obuf obData;
    cmd_match_hall_rooms_head cmdHead;
    cmdHead.nRoomCount = 1;

    cmd_match_hall_add_room cmdData;
    make(pRoomData, cmdData);
    obData<<cmdHead<<cmdData;

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_HALL_ADD_ROOM, -1, 0, obData.data(), obData.size());
}

char const* MatchClient::getMapPicPath( DWORD dwMatchType )
{
    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoomByMatchType(dwMatchType);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeMatchRoom! " );
        return false;
    }

    return getTextInfo( pSchemeMatchRoom->m_PicPathTextInfoID );
}

void MatchClient::onViewCmdMatchHallRoomData( int nParam, const char * strParam, void * ptrParam )
{
    DWORD RoomID = static_cast<DWORD>(nParam);
    SMsgCreateRoom_OC* pRoom = getRoomData( RoomID );
    if (nullptr == pRoom)
    {
        TraceLn(__FUNCTION__": nullptr == pRoom RoomID= " << RoomID);
        return;
    }

    SSchemeMatchRoom* pSchemeMatchRoom = getSchemeMatchRoomByMatchType(pRoom->dwMatchTypeID);
    if (nullptr == pSchemeMatchRoom)
    {
        ErrorLn(__FUNCTION__": nullptr == pSchemeMatchRoom");
        return;
    }

    cmd_match_create_room_map_info cmd;
    strcpy_s(cmd.szPicPath, sizeof(cmd.szPicPath), getMapPicPath( pRoom->dwMatchTypeID ) );
    strcpy_s(cmd.szDesc, sizeof(cmd.szDesc), getMapDesc( pRoom->dwMatchTypeID ));
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCM_MATCH_HALL_ROOM_DATA, 0, 0, &cmd, sizeof(cmd));
}

void MatchClient::onViewCmdMatchToChooseVocation( int nParam, const char * strParam, void * ptrParam )
{
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_EXIT_HALL;						// 子消息代码  EMRoomMessageCode

    ob << MsgHead<<outData;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onMsgExitHall( SNetMsgHead* head, void* data, size_t len )
{
    m_mapRooms.clear();
}

void MatchClient::onMsgExitHallFromLegendCupRoom( SNetMsgHead* head, void* data, size_t len )
{
	m_mapRooms.clear();
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_ENTER_SCENE, 0, 0, 0, 0);
}

void MatchClient::onViewCmdEnterHall(int nParam, const char * strParam, void * ptrParam)
{
    //设置在大厅标识
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_ENTER_HALL;						// 子消息代码  EMRoomMessageCode

    ob << MsgHead<<outData;


    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdExitHall(int nParam, const char * strParam, void * ptrParam)
{
    obuf256 ob;
    SNetMsgHead MsgHead;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_EXIT_HALL;						// 子消息代码  EMRoomMessageCode

    ob << MsgHead<<outData;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdBeginMatch(int nParam, const char * strParam, void * ptrParam)
{
    // todo 请求撮合类型
    m_nMatchType = nParam;

    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byKeyModule    = MSG_MODULEID_MATCH;
    head.byKeyAction    = MSG_MATCH_DATE_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_MATCH_REQUEST_ADDMATCH;						// 子消息代码  EMMatchDataSubMsgCode
    outData.dwUserData1	= m_nMatchType;						// 用户数据1 EMMatchType
    outData.dwUserData2	= 0;						// 用户数据2
    outData.dwUserData3	= 0;						// 用户数据3

    SMsgMatchRequestInfo_SO RequestInfo;

    obuf256 ob;
    ob <<head<<outData;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdCancelMatch( int nParam, const char * strParam, void * ptrParam )
{
    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byKeyModule    = MSG_MODULEID_MATCH;
    head.byKeyAction    = MSG_MATCH_DATE_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_MATCH_CANCEL_MATCH;	        // 子消息代码
    outData.dwUserData1	= m_nMatchType;						    // 用户数据1
    outData.dwUserData2	= 0;						            // 用户数据2
    outData.dwUserData3	= 0;						            // 用户数据3

    obuf256 ob;
    ob <<head<<outData;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdAcceptMatch( int nParam, const char * strParam, void * ptrParam )
{
    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byKeyModule    = MSG_MODULEID_MATCH;
    head.byKeyAction    = MSG_MATCH_DATE_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_MATCH_ACCEPT_MATCH;	        // 子消息代码
    outData.dwUserData1	= m_nMatchType;						        // 用户数据1
    outData.dwUserData2	= nParam;						            // 用户数据2
    outData.dwUserData3	= 0;						            // 用户数据3

    obuf256 ob;
    ob <<head<<outData;

    net_senddata( ob.data(), ob.size() );
}

void MatchClient::onViewCmdRefuseMatch( int nParam, const char * strParam, void * ptrParam )
{
    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byKeyModule    = MSG_MODULEID_MATCH;
    head.byKeyAction    = MSG_MATCH_DATE_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_MATCH_REFUSE_MATCH;	        // 子消息代码
    outData.dwUserData1	= m_nMatchType;						        // 用户数据1
    outData.dwUserData2	= nParam;						            // 用户数据2
    outData.dwUserData3	= 0;						                // 用户数据3

    obuf256 ob;
    ob <<head<<outData;

    net_senddata( ob.data(), ob.size() );
}


void MatchClient::onViewCmdCustomerInviteRespon(int nParam, const char * strParam, void * ptrParam)
{

    if (ptrParam == nullptr)
    {
        ErrorLn(__FUNCTION__": ptrParam == nullptr");
        return;
    }

    cmd_customer_invite_respon* pCmd = static_cast<cmd_customer_invite_respon*>( ptrParam );

    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    head.byKeyModule    = MSG_MODULEID_MATCH;
    head.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_MATCH_CUSTOMER_INVITE_RESPON;						// 子消息代码  EMRoomMessageCode

    SMsg_MasterInviteRespon_CS MsgInfo;
    MsgInfo.dwRoomID = pCmd->nRoomID;
    MsgInfo.dwInviteActorID = pCmd->dwInviteActorID;
    sstrcpyn(MsgInfo.szInviteName ,pCmd->szPlayerName, sizeof(MsgInfo.szInviteName));

    obuf256 ob;
    ob << head<<outData << MsgInfo;

    net_senddata( ob.data(), ob.size() );
}


void MatchClient::onMSEnterHall(SNetMsgHead* head, void* data, size_t len)
{
    //接收数据显示房间
    if (len < sizeof(SMsgEnterHall_OC))
    {
        ErrorLn(__FUNCTION__": SMsgEnterHall len= " << len);
        return;
    }
    SMsgEnterHall_OC* pMsgEnterHall = (SMsgEnterHall_OC*)data;
    data = (void*)(pMsgEnterHall + 1);
    len -= sizeof(SMsgEnterHall_OC);
    if ( !pMsgEnterHall->m_bOK )
    {
        TraceLn(__FUNCTION__": pMsgEnterHall->m_bOK=false ");
        return;
    }

    addHallRoom( data, len);
    size_t stCount = m_mapRooms.size();
    TraceLn("MatchClient::showHall m_mapRooms.size()= " << stCount);

    // 房间数据下发
    obuf obData;
    cmd_match_hall_rooms_head cmdHead;
    cmdHead.nRoomCount = m_mapRooms.size();
    obData<<cmdHead;
    MAP_Rooms::iterator itor = m_mapRooms.begin();
    for (;itor != m_mapRooms.end(); ++itor)
    {
        SMsgCreateRoom_OC& roomData = itor->second;
        cmd_match_hall_add_room cmdMatchHallAddRoom;
        make(&roomData, cmdMatchHallAddRoom);
        obData<<cmdMatchHallAddRoom;
    }
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_HALL_ADD_ROOM, 0, 0, obData.data(), obData.size());
}

void MatchClient::onMsgCancelMatch(SNetMsgHead* head, void* data, size_t len)
{
    //隐藏等待时间界面
    m_nMatchType = MatchType_None;
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_WND_HIDE, 0, 0, 0, 0);
}

void MatchClient::onMsgIsMatching(SNetMsgHead* head, void* data, size_t len)
{
    ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
    if ( NULL==pCenter )
    {
        ErrorLn(__FUNCTION__": NULL==pCenter");
        return;
    }
        
    ISchemeMatchRoom * pMatchRoom = pCenter->getSchemeMatchRoom();
    if (pMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__": NULL==pCenter");
        return;
    }

    if (len != sizeof(SMsgAddOrRematchResultInfo))
    {
        ErrorLn(__FUNCTION__": length error");
        return;
    }
    SMsgAddOrRematchResultInfo *pData = (SMsgAddOrRematchResultInfo *)data;
    m_nMatchType = pData->dwMatchTypeID;

    //显示等待时间界面
	cmd_add_match_info cmdInfo;
	cmdInfo.nMatchTypeID = pData->dwMatchTypeID;
	cmdInfo.nAvgWaitTimes = pData->dwAvgWaitTimes;
    cmdInfo.nCurMatchPlayerCount = pData->dwCurMatchPlayerCount;
    cmdInfo.nMaxPlayerCount = 0;
    SSchemeMatchRoom * pSchemeRoom = pMatchRoom->getSchemeMatchRoomByMatchTypeID(pData->dwMatchTypeID);
    if (pSchemeRoom != NULL)
    {
        cmdInfo.nMaxPlayerCount =  pSchemeRoom->getPlayerCountMax();
    }
        
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_WND_SHOW, 0, 0, &cmdInfo, sizeof(cmdInfo));
}

void MatchClient::onMsgMatchOK(SNetMsgHead* head, void* data, size_t len)
{
    //撮合OK
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_WND_HIDE, 0, 0, 0, 0);
}

void MatchClient::onMsgMatchFail(SNetMsgHead* head, void* data, size_t len)
{
    //撮合失败
    m_nMatchType = MatchType_None;
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MATCH_WND_HIDE, 0, 0, 0, 0);
}

// 离开
void MatchClient::onViewSendExitGameMsg()
{
    //// 取得主角对象
    //IClientEntity * pHero = gClientGlobal->getHero();
    //if (!pHero)
    //{
    //	return;
    //}

    //obuf256 ob;
    //CS_ExitGameMsg data;
    //memset(&data,0,sizeof(CS_ExitGameMsg));

    //data.uid = 0;

    //SNetMsgHead head;
    //head.byDestEndPoint = MSG_ENDPOINT_SCENE;
    //head.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    //head.byKeyModule    = MSG_MODULEID_GAME;
    //head.byKeyAction    = CS_MSG_USER_EXITGAME;

    //ob << head << data;

    //gClientGlobal->getNetConnection()->SendData(ob.data(), ob.size());
}

// 离开战场
void MatchClient::onViewSendExitWarMsg()
{
 
    //obuf256 ob;
    //CS_ExitGameMsg data;
    //memset(&data,0,sizeof(CS_ExitGameMsg));

    //data.uid = 0;

    //SNetMsgHead MsgHead;
    //MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    //MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    //MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    //MsgHead.byKeyAction    = MSG_MATCH_ROOM_MODULEMSG;

    //SMsgMatchDataSubMsg outData;
    //outData.dwMsgCode	= CS_MSG_EXIT_WAR;						// 子消息代码  EMRoomMessageCode

    //ob << MsgHead<<outData << data;

    //gClientGlobal->getNetConnection()->SendData(ob.data(), ob.size());
}

void MatchClient::enterHall(void * context,int len)
{
    initFromRoom( context,len );
    showHall();
}

void MatchClient::loadSenece(DWORD dwMapID, Vector3 vecBornPos)
{
    CurrentMatchRoomInfo::getInstance().loadSenece(dwMapID, vecBornPos);
}

DWORD MatchClient::getCurRoomMatchType()
{
    SMsgCreateRoom_OC* pRoom = getRoomData( CurrentMatchRoomInfo::getInstance().getCurrentRoomID());
    if (nullptr == pRoom)
    {
        return 0;
    }
    return pRoom->dwMatchTypeID;
}


int MatchClient::getCurRoomType()
{
    SMsgCreateRoom_OC* pRoom = getRoomData( CurrentMatchRoomInfo::getInstance().getCurrentRoomID());
    if (nullptr == pRoom)
    {
        return 0;
    }
    return pRoom->nRoomType;
}


DWORD MatchClient::getCurRoomID()
{
    SMsgCreateRoom_OC* pRoom = getRoomData( CurrentMatchRoomInfo::getInstance().getCurrentRoomID());
    if (nullptr == pRoom)
    {
        return 0;
    }
    return pRoom->dwRoomID;
}

void MatchClient::onViewMasterViewLoaded(int nParam, const char * strParam, void * ptrParam)
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return;
    }
    UID uid = pClientEntity->getUID();
    bool bMobaMode = (UID_2_SCENE(uid) >= (MAP_MAXCOUNT + 1));
    if (bMobaMode)
    {
        return;
    }

    obuf ob;
    SNetMsgHead MsgHead;
    MsgHead.byDestEndPoint = MSG_ENDPOINT_SCENE;
    MsgHead.bySrcEndPoint  = MSG_ENDPOINT_CLIENT;
    MsgHead.byKeyModule    = MSG_MODULEID_MATCH;
    MsgHead.byKeyAction    = MSG_MATCH_SERVICE_MODULEMSG;

    SMsgMatchDataSubMsg outData;
    outData.dwMsgCode	= CS_MSG_MATCH_ACTOR_VIEW_LOADED;						// 子消息代码  EMRoomMessageCode

    ob << MsgHead<<outData;

    net_senddata( ob.data(), ob.size() );
}


/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT IMatchClient * CreateMatchClient(void)
{
	// 如果创建了，则直接返回
	if(g_pMatchClient != NULL)
	{
		return g_pMatchClient;
	}

	// 实例化Match客户端
	MatchClient * pMatchClient = new MatchClient();
	if(pMatchClient == NULL || !pMatchClient->Create())
	{
		safeRelease(pMatchClient);
		return NULL;
	}

	g_pMatchClient = pMatchClient;
	return g_pMatchClient;
}