/*******************************************************************
** 文件名:	WarMatch.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2010-10-27
** 版  本:	1.0
** 描  述:	跨服竞技撮合系统服务器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "StdAfx.h"
#include "Match_SchemeDef.h"
#include "WarMatch.h"
#include "IActorService.h"
#include "EntityHelper.h"
#include "time.h"
#include "TeamHelper.h"
#include "ICenterConnectorService.h"
#include "IBridgeConnector.h"
#include "ExternalHelper.h"
#include "Chat_ManagedDef.h"
#include "MatchDef.h"
#include <bitset>
#include "PlayerKillMessageDef.h"

//////////////////////////////////////////////////////////////////////////

// 构造函数
CWarMatch::CWarMatch(void)
{

}

// 析构函数
CWarMatch::~CWarMatch(void)
{
}


// 创建
bool CWarMatch::Create(void)
{

	return true;
}
// 释放
void CWarMatch::release(void)
{

}

/** 处理跨区桥服务器发过来的消息
@name         : 处理其他服务器通过跨区桥服务器转发的消息
@param actionId  : 消息码
@param bridgeHead  : 桥消息头
@param head  : 消息头
@param data  : 消息内容
@param len:消息长度
*/
void CWarMatch::HandleBridgeMessage(unsigned long actionId, SMsgBridgeHead &bridgeHead, SNetMsgHead* head, LPSTR data, size_t len)
{
	if(data==NULL|| len < sizeof(SMsgMatchDataSubMsg))
	{
		return;
	}

	SMsgMatchDataSubMsg * pMsg = (SMsgMatchDataSubMsg *)(data);

#ifdef MATCH_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "CWarMatch::HandleBridgeMessage()  跨服 服务器撮合信息子消息:来源游戏世界ID=%d,来源服务器ID=%d,子消息代码=%d,len=%d,用户数据=%d,%d,%d",
		bridgeHead.dwWorldID,bridgeHead.dwServerID,pMsg->dwMsgCode,len,pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3);
	TraceLn(szBuf);
#endif

	LPSTR pMsgData = data + sizeof(SMsgMatchDataSubMsg);
	size_t nNewlen = len - sizeof(SMsgMatchDataSubMsg);


	// EMMatchDataSubMsgCode
	switch(pMsg->dwMsgCode)
	{
	case OS_MSG_MATCH_RESULT:		// 撮合结果
		{
			// 撮合结果处理
			OnMatchStrikeResult(pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3,pMsgData, nNewlen);
		}
		break;
	default:
		{
			ErrorLn("Have a message can't ，dwMsgCode = "<< pMsg->dwMsgCode);
		}
		break;
	}
}	

//////////////////////////////////////////////////////////////////////////

/** 服务器之间消息处理
@param msg 网络消息
*/
void CWarMatch::onTransmit(DWORD server, ulong actionId, SNetMsgHead* head, void * data, size_t len)
{
	if(data==NULL|| len < sizeof(SMsgMatchDataSubMsg))
	{
		return;
	}

	SMsgMatchDataSubMsg * pMsg = (SMsgMatchDataSubMsg *)(data);

#ifdef MATCH_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "CWarMatch::onTransmit()  服务器撮合信息子消息:服务器ID=%d,子消息代码=%d,用户数据=%d,%d,%d",
		server,pMsg->dwMsgCode,pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3);
	TraceLn(szBuf);
#endif

	LPSTR pMsgData = (LPSTR)data + sizeof(SMsgMatchDataSubMsg);
	size_t nNewlen = len - sizeof(SMsgMatchDataSubMsg);

	// EMMatchDataSubMsgCode
	switch(pMsg->dwMsgCode)
	{
	case OS_MSG_MATCH_RESULT:		// 撮合结果
		{
			// 撮合结果处理
			OnMatchStrikeResult(pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3,pMsgData,nNewlen);
		}
		break;

    case OS_MSG_MATCH_SCENE_CHECKDATA:		// 收集信息检查
        {
            // 撮合结果处理
            OnMatchRequestSceneCheck(pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3,pMsgData,nNewlen);
        }
        break;

    case OS_MSG_SYN_PLAYER_ROOM_STATE:		// 同步人物房间状态
        {
            // 同步人物房间状态
            onSysPlayerGameSatate(pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3,pMsgData,nNewlen);
        }
        break;

    case OS_MSG_MATCH_AUTO_ACCEPT_MATCH:		// 自动接收比赛
        {
            // 自动接收比赛
            OnMsgAutoAcceptMatch(pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3,pMsgData,nNewlen);
        }
        break;
        
	default:
		{
			ErrorLn(__FUNCTION__": not handle message! dwMsgCode = "<< pMsg->dwMsgCode);
		}
		break;
	}
}



void CWarMatch::onRequestJoinStrike(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len)
{

}



/** 来自客户端消息处理
@param msg 网络消息
*/
void CWarMatch::onMessage(ClientID clientID, void * data, size_t len)
{
	if(data==NULL|| len < sizeof(SMsgMatchDataSubMsg))
	{
		return;
	}

	SMsgMatchDataSubMsg * pMsg = (SMsgMatchDataSubMsg *)(data);

#ifdef MATCH_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	sprintf_s(szBuf,sizeof(szBuf), "CWarMatch::onMessage()  客户端撮合信息子消息:角色=%d:%s,子消息代码=%d,用户数据=%d,%d,%d",
		pPerson->GetNumProp(CREATURE_PROP_PDBID),pPerson->GetName(),pMsg->dwMsgCode,pMsg->dwUserData1,pMsg->dwUserData2,pMsg->dwUserData3);
	TraceLn(szBuf);
#endif

	LPSTR pMsgData =(LPSTR) data + sizeof(SMsgMatchDataSubMsg);
	size_t nNewlen = len - sizeof(SMsgMatchDataSubMsg);

	// EMMatchDataSubMsgCode
	switch(pMsg->dwMsgCode)
	{
	case CS_MSG_MATCH_REQUEST_ADDMATCH:		// 加入竞技撮合请求
		{
			OnMsgRequestAddMatch(clientID,pMsg->dwUserData1,(BYTE)pMsg->dwUserData2);
		}
		break;
	case CS_MSG_MATCH_CANCEL_MATCH:		// 请求检测取消操作事件
		{
			if (nNewlen>0)
			{
				break;
			}

			OnMsgCheckCancelMatch(clientID,pMsg->dwUserData1);
		}
		break;
    case CS_MSG_MATCH_ACCEPT_MATCH:		// 确定比赛
        {
            if (nNewlen>0)
            {
                break;
            }

            OnMsgAcceptMatch(clientID,pMsg->dwUserData1,pMsg->dwUserData2);
        }
        break;
    case CS_MSG_MATCH_REFUSE_MATCH:		// 拒绝比赛
        {
            if (nNewlen>0)
            {
                break;
            }

            OnMsgRefuseMatch(clientID,pMsg->dwUserData1,pMsg->dwUserData2);
        }
        break;
	case CS_MSG_MATCH_RECOMMEND_RESULT:
		{
			if (nNewlen>0)
			{
				break;
			}

			OnMsgRecommendRankResult(clientID, (BYTE)pMsg->dwUserData1);
		}
		break;

	default:
		{
			ErrorLn(_GT("跨服竞技撮合系统有一个客户端撮合信息子消息未处理，dwMsgCode = ")<< pMsg->dwMsgCode);
		}
		break;
	}
}

void CWarMatch::OnMsgRecommendRankResult(ClientID clientID, BYTE byAccept)
{
	if (NULL == clientID)
	{
		return;
	}

	ActorHelper helper(clientID, ACTOR_CLIENT);
	IActorService *pActorService = helper.m_ptr;
	if(pActorService == NULL)
	{
		ErrorLn(__FUNCTION__" pActorService == NULL");
		return;
	}
	PDBID idActor = pActorService->getProperty_Integer(PROPERTY_ID);
	WORD WorldID = pActorService->getFromGameWorldID();

	// 发本区检测信息消息给社会服
	SendDataToSocial(MSG_MATCH_DATE_MODULEMSG, CS_MSG_MATCH_RECOMMEND_RESULT, MatchType_Rank, idActor, WorldID,  (const char *)&byAccept, sizeof(byAccept));
}


void CWarMatch::OnMsgAutoAcceptMatch(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len)
{
    // 根据clientID 获取DBID 和 世界ID
    DWORD dwMatchTypeID = dwUserData;
    DWORD dwCheckID = dwResultFlag;
    ActorHelper helper(dwActorID, ACTOR_DBID);
    IActorService *pActorService = helper.m_ptr;
    bool bCanAccept = true;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__" pActorService == NULL");
        bCanAccept = false;
    }

    // 玩家房间的附加数据从这里发到社会服
    SMsgMatchRequestOtherInfo_SO*  pInfo = new SMsgMatchRequestOtherInfo_SO;
    if (pInfo ==NULL)
    {
        ErrorLn(__FUNCTION__": new SMsgMatchRequestOtherInfo_SO pInfo ==NULL");
        bCanAccept = false;
    }
    
    if (!bCanAccept)
    {
        // 发送拒绝
        SendDataToSocial(MSG_MATCH_DATE_MODULEMSG, CS_MSG_MATCH_REFUSE_MATCH, dwMatchTypeID, dwActorID, getThisGameWorldID(),  (const char *)&dwCheckID, sizeof(dwCheckID));
        safeDelete(pInfo);
    }
    else
    {
        PDBID idActor = pActorService->getProperty_Integer(PROPERTY_ID);
        WORD WorldID = pActorService->getFromGameWorldID();
        obuf sendData;
        pInfo->bySex = pActorService->getProperty_Integer(PROPERTY_SEX);
        pInfo->dwActorID = idActor;
        pInfo->nCurRunePageIdx = pActorService->getCurRunePage();
        pInfo->nTalentEffectPageIndex = pActorService->getCurTalentPage();
        //房间所需附加数据
        pActorService->getPlayerRoomExData(&(pInfo->sExData));
        pInfo->sExData.clientID = pActorService->getClientID();
        sstrcpyn(pInfo->szName,pActorService->getName().data(), sizeof(pInfo->szName));

        sendData<<dwCheckID;
        sendData.push_back(pInfo,sizeof(SMsgMatchRequestOtherInfo_SO));
        // 发本区检测信息消息给社会服
        SendDataToSocial(MSG_MATCH_DATE_MODULEMSG, CS_MSG_MATCH_ACCEPT_MATCH, dwMatchTypeID, idActor, WorldID,  sendData.data(), sendData.size());
        safeDelete(pInfo);
    }
}

void CWarMatch::OnMsgAcceptMatch(ClientID clientID,DWORD dwMatchTypeID,DWORD dwCheckID)
{
    if (INVALID_CLIENT == clientID)
    {
        return;
    }

    // 根据clientID 获取DBID 和 世界ID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__" pActorService == NULL");
        return;
    }
    PDBID idActor = pActorService->getProperty_Integer(PROPERTY_ID);
    WORD WorldID = pActorService->getFromGameWorldID();

    // 玩家房间的附加数据从这里发到社会服

    SMsgMatchRequestOtherInfo_SO*  pInfo = new SMsgMatchRequestOtherInfo_SO;
    if (pInfo ==NULL)
    {
        ErrorLn(__FUNCTION__": new SMsgMatchRequestOtherInfo_SO pInfo ==NULL");
        return;
    }

    obuf sendData;
    pInfo->bySex = pActorService->getProperty_Integer(PROPERTY_SEX);
    pInfo->dwActorID = idActor;
    pInfo->nCurRunePageIdx = pActorService->getCurRunePage();
    pInfo->nTalentEffectPageIndex = pActorService->getCurTalentPage();
    //房间所需附加数据
    pActorService->getPlayerRoomExData(&(pInfo->sExData));
    pInfo->sExData.clientID = clientID;

    sstrcpyn(pInfo->szName,pActorService->getName().data(), sizeof(pInfo->szName));

    sendData<<dwCheckID;
    sendData.push_back(pInfo,sizeof(SMsgMatchRequestOtherInfo_SO));

    delete pInfo;

    // 发本区检测信息消息给社会服
    SendDataToSocial(MSG_MATCH_DATE_MODULEMSG, CS_MSG_MATCH_ACCEPT_MATCH, dwMatchTypeID, idActor, WorldID,  sendData.data(), sendData.size());

    
}

void CWarMatch::OnMsgRefuseMatch(ClientID clientID,DWORD dwMatchTypeID, DWORD dwCheckID)
{
    if (NULL == clientID)
    {
        return;
    }

    // 根据clientID 获取DBID 和 世界ID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__" pActorService == NULL");
        return;
    }
    PDBID idActor = pActorService->getProperty_Integer(PROPERTY_ID);
    WORD WorldID = pActorService->getFromGameWorldID();

    // 发本区检测信息消息给社会服
    SendDataToSocial(MSG_MATCH_DATE_MODULEMSG, CS_MSG_MATCH_REFUSE_MATCH, dwMatchTypeID, idActor, WorldID,  (const char *)&dwCheckID, sizeof(dwCheckID));
}

// 场景服发给社会服请求检测取消操作事件
void CWarMatch::OnMsgCheckCancelMatch(ClientID clientID,DWORD dwMatchTypeID)
{
	if (NULL == clientID)
	{
		return;
	}
	
	// 根据clientID 获取DBID 和 世界ID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__" pActorService == NULL");
        return;
    }
	PDBID idActor = pActorService->getProperty_Integer(PROPERTY_ID);
	WORD WorldID = pActorService->getFromGameWorldID();
	// 发本区检测信息消息给社会服
	SendDataToSocial(MSG_MATCH_DATE_MODULEMSG, CS_MSG_MATCH_CANCEL_MATCH, dwMatchTypeID, idActor, WorldID,  0, 0);
}

/** 收到请求撮合
@param  IPerson * pPerson		: 提交撮合信息的人物
@param  DWORD dwMatchTypeID		: 撮合分类ID
@param	BYTE nRateFlag			: 倍率位标识
@return  bool :成功否
*/
bool CWarMatch::OnMsgRequestAddMatch(ClientID clientID,DWORD dwMatchTypeID,BYTE nRateFlag)
{
	if (NULL==clientID)
	{
		return false;
	}

    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__" pActorService == NULL");
        return false;
    }

    DWORD dwActorID = pActorService->getProperty_Integer(PROPERTY_ID);
    if (IsGM(dwActorID))
    {
        g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_CANT_REQUEST,"");
        return false;
    }
    
    // 引导地图 排位  压力测试类型 不用判定场次
    // 其他比赛类型 判定场次 自动转换到人机
    if ( !(MatchType_NewPlayerGuide2 == dwMatchTypeID || MatchType_Rank == dwMatchTypeID || MatchType_PressureTest == dwMatchTypeID || MatchType_MultiCampMatch== dwMatchTypeID))
    {
        // 如果是小于一定场次自动进入人机撮合
        if ( pActorService->getTotalMatchNum() <= MATCH_PERSON_AT_LEAST_PKNUM )
        {
            dwMatchTypeID = MatchType_First5Match;
        }
    }

    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return false;
    }

    ISchemeMatchRoom * pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom();
    if (pSchemeMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__" can't pSchemeMatchRoom == NULL");
        return false;
    }
    
    if (pSchemeMatchRoom->getSchemeMatchRoomByMatchTypeID(dwMatchTypeID) == nullptr )
    {
        ErrorLn(__FUNCTION__" can't find SchemeInfo dwMatchTypeID ="<<dwMatchTypeID);
        return false;
    }

	obuf1024 obufData;
	// 场景服发给社会服请求撮合信息
	SMsgMatchRequestInfo_SO sendData;
    sendData.idActor        = pActorService->getProperty_Integer(PROPERTY_ID);
    sendData.dwMatchType    = dwMatchTypeID;
    sendData.dwWorldID      = pActorService->getFromGameWorldID();
    sendData.dwSrcSceneID   = pActorService->getSceneID();
	obufData.push_back(&sendData, sizeof(SMsgMatchRequestInfo_SO));

	// 发本区撮合信息消息给社会服
	return SendDataToSocial(MSG_MATCH_DATE_MODULEMSG, CS_MSG_MATCH_REQUEST_ADDMATCH,dwMatchTypeID,sendData.idActor,0,obufData.data(),obufData.size());

}


// 撮合结果处理
void CWarMatch::OnMatchStrikeResult(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len)
{
	// 是否发给客户端
    IActorService* pPerson = ActorHelper(dwActorID, ACTOR_DBID).m_ptr;
    if (pPerson == NULL)
    {
        ErrorLn(__FUNCTION__": pPerson == NULL dwActorID="<<dwActorID);
        return; 
    }

	bool bSendToClient = true;

	// 结果 EMMatchStrikeResultFlag
	switch(dwResultFlag)
	{
	case MatchResult_AddOK:		// 加入成功
		{
			
            // 发送到客户端的数据
            if (len != sizeof(SMsgAddOrRematchResultInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_MATCH);
            SMsgAddOrRematchResultInfo *pInfo = (SMsgAddOrRematchResultInfo *)data;
            obuf ob;
            ob.push_back(pInfo, sizeof(SMsgAddOrRematchResultInfo));
            SendDataToClient( dwActorID, SC_MSG_IS_MATCHING, ob.data(), ob.size());
		}
		break;
    case MatchResult_ReMatch:		// 重新进入匹配
        {
            
            if (len != sizeof(SMsgAddOrRematchResultInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_MATCH);
            // 发送到客户端的数据
            SMsgAddOrRematchResultInfo *pInfo = (SMsgAddOrRematchResultInfo *)data;
            obuf ob;
            ob.push_back(pInfo, sizeof(SMsgAddOrRematchResultInfo));
            SendDataToClient( dwActorID, SC_MSG_IS_MATCHING, ob.data(), ob.size());

            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_ACTOR_OUT_REMATCH,"");
        }
        break;
	case MatchResult_ExistAdd:	// 已经加入
		{
            pPerson->setGameState(GAME_STATE_MATCH);
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_YOU_HAVEREQUEST,"");
		}
		break;
	case MatchResult_CancleOK:	// 退出撮合
		{
			
            pPerson->setGameState(GAME_STATE_IDLE);
            SendDataToClient(dwActorID, SC_MSG_CANCEL_MATCH, 0, 0);
		}
		break;
    case MatchResult_OtherCancle:	// 其他玩家退出撮合
        {
            if (len != sizeof(SMsgMatchOtherCancelInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_IDLE);
            SMsgMatchOtherCancelInfo *pData = (SMsgMatchOtherCancelInfo *)data;
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_ACTOR_EXIT_MATCH,pData->szName);
        }
        break;
        
	case MatchResult_MatchOK:	// 撮合成功
		{

            pPerson->setGameState(GAME_STATE_INROOM);
            if(pPerson->getPKState() != PK_STATE_NOMAL)
            {
                // 通知PK部件停止
                pPerson->forceEndPK();
            }
            SendDataToClient(dwActorID, SC_MSG_MATCH_OK, 0, 0);
		}
		break;
	case MatchResult_MatchFail:	// 撮合失败
		{
            pPerson->setGameState(GAME_STATE_IDLE);
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_FAIL_RETRY);
            SendDataToClient(dwActorID, SC_MSG_MATCH_FAIL, 0, 0);
		}
    case MatchResult_ActorLogOut:	// 有玩家登出
        {
            if (len != sizeof(SMsgMatchOutInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_IDLE);
            SMsgMatchOutInfo *pData = (SMsgMatchOutInfo *)data;
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_ACTOR_EXIT_MATCH,pData->szName);
            SendDataToClient(dwActorID, SC_MSG_MATCH_FAIL, 0, 0);
        }
		break;
    case MatchResult_RefuseMatch:	// 玩家拒绝比赛
        {
            if (len != sizeof(SMsgMatchRefuseInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_IDLE);
            SMsgMatchRefuseInfo *pData = (SMsgMatchRefuseInfo *)data;
            if (pData->bIsSelf)
            {
                g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_SELF_REFUSE_MATCH);
            }
            else
            {
                 g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_REFUSE_MATCH,pData->szName);
            }
           
            SendDataToClient(dwActorID, SC_MSG_MATCH_REFUSE, 0, 0);
        }
        break;

    case MatchResult_RefuseReMatch:	// 被玩家拒绝比赛重新进入比赛
        {
            if (len != sizeof(SMsgAddOrRematchResultInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_MATCH);
            // 发送到客户端的数据
            SMsgAddOrRematchResultInfo *pInfo = (SMsgAddOrRematchResultInfo *)data;
            obuf ob;
            ob.push_back(pInfo, sizeof(SMsgAddOrRematchResultInfo));
            SendDataToClient( dwActorID, SC_MSG_IS_MATCHING, ob.data(), ob.size());

            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_REFUSE_REMATCH);
        }
        break;
    case MatchResult_CreateRoomFailedReMatch:	// 创建房间失败重新进入撮合
        {
            if (len != sizeof(SMsgAddOrRematchResultInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_MATCH);
            // 发送到客户端的数据
            SMsgAddOrRematchResultInfo *pInfo = (SMsgAddOrRematchResultInfo *)data;
            obuf ob;
            ob.push_back(pInfo, sizeof(SMsgAddOrRematchResultInfo));
            SendDataToClient( dwActorID, SC_MSG_IS_MATCHING, ob.data(), ob.size());

            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_CREATEROOM_FAIL_REFUSE_MATCH);
        }
        break;

    case MatchResult_GroupCampFailedReMatch:	// 分组阵营失败
        {
            if (len != sizeof(SMsgAddOrRematchResultInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_MATCH);
            // 发送到客户端的数据
            SMsgAddOrRematchResultInfo *pInfo = (SMsgAddOrRematchResultInfo *)data;
            obuf ob;
            ob.push_back(pInfo, sizeof(SMsgAddOrRematchResultInfo));
            SendDataToClient( dwActorID, SC_MSG_IS_MATCHING, ob.data(), ob.size());

            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_GROUP_CAMP_FAIL_REMATCH);
        }
        break;
    case MatchResult_NotEnoughPersonReMatch:	// 人数不够
        {
            if (len != sizeof(SMsgAddOrRematchResultInfo))
            {
                return;
            }
            pPerson->setGameState(GAME_STATE_MATCH);
            // 发送到客户端的数据
            SMsgAddOrRematchResultInfo *pInfo = (SMsgAddOrRematchResultInfo *)data;
            obuf ob;
            ob.push_back(pInfo, sizeof(SMsgAddOrRematchResultInfo));
            SendDataToClient( dwActorID, SC_MSG_IS_MATCHING, ob.data(), ob.size());

            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_MATCH_NOT_ENOUGH_PERSON_REMATCH);
        }
        break;
        
    case MatchResult_CollectCheckError:	// 被玩家拒绝比赛重新进入比赛
        {
            // 发送到客户端的数据
            DWORD dwChatID = dwUserData;
            if (len > 0)
            {
                char *pInfo = (char *)data;
                g_EHelper.showSystemMessage(dwActorID, (EMChatTipID)dwChatID, pInfo);
            }
            else
            {
                g_EHelper.showSystemMessage(dwActorID, (EMChatTipID)dwChatID, "");
            }
            
        }
        break;

    case MatchResult_HavePunishInfo:	// 正在被惩罚消息
        {
            // 发送到客户端的数据
            SendDataToClient( dwActorID, SC_MSG_MATCH_REQUEST_PUNISH_TIMEINFO, (LPCSTR) data, len);
        }
        break;

    default:
        break;
	}
}

void CWarMatch::OnMatchRequestSceneCheck(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len)
{
    IActorService* pPerson = ActorHelper(dwActorID, ACTOR_DBID).m_ptr;

    bool isPassed = true;
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__ ": nullptr == pSchemeCenter");
        return;
    }


    if (data == NULL || len < sizeof(SMsgMatchCheck_OS))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< len);
        return;
    }

    SMsgMatchCheck_OS* pMsg = (SMsgMatchCheck_OS*) data;
    ActorHelper helper(pMsg->dwCheckActorID, ACTOR_DBID);
    IActorService * pActorService = helper.m_ptr;
    if (pActorService == NULL)
    {
        ErrorLn(__FUNCTION__": pActorService == NULL");
        return;
    }

    // 向玩家发送传送进竞技场消息
    SMsgMatchCheckResult_SO sendData;
    sendData.dwRequestNodeID = pMsg->dwRequestNodeID;							// 收集信息查找对应ID
    sendData.nMatchTypeID = pMsg->nMatchTypeID;                     // 撮合类型ID
    sendData.dwActorID  = pMsg->dwCheckActorID;                     // 玩家ID
    sstrcpyn(sendData.szName, pActorService->getName().data(), sizeof(sendData.szName));

    ISchemeMatchRankLimitInfo* pRankLimit = pSchemeCenter->getSchemeMatchRankLimit();
    // 英雄数量检查
    if (pRankLimit)
    {
        // 检查玩家等级
        SMatchRankLimitHeroSchemeInfo* pMatchRankLimitHeroSchemeInfo = pRankLimit->getRankLimitHeroShemeInfo((EMMatchType)pMsg->nMatchTypeID);
        if (pMatchRankLimitHeroSchemeInfo)
        {
            isPassed = pPerson->getProperty_Integer(PROPERTY_LEVEL) >= pMatchRankLimitHeroSchemeInfo->nLimitPlayerLevel? true: false;
            if (!isPassed)
            {
                sendData.nOtherFailedDesID = pRankLimit->getMatchPlayerLevelFailedDesID(pMsg->nMatchTypeID, false);
                sendData.nSelfFailedDesID = pRankLimit->getMatchPlayerLevelFailedDesID(pMsg->nMatchTypeID, true);
            }

            // 检查英雄限定
            if (isPassed)
            {
                isPassed = pActorService->isMatchHeroCheckPassed((EMMatchType)pMsg->nMatchTypeID);
                if (!isPassed)
                {
                    sendData.nOtherFailedDesID = pRankLimit->getMatchHeroFailedDesID(pMsg->nMatchTypeID, false);
                    sendData.nSelfFailedDesID = pRankLimit->getMatchHeroFailedDesID(pMsg->nMatchTypeID, true);
                }
            }
        }
    }

    // 通知社会服结果
    sendData.bResult = isPassed;

    obuf obSendData;
    obSendData.push_back(&sendData,sizeof(sendData));
    SendDataToSocial(MSG_MATCH_DATE_MODULEMSG, OS_MSG_MATCH_SCENE_CHECKDATA, pMsg->nMatchTypeID, 0, 0,obSendData.data(), obSendData.size());

}


void CWarMatch::onSysPlayerGameSatate(DWORD dwActorID,DWORD dwResultFlag,DWORD dwUserData,void * data, size_t len)
{
    // 同步下服务器发下来的状态
    IActorService* pPerson = ActorHelper(dwActorID, ACTOR_DBID).m_ptr;
    if (pPerson != NULL)
    {
        pPerson->setGameState(dwResultFlag);
    }
}

// 发本区撮合信息消息给社会服
bool CWarMatch::SendDataToSocial(BYTE byKeyAction,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen)
{
	// 社会服数据
	ICenterConnectorService *pCenterConnector = gServerGlobal->getCenterConnectorService();
	if (nullptr == pCenterConnector)
	{
		ErrorLn("CWarMatch::SendDataToSocial failed( void * buf, size_t len )! nullptr == pCenterConnector");
		return false;
	}

	obuf256 obMsg;

	SMsgMatchDataSubMsg outData;
	outData.dwMsgCode	= dwMsgCode;						// 子消息代码  EMMatchDataSubMsgCode
	outData.dwUserData1	= dwUserData1;						// 用户数据1
	outData.dwUserData2	= dwUserData2;						// 用户数据2
	outData.dwUserData3	= dwUserData3;						// 用户数据3

	obMsg.push_back(&outData,sizeof(outData));
	obMsg.push_back(pData, dwLen);
	
	pCenterConnector->sendToSocial(MSG_MODULEID_MATCH, byKeyAction, obMsg.data(), obMsg.size());
	return true;
}

// 发本区撮合信息消息给场景服
bool CWarMatch::SendDataToZone(DWORD dwZoneServerID,BYTE byKeyAction,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen)
{
	obuf256 obMsg;
	SNetMsgHead header;
	header.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	header.byDestEndPoint= MSG_ENDPOINT_SCENE;
	header.byKeyModule  = MSG_MODULEID_MATCH;
	header.byKeyAction  = byKeyAction;						// 服务器撮合信息子消息

	SMsgMatchDataSubMsg outData;
	outData.dwMsgCode	= dwMsgCode;						// 子消息代码  EMMatchDataSubMsgCode
	outData.dwUserData1	= dwUserData1;						// 用户数据1
	outData.dwUserData2	= dwUserData2;						// 用户数据2
	outData.dwUserData3	= dwUserData3;						// 用户数据3

	obMsg<<header<<outData;

	// 发数据到场景服
	
	IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
	if (nullptr == pMessageDispatch)
	{
		ErrorLn(__FUNCTION__": failed! can not found the IMessageDispatch! ");
		return false;
	}
	pMessageDispatch->dispatchTransmit(dwZoneServerID, obMsg.data(), obMsg.size());

	return true;
}


// 发跨区信息消息 dwServerID:服务器ID ,跨区信息专用
bool CWarMatch::SendDataBridgeToServer(DWORD dwServerID,BYTE byKeyAction,DWORD dwMsgCode,DWORD dwUserData1,DWORD dwUserData2,DWORD dwUserData3,const char * pData,DWORD dwLen)
{
	IBridgeConnectorService* pBridgeConnector = gServerGlobal->getBridgeConnectorService();
	if (pBridgeConnector == NULL)
	{
		return false;
	}

	obuf256 obMsg;
	SNetMsgHead header;
	header.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	header.byDestEndPoint= MSG_ENDPOINT_BRIDGE;
	header.byKeyModule  = MSG_MODULEID_MATCH;
	header.byKeyAction  = byKeyAction;						// 服务器撮合信息子消息

	SMsgMatchDataSubMsg outData;
	outData.dwMsgCode	= dwMsgCode;						// 子消息代码  EMMatchDataSubMsgCode
	outData.dwUserData1	= dwUserData1;						// 用户数据1
	outData.dwUserData2	= dwUserData2;						// 用户数据2
	outData.dwUserData3	= dwUserData3;						// 用户数据3

	obMsg<<header<<outData;
	ibuffer t_in(obMsg.data(),obMsg.size());

	pBridgeConnector->SendDataToServer(dwServerID, t_in, false);

	return true;
}

// 发跨区信息消息 dwServerID:服务器ID ,跨区信息专用
bool CWarMatch::SendDataToClient(DWORD dwPDBID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
    ActorHelper actorHelper(dwPDBID, ACTOR_DBID);
    if (actorHelper.m_ptr == NULL)
        return false;

    // 通知客户端
    obuf256 obufData;

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_MATCH;	
    head.byKeyAction   = byKeyAction;

    obufData << head ;
    obufData.push_back(pszData,nDataLen);

    actorHelper.m_ptr->send2Client(PACKAGE_PTR(new string((const char *)obufData.data(),obufData.size())));
    return true;
}