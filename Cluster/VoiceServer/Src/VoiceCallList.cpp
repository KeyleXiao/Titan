/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\VoiceCallList.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2013-07-29
** 版  本:	1.0
** 描  述:	语音召集缓存列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "VoiceCallList.h"
#include "ClientUser.h"
#include "GlobalVoiceServer.h"
#include "Room.h"

#define VOICE_AD_TIMEOUT	1800		// 已拉招集的过期时间
#define VOICE_CALLFANS_TIMEOUT	1800	// 已拉粉丝的过期时间




// 分数多的的排在前面
static bool SortListCallResultCompare(const SVoiceCheckCacheResultInfo &elem1, const SVoiceCheckCacheResultInfo &elem2)
{
	if( elem1.dwScore > elem2.dwScore )
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////


// 构造函数
CVoiceCallList::CVoiceCallList(void)
{

}

// 析构函数
CVoiceCallList::~CVoiceCallList(void)
{

}


///////////////////// TimerHandler /////////////////////////////////////////////////////
// 定时器，计算麦序时间等
void CVoiceCallList::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_TimeoutCheck:	// 检测是否超时
		{
			DWORD dwTimeNow = (DWORD)time(NULL);
			for( TMAP_ADCacheList::iterator it1 = m_ADCacheList.begin(); it1 != m_ADCacheList.end(); )
			{
				// 已经超时
				if( dwTimeNow >= it1->second.dwTime + VOICE_AD_TIMEOUT )
				{
					m_ADCacheList.erase(it1 ++);
				}
				else
				{
					it1 ++;
				}
			}

			for( TMAP_CallFansCacheList::iterator it2 = m_CallFansCacheList.begin(); it2 != m_CallFansCacheList.end(); )
			{
				// 已经超时
				if( dwTimeNow >= it2->second.dwTime + VOICE_CALLFANS_TIMEOUT )
				{
					m_CallFansCacheList.erase(it2 ++);
				}
				else
				{
					it2 ++;
				}
			}
		}
		break;
	}
}


// 创建
bool CVoiceCallList::Create(void)
{
	gGlobalServer->SetTimer( ETimerEventID_TimeoutCheck, 30000, this, "CVoiceCallList::Create" );
	
	return true;
}

// 释放
void CVoiceCallList::Release(void)
{
	// 关闭定时器
	gGlobalServer->KillTimer( ETimerEventID_TimeoutCheck, this );
}

// 添加一个语音召集令缓存
bool CVoiceCallList::AddVoiceADCacheData( const SVoiceADCacheData & cacheData )
{
	m_ADCacheList[cacheData.nID] = cacheData;

	return true;
}

// 添加一个邀请粉丝缓存
bool CVoiceCallList::AddCallFansCacheData( const SVoiceCallFansCacheData & cacheData )
{
	m_CallFansCacheList[cacheData.dwUDBID] = cacheData;

	return true;
}

// 检测召集缓存
void CVoiceCallList::CheckCache( CClientUser & user )
{
#define VOICE_CALLLIST_CACHE_COUNT 6

	// 临时结果列表
	TLIST_SORTCheckCacheResult tmpList;

	// 检查当前粉丝邀请缓存列表
	CheckUserCallFansCache(user,tmpList);
	CheckUserVoiceADCache(user,tmpList);

	// 只要前6条
	DWORD dwCount = tmpList.size();
	if( dwCount > 0 )
	{
		if( dwCount >= VOICE_CALLLIST_CACHE_COUNT )
		{
			dwCount = VOICE_CALLLIST_CACHE_COUNT;
		}

		partial_sort(tmpList.begin(), tmpList.begin()+dwCount, tmpList.end(), SortListCallResultCompare );
	}

	DWORD dwSendCount = 0;
	DWORD dwClientID = user.GetClientID();
	SVoiceCheckCacheResultInfo InfoArray[VOICE_CALLLIST_CACHE_COUNT];
	memset( InfoArray, 0, sizeof(InfoArray) );
	for( TLIST_SORTCheckCacheResult::iterator it = tmpList.begin(); it != tmpList.end(); ++ it )
	{
		// 找出最大的前6个
		if( dwSendCount >= VOICE_CALLLIST_CACHE_COUNT )
		{
			dwSendCount = VOICE_CALLLIST_CACHE_COUNT;
			break;
		}

		InfoArray[dwSendCount++] = *it;
	}

	for( int i = dwSendCount - 1; i >= 0; i -- )
	{
		SVoiceCheckCacheResultInfo & info = InfoArray[i];
		if( info.pCallData != NULL )
		{
			if( info.bIsCallFans )
			{
				SendCallFansCacheDataToClient( dwClientID, *((SVoiceCallFansCacheData*)info.pCallData) );
			}
			else
			{
				SendADCacheDataToClient( dwClientID, *((SVoiceADCacheData*)info.pCallData) );
			}
		}
	}

}





//////////////////////////////////////////////////////////////////////////
// 检测用户的语音召集令缓存
void CVoiceCallList::CheckUserVoiceADCache( CClientUser & user ,TLIST_SORTCheckCacheResult &tmpList)
{
	CClientList & clientList = CClientList::getInstance();
	CClientUser * pUserFind = NULL;

	CChannelList & channelList = CChannelList::getInstance();

	for( TMAP_ADCacheList::iterator it = m_ADCacheList.begin(); it != m_ADCacheList.end(); ++ it )
	{
		SVoiceADCacheData & cacheData = it->second;

		// 如果是自己，跳过
		if( cacheData.dwUDBID == user.GetUserInfo().dwUDBID )
		{
			continue;
		}

		CClientUser * pUser = clientList.GetClientByUDBID(cacheData.dwUDBID);
		if( pUser == NULL )		// 发送邀请的人不在线
		{
			continue;
		}

		// 发送邀请的人都不在该房间了
		if( cacheData.nID != pUser->GetCurRoomID() )
		{
			continue;
		}

		SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&cacheData.nID);
		LONGLONG nID = user.GetChannelID(pIDInfo->dwType);
		SVoiceChannelID * pCheckIDInfo = (SVoiceChannelID *)(&nID);

		// 如果是个人的,或和自己相关的频道
		if( (VoiceChannel_Personal==pIDInfo->dwType)
			|| (pIDInfo->dwWorldID == pCheckIDInfo->dwWorldID && pIDInfo->dwType == pCheckIDInfo->dwType && pIDInfo->dwID == pCheckIDInfo->dwID) )
		{
			CRoom * pRoom = channelList.GetRoom(cacheData.nID);
			if( pRoom != NULL )
			{
				DWORD dwUserCount = pRoom->Count();
				switch(pRoom->GetType())
				{
				case VoiceChannel_GM:	// 如果是官方频道，需要判断是否要被排除
					{
						// 只发自己区
						bool bSelfWorld		= ( cacheData.dwBroadcastType & VoiceBroadcast_SelfWorld ) > 0 ? true:false;
						// 全区
						bool bAllWorld		= ( cacheData.dwBroadcastType & VoiceBroadcast_AllWorld ) > 0 ? true:false;
						// 除自己区
						bool bExcepSelfWorld = ( cacheData.dwBroadcastType & VoiceBroadcast_ExceptSelfWorld ) > 0 ? true:false;

						bool bSend = false;
						// 全区
						if ( bAllWorld )
						{
							// 发送 除了自己区的 其他所有区
							if ( bExcepSelfWorld  )
							{
								if( user.GetServerID() != pUser->GetServerID() )
								{
									bSend = true;
								}
							}
							// 所有区
							else
							{
								bSend = true;
							}
						}
						// 非全区 只发自己区
						else if ( bSelfWorld )
						{
							if( user.GetServerID() == pUser->GetServerID() )
							{
								bSend = true;
							}
						}

						// 需要发送AD
						if( bSend )
						{
							// 粉丝邀请,召集令符合条件结果,内部用
							SVoiceCheckCacheResultInfo addNode;
							addNode.bIsCallFans = false;
							addNode.dwScore = dwUserCount;
							addNode.pCallData = &cacheData;	
							tmpList.push_back(addNode);
						}
					}
					break;

				case VoiceChannel_Personal:		// 个人频道需要判断是否是管理员
					{
						// 如果是管理员
						if( NULL != pRoom->GetChannel().GetVest(user.GetUserInfo().dwUDBID) )
						{
							// 粉丝邀请,召集令符合条件结果,内部用
							SVoiceCheckCacheResultInfo addNode;
							addNode.bIsCallFans = false;
							addNode.dwScore = dwUserCount;
							addNode.pCallData = &cacheData;	
							tmpList.push_back(addNode);
						}
					}
					break;

				default:
					{
						// 粉丝邀请,召集令符合条件结果,内部用
						SVoiceCheckCacheResultInfo addNode;
						addNode.bIsCallFans = false;
						addNode.dwScore = dwUserCount;
						addNode.pCallData = &cacheData;	
						tmpList.push_back(addNode);
					}
					break;
				}	
			}
		}
	}
}

// 检测用户的粉丝邀请缓存，必须要在用户数据读取完成之后调用
void CVoiceCallList::CheckUserCallFansCache( CClientUser & user,TLIST_SORTCheckCacheResult &tmpList )
{
	CClientList & clientList = CClientList::getInstance();
	CClientUser * pUserFind = NULL;
	CChannelList & channelList = CChannelList::getInstance();

	for( TMAP_CallFansCacheList::iterator it = m_CallFansCacheList.begin(); it != m_CallFansCacheList.end(); ++ it )
	{
		SVoiceCallFansCacheData & cacheData = it->second;

		// 如果是自己，跳过
		if( cacheData.dwUDBID == user.GetUserInfo().dwUDBID )
		{
			continue;
		}

		// 邀请粉丝的人不在线
		CClientUser * pUser = clientList.GetClientByUDBID(cacheData.dwUDBID);
		if( pUser == NULL )
		{
			continue;
		}

		// 邀请粉丝的人不在房间
		if( cacheData.nID != pUser->GetCurRoomID() )
		{
			continue;
		}

		// 如果该用户是邀请粉丝的用户的粉丝
		if( user.IsFansUser(cacheData.dwUDBID) )
		{
			CRoom * pRoom = channelList.GetRoom(cacheData.nID);
			if( pRoom != NULL )
			{
				DWORD dwUserCount = pRoom->Count();

				// 粉丝邀请,召集令符合条件结果,内部用
				SVoiceCheckCacheResultInfo addNode;
				addNode.bIsCallFans = true;
				addNode.dwScore = dwUserCount;
				addNode.pCallData = &cacheData;	
				tmpList.push_back(addNode);
			}
		}
	}
}

// 发送AD缓存数据到客户端
void CVoiceCallList::SendADCacheDataToClient( DWORD dwClientID, const SVoiceADCacheData & cacheData )
{
	obuf2048 obufData;

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_AD;
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	// 拉人数据
	SMsgVoiceAD_SC ADData;
	ADData.dwTime = cacheData.dwTime;
	ADData.dwPDBID = cacheData.dwPDBID;
	ADData.dwUDBID = cacheData.dwUDBID;
	ADData.nSex = cacheData.nSex;
	ADData.nADType = (BYTE)cacheData.dwADType;
	ADData.nChannelType = (BYTE)cacheData.dwChannelType;
	ADData.nRoomID = (BYTE)cacheData.dwRoomID;
	ADData.dwValueID = cacheData.dwValueID;
	ADData.nTextLen = (WORD)strlen(cacheData.szText);
	obufData.push_back( &ADData, sizeof(SMsgVoiceAD_SC) );

	// 附加文本
	if( ADData.nTextLen > 0 )
	{
		obufData.push_back( cacheData.szText, ADData.nTextLen );
	}

	gGlobalServer->SendDataToClient( dwClientID, obufData.data(), obufData.size() );
}

// 发送粉丝邀请缓存数据到客户端
void CVoiceCallList::SendCallFansCacheDataToClient( DWORD dwClientID, const SVoiceCallFansCacheData & cacheData )
{
	obuf1024 obufData;
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_CALLFANS;			// 召集粉丝
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	SVoiceCallFans_VC userData;
	userData.dwTime = cacheData.dwTime;
	userData.dwClientID = cacheData.dwClientID;					// 客户端ID
	userData.nID = cacheData.nID;						// 召集房间ID
	userData.nSex = cacheData.nSex;
	userData.nCountryID = cacheData.nCountryID;			// 国家ID
	userData.dwFromWorldID = cacheData.dwFromWorldID;							// 世界ID
	userData.dwUDBID = cacheData.dwUDBID;										// UDBID
	sstrcpyn( userData.szNickname, cacheData.szNickname, sizeof(userData.szNickname) );		// 昵称
	userData.nMsgLen = (WORD)strlen(cacheData.szText);
	userData.nParam1 = (BYTE)cacheData.dwData1;
	userData.nParam2 = (BYTE)cacheData.dwData2;
	obufData.push_back( &userData, sizeof(SVoiceCallFans_VC) );

	// 文本信息
	if( userData.nMsgLen > 0 )
	{
		obufData.push_back( cacheData.szText, userData.nMsgLen );
	}

	gGlobalServer->SendDataToClient( dwClientID, obufData.data(), obufData.size() );
}