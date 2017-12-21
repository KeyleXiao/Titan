/*******************************************************************
** 文件名:	GameVoiceClient.cpp
** 版  权:	(C) 深圳冰川网络技术股份公司
** 创建人:	谢晓鑫
** 日  期:	2012-11-26
** 版  本:	2.0
** 描  述:	语音系统
** 应  用: 	

**************************** 修改记录 ***************************
** 描  述: ***
** 修改人: 
** 日  期: 
********************************************************************/
#include "StdAfx.h"
#include "GameVoiceClient.h"
#include "ISchemeEngine.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "GameViewDef.h"
#include "IChatClient.h"

/// 构造函数
CGameVoiceClient::CGameVoiceClient(void)
{
	m_pVoiceEngine = NULL;	// 语音引擎

}

/// 析构函数
CGameVoiceClient::~CGameVoiceClient()
{
}

/// 初始化
bool CGameVoiceClient::Create(void)
{
    // 语音系统暂时不支持64位
#if defined(_WIN64)
    return true;
#else
    /*
	ISchemeEngine* pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	bool bResult = pSchemeEngine->LoadScheme( VOICE_CONFIG_FILENAME, (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("加载配置文件失败。文件名=" << VOICE_CONFIG_FILENAME);
		return false;
	}

	bResult = pSchemeEngine->LoadScheme( VOICE_GIFT_CONFIG_FILENAME, (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("加载配置文件失败。文件名=" << VOICE_GIFT_CONFIG_FILENAME);
		return false;
	}
    */

	IMessageDispatch* pMessageDispatch=gClientGlobal->getMessageDispatch();
	if (pMessageDispatch==NULL)
	{
		return false;
	}

	bool bResult = false;
	try
	{
		// 创建语音引擎
		m_pVoiceEngine = CreateVoiceEngine(static_cast<IVoiceEngineProvider*>(this));
		if( NULL != m_pVoiceEngine )
		{
			bResult = true;
		}
	}
	catch (...)
	{
	
	}

	// 订阅 MSG_MODULEID_VOICE 模块消息
	pMessageDispatch->registerMessageHandler(MSG_MODULEID_VOICE, static_cast<IMessageHandler *>(this));

    // 显示层事件
    IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
    if(pEntityClient)
    {
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_OPEN_MIC,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_CHANGE_MUTE_STATE,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_CHANGE_MIC_ORDER_MODE,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_SET_ROOM_FLAG,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_ENTER_ROOM,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_LOCK_ROOM,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_MIC_ORDER,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_SET_MIC_ORDER_TIME,this );
        pEntityClient->registerGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_ADD_VEST,this );
    }

	return bResult;

#endif
}

// 释放操作
void CGameVoiceClient::Release(void)
{
    // 语音系统暂时不支持64位
#ifndef _WIN64
	IMessageDispatch* pMessageDispatch=gClientGlobal->getMessageDispatch();
	if (pMessageDispatch!=NULL)
	{
		// 取消订阅 MSG_MODULEID_VOICE 模块消息
		pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_VOICE, this);
	}
    
    // 显示层事件
    IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
    if(pEntityClient)
    {
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_OPEN_MIC );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_CHANGE_MUTE_STATE );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_CHANGE_MIC_ORDER_MODE );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_SET_ROOM_FLAG );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_ENTER_ROOM );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_LOCK_ROOM );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_MIC_ORDER );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_SET_MIC_ORDER_TIME );
        pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_VOICE_REQUEST_ADD_VEST );
    }

	if( NULL != m_pVoiceEngine )
	{
		IVoiceEngine* pBak = m_pVoiceEngine;  // 修复客户端切换选区语音脚本报错
		m_pVoiceEngine = NULL;
		pBak->Release(); 
	}
#endif

    releaseInstance();
}

// 获得语音引擎
IVoiceEngine * CGameVoiceClient::GetVoiceEngine(void)
{
	return m_pVoiceEngine;
}


// 获得播放视频游戏处理显示接口
IGameVideoShow * CGameVoiceClient::GetGameVideoShow(void)
{
	return m_pVideoShow;
}

// 获取语音配置数值
int CGameVoiceClient::GetVoiceSchemeInfoNum( DWORD dwNumID )
{
	if( dwNumID >= VoiceConfigNumID_Max )
	{
		return 0;
	}

	return m_VoiceSchemeInfo.nVoiceConfigNum[dwNumID];
}

// 获取打折信息
const SVoiceRebateData * CGameVoiceClient::GetVoiceRebateData()
{
	if( m_pVoiceEngine == NULL )
	{
		return NULL;
	}

	IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
	if( pVoiceClient == NULL )
	{
		return NULL;
	}

	// 获取今天的月日
	time_t t = (time_t)pVoiceClient->GetServerTime();
	tm local;
	memset(&local,0,sizeof(local));
	localtime_s(&local,&t);
	int nMonth = local.tm_mon + 1;
	int nMDay = local.tm_mday;

	for( int i = 0; i < VOICE_MAX_GIFT_REBATE_COUNT; i ++ )
	{
		SVoiceRebateData & rebateData = m_VoiceSchemeInfo.VoiceRebateData[i];

		// 如果开放，并且同月同日，就说明是打折日
		if( rebateData.nOpen == 1 && nMonth == rebateData.nMonth && nMDay == rebateData.nDay )
		{
			return &rebateData;
		}
	}

	return NULL;
}

// 获取语音礼物配置
const SVoiceGiftSchemeInfoEx * CGameVoiceClient::GetVoiceGiftSchemeInfo( DWORD dwID )
{
	TMAP_VoiceGiftSchemeInfo::iterator it = m_VoiceGiftSchemeInfo.find(dwID);
	if( it == m_VoiceGiftSchemeInfo.end() )
	{
		return NULL;
	}

	return &(it->second);
}

// 指向礼物列表最前端
void CGameVoiceClient::GiftMoveHead(void)
{
	m_SchemeInfoItr = m_VoiceGiftSchemeInfo.begin();
}

// 指向礼物列表最末端
void CGameVoiceClient::GiftMoveEnd(void)
{
	m_SchemeInfoItr = m_VoiceGiftSchemeInfo.end();
}

// 指向礼物下一个, true:没到结尾 false:到结尾
bool CGameVoiceClient::GiftMoveNext(void)
{
	//到了结尾
	if(	!m_VoiceGiftSchemeInfo.empty() && m_SchemeInfoItr != m_VoiceGiftSchemeInfo.end())
	{
		++m_SchemeInfoItr;
		if(m_SchemeInfoItr != m_VoiceGiftSchemeInfo.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

// 取得礼物列表当前指向的数据
const SVoiceGiftSchemeInfoEx * CGameVoiceClient::GiftGetListData(void)
{
	if (m_SchemeInfoItr==m_VoiceGiftSchemeInfo.end())
	{
		return NULL;
	}

	return  &(m_SchemeInfoItr->second);
}

//////////////////////////////////////////////////////////////////////////


/// 获取时间轴
TimerAxis* CGameVoiceClient::GetTimerAxis()
{
	return gClientGlobal->getTimerAxis();
}

/// 获取事件服务器
IEventEngine* CGameVoiceClient::GetEventEngine()
{
	return gClientGlobal->getEventEngine();
}

/// 获取配置引擎
ISchemeEngine* CGameVoiceClient::GetSchemeEngine()
{
	return gClientGlobal->getSchemeEngine();
}

/// 发送数据
bool CGameVoiceClient::SendData(const char* data, size_t size)
{
    return net_senddata(data, size);
}

/// 是否游戏网络连接中
bool CGameVoiceClient::IsConnected()
{
    return gClientGlobal->getNetConnection()->IsConnected();
}

// 语音引擎的事件回调
void CGameVoiceClient::OnVoiceEvent( DWORD nEventID, void *pEventHead, void *pData, DWORD dwLen )
{
	SEventVoiceBaseHead *pHead = (SEventVoiceBaseHead *)pEventHead;

	bool bHandle = false;

	switch(nEventID)
	{
    case EVENT_VOICE_CLIENT_CONNECTED:
        {
            // 默认进官方大厅
//             IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
//             if(pVoiceClient == NULL)
//             {
//                 ErrorLn(__FUNCTION__ << __LINE__);
//                 return;
//             }
//             SVoiceUserInfo* pUser = pVoiceClient->getVoiceUser();
// 
//             SVoiceSubMsgEnterRoom_CV msg;
//             msg.nRoomID = pUser->channelList[VoiceChannel_GM].channleID;
//             ((SVoiceChannelID*)&msg.nRoomID)->dwRoomID = 0;
// 
//             pVoiceClient->SendDataToServer(MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_EnterRoom, 0, 0, 0, 0, 0, (const char*)&msg, sizeof(msg));
        }
        break;

    case EVENT_VOICE_CLIENT_ALLCHANNELINFOUPDATE: // 频道列表更新
        {
            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return;
            }
            SVoiceUserInfo* pUser = pVoiceClient->getVoiceUser();

            cmd_voice_update_channel_list cmd;
            for(int i = 0; i < VoiceChannel_Max; ++i)
            {
                cmd.channelList[i] = ((SVoiceChannelID*)&(pUser->channelList[i].channleID))->dwID;
            }

            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VOICE_CHANNEL_LIST, 0, nullptr, &cmd, sizeof(cmd));
        }
        break;

    case EVENT_VOICE_CLIENT_ENTERROOMRESULT:		// 进入房间结果
        {
            if(pHead->dwData1 == 0)
            {
                LONGLONG * pRoomID = (LONGLONG *)pData;
                SVoiceChannelID *pIDInfo = (SVoiceChannelID *)pRoomID;

                cmd_voice_enter_room cmd;
                cmd.nWorldID = pIDInfo->dwWorldID;
                cmd.nType = pIDInfo->dwType;
                cmd.nID = pIDInfo->dwID;
                cmd.nRoomID = pIDInfo->dwRoomID;
                cmd.nRight = pHead->dwData2;

                gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VOICE_ENTER_ROOM, 0, nullptr, &cmd, sizeof(cmd));
            }

            // 密码错误
            if(pHead->dwData1 == 1)
            {
                // 飘字
                gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_VOICE_ENTER_ROOM_PASSWD_ERROR);
            }
        }
        break;

    case EVENT_VOICE_CLIENT_CHANNELINFOUPDATE:    // 频道信息更新
        {
            if(m_pVoiceEngine == NULL)
            {
                ErrorLn(__FUNCTION__ << ", m_pVoiceEngine == NULL");
                return;
            }
            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            // 更新房间列表基本信息给显示层
            for(int i = VoiceRoomType_Main; i < VoiceRoomType_Max; i ++)
            {
                IRoom* pRoom = pVoiceClient->GetRoom(i);
                if(pRoom == NULL)
                {
                    continue;
                }
                cmd_voice_update_room_base_info cmd;
                cmd.nRoomID = i;
                cmd.nUserNum = pRoom->Count();
                cmd.isSpeaking = pRoom->IsTalking() ? 1 : 0;
                cmd.isLocked = pRoom->GetStateFlag(VoiceRoomState_Lock) ? 1 : 0;

                gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VOICE_UPDATE_ROOM_BASE_INFO, 0, nullptr, &cmd, sizeof(cmd));
            }
        }
        break;

    case EVENT_VOICE_CLIENT_ROOMINFOUPDATE:   // 房间信息更新
        {
            if(m_pVoiceEngine == NULL)
            {
                ErrorLn(__FUNCTION__ << ", m_pVoiceEngine == NULL");
                return;
            }
            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << ", pVoiceClient == NULL");
                return;
            }
            
            IRoom* pRoom = pVoiceClient->GetRoom();
            if(pRoom == NULL)
            {
                ErrorLn(__FUNCTION__ << ", pRoom == NULL");
                return;
            }

            const SVoiceCurMicUserInfo* pCurMicUser = pRoom->GetCurMicUser();

            cmd_voice_update_room_info cmd;
            cmd.baseInfo.nRoomID = pRoom->GetID();
            cmd.baseInfo.nUserNum = pRoom->Count();
            cmd.baseInfo.isSpeaking = pRoom->IsTalking();

            const SVoiceRoomBaseInfo* pInfo = pRoom->GetBaseInfo(); 
            cmd.nRoomFlag = pInfo->dwStateFlag;
            cmd.nMicOrderMode = pInfo->nMicState;
            cmd.nMicOrderInterval = pInfo->dwMicOrderTime;
            cmd.nCurMicUserClientID = 0;
            if(pCurMicUser != NULL)
            {
                cmd.nCurMicUserClientID = pCurMicUser->dwMicClientID;
            }

            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VOICE_UPDATE_ROOM_INFO, 0, nullptr, &cmd, sizeof(cmd));
        }
        break;

    case EVENT_VOICE_CLIENT_USERINFOUPDATE:  // 用户信息更新
        {
            cmd_voice_update_user_info cmd;
            cmd.nClientID = pHead->dwData1;

            if(pData != NULL)
            {
                cmd.isAdd = 1;

                SVoiceUserInfo_Table* pUser = (SVoiceUserInfo_Table*)pData;   
                cmd.nPDBID = pUser->context.dwPDBID;
                cmd.nUDBID = pUser->context.dwUDBID;
                cmd.nWorldID = pUser->context.dwFromWorldID;
                cmd.nUserState = pUser->context.dwStateFlag;
                memcpy(cmd.szUserName, pUser->context.szName, sizeof(cmd.szUserName));
                cmd.nSex = pUser->context.nSex;
                cmd.nRankLevel = pUser->context.nRankLevel;
                memcpy(cmd.szKinName, pUser->context.szKinName, sizeof(cmd.szKinName));
                cmd.nMicIndex = pUser->context.nMicIndex;
                
                cmd.nUserType = m_pVoiceEngine->GetVoiceClient()->getVoiceUserType(&(pUser->context));
                cmd.nUserRight = m_pVoiceEngine->GetVoiceClient()->calcVoiceUserRight(&(pUser->context)); 
            }
            else
            {
                cmd.isAdd = 0;
            }

            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VOICE_UPDATE_USER_INFO, 0, nullptr, &cmd, sizeof(cmd));
        }
        break;

    case EVENT_VOICE_CLIENT_ISSPEAKING:  // 用户是否正在说话
        {
            cmd_voice_update_user_ispeaking cmd;
            cmd.nClientID = pHead->dwData1;
            cmd.isSpeaking = pHead->dwData2;

            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VOICE_UPDATE_USER_ISSPEAKING, 0, nullptr, &cmd, sizeof(cmd));
        }
        break;

    case EVENT_VOICE_CLIENT_AD:		// 语音召集令
        {
            bHandle = true;

            SMsgVoiceAD_SC *pADData = (SMsgVoiceAD_SC *)pData;
        }
        break;

	case EVENT_VOICE_CLIENT_VIDEOFRAME:	//  视频数据
		{
			bHandle = true;

			// 收到的数据格式不对
			if( dwLen <= 0 ||  pHead->dwData2!=dwLen)
			{
				break;
			}

			//m_gameVideoShow.UpdateVideoFrame(pHead->dwData1,(BYTE*)pData,dwLen);
		}
		break;
	case EVENT_VOICE_CLIENT_CHATMSG:	//  聊天  语音服务器发来的聊天消息
		{
			bHandle = true;

			// 没有聊天数据，直接退出
			if( dwLen <= sizeof(SVoiceSubMsgSendChatMsg_VC) )
			{
				break;
			}

			SVoiceSubMsgSendChatMsg_VC *pChatMsg = (SVoiceSubMsgSendChatMsg_VC *)pData;
			if( dwLen != sizeof(SVoiceSubMsgSendChatMsg_VC) + pChatMsg->dwTextLen )
			{
				break;
			}

			char szText[1024] = {0};
			if( pChatMsg->dwTextLen >= sizeof(szText) )
			{
				break;
			}

			char *pChatText = (char *)pData + sizeof(SVoiceSubMsgSendChatMsg_VC);
			strncpy( szText, pChatText, pChatMsg->dwTextLen );
			
			if( pChatMsg->bTrace )
			{
				TraceLn(szText);
			}
			else
			{
                gClientGlobal->getChatClient()->showSystemMessage(CHAT_TIP_VOICE_BEGIN, szText);
			}
		}
		break;

	case EVENT_VOICE_CLIENT_TEXTCHATMESSAGE:	// 文本聊天
		{
			bHandle = true;

			SMsgVoiceTextChatMessage_VC * pMsg = (SMsgVoiceTextChatMessage_VC *)pData;

			char szName[64] = {0};
			char *pName = (char*)pData + sizeof(SMsgVoiceTextChatMessage_VC);
			sstrcpyn( szName, pName, pMsg->nNameLen+1 );

			char szText[512] = {0};
			char *pText = (char*)pData + sizeof(SMsgVoiceTextChatMessage_VC) + pMsg->nNameLen;
			
			sstrcpyn( szText, pText, pMsg->nTextLen+1 );
			
			// 频道为0 代表私聊
			if ( pMsg->nChannel == 0 )
			{
				// 当前时间 
				char szTimeText[64] = {0};
				char szTimeTextTemp[64] = {0};
	
				// 世界名称
				char szWorldName[32] = {0};

				DWORD tmp_now = (DWORD)time(NULL);
				time_t tSec = tmp_now;
				tm* tmTime = localtime(&tSec);
				if (tmTime != NULL)
				{
					sprintf_s(szTimeTextTemp, sizeof(szTimeTextTemp), "%02d:%02d:%02d", tmTime->tm_hour, tmTime->tm_min,tmTime->tm_sec);
				}

				// 目标PDBID
				DWORD dwTargetPDBID = pMsg->dwTarget;
				// 发送者PDBID
				DWORD dwSenderPDBID = pMsg->dwPDBID;

				// 传参
				CLuaParam luaparamTemp[5];
				luaparamTemp[0] = dwTargetPDBID ;	
				luaparamTemp[1] = dwSenderPDBID  ;
				luaparamTemp[2] = szText;
				
				IClientEntity* hero = gClientGlobal->getHero();
				if ( hero )
				{
					DWORD dwHeroPDBID = hero->getIntProperty(PROPERTY_ID);
					// 如果发送者 不是自己 则根据私聊原来逻辑 赋予目标和发送者相同PDBID，对，是相同PDBID -_-!!，代表着对方接收到私聊消息 否则 是自己收到自己发的消息
					if ( dwHeroPDBID != dwSenderPDBID )
					{
						luaparamTemp[0] = dwSenderPDBID ;	
						dwTargetPDBID	= dwSenderPDBID;
						
						if ( m_pVoiceEngine )
						{	
							IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
							if ( pVoiceClient )
							{
								const char* szWorldNameTemp = pVoiceClient->GetWorldName( pMsg->dwWorldID );
								if ( szWorldNameTemp && strlen( szTimeTextTemp ) > 2 )
								{
									sstrcpyn( szWorldName, szWorldNameTemp, sizeof(szWorldName) );
									// 区名
									ssprintf_s( szTimeText,sizeof(szTimeText), "%s][%s"  ,szWorldName ,szTimeTextTemp);
								}
							}
						}
					}	
					// 是自己 直接用时间
					else
					{
						ssprintf_s( szTimeText,sizeof(szTimeText), "%s"  ,szTimeTextTemp);
					}
					
				}	



				luaparamTemp[3] = szTimeText;

				// 发送者与目标者 相同则代表着 对方接收到了私聊消息
				if ( dwTargetPDBID == dwSenderPDBID )
				{
					luaparamTemp[4] = szName;
				}
				else
				{
					luaparamTemp[4] = "";
				}

#ifndef VOICE_PRINT_DEBUG_INF
				TraceLn("dwTargetPDBID:"<<dwTargetPDBID<<",dwSenderPDBID:"<<dwSenderPDBID);
#endif

				//GetLuaEngine()->RunLuaFunction("Friend_OnMsgArrived",luaparamTemp,5);
			}
			// 语音频道消息
			else
			{
				//gClientGlobal->getChatClient()->addVoiceChatMessage( pMsg->nChannel, pMsg->dwWorldID, pMsg->nCountry, pMsg->dwPDBID, pMsg->dwTarget, szName, szText );
			}
			
			
		}
		break;

	case EVENT_VOICE_CLIENT_VOICELIST:		// 正在语音人员列表(GM)
		{
		}
		break;

	case EVENT_VOICE_CLIENT_CHATLIST:			// 私聊列表
		{
			if( pHead->dwData1 == EventChatList_Add )
			{
				bHandle = true;
				char szNameInviter[VOICE_NAME_MAXSIZE] = {0};
				char szNameAcceptor[VOICE_NAME_MAXSIZE] = {0};
				
				SMsgVoiceChatNode_VC * pNodeData = (SMsgVoiceChatNode_VC *)pData;
				char * pNameInviter = (char*)pData + sizeof(SMsgVoiceChatNode_VC);
				sstrcpyn( szNameInviter, pNameInviter, pNodeData->nNameLenInviter + 1 );

				char * pNameAcceptor = (char*)pData + sizeof(SMsgVoiceChatNode_VC) + pNodeData->nNameLenInviter;
				sstrcpyn( szNameAcceptor, pNameAcceptor, pNodeData->nNameLenAcceptor + 1 );

				CLuaParam luaparam[7];
				luaparam[0] = pNodeData->dwChatID;
				luaparam[1] = pNodeData->dwWorldIDInviter;
				luaparam[2] = pNodeData->dwWorldIDAcceptor;
				luaparam[3] = szNameInviter;
				luaparam[4] = szNameAcceptor;
				luaparam[5] = pNodeData->dwInviterUDBID;
				luaparam[6] = pNodeData->dwAcceptorUDBID;

				//GetLuaEngine()->RunLuaFunction( "Voice_ChatListAdd", luaparam, 7 );
			}
		}
		break;

	case EVENT_VOICE_CLIENT_CHANNELAUDIO:		// 频道语音推送
		{
			bHandle = true;

			LONGLONG * pRoomID = (LONGLONG *)pData;
			SVoiceChannelID *pIDInfo = (SVoiceChannelID *)pRoomID;
#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn("EVENT_VOICE_CLIENT_CHANNELAUDIO pHead->dwData1=" << pHead->dwData1);
#endif

			CLuaParam luaparam[5];
			luaparam[0] = pHead->dwData1;
			luaparam[1] = pIDInfo->dwID;
			luaparam[2] = pIDInfo->dwRoomID;
			luaparam[3] = pIDInfo->dwType;
			luaparam[4] = pIDInfo->dwWorldID;

			//GetLuaEngine()->RunLuaFunction( "Voice_ShowChannelAudioIcon", luaparam, 5 );
		}
		break;

	case EVENT_VOICE_CLIENT_GIFTSENDINFO:		// 送礼信息
		{
			bHandle = true;

			SMsgVoiceGiftSendInfo * pMsg = (SMsgVoiceGiftSendInfo *)pData;

			char szNameSend[VOICE_MSGSTR_MAXSIZE] = {0};
			char szName[VOICE_MSGSTR_MAXSIZE] = {0};
			char szMessage[1024] = {0};

			// 赠送人
			char *pName = (char*)pData + sizeof(SMsgVoiceGiftSendInfo);
			sstrcpyn( szNameSend, pName, pMsg->nNameLenSend+1 );

			// 接收人
			pName = (char*)pData + sizeof(SMsgVoiceGiftSendInfo) + pMsg->nNameLenSend;
			sstrcpyn( szName, pName, pMsg->nNameLen + 1 );

			// 赠言
			pName = (char*)pData + sizeof(SMsgVoiceGiftSendInfo) + pMsg->nNameLenSend + pMsg->nNameLen; 
			sstrcpyn( szMessage, pName, pMsg->nMessageLen + 1 );

			CLuaParam luaparam[16];
			luaparam[0] = pMsg->dwTime;
			luaparam[1] = pMsg->dwPDBIDSend;	// 赠送人
			luaparam[2] = pMsg->nWorldIDSend;	// 赠送人世界ID
			luaparam[3] = pMsg->nCountrySend;	// 赠送人国家ID

			luaparam[4] = pMsg->dwPDBID;	// 接收人
			luaparam[5] = pMsg->nWorldID;	// 接收人世界ID
			luaparam[6] = pMsg->nCountry;	// 接收人国家ID

			luaparam[7] = pMsg->nGiftID;	// 礼物ID
			luaparam[8] = pMsg->nGiftLevel;	// 礼物等级

			luaparam[9] = szNameSend;	// 赠送人名称
			luaparam[10] = szName;		// 接收人名称
			luaparam[11] = szMessage;	// 赠言

			luaparam[12] = pMsg->dwFlag;	//  位标识	EMVoiceSendGiftFlag
			luaparam[13] = pMsg->dwData1;	// 备用自定义1
			luaparam[14] = pMsg->dwData2;	// 备用自定义2
			luaparam[15] = pMsg->dwData3;	// 备用自定义3

			//GetLuaEngine()->RunLuaFunction( "OnEventVoiceGiftSendInfo", luaparam, 16 );
		}
		break;

	case EVENT_VOICE_CLIENT_ADMINENTERINFO:
		{
			bHandle = true;

			SMsgVoiceAdminEnterInfo_VC *pMsg = (SMsgVoiceAdminEnterInfo_VC *)pData;
			char szAdminName[VOICE_MSGSTR_MAXSIZE] = {0};
			
			char * pName = (char*)pData + sizeof(SMsgVoiceAdminEnterInfo_VC); 
			sstrcpyn( szAdminName, pName, pMsg->nNameLen + 1 );

			CLuaParam luaparam[6];
			luaparam[0] = pMsg->bEnter ? 1 : 0;	// 是否进入 true进，false 出
			luaparam[1] = pMsg->dwClientID;	// 客户端ID
			luaparam[2] = pMsg->nWorldID;	// 来源世界ID
			luaparam[3] = pMsg->dwPDBID;	// PDBID
			luaparam[4] = pMsg->nRoomID;	// 进出的房间ID
			luaparam[5] = szAdminName;	// 角色名称
			//GetLuaEngine()->RunLuaFunction( "OnEventVoiceAdminEnterInfo", luaparam, 6 );
		}
		break;
	}

	if( !bHandle )
	{
		//GetLuaEngine()->RunLuaFunction("Voice_OnVoiceEvent",luaparam,4);
	}
}

// 游戏网络消息处理
void CGameVoiceClient::onMessage( SNetMsgHead* head, void* data, size_t len)
{
    switch(head->byKeyAction)
    {
    case MSG_VOICE_AD:		// 频道拉人
        OnMessageAD( head->byKeyAction, head, data, len);
        break;

    default:
        m_pVoiceEngine->OnGameMessage( head->byKeyAction, head, data, len );
        break;
    }
}

bool CGameVoiceClient::onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam )
{
    switch(eventID)
    {
    case GVIEWCMD_VOICE_REQUEST_ENTER_ROOM: // 请求进入房间
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_enter_room* pCmd = (cmd_voice_req_enter_room*)ptrParam;
            if(pCmd->nType >= VoiceChannel_Max || pCmd->nType < VoiceChannel_None)
            {
                ErrorLn(__FUNCTION__ << ", type=" << pCmd->nType);
                return false;
            }

            if(pCmd->nRoomID < VoiceRoomType_Main || pCmd->nRoomID >= VoiceRoomType_Max)
            {
                ErrorLn(__FUNCTION__ << ", type=" << pCmd->nRoomID);
                return false;
            }

            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            SVoiceUserInfo* pUser = pVoiceClient->getVoiceUser();

            SVoiceSubMsgEnterRoom_CV msg;
            msg.nRoomID = pUser->channelList[pCmd->nType].channleID;

            SVoiceChannelID* pIDInfo = (SVoiceChannelID*)&msg.nRoomID;
            pIDInfo->dwRoomID = pCmd->nRoomID; 

            memcpy(msg.szPassword, pCmd->szPassword, sizeof(msg.szPassword));
            
            pVoiceClient->SendDataToServer(MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_EnterRoom, 0, 0, 0, 0, 0, (const char*)&msg, sizeof(msg));
        }
        break;
    case GVIEWCMD_VOICE_REQUEST_OPEN_MIC:  // 请求打开/关闭麦
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_open_mic* pCmd = (cmd_voice_req_open_mic*)ptrParam;

            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            pVoiceClient->OpenMic(pCmd->isOpen == 0 ? false : true);
        }
        break;
    case GVIEWCMD_VOICE_REQUEST_CHANGE_MUTE_STATE: // 请求静音/取消静音
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_change_mute_state* pCmd = (cmd_voice_req_change_mute_state*)ptrParam;

            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }

            int nOpenHeadset = (pCmd->isMute == 0 ? 1 : 0); 
            pVoiceClient->SendDataToServer(MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_OpenHeadset, nOpenHeadset);
        }
        break;
    case GVIEWCMD_VOICE_REQUEST_CHANGE_MIC_ORDER_MODE:  // 请求改变控麦模式
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_change_mic_order_mode* pCmd = (cmd_voice_req_change_mic_order_mode*)ptrParam;

            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            pVoiceClient->SendDataToServer(MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_ChangeMicState, pCmd->nMode);
        }
        break;
    case GVIEWCMD_VOICE_REQUEST_SET_ROOM_FLAG:  // 请求设置房间状态标志
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_set_room_flag* pCmd = (cmd_voice_req_set_room_flag*)ptrParam;

            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            pVoiceClient->SendDataToServer(MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_SetRoomStateFlag, pCmd->nFalg, pCmd->isSet);
        }
        break;
    case GVIEWCMD_VOICE_REQUEST_LOCK_ROOM:  // 请求设置房间密码
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_lock_room* pCmd = (cmd_voice_req_lock_room*)ptrParam;

            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            pVoiceClient->SendDataToServer(MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_SetRoomLock, pCmd->nRoomID, pCmd->isLock, 0, 0, 0, pCmd->szPassword, strlen(pCmd->szPassword));
        }
        break;
    case GVIEWCMD_VOICE_REQUEST_MIC_ORDER:  // 请求上麦/下麦
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_mic_order* pCmd = (cmd_voice_req_mic_order*)ptrParam;

            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            pVoiceClient->SendDataToServer(MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_AddMicOrder, pCmd->isAcquire);
        }
        break;
    case GVIEWCMD_VOICE_REQUEST_SET_MIC_ORDER_TIME:  // 请求设置麦序时间
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_set_mic_order_time* pCmd = (cmd_voice_req_set_mic_order_time*)ptrParam;

            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            pVoiceClient->SendDataToServer(MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_SetMicTime, pCmd->nRoomID, pCmd->nSec);
        }
        break;
    case GVIEWCMD_VOICE_REQUEST_ADD_VEST: // 请求添加马甲
        {
            if(ptrParam == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            cmd_voice_req_add_vest* pCmd = (cmd_voice_req_add_vest*)ptrParam;
            IVoiceClient* pVoiceClient = m_pVoiceEngine->GetVoiceClient();
            if(pVoiceClient == NULL)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return false;
            }
            pVoiceClient->AddVest(pCmd->nUserID, pCmd->isAdd != 0, pCmd->nVestType);
        }
        break;
    default:
        {
            TraceLn(__FUNCTION__ << ", event =" << eventID);
        }
    }
    return true;
}

///////////////////////// ISchemeUpdateSink /////////////////////////
/// purpose: CSV配置载入时通知
bool CGameVoiceClient::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	if ( strcmpi( szFileName, VOICE_GIFT_CONFIG_FILENAME ) == 0)
	{
		return LoadVoiceGiftSchemeInfo(reader.pCSVReader);
	}

    if ( strcmpi( szFileName, VOICE_CONFIG_FILENAME ) == 0)
    {
        return LoadVoiceSchemeInfo(reader.pXMLReader);
    }

	return false;
}

/// purpose: CSV配置动态更新时通知
bool CGameVoiceClient::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad( reader, szFileName );
}

// 获得游戏激活状态
bool CGameVoiceClient::GetGameActivate()
{
    return true;
}


// 频道拉人
void CGameVoiceClient::OnMessageAD( ulong actionId, SNetMsgHead* head, void* data, size_t len)
{
	if( NULL == data )
	{
		return ;
	}

	if( len < sizeof(SMsgVoiceAD_SC) )
	{
		ErrorLn("CGameVoiceClient::OnMessageAD 频道拉人 len=" << len);
		return ;
	}

	SMsgVoiceAD_SC *pADData = (SMsgVoiceAD_SC *)data;

#ifdef VOICE_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[2048]={0};
	sprintf_s(szBuf, _countof(szBuf), "数据len=%d, 性别=%d, UDBID=%d, 频道类型=%d, 子房间ID=%d, 附加拉人效果=%d, 附加文本长度=%d",
		len, pADData->nSex, pADData->dwUDBID, pADData->nChannelType, pADData->nRoomID, pADData->nADType, pADData->dwTextLen );
	TraceLn(szBuf);
#endif

	if( len != sizeof(SMsgVoiceAD_SC) + pADData->nTextLen )
	{
		ErrorLn("CGameVoiceClient::OnMessageAD 频道拉人数据出错 len=" << len);
		return;
	}

	char szText[2048] = {0};
	char *pText = (char*)data + sizeof(SMsgVoiceAD_SC);
	sstrcpyn( szText, pText, pADData->nTextLen + 1 );

	CLuaParam luaparam[6];
	luaparam[0] = pADData->dwUDBID;
	luaparam[1] = pADData->nSex;
	luaparam[2] = pADData->nADType;
	luaparam[3] = pADData->nChannelType;
	luaparam[4] = pADData->nRoomID;
	luaparam[5] = szText;

	//GetLuaEngine()->RunLuaFunction( "Voice_VoiceAD", luaparam, 6 );

}

// 加载语音配置文件
bool CGameVoiceClient::LoadVoiceSchemeInfo( TiXmlDocument* pTiXmlDocument )
{
	// 找到根节点 scheme
	TiXmlElement * pRootElement = pTiXmlDocument->FirstChildElement("scheme");
	if(pRootElement == NULL)
	{
		ErrorLn("CVoiceServer::LoadVoiceConfigScheme() 找不到配置文件xml根节点[scheme]" );
		return false;
	}

	char szName[128] = {0};
	// 读取数值
	for (DWORD i = VoiceConfigNumID_VoiceGiftOpen; i < VoiceConfigNumID_Max; i++ )
	{
		sprintf_s( szName, sizeof(szName), "number%d", i+1 );

		//m_gameUserCard.GetXmlChildAttributeInt( pRootElement, szName, "value", &(m_VoiceSchemeInfo.nVoiceConfigNum[i]) );
	}

	// 读取打折信息
	for( DWORD i = 0; i < VOICE_MAX_GIFT_REBATE_COUNT; i ++ )
	{
		SVoiceRebateData & data = m_VoiceSchemeInfo.VoiceRebateData[i];
		sprintf_s( szName, sizeof(szName), "RebateDay%d", i + 1 );

		// 是否开启打折
		int nOpen = 0;
		//m_gameUserCard.GetXmlChildAttributeInt( pRootElement, szName, "open", &nOpen );
		data.nOpen = (BYTE)nOpen;

		// 打折日期
		int nDateNum = 0;
		//m_gameUserCard.GetXmlChildAttributeInt( pRootElement, szName, "date", &nDateNum );
		data.nMonth = nDateNum/100;
		data.nDay = nDateNum%100;

		// 打折说明
		//m_gameUserCard.GetXmlChildAttributeString( pRootElement, szName, "tip", data.szTips, sizeof(data.szTips) );
	}

	return true;
}

// 加载语音礼物配置文件
bool CGameVoiceClient::LoadVoiceGiftSchemeInfo( ICSVReader * pCSVReader )
{
	m_VoiceGiftSchemeInfo.clear();

	int nCount = pCSVReader->GetRecordCount();
	for(int i = 0; i < nCount; i++)
	{	
		SVoiceGiftSchemeInfoEx giftSchemeInfo;

		giftSchemeInfo.nID = pCSVReader->GetInt(i, 0, 0);			// ID
		if( giftSchemeInfo.nID > 1000 )
		{
			ErrorLn("CGameVoiceClient::LoadVoiceGiftConfigScheme ID: 行=" << i << "，礼物ID太大！ID=" << giftSchemeInfo.nID );
			return false;
		}

		if( m_VoiceGiftSchemeInfo.find(giftSchemeInfo.nID) != m_VoiceGiftSchemeInfo.end() )
		{
			ErrorLn("CGameVoiceClient::LoadVoiceGiftConfigScheme ID: 行=" << i << "，礼物ID重复！ID=" << giftSchemeInfo.nID );
			return false;
		}

		int nLen = sizeof(giftSchemeInfo.szName);
		pCSVReader->GetString( i, 1, giftSchemeInfo.szName, nLen );	// 礼物名称 

		giftSchemeInfo.nEnable = pCSVReader->GetInt(i,2,0);				// 是否启用

		giftSchemeInfo.nLevel = pCSVReader->GetInt(i, 3, 0);	// 等级
		if( giftSchemeInfo.nLevel > VOICE_MAX_GIFT_LEVEL )
		{
			ErrorLn("CGameVoiceClient::LoadVoiceGiftConfigScheme ID: 行=" << i << "，礼物等级已超过" << VOICE_MAX_GIFT_LEVEL << "级！ID=" << giftSchemeInfo.nID );
			return false;
		}

		int nPrice = pCSVReader->GetInt(i, 4, 0);	// 单价
		if( nPrice <= 3 )
		{
			ErrorLn( "CGameVoiceClient::LoadVoiceGiftConfigScheme 价钱 行=" << i << "，礼物价格小于或等于3" );
			return false;
		}
		giftSchemeInfo.dwPrice = nPrice;

		giftSchemeInfo.nBroadcast = pCSVReader->GetInt( i, 5, 0 );  // 广播范围

		giftSchemeInfo.nRebate = pCSVReader->GetInt(i, 6, 0);	// 折扣
		if( giftSchemeInfo.nRebate > 100 || giftSchemeInfo.nRebate < 50 )
		{
			ErrorLn( "CGameVoiceClient::LoadVoiceGiftConfigScheme 折扣 行=" << i << "，折扣大于100或小于50！" );
			return false;
		}

		giftSchemeInfo.dwSendPoint = pCSVReader->GetInt(i, 7, 0);		// 赠送方获得积分数
		if( giftSchemeInfo.dwSendPoint > giftSchemeInfo.dwPrice )
		{
			ErrorLn( "CGameVoiceClient::LoadVoiceGiftConfigScheme 赠送方获得积分数 行=" << i << "，积分过大！" );
			return false;
		}

		giftSchemeInfo.dwSendGoodsID = pCSVReader->GetInt(i, 8, 0);		// 赠送方获得物品ID
		giftSchemeInfo.dwMaleSendTitleID = pCSVReader->GetInt(i, 9, 0);		// 赠送方获得称号ID
		giftSchemeInfo.dwFemaleSendTitleID = pCSVReader->GetInt(i, 10, 0);		// 赠送方获得称号ID
		giftSchemeInfo.dwSendBuffID = pCSVReader->GetInt(i, 11, 0);		// 赠送方获得BUFFID
		giftSchemeInfo.dwSendFlower = pCSVReader->GetInt(i, 12, 0);		// 赠送方获得鲜花数

		giftSchemeInfo.dwRecvBlueDiamond = pCSVReader->GetInt(i, 13, 0);		// 接收方获得蓝钻数
		if( giftSchemeInfo.dwRecvBlueDiamond > giftSchemeInfo.dwPrice*3 )
		{
			ErrorLn( "CGameVoiceClient::LoadVoiceGiftConfigScheme 接收方获得蓝钻数 行=" << i << "，蓝钻数过大！" );
			return false;
		}

		giftSchemeInfo.dwRecvGoodsID = pCSVReader->GetInt(i, 14, 0);	// 接收方获得物品ID
		giftSchemeInfo.dwMaleRecvTitleID = pCSVReader->GetInt(i, 15, 0);	// 接收方获得称号ID
		giftSchemeInfo.dwFemaleRecvTitleID = pCSVReader->GetInt(i, 16, 0);	// 接收方获得称号ID
		giftSchemeInfo.dwRecvBuffID = pCSVReader->GetInt(i, 17, 0);		// 接收方获得BUFFID
		giftSchemeInfo.dwRecvFlower = pCSVReader->GetInt(i, 18, 0);		// 接收方获得鲜花数

		giftSchemeInfo.nIconPos = pCSVReader->GetInt(i,19,0);			// 动画位置
		giftSchemeInfo.nShowPos = pCSVReader->GetInt(i,20,0);			// 显示位置
		

		nLen = sizeof(giftSchemeInfo.szTooltips);	
		pCSVReader->GetString( i, 21, giftSchemeInfo.szTooltips, nLen ); // tooptips

		giftSchemeInfo.nCoolTime = pCSVReader->GetInt(i,22,0);			 // 冷却时间

		giftSchemeInfo.dwEffectID = pCSVReader->GetInt(i,23,0);			 // 场景层光效ID
		giftSchemeInfo.dwUIEffectID = pCSVReader->GetInt(i,24,0);		 // UI层光效ID

		nLen = sizeof(giftSchemeInfo.szWords1);
		pCSVReader->GetString( i, 25, giftSchemeInfo.szWords1, nLen ); // 赠言1

		nLen = sizeof(giftSchemeInfo.szWords2);
		pCSVReader->GetString( i, 26, giftSchemeInfo.szWords2, nLen ); // 赠言2

		nLen = sizeof(giftSchemeInfo.szWords3);
		pCSVReader->GetString( i, 27, giftSchemeInfo.szWords3, nLen ); // 赠言3

		m_VoiceGiftSchemeInfo.insert( TMAP_VoiceGiftSchemeInfo::value_type( giftSchemeInfo.nID, giftSchemeInfo ) );
	}

	return true;
}

