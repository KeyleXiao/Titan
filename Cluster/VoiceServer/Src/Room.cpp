/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ClientUser.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2012-12-10
** 版  本:	1.0
** 描  述:	频道
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "Channel.h"
#include "ClientList.h"
#include "GlobalVoiceServer.h"


#define VOICE_MIC_TIME_LEN			300			// 麦序默认时间 5分钟（300秒）
#define VOICE_MAX_MIC_TOTAL_TILE	9999		// 麦上人说话最大时间

#define VOICE_MAX_ROOMSPEAK_COUNT	20			// 一个房间最多已开麦说话的用户个数,和队伍人数差不多,这样队伍抢不用冲突
#define VOICE_MAX_ROOMSPEAK_DELAYS	3000		// 自由模式下,已开麦说话的用户多少毫秒不说,算是没开麦,默认3000毫秒


// 构造函数
CRoom::CRoom(void)
	: m_dwID(0)
	, m_nID(0)
	, m_pChannel(NULL)
	, m_bChanged(false)
	, m_bLoaded(false)
	, m_bSaved(true)
	, m_dwStateFlag(0)
	, m_dwMicState(0)
	, m_dwLastClearSpeakListTick(0)
	, m_dwCountUpdateTime(0)
	, m_dwLastAudioTime(0)
	, m_dwSpare1(0)
	, m_dwSpare2(0)
	, m_dwSpare3(0)
	, m_dwChatCoolDown(0)
	, m_bStatsMaxMember(false)
	, m_dwMaxMemberNum(0)
{
	memset( m_szPassword, 0, sizeof(m_szPassword) );
	memset( m_szName, 0, sizeof(m_szName) );
}

// 析构函数
CRoom::~CRoom(void)
{

}


///////////////////// TimerHandler /////////////////////////////////////////////////////
// 定时器，计算麦序时间等
void CRoom::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_MicOrder:
		{
			m_MicInfo.dwMicStartTime = (DWORD)time(NULL);

			// 如果增加了时间，继续计时
			if( m_MicInfo.dwMicOrderCount > 0 )
			{
				
				m_MicInfo.dwMicOrderCount --;
			}
			
			// 到时间了
			if( m_MicInfo.dwMicOrderCount <= 0 )
			{
				// 杀掉定时器
				gGlobalServer->KillTimer( ETimerEventID_MicOrder, this );

				CClientUser * pUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
				if( NULL != pUser )
				{
					DWORD dwError = 0;
					DelMicOrder( *pUser, dwError );
				}
			}
			else if( m_MicInfo.bRestartTimer )	// 控麦结束后，要重新恢复定时器周期时间
			{
				m_MicInfo.dwMicTimeLeft = 0;			// 控麦下，当前麦序上当前计时周期里还剩多少秒
				m_MicInfo.bRestartTimer = false;		// 要重新启动计时器

				// 杀掉定时器，重启
				gGlobalServer->KillTimer( ETimerEventID_MicOrder, this );
				gGlobalServer->SetTimer( ETimerEventID_MicOrder, m_MicInfo.dwCurMicOrderTime*1000, this, "CRoom::OnTimer" );
			}
		}
		break;

	case ETimerEventID_MicCheck:	// 检测麦序时间是否过期
		{
			// 当前有人
			if( m_MicInfo.dwCurMicUser > 0 )
			{
				CClientUser * pUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
				if( NULL != pUser )
				{
					// 判断上一次说话的时间和当前相差多少
					DWORD dwNow = ::GetTickCount();
					// 如果在这段时间里都没有说话，直接下麦，并且没有控麦，控麦就直接跳过了，
					if( dwNow >= pUser->m_dwLastAudioTime + m_MicInfo.dwMicInvalidTime*1000 )
					{
#ifdef VOICE_PRINT_DEBUG_INF
						ErrorLn("CRoom::OnTimer 系统自动下麦，当前时间=" << dwNow << "，上次说话时间=" << (pUser->m_dwLastAudioTime) << "，无效时间长=" << m_MicInfo.dwMicInvalidTime );
#endif
						char szText[128] = {0};
						sprintf_s( szText, sizeof(szText), "你在%d秒内没有说话，系统自动下麦！", m_MicInfo.dwMicInvalidTime );
						DWORD dwError = 0;
						DelMicOrder( *pUser, dwError );
						pUser->SendChatMsg(szText);
					}
				}
			}
		}
		break;
	}
}


//数据库请求返回回调方法
void CRoom::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	// 出错了就报一下
	if ( nDBRetCode!=DBRET_CODE_SUCCEED )
	{
		ErrorLn("CRoom::OnReturn() 出错了 nCmdID=" << nCmdID << " nDBRetCode=" << nDBRetCode);
		if ( pszDBRetDesc )
		{
			ErrorLn(pszDBRetDesc);
		}

		return;
	}

	switch(nCmdID)
	{
	case VOICEDB_REQUEST_VOICEROOM_SAVE:		// 保存语音频道房间信息
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CRoom::OnReturn() 保存语音频道房间信息 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			// 是否已保存数据
			m_bSaved = true;

			// 是否已加载数据
			m_bLoaded = true;

		}
		break;

	default:
		break;
	}
}



// 创建
bool CRoom::Create( CChannel* pChannel, DWORD dwID )
{
	m_pChannel = pChannel;

	if( dwID >= VoiceRoomType_Max )
	{
		ErrorLn( "房间ID超过了最大值，dwID=" << VoiceRoomType_Max );
		return false;
	}

	m_dwID = dwID;

	const SVoiceChannelID & IDInfo = m_pChannel->GetIDInfo();

	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&m_nID);
	pIDInfo->dwRoomID = m_dwID;		// 房间ID
	pIDInfo->dwType = IDInfo.dwType;		// 频道类型
	pIDInfo->dwWorldID = IDInfo.dwWorldID;	// 世界ID
	pIDInfo->dwID = IDInfo.dwID;

	m_dwType = m_pChannel->GetType();

#ifdef VOICE_CLIENTID_BUFFLIST
	// 初始化m_audioList
	m_audioList.pData	= new DWORD[VOICE_ROOM_MIN_BUFF_SIZE];
	m_audioList.dwSize	= VOICE_ROOM_MIN_BUFF_SIZE;
	m_audioList.dwCount	= 0;
#endif

	// 初始化
	const SVoiceRoomConfig & config = GetRoomConfig( m_dwType, m_dwID );
	m_dwMicState = config.dwMicState;
	
	// 如果是麦序模式，要设置标记
	if( m_dwMicState == VoiceMicState_MicOrder )
	{
		SetFlag( VoiceRoomState_AddMicOrder, true, false, false );
	}

	m_MicInfo.dwMicOrderTime = config.dwMicTimeLen;
	m_MicInfo.dwMicInvalidTime = config.dwMicInvalidTime;

	m_dwChatCoolDown = GetChannelConfig(m_dwType).dwChatCoolDown;

	return true;
}

// 释放
void CRoom::Release(void)
{
	// 关闭数据库回调
	GameDatabaseAccessor::getInstance().RemoveDBRetSink(static_cast<IDBRetSink *>(this));
	// 关闭麦序定时器
	gGlobalServer->KillTimer( ETimerEventID_MicOrder, this );

	gGlobalServer->KillTimer( ETimerEventID_MicCheck, this );

	m_mapList.clear();

	m_mapMic.clear();

#ifdef VOICE_CLIENTID_BUFFLIST
	m_audioList.dwCount = 0;
	delete [] m_audioList.pData;
	m_audioList.pData = NULL;
#endif
}

// 获得频道
CChannel & CRoom::GetChannel(void)
{
	return *m_pChannel;
}

// 获取房间ID
DWORD CRoom::GetID(void)
{
	return m_dwID;
}

// 获取房间ID
LONGLONG CRoom::GetIDEx(void)
{
	return m_nID;
}

// 获取房间类型
DWORD CRoom::GetType(void)
{
	return m_dwType;
}

// 设置位状态标记
void CRoom::SetFlag( DWORD dwFlag, bool bSet, bool bSync, bool bSaveDB )
{
	// 清除指定标识
	DWORD dwMyNewFlag = m_dwStateFlag & (~dwFlag);
	// 如果指定标识为true,设回状态
	if (bSet)
	{
		dwMyNewFlag =  dwMyNewFlag | dwFlag;
	}

	m_dwStateFlag = dwMyNewFlag;

	if( bSaveDB )
	{
		OnDataChanged();
		SaveData();
	}

	if( bSync )
	{
		// 取得房间里所有用户ID
		DWORD dwMaxCount = 0;

		// 如果是加锁，要同步所有频道的人
		if( dwFlag == VoiceRoomState_Lock )
		{
			dwMaxCount = m_pChannel->Count();
		}
		else
		{
			dwMaxCount = Count();
		}
		// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
		__VOICE_GET_ID_BUFFER_START(dwMaxCount);

		DWORD dwCount = 0;
		// 如果是加锁，要同步所有频道的人
		if( dwFlag == VoiceRoomState_Lock )
		{
			m_pChannel->GetClientIDList( pClientIDArray, dwBuffSize, dwCount );
		}
		else
		{
			GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
		}

		if( dwCount > 0 )
		{
			UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_Base );
		}

		// 是new的内存,用完释放
		// ID缓存分配结束
		__VOICE_GET_ID_BUFFER_END;
	}
}

// 获取位状态标记
bool CRoom::GetFlag( DWORD dwFlag )
{
	if ( (m_dwStateFlag & dwFlag) != 0 )
	{
		return true;
	}

	return false;

}

// 获取状态标记值
DWORD CRoom::GetFlagValue(void)
{
	return m_dwStateFlag;
}



// 设置密码
bool CRoom::SetPassword( const char * szPassword )
{
	if( NULL == szPassword )
	{
		return false;
	}

	sstrcpyn( m_szPassword, szPassword, sizeof(m_szPassword) );

	OnDataChanged();
	SaveData();

	return true;
}

// 校验密码
bool CRoom::CheckPassword( const char * szPassword )
{
	// 如果没有设置密码锁，直接返回TRUE
	if( !GetFlag(VoiceRoomState_Lock) )
	{
		return true;
	}

	// 为空，无法校验，返回false
	if( NULL == szPassword )
	{
		return false;
	}

	string strRoomPass = m_szPassword;
	string strUserPass = szPassword;

	return (strUserPass == strRoomPass);
}

// 获取总人数
DWORD CRoom::Count(void)
{
	return m_mapList.size();
}

// 添加一个用户
bool CRoom::Add( CClientUser & user )
{
	DWORD dwClientID = user.GetClientID();

	TMAP_UserList::iterator it = m_mapList.find(dwClientID);
	if( it != m_mapList.end() )
	{
		// 添加失败，已存在
#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CRoom::Add 用户已经存在了，" << user.ToString() );
#endif
		return false;
	}

	SRoomUserData userData;

	m_mapList.insert( TMAP_UserList::value_type(dwClientID, userData) ); 

	// 如果开启了统计峰值
	if(m_bStatsMaxMember)
	{
		DWORD dwCurMemberNum = m_mapList.size();
		m_dwMaxMemberNum = (dwCurMemberNum > m_dwMaxMemberNum) ? dwCurMemberNum : m_dwMaxMemberNum;
	}

	// 设置用户当前房间信息
	user.SetCurRoom(*this);

    // 更新这个玩家信息给房间里的玩家
    user.UpdateUserInfoToClient(true, -1);

	// 有变化才重新计算列表，提高效率
	m_bChanged = true;

	// 通知频道，用户数变化
	m_pChannel->OnUsersChange( *this, user, true );
    
    // 更新频道信息给客户端
    GetChannel().UpdateChannelInfoToClient(&dwClientID, 1, VoiceUpdateChannelType_All);

    // 更新房间信息给客户端
    UpdateRoomInfoToClient(&dwClientID, 1, VoiceUpdateRoomType_All);

    if(GetChannel().GetFlag(VoiceChannelState_NoUserList))
    {
        // 如果不支持用户列表 更新麦序上的用户列表到客户端
        updateUserList2Client<TMAP_MicList>(dwClientID, &m_mapMic);
    }
    else
    {
        // 更新房间用户列表给客户端
        updateUserList2Client<TMAP_UserList>(dwClientID, &m_mapList);
    }

    // 粉丝关系
    UpdateFansInfoToClient(user);

    // 其他房间的基本信息
	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	{
		// 如果不是当前进入的房间，更新基本信息
		if( m_dwID != i )
		{
			m_pChannel->GetRoom(i).UpdateRoomInfoToClient( &dwClientID, 1, VoiceUpdateRoomType_Base );
		}
	}

	return true;
}

// 删除一个用户
bool CRoom::Del( CClientUser & user )
{
	// 通知逻辑层
	gGlobalServer->OnUserLeaveRoom(this, &user);

	DWORD dwClientID = user.GetClientID();

	TMAP_UserList::iterator it = m_mapList.find(dwClientID);
	if( it == m_mapList.end() )
	{
		// 找不到，删除失败
#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CRoom::Del 用户不在房间内，无法移除，" << user.ToString() );
#endif
		return false;
	}

	m_mapList.erase(it);

	// 如果是麦序模式
	if( m_dwMicState == VoiceMicState_MicOrder )
	{
		DWORD dwError = 0;
		DelMicOrder( user, dwError );
	}
	else
	{
		// 退出房间，要关麦
		user.SetFlag( VoiceUserStateFlag_Mic, false );
	}
	
	// 有变化才重新计算列表，提高效率
	m_bChanged = true;

	// 通知频道，用户数变化，当无用户时，频道自动销毁
	m_pChannel->OnUsersChange( *this, user, false );

    // 房间广播玩家离开
    user.UpdateUserInfoToClient(true, 0);

    // 从房间观察者移除
    GetChannel().DelObserver(dwClientID, m_dwID);

	// 清除用户当前房间信息
	user.ClearCurRoom();

	return true;
}

// 设置用户ID列表需重新计算
void CRoom::OnChanged(void)
{
	m_bChanged = true;
}

// 获取广播语音用户ID列表，内部有缓存机制，有变化才重新计算列表，提高效率
const DWORD * CRoom::GetAudioList( DWORD & dwCount )
{
#ifndef VOICE_CLIENTID_BUFFLIST
	dwCount = 0;
	return NULL;
#endif

	// 需要重新计算
	if( m_bChanged )
	{
		m_bChanged = false;

		// 分配AudioList内存，返回true表示重新分配，
		GetAudioListBuff();

		m_audioList.dwCount = 0;
		for(TMAP_UserList::iterator it = m_mapList.begin(); it != m_mapList.end(); ++it)
		{
			m_audioList.pData[m_audioList.dwCount] = it->first;
			m_audioList.dwCount ++;
		}
	}

	// 如果不需要计算，直接返回原来的列表
	dwCount = m_audioList.dwCount;

	return m_audioList.pData;
}

// 获取房间里所有用户ID列表 bNoMute为true时，取非静音用户
DWORD CRoom::GetUserIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, bool bNoMute )
{
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;
	for(TMAP_UserList::iterator it = m_mapList.begin(); it != m_mapList.end(); ++it)
	{
		if (dwFindCounts >= dwIDArrayCount )
		{
			break;
		}

		// 如果是取非静音用户，且用户是静音了，跳过
		if( bNoMute && it->second.bMute )
		{
			continue;
		}

		(*pClientID) = it->first;

		pClientID ++;
		dwFindCounts++;
	}

	dwGetCounts = dwFindCounts;

	return dwFindCounts;
}

// 获取房间里麦上人的粉丝ID列表
DWORD CRoom::GetFansIDList( DWORD dwUDBID, DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts )
{
	if( dwUDBID == 0 || pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;
	CClientList & clientList = CClientList::getInstance();
	for(TMAP_UserList::iterator it = m_mapList.begin(); it != m_mapList.end(); ++it)
	{
		if (dwFindCounts >= dwIDArrayCount )
		{
			break;
		}

		DWORD dwClientID = it->first;
		CClientUser * pUser = clientList.GetUser(dwClientID);
		if( NULL == pUser )
		{
			continue;
		}

		// 如果不是他的粉丝
		if( !pUser->IsFansUser(dwUDBID) )
		{
			continue;
		}

		(*pClientID) = dwClientID;

		pClientID ++;
		dwFindCounts++;
	}

	dwGetCounts = dwFindCounts;

	return dwFindCounts;
}


// 设置麦控类型
void CRoom::SetMicState( DWORD dwState )
{
	if( m_dwMicState == dwState )
	{
		return ;
	}

	// 保存旧的麦类型
	DWORD dwOldMicState = m_dwMicState;

	// 进入之前要退出上一个模式
	switch(dwOldMicState)
	{
	case VoiceMicState_MicMaster:		/// 主席模式
	case VoiceMicState_MicFree:				/// 自由麦
		{
			// 清除所有开麦说话用户列表
			ClearSpeak();

			// 关闭所有人的麦
			CClientList & clientList = CClientList::getInstance();
			for(TMAP_UserList::iterator it = m_mapList.begin(); it != m_mapList.end(); ++it)
			{
				CClientUser * pUser = clientList.GetUser(it->first);
				if( pUser != NULL )
				{
					pUser->SetFlag( VoiceUserStateFlag_Mic, false );
				}
			}
		}
		break;
	case VoiceMicState_MicOrder:			/// 麦序
		{
			// 清空麦序
			ClearAllMicOrder();
		}
		break;
	}

	m_dwMicState = dwState;

	// 进入新的模式要初始化
	switch(m_dwMicState)
	{
	case VoiceMicState_MicMaster:		/// 主席模式
		{
			SetFlag( VoiceRoomState_KeySpeak, false, false, false );
		}
		break;
	case VoiceMicState_MicFree:				/// 自由麦
		{
			// 清除所有开麦说话用户列表
			ClearSpeak();
			// 强设按键发言
			SetFlag( VoiceRoomState_KeySpeak, false, false, false );

			char szMsg[512] = {0};
			sprintf_s( szMsg, sizeof(szMsg), "当前为自由模式，所有人都可发言，但同时只能%d人发言！", VOICE_MAX_ROOMSPEAK_COUNT );
			GetChannel().SendChatMsg( szMsg, m_dwID );
		}
		break;
	case VoiceMicState_MicOrder:			/// 麦序
		{
			// 清空麦序
			ClearAllMicOrder();
			SetFlag( VoiceRoomState_KeySpeak, false, false, false );
			SetFlag( VoiceRoomState_AddMicOrder, true, false, false );
			SetFlag( VoiceRoomState_HoldMic, false, false, false );
		}
		break;
	}

	OnDataChanged();
	SaveData();

	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(Count());

	// 取用户ID列表
	DWORD dwCount = 0;
	GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
	if( dwCount > 0 )
	{
		// 同步房间信息
		UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_Base );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;
}

// 获取麦控类型
DWORD CRoom::GetMicState(void)
{
	return m_dwMicState;
}


// 加入开麦说话用户到列表,如果超了最大数就失败
bool CRoom::AddSpeak(CClientUser & user)
{
	// 客户端ID
	DWORD dwClientID = user.GetClientID();
	// 查找节点
	TMAP_SpeakList::iterator iter = m_mapSpeak.find(dwClientID);
	if( iter != m_mapSpeak.end())
	{
		// 已经在表里就不处理了
		return true;
	}

	// 已开麦说话的用户数
	DWORD dwSpeakCouts = m_mapSpeak.size();

	DWORD dwNowTicks = ::GetTickCount();	// 当前Ticks

	// 人数超了时,清理一些没说话且没开麦的用户,用来容错处理,以防有人不说了,但还有记录在,5秒冷却
	if (dwSpeakCouts>=VOICE_MAX_ROOMSPEAK_COUNT && dwNowTicks>(m_dwLastClearSpeakListTick+VOICE_MAX_ROOMSPEAK_DELAYS) )
	{
		// 最后清理过时已开麦说话的用户列表时间
		m_dwLastClearSpeakListTick = dwNowTicks;

		CClientList & clientList = CClientList::getInstance();

		// 遍历所有,清理一些没说话且没开麦的用户
		for(TMAP_SpeakList::iterator it = m_mapSpeak.begin(); it != m_mapSpeak.end(); )
		{
			bool bRemove = false;	// 是否删除
			// 找到用户
			CClientUser * pClientUser = clientList.GetUser(it->first);
			if (NULL!=pClientUser)
			{
				// 如果没开麦,并超时,要删除
				if (pClientUser->GetFlag(VoiceUserStateFlag_Mic)==false && dwNowTicks>(pClientUser->m_dwLastAudioTime+VOICE_MAX_ROOMSPEAK_DELAYS))
				{
					bRemove = true;
				}
			}
			else
			{
				// 人都找不到了,删除
				bRemove = true;
			}
			// 删除处理
			if (bRemove)
			{
				m_mapSpeak.erase(it++);
			}
			else
			{
				it++;
			}
		}

		// 最新个数
		dwSpeakCouts = m_mapSpeak.size();
	}

	// 超过了就不能加入
	if (dwSpeakCouts>=VOICE_MAX_ROOMSPEAK_COUNT)
	{
		return false;
	}

	SVoiceSpeakInfo addNode;		// 已开麦说话的用户信息
	addNode.dwClientID		= dwClientID;				// 客户端ID
	addNode.dwTickTime		= dwNowTicks;				// 麦说话Tick时间

	m_mapSpeak.insert(TMAP_SpeakList::value_type(dwClientID, addNode));	// 插入数据

	return true;
}


// 删除开麦说话用户到列表
bool CRoom::DelSpeak(CClientUser & user)
{
	// 客户端ID
	DWORD dwClientID = user.GetClientID();
	// 查找节点
	TMAP_SpeakList::iterator iter = m_mapSpeak.find(dwClientID);
	if( iter != m_mapSpeak.end())
	{
		m_mapSpeak.erase(iter);

		return true;
	}
	return false;
}

// 清除所有开麦说话用户列表
bool CRoom::ClearSpeak()
{
	m_mapSpeak.clear();
	return true;
}





// 上麦
bool CRoom::AddMicOrder( CClientUser & user, DWORD dwReason )
{
	DWORD dwClientID = user.GetClientID();
	
	// 如果不在房间，直接返回
	if( user.GetCurRoomID() != m_nID )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CRoom::AddMicOrder 抢麦失败，用户房间=" << user.GetCurRoomID() << ", 当前房间=" << m_nID);
#endif
		return false;
	}

	// 如果还在麦序上，返回
	TMAP_MicList::iterator it = m_mapMic.find(user.GetMicIndex());
	if( it != m_mapMic.end() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CRoom::AddMicOrder 抢麦失败，还在麦序上 dwClientID=" << dwClientID << "，麦序=" << it->second.dwIndex );
#endif
		return false;
	}

	m_MicInfo.dwMaxMicIndex ++;

	// 添加麦序
	SMicOrderData orderData;
	orderData.dwClientID = dwClientID;				// 客户端ID
	orderData.dwIndex = m_MicInfo.dwMaxMicIndex;			// 序号
	pair<TMAP_MicList::iterator,bool> retPair = m_mapMic.insert( TMAP_MicList::value_type(m_MicInfo.dwMaxMicIndex, orderData) );
	if( !retPair.second )
	{
		return false;
	}

	// 修改玩家麦序
	user.SetMicIndex(m_MicInfo.dwMaxMicIndex);

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("抢麦成功，客户端ID=" << dwClientID << ", 拥有麦序=" << m_MicInfo.dwMaxMicIndex);
#endif

	// 通知更新
	OnMicOrderChange( user );

	// 如果是第一个人，要给麦
	SetMicUser();

	return true;
}


// 下麦 dwError: 0 无错 1 不在房间里 2 不在麦序上
bool CRoom::DelMicOrder( CClientUser & user, DWORD & dwError, DWORD dwReason )
{
	dwError = 0;
	DWORD dwClientID = user.GetClientID();
	
	// 如果不在房间，直接返回
	if( user.GetCurRoomID() != m_nID )
	{
		dwError = 1;
		return false;
	}

	// 如果不在麦序上，直接返回，或者不是这个人，直接返回
	if( !IsOnMicOrder( user.GetMicIndex(), dwClientID ) )
	{
		dwError = 2;
		return false;
	}

	// 如果这个人在麦上说话，要移除麦
	if( dwClientID == m_MicInfo.dwCurMicUser )
	{
		// 杀掉定时器
		gGlobalServer->KillTimer( ETimerEventID_MicOrder, this );

		// 下掉当前麦上人
		RemoveMicUser(true);
	}
	else	// 不在麦上，如果有连麦，要把连麦标记去掉，并且关麦
	{
		if( user.GetFlag(VoiceUserStateFlag_ShareMic) )
		{
			user.SetFlag( VoiceUserStateFlag_ShareMic|VoiceUserStateFlag_Mic, false, true );
		}
	}

	// 移除结点
	DelFromMicList(user.GetMicIndex());

	// 如果是最后一个了，把总麦序清零
	if( m_mapMic.empty() )
	{
		m_MicInfo.dwMaxMicIndex = 0;
	}
	
#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("CRoom::DelMicOrder 下麦成功，客户端ID=" << dwClientID << ", 当前最大麦序=" << m_MicInfo.dwMaxMicIndex);
#endif

	// 通知更新
	OnMicOrderChange( user, false );

	// 清空麦序
	user.SetMicIndex(0);

	// 把麦给麦序的下一个人
	SetMicUser();

	return true;
}


// 当前麦序个数
DWORD CRoom::GetMicOrderCount(void)
{
	return m_mapMic.size();
}


// 把一个用户添加到麦序，bAdd为true表示添加
bool CRoom::AddClientToMicOrder( DWORD dwPDBID, bool bAdd )
{
	// 用户不存在，返回
	CClientUser * pUser = CClientList::getInstance().GetClientByPDBID(dwPDBID);
	if( NULL == pUser )
	{
		return false;
	}

	// 添加到麦序
	if( bAdd )
	{
		return AddMicOrder(*pUser);
	}
	else	// 删除麦序
	{
		DWORD dwError = 0;
		return DelMicOrder( *pUser, dwError );
	}
}



// 麦序变化
void CRoom::OnMicOrderChange( CClientUser & user, bool bAdd )
{
	// 取得房间观察者数量
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(m_pChannel->GetObserversCount(m_dwID));

	// 得到观察者列表
	DWORD dwCount = 0;
	m_pChannel->GetObserverIDList( pClientIDArray, dwBuffSize, dwCount, m_dwID );

	if( dwCount > 0 )
	{
		// 如果是添加
		if( bAdd )
		{
			// 如果房间不需要同步成员列表，则更新麦序的时候要同步成员列表
			if( m_pChannel->GetFlag(VoiceChannelState_NoUserList) )
			{
				DWORD dwClientID = user.GetClientID();
			}
		}
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;	
}


// 把当前麦上的人下掉，返回true，表示成功移除麦上的人
bool CRoom::RemoveMicUser( bool bSyncMicUser )
{
	bool bResult = false;

	CClientList & clientList = CClientList::getInstance();
	CClientUser * pUser = clientList.GetUser(m_MicInfo.dwCurMicUser);
	if( NULL != pUser )
	{
		// 关掉此人麦克风
		pUser->SetFlag( VoiceUserStateFlag_Mic, false );
		bResult = true;
	}

	m_MicInfo.dwCurMicUser = 0;			// 麦上的人
	m_MicInfo.dwMicStartTime = 0;		// 开始时间
	m_MicInfo.dwMicOrderCount = 0;		// 有几次麦序，麦序总时间为dwMicOrderCount*dwDefaultMicOrderTime
	m_MicInfo.dwMicTimeLeft = 0;		// 控麦下，当前麦序上当前计时周期里还剩多少秒
	m_MicInfo.bRestartTimer = false;	// 要重新启动计时器

	// 把所有连麦的人都清掉
	for( TMAP_MicList::iterator it2 = m_mapMic.begin(); it2 != m_mapMic.end(); ++it2  )
	{
		CClientUser * pOrderUser = clientList.GetUser(it2->second.dwClientID);
		if( pOrderUser != NULL )
		{
			// 如果有连麦，去掉标记，关闭麦克风
			if( pOrderUser->GetFlag(VoiceUserStateFlag_ShareMic) )
			{
				pOrderUser->SetFlag( VoiceUserStateFlag_ShareMic|VoiceUserStateFlag_Mic, false, true );
			}
		}
	}

	if( bSyncMicUser )
	{
		// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
		__VOICE_GET_ID_BUFFER_START(Count());

		DWORD dwCount = 0;
		GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
		if( dwCount > 0 )
		{
			UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_MicUser );
		}

		// 是new的内存,用完释放
		// ID缓存分配结束
		__VOICE_GET_ID_BUFFER_END;
	}

	// 杀掉检测定时器
	gGlobalServer->KillTimer( ETimerEventID_MicCheck, this );

	return bResult;
}

// 把麦给麦序上第一个人
void CRoom::SetMicUser(void)
{
	if( m_MicInfo.dwCurMicUser > 0 )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CRoom::SetMicUser 当前麦上已经有人了呢，dwCurMicUser=" << m_MicInfo.dwCurMicUser);
#endif
		return;
	}

	if( m_mapMic.size() > 0 )
	{
		// 如果麦序时间为0，则给默认时间
		m_MicInfo.dwCurMicOrderTime = m_MicInfo.dwMicOrderTime;
		if( 0 == m_MicInfo.dwCurMicOrderTime )
		{
			m_MicInfo.dwCurMicOrderTime = VOICE_MIC_TIME_LEN;
		}

		// 取出麦序头
		TMAP_MicList::iterator it = m_mapMic.begin();

		m_MicInfo.dwCurMicUser = it->second.dwClientID;		// 记录说话的人
		m_MicInfo.dwMicStartTime = (DWORD)time(NULL);		// 开始时间
		m_MicInfo.dwMicOrderCount = 1;						// 有几次麦序，麦序总时间为dwMicOrderCount*dwDefaultMicOrderTime
		m_MicInfo.dwMicTimeLeft = 0;						// 控麦下，当前麦序上当前计时周期里还剩多少秒
		m_MicInfo.bRestartTimer = false;					// 要重新启动计时器

		// 如果没有控麦，才启动定时器
		if( !GetFlag(VoiceRoomState_HoldMic) )
		{
			gGlobalServer->SetTimer( ETimerEventID_MicOrder, m_MicInfo.dwCurMicOrderTime*1000, this, "CRoom::SetMicUser" );

			if( m_MicInfo.dwMicInvalidTime <= 0 )
			{
				m_MicInfo.dwMicInvalidTime = 30;
			}
			gGlobalServer->SetTimer( ETimerEventID_MicCheck, m_MicInfo.dwMicInvalidTime*1000, this, "CRoom::SetMicUser", 1 );
		}

#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CRoom::SetMicUser 轮到我说话了，dwCurMicUser=" << m_MicInfo.dwCurMicUser << "，麦序=" << it->second.dwIndex );
#endif

	}

	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(Count());

	DWORD dwCount = 0;
	GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
	if( dwCount > 0 )
	{
        CClientUser* pCurMicUser = CClientList::getInstance().FindUser(m_MicInfo.dwCurMicUser);
        if(pCurMicUser != NULL)
        {
            pCurMicUser->UpdateUserInfoToClient(true, -1);
        }
		UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_MicUser );
	}

	// 查找麦上人所有的粉丝列表，并更新关系到客户端
	CClientUser * pMicUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
	if( pMicUser != NULL )
	{
		GetFansIDList( pMicUser->GetUserInfo().dwUDBID, pClientIDArray, dwBuffSize, dwCount );
		if( dwCount > 0 )
		{
			gGlobalServer->BroadcastVoiceDataToClient( pClientIDArray, dwCount, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_MicFans, m_dwID, 1 );
		}
	}
	
	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;
}

// 是否是麦上的人
bool CRoom::IsMicUser( DWORD dwClientID )
{
	return (dwClientID == m_MicInfo.dwCurMicUser);
}


// 清空麦序
bool CRoom::ClearAllMicOrder()
{
	// 麦序没人
	if( m_mapMic.empty() )
	{
		return false;
	}

#ifdef  VOICE_PRINT_DEBUG_INF
	TraceLn("CRoom::ClearAllMicOrder 清空麦序");
#endif

	// 杀掉定时器
	gGlobalServer->KillTimer( ETimerEventID_MicOrder, this );

	// 移除麦上的人
	RemoveMicUser(true);

	// 把麦上的人麦序置0
	CClientList & clientList = CClientList::getInstance();
	for( TMAP_MicList::iterator it = m_mapMic.begin(); it != m_mapMic.end(); ++it  )
	{
		CClientUser *pUser = clientList.GetUser(it->second.dwClientID);
		if( pUser != NULL )
		{
			pUser->SetMicIndex(0);
		}
	}

	// 清空麦序
	m_mapMic.clear();

	// 清空最大麦序
	m_MicInfo.dwMaxMicIndex = 0;

	return true;
}


// 增加麦上说话时间
bool CRoom::AddMicOrderTime( bool bAdd, DWORD dwTimeCount )
{
	// 如果麦上没人，不需要调整
	if( m_MicInfo.dwCurMicUser <= 0 )
	{
		return true;
	}

	if( bAdd )
	{
		DWORD dwTotalTimeCount = 0;

		// 如果超过1小时了，默认增加一个小时
		if( dwTimeCount * m_MicInfo.dwCurMicOrderTime >= 3600 )
		{
			dwTotalTimeCount = m_MicInfo.dwMicOrderCount + 3600/m_MicInfo.dwCurMicOrderTime;
		}
		else
		{
			dwTotalTimeCount = m_MicInfo.dwMicOrderCount + dwTimeCount;
		}

		if( dwTotalTimeCount * m_MicInfo.dwCurMicOrderTime > VOICE_MAX_MIC_TOTAL_TILE )
		{
			return false;
		}

		m_MicInfo.dwMicOrderCount = dwTotalTimeCount;
	}
	else
	{

		if( m_MicInfo.dwMicOrderCount > dwTimeCount )
		{
			m_MicInfo.dwMicOrderCount -= dwTimeCount;
		}
		else
		{
			m_MicInfo.dwMicOrderCount = 1;
		}
	}

	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(Count());

	DWORD dwCount = 0;
	GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
	if( dwCount > 0 )
	{
		//UpdateMicUserInfo( pClientIDArray, dwCount );
		UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_MicUser );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;

	return true;
}


// 设置麦序时间
DWORD CRoom::SetDefaultMicOrderTime( DWORD dwTime )
{
	if( dwTime == m_MicInfo.dwMicOrderTime )
	{
		return dwTime;
	}

	if( dwTime <= 30 )
	{
		m_MicInfo.dwMicOrderTime = 30;
	} 
	else
	{
		m_MicInfo.dwMicOrderTime = dwTime;
	}

	OnDataChanged();
	SaveData();

	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(Count());

	// 取用户ID列表
	DWORD dwCount = 0;
	GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
	if( dwCount > 0 )
	{
		// 同步房间信息
		UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_Base );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;

	return m_MicInfo.dwMicOrderTime;
}

// 移动麦序，dwMoveType为EMVoiceMoveMicOrderType
bool CRoom::MoveMicOrder( DWORD dwClientID, DWORD dwMoveType )
{
	CClientUser *pUser = CClientList::getInstance().GetUser(dwClientID);
	if( pUser == NULL )
	{
		return false;
	}

	// 不在房间，返回
	if( pUser->GetCurRoomID() != m_nID )
	{
		return false;
	}

	// 如果人数小于2，返回
	if( m_mapMic.size() < 2 )
	{
		return false;
	}

	// 如果不在麦序上，返回
	TMAP_MicList::iterator it = m_mapMic.find(pUser->GetMicIndex());
	if( it == m_mapMic.end() || it == m_mapMic.begin() || it->second.dwClientID != dwClientID )
	{
		return false;
	}

	switch(dwMoveType)
	{
	case VoiceMoveMicOrder_Up:		// 上移麦序
		{
			TMAP_MicList::iterator itFront = it;
			itFront--;	// 前面的麦序

			// 已经是2号位了，返回
			if( it == (++ m_mapMic.begin()) )
			{
				return false;
			}

			DWORD dwFrontClientID = itFront->second.dwClientID;
			CClientUser * pFrontUser = CClientList::getInstance().GetUser(dwFrontClientID);
			if( NULL == pFrontUser )
			{
				ErrorLn("CRoom::MoveMicOrder 上移麦序 前面的人不存在？客户端ID=" << dwFrontClientID );
				return false;
			}
			
			// 处理上移的麦序
			itFront->second.dwClientID = dwClientID;
			pUser->SetMicIndex(itFront->second.dwIndex);

			// 处理下移的麦序
			it->second.dwClientID = dwFrontClientID;
			pFrontUser->SetMicIndex(it->second.dwIndex);
		}
		break;

	case VoiceMoveMicOrder_Down:		// 下移麦序
		{
			TMAP_MicList::iterator itBehind = it;							
			itBehind ++;		// 下一个麦序

			// 已经是最后一个了，返回
			if( itBehind == m_mapMic.end() )
			{
				return false;
			}

			DWORD dwBehindClientID = itBehind->second.dwClientID;
			CClientUser * pBehindUser = CClientList::getInstance().GetUser(dwBehindClientID);
			if( NULL == pBehindUser )	// 后面没找到人
			{
				ErrorLn("CRoom::MoveMicOrder 下移麦序 后面的人不存在？客户端ID=" << dwBehindClientID );
				return false;
			}

			// 处理下移的麦序
			itBehind->second.dwClientID = dwClientID;
			pUser->SetMicIndex(itBehind->second.dwIndex);

			// 处理上移的麦序
			it->second.dwClientID = dwBehindClientID;
			pBehindUser->SetMicIndex(it->second.dwIndex);
		}
		break;

	case VoiceMoveMicOrder_Jump2:	// 跳到2号麦序
		{
			TMAP_MicList::iterator itSencond = ++ m_mapMic.begin();	// 2号麦序

			// 自己就是2号麦序，返回
			if( itSencond == it || itSencond == m_mapMic.end() )
			{
				return false;
			}

			DWORD dwSecondClientID = itSencond->second.dwClientID;
			CClientUser * pSecondUser = CClientList::getInstance().GetUser(dwSecondClientID);
			if( NULL == pSecondUser )
			{
				ErrorLn("CRoom::MoveMicOrder 上移麦序 2号麦序人不存在？客户端ID=" << dwSecondClientID );
				return false;
			}

			// 处理上移的麦序
			itSencond->second.dwClientID = dwClientID;
			pUser->SetMicIndex(itSencond->second.dwIndex);

			// 处理下移的麦序
			it->second.dwClientID = dwSecondClientID;
			pSecondUser->SetMicIndex(it->second.dwIndex);
		}
		break;

	default:
		{
			return false;
		}
	}

	return true;
}

// 控麦
void CRoom::HoldMic( bool bHold )
{
	// 控麦
	if( bHold )
	{
		SetFlag( VoiceRoomState_HoldMic, bHold, true );

		// 杀掉定时器
		gGlobalServer->KillTimer( ETimerEventID_MicOrder, this );
		gGlobalServer->KillTimer( ETimerEventID_MicCheck, this );

		// 如果当前麦上有人，要记录当前这个计时器还剩多少时间
		m_MicInfo.bRestartTimer = false;
		if( m_MicInfo.dwCurMicUser > 0 )
		{
			m_MicInfo.bRestartTimer = true;

			// 上次控麦时，周期有剩余时间
			if( m_MicInfo.dwMicTimeLeft > 0 )
			{
#ifdef VOICE_PRINT_DEBUG_INF
				ErrorLn( "CRoom::HoldMic 当前周期剩余时间=" << m_MicInfo.dwMicTimeLeft << "，说话时间=" << ((DWORD)time(NULL)-m_MicInfo.dwMicStartTime) );
#endif
				m_MicInfo.dwMicTimeLeft = m_MicInfo.dwMicTimeLeft - ((DWORD)time(NULL)-m_MicInfo.dwMicStartTime);

#ifdef VOICE_PRINT_DEBUG_INF
				ErrorLn( "CRoom::HoldMic 上次控麦有剩余时间，当前周期剩余时间=" << m_MicInfo.dwMicTimeLeft << "，周期数=" << m_MicInfo.dwMicOrderCount );
#endif
			}
			else // 没有控麦，或者周期没有剩余时间
			{
				m_MicInfo.dwMicTimeLeft = m_MicInfo.dwCurMicOrderTime - ((DWORD)time(NULL)-m_MicInfo.dwMicStartTime);

#ifdef VOICE_PRINT_DEBUG_INF
				ErrorLn( "CRoom::HoldMic 没有控麦，或者周期没有剩余时间，当前周期剩余时间=" << m_MicInfo.dwMicTimeLeft << "，周期数=" << m_MicInfo.dwMicOrderCount );
#endif
			}

			if( m_MicInfo.dwMicTimeLeft == 0 && m_MicInfo.dwMicOrderCount > 0 )		// 控麦的时候，周期刚好走完，要把次数减一
			{
				m_MicInfo.dwMicOrderCount --;
			}

			CClientUser* pMicUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
			if( NULL != pMicUser )
			{
				// 如果没有说话权限的，要关掉麦克风
				if( !(pMicUser->GetRight(VoiceRight_Speak)) )
				{
					// 先判断麦是否已经打开
					bool bMicOpen = pMicUser->GetFlag(VoiceUserStateFlag_Mic);

					// 如果麦打开了，要关闭麦
					if( bMicOpen )
					{
						pMicUser->SetFlag(VoiceUserStateFlag_Mic, false);
					}

					pMicUser->SetNeedOpenMic(bMicOpen);
				}
				else
				{
					pMicUser->SetNeedOpenMic(false);
				}
			}
		}
	}
	else  // 释放麦
	{
		// 如果控麦的时候有人，并且上次控麦的周期还有
		if( m_MicInfo.dwCurMicUser > 0 && m_MicInfo.dwMicOrderCount > 0 )
		{
			SetFlag( VoiceRoomState_HoldMic, bHold, false, true );

			m_MicInfo.dwMicStartTime = (DWORD)time(NULL);

			// 如果剩余时间大于0，则用剩余时间起定时器，否则用默认周期起定时器
			if( m_MicInfo.dwMicTimeLeft > 0 )
			{
				gGlobalServer->SetTimer( ETimerEventID_MicOrder, m_MicInfo.dwMicTimeLeft*1000, this, "CRoom::HoldMic" );
			}
			else
			{
				gGlobalServer->SetTimer( ETimerEventID_MicOrder, m_MicInfo.dwCurMicOrderTime*1000, this, "CRoom::HoldMic" );
			}

			if( m_MicInfo.dwMicInvalidTime <= 0 )
			{
				m_MicInfo.dwMicInvalidTime = 30;
			}
			gGlobalServer->SetTimer( ETimerEventID_MicCheck, m_MicInfo.dwMicInvalidTime*1000, this, "CRoom::HoldMic", 1 );

			// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
			__VOICE_GET_ID_BUFFER_START(Count());

			DWORD dwCount = 0;
			GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
			if( dwCount > 0 )
			{
				// 把麦上的用户信息发送给房间所有人发送麦信息
				//UpdateMicUserInfo( pClientIDArray, dwCount );
				UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_All );
			}

			// 是new的内存,用完释放
			// ID缓存分配结束
			__VOICE_GET_ID_BUFFER_END;

			CClientUser* pMicUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
			if( NULL != pMicUser )
			{
				// 如果要开麦，才把麦打开
				if( pMicUser->IsNeedOpenMic() )
				{
					pMicUser->SetFlag(VoiceUserStateFlag_Mic, true);	

					// 复原状态
					pMicUser->SetNeedOpenMic(false);
				}
			}
		}
		else
		{
			SetFlag( VoiceRoomState_HoldMic, bHold, true );

			// 把麦给下一个人
			SetMicUser();
		}
	}
}


// 是否在麦序上
bool CRoom::IsOnMicOrder( DWORD dwMicIndex, DWORD dwClientID )
{
	if( m_mapMic.empty() )
	{
		return false;
	}

	TMAP_MicList::iterator it = m_mapMic.find(dwMicIndex);
	if( it != m_mapMic.end() && it->second.dwClientID == dwClientID )
	{
		return true;
	}

	return false;
}

// 从麦序列表中删除一个麦序
bool CRoom::DelFromMicList( DWORD dwMicIndex )
{
	// 如果不在麦序上，直接返回，或者不是这个人，直接返回
	TMAP_MicList::iterator it = m_mapMic.find(dwMicIndex);
	if( it != m_mapMic.end() )
	{
		m_mapMic.erase(it);
		return true;
	}

	return false;
}

// 得到用户列表
CRoom::TMAP_UserList & CRoom::GetUserList(void)
{
	return m_mapList;
}

// 得到麦序列表
CRoom::TMAP_MicList & CRoom::GetMicList(void)
{
	return m_mapMic;
}

// 更新房间信息到客户端
void CRoom::UpdateRoomInfoToClient( DWORD * pDesClientArray, DWORD wDesClientNum, BYTE nUpdateType )
{
	obuf obufData;

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_ROOMINFO_UPDATE;
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	SMsgVoiceUpdateRoomHead_VC updateHeader;
	updateHeader.nUpdateType = nUpdateType;
	updateHeader.nRoomID = (BYTE)m_dwID;
	obufData.push_back( &updateHeader, sizeof(SMsgVoiceUpdateRoomHead_VC) );

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("CRoom::UpdateRoomInfoToClient 同步房间信息到客户端，nUpdateType=" << nUpdateType);
#endif

	switch(nUpdateType)
	{
	case VoiceUpdateRoomType_All:		// 全部
		{
			SMsgVoiceUpdateRoomBase_VC baseInfo;
			baseInfo.nMicState = (BYTE)m_dwMicState;
			baseInfo.dwStateFlag = m_dwStateFlag;
			baseInfo.dwUserCount = m_mapList.size();
			baseInfo.dwChatCoolDown = (WORD)m_dwChatCoolDown;
			baseInfo.dwMicOrderTime = (WORD)(m_MicInfo.dwMicOrderTime);
			obufData.push_back( &baseInfo, sizeof(SMsgVoiceUpdateRoomBase_VC) );

			// 公告信息
			SMsgVoiceUpdateRoomRemark_VC remark;
			remark.nRemarkLen = m_strRemark.size();
			obufData.push_back( &remark, sizeof(SMsgVoiceUpdateRoomRemark_VC) );

			// 如果是麦序模式，才发麦上用户信息
			if( m_dwMicState == VoiceMicState_MicOrder )
			{
				SMsgVoiceUpdateRoomMicUser_VC micUser;
				micUser.dwClientID = m_MicInfo.dwCurMicUser;			// 当前在麦上的人，ClientID
				
				// 如果当前周期时间有剩余，说明至少进入了一个周期
				if( m_MicInfo.dwMicTimeLeft > 0 )
				{
					micUser.dwMicEndTime = (m_MicInfo.dwMicStartTime + m_MicInfo.dwCurMicOrderTime * (m_MicInfo.dwMicOrderCount-1) + m_MicInfo.dwMicTimeLeft);			// 麦序结束时间
				}
				else
				{
					micUser.dwMicEndTime = (m_MicInfo.dwMicStartTime + m_MicInfo.dwCurMicOrderTime * m_MicInfo.dwMicOrderCount);			// 麦序结束时间
				}

				micUser.dwFansCount = 0;
				micUser.dwWorldID = 0;
				if( m_MicInfo.dwCurMicUser > 0 )
				{
					CClientUser * pMicUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
					if( pMicUser != NULL )
					{
						micUser.dwFansCount = pMicUser->GetUserInfo().dwFansCount;
						micUser.dwWorldID = (WORD)(pMicUser->GetWorldID());		// 所在游戏世界ID，可能与自身游戏世界ID不一样
					}
				}

				obufData.push_back( &micUser, sizeof(SMsgVoiceUpdateRoomMicUser_VC) );

#ifdef VOICE_PRINT_DEBUG_INF
				ErrorLn("CRoom::UpdateRoomInfoToClient 更新全部，当前时间=" << m_MicInfo.dwMicStartTime 
																 << ", 结束时间=" << micUser.dwMicEndTime 
																 << "，说话时间为=" << (micUser.dwMicEndTime - m_MicInfo.dwMicStartTime));
#endif
			}
			
			obufData.push_back( m_strRemark.c_str(), remark.nRemarkLen );
		}
		break;

	case VoiceUpdateRoomType_Base:		// 基础信息
		{
			SMsgVoiceUpdateRoomBase_VC baseInfo;
			baseInfo.nMicState = (BYTE)m_dwMicState;
			baseInfo.dwStateFlag = m_dwStateFlag;
			baseInfo.dwUserCount = m_mapList.size();
			baseInfo.dwChatCoolDown = (WORD)m_dwChatCoolDown;
			baseInfo.dwMicOrderTime = (WORD)(m_MicInfo.dwMicOrderTime);
			obufData.push_back( &baseInfo, sizeof(SMsgVoiceUpdateRoomBase_VC) );
		}
		break;

	case VoiceUpdateRoomType_MicUser:		// 麦上的人信息
		{
			if( m_dwMicState != VoiceMicState_MicOrder )
			{
				return ;
			}

			SMsgVoiceUpdateRoomMicUser_VC micUser;
			micUser.dwClientID = m_MicInfo.dwCurMicUser;			// 当前在麦上的人，ClientID

			// 如果当前周期时间有剩余，说明至少进入了一个周期
			if( m_MicInfo.dwMicTimeLeft > 0 )
			{
				micUser.dwMicEndTime = (m_MicInfo.dwMicStartTime + m_MicInfo.dwCurMicOrderTime * (m_MicInfo.dwMicOrderCount-1) + m_MicInfo.dwMicTimeLeft);			// 麦序结束时间
			}
			else
			{
				micUser.dwMicEndTime = (m_MicInfo.dwMicStartTime + m_MicInfo.dwCurMicOrderTime * m_MicInfo.dwMicOrderCount);			// 麦序结束时间
			}

			micUser.dwFansCount = 0;
			micUser.dwWorldID = 0;
			if( m_MicInfo.dwCurMicUser > 0 )
			{
				CClientUser * pMicUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
				if( pMicUser != NULL )
				{
					micUser.dwFansCount = pMicUser->GetUserInfo().dwFansCount;
					micUser.dwWorldID = (WORD)(pMicUser->GetWorldID());		// 所在游戏世界ID，可能与自身游戏世界ID不一样
				}
			}

			obufData.push_back( &micUser, sizeof(SMsgVoiceUpdateRoomMicUser_VC) );

#ifdef VOICE_PRINT_DEBUG_INF
			ErrorLn("CRoom::UpdateRoomInfoToClient 更新麦上的人信息，当前时间=" << m_MicInfo.dwMicStartTime 
				<< ", 结束时间=" << micUser.dwMicEndTime 
				<< "，说话时间为=" << (micUser.dwMicEndTime - m_MicInfo.dwMicStartTime));
#endif
		}
		break;

	case VoiceUpdateRoomType_Remark:		// 公告信息
		{
			// 公告信息
			SMsgVoiceUpdateRoomRemark_VC remark;
			remark.nRemarkLen = m_strRemark.size();
			obufData.push_back( &remark, sizeof(SMsgVoiceUpdateRoomRemark_VC) );
			obufData.push_back( m_strRemark.c_str(), remark.nRemarkLen );
		}
		break;
	}

	// 发送
	if( 1 == wDesClientNum )
	{
		gGlobalServer->SendDataToClient( pDesClientArray[0], obufData.data(), obufData.size() );
	}
	else
	{
		gGlobalServer->BroadcastDataToClient( pDesClientArray, wDesClientNum, obufData.data(), obufData.size() );
	}
}

// 更新粉丝信息到客户端
void CRoom::UpdateFansInfoToClient( CClientUser & user )
{
	if( m_MicInfo.dwCurMicUser > 0 )
	{
		CClientUser * pUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
		if( pUser != NULL )
		{
			// 如果玩家是麦上人的粉丝
			if( user.IsFansUser(pUser->GetUserInfo().dwUDBID) )
			{
				gGlobalServer->SendVoiceDataToClient( user.GetClientID(), MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_MicFans, m_dwID, 1 );
			}
			else
			{
				gGlobalServer->SendVoiceDataToClient( user.GetClientID(), MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_MicFans, m_dwID, 0 );
			}
		}
	}
}



// 设置备注信息
void CRoom::SetRemark( const char * szRemark )
{
	if( szRemark == NULL )
	{
		return;
	}

	// 如果公告与原来一样不修改
	if( strcmp(szRemark, m_strRemark.c_str()) == 0 )
	{
		return;
	}

	m_strRemark = szRemark;

	OnDataChanged();
	SaveData();

	// 取得本房间观察者数量
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(m_pChannel->GetObserversCount(m_dwID));

	DWORD dwCount = 0;
	m_pChannel->GetObserverIDList( pClientIDArray, dwBuffSize, dwCount, m_dwID );
	if( dwCount > 0 )
	{
		UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_Remark );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;
}

// 获取备注信息
const char * CRoom::GetRemark(void)
{
	return m_strRemark.c_str();
}

// 广播文字聊天
void CRoom::sendChatMessage( CClientUser & sender, ulong channel, const char* text, DWORD target, bool bSelf )
{
	if( text == NULL )
	{
		return ;
	}

	obuf1024 obufData;

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_TEXTCHATMESSAGE;

	SVoiceUserInfo & userInfo = sender.GetUserInfo();

	SMsgVoiceTextChatMessage_VC chatMsg;
	chatMsg.nChannel	= (BYTE)channel;				// 文字频道
	chatMsg.dwWorldID	= userInfo.dwFromWorldID;			// 发送者世界ID
	chatMsg.nCountry	= (BYTE)(sender.GetChannelTypeID(VoiceChannel_Country));				// 国家ID
	chatMsg.dwPDBID		= userInfo.dwPDBID;				// 发送者PDBID
	chatMsg.dwTarget	= target;				// 接收者PDBID
	chatMsg.nNameLen	= strlen(userInfo.szName);				// 发送者名字长度
	chatMsg.nTextLen	= strlen(text);				// 文字聊天

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&chatMsg, sizeof(SMsgVoiceTextChatMessage_VC));

	if( chatMsg.nNameLen > 0 )
	{
		obufData.push_back( userInfo.szName, chatMsg.nNameLen );
	}

	if( chatMsg.nTextLen > 0 )
	{
		obufData.push_back( text, chatMsg.nTextLen );
	}

	// 如果是私聊
	if( target > 0 )
	{
		// 根据原来私聊的逻辑 私聊双方都要发送消息
		// 发回自己
		gGlobalServer->SendDataToClient( sender.GetClientID(), obufData.data(), obufData.size() );
		
		
		// 发给目标
		CClientUser * pUser = CClientList::getInstance().GetClientByPDBID(target);
		if( pUser != NULL )
		{
			gGlobalServer->SendDataToClient( pUser->GetClientID(), obufData.data(), obufData.size() );
		}
		else
		{
			sender.SendChatMsg("此用户未连入语音服务器！");
		}
	}
	else	// 不是私聊，要广播到房间所有人
	{
		if( bSelf )		// 只发给自己
		{
			gGlobalServer->SendDataToClient( sender.GetClientID(), obufData.data(), obufData.size() );
		}
		else
		{
			// 取得房间里所有用户ID
			// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
			__VOICE_GET_ID_BUFFER_START(Count());

			DWORD dwCount = 0;
			GetUserIDList( pClientIDArray, dwBuffSize, dwCount );

			if( dwCount > 0 )
			{
				gGlobalServer->BroadcastDataToClient( pClientIDArray, dwCount, obufData.data(), obufData.size() );
			}

			// 是new的内存,用完释放
			// ID缓存分配结束
			__VOICE_GET_ID_BUFFER_END;
		}
	}
}


// 更新用户静音状态
void CRoom::UpdateUserMuteState( DWORD dwClientID, bool bMute )
{
	TMAP_UserList::iterator it = m_mapList.find(dwClientID);
	if( it != m_mapList.end() )
	{
		it->second.bMute = bMute;
	}
}

// 连麦
bool CRoom::ShareMic( DWORD dwClientID, bool bShare )
{
	CClientUser *pUser = CClientList::getInstance().GetUser(dwClientID);
	if( pUser == NULL )
	{
		return false;
	}

	// 不在房间，返回
	if( pUser->GetCurRoomID() != m_nID )
	{
		return false;
	}

	// 如果不在麦序上，返回
	TMAP_MicList::iterator it = m_mapMic.find(pUser->GetMicIndex());
	if( it == m_mapMic.end() || it == m_mapMic.begin() || it->second.dwClientID != dwClientID )
	{
		return false;
	}

	if( bShare )
	{
		// 没有连麦过才设置连麦标记
		if( !pUser->GetFlag(VoiceUserStateFlag_ShareMic) )
		{
			pUser->SetFlag( VoiceUserStateFlag_ShareMic, true, true );
		}
	}
	else
	{
		// 有连麦过才取消设置连麦标记，取消标记的时候要关麦
		if( pUser->GetFlag(VoiceUserStateFlag_ShareMic) )
		{
			pUser->SetFlag( VoiceUserStateFlag_ShareMic|VoiceUserStateFlag_Mic, false, true );
		}
	}

	return true;
}


// 更新一下当前所有用户房间信息,主要是用来数据库加载数据后,需刷新一下相关状态和权限
bool CRoom::UpdateAllUserCurRoomInf(void)
{
	// 更新一下当前所有用户房间信息
	CClientList & clientList = CClientList::getInstance();
	for(TMAP_UserList::iterator it = m_mapList.begin(); it != m_mapList.end(); ++it)
	{
		CClientUser * pUser = clientList.GetUser(it->first);
		if( pUser != NULL )
		{
			SVoiceVestInfo * pVest = m_pChannel->GetVest(pUser->GetUserInfo().dwUDBID);
			if( pVest != NULL )
			{
				pUser->SetVestUserType( pVest->nNewUserType, true, true );
			}
		}
	}

	return true;
}

// 分配AudioList内存，返回true表示重新分配，
bool CRoom::GetAudioListBuff(void)
{
	DWORD dwUserCount = m_mapList.size();
	DWORD dwNewBuffSize = m_audioList.dwSize;

	bool bReSize = false;
	// 要扩容
	if( dwUserCount >= m_audioList.dwSize*0.95 )
	{
		dwNewBuffSize *= 2;
		bReSize = true;
	}
	// 缩小
	else if( VOICE_ROOM_MIN_BUFF_SIZE+dwUserCount < m_audioList.dwSize*0.3 )
	{
		dwNewBuffSize /= 2; 
		bReSize = true;
	}

	if (!bReSize)
	{
		return false;
	}

	DWORD * pNewBuf = new DWORD[dwNewBuffSize];

	// 拷贝
	//memcpy(pNewBuf,m_audioList.pData,m_audioList.dwCount*sizeof(DWORD));

	delete []m_audioList.pData;

	// 存新的
	m_audioList.pData = pNewBuf;
	m_audioList.dwSize = dwNewBuffSize;

	return true;
}


// 收到数据库数据
void CRoom::ImportDBInfo(DB_Table_VoiceRoom &dbInfo)
{
	m_bLoaded = true;
	m_bSaved = true;

	m_dwSpare1	= dbInfo.dwSpare1;			// 备用1
	m_dwSpare2	= dbInfo.dwSpare2;			// 备用2
	m_dwSpare3	= dbInfo.dwSpare3;			// 备用3
	sstrcpyn( m_szName, dbInfo.szName, sizeof(m_szName) );	// 房间名称
	
	m_dwStateFlag = dbInfo.dwFlag;			// 状态标识
	m_MicInfo.dwMicOrderTime = dbInfo.dwMicTimes;			// 默认麦序时间
	sstrcpyn( m_szPassword, dbInfo.szPassword, sizeof(m_szPassword) );		// 密码
	m_strRemark = dbInfo.szTitle;			// 房间公告
	m_dwMicState = dbInfo.dwMicState;
}


// 是否支持数据库存储,团队不需要存储数据
bool CRoom::IsSupportDB()
{
	if (VoiceChannel_Group==m_dwType||VoiceChannel_Team==m_dwType)
	{
		return false;
	}
	return true;
}

// 数据是否加载完成
bool CRoom::IsLoaded(void)
{
	return m_bLoaded;
}

// 设置是否加载完成
void CRoom::SetLoaded( bool bLoaded )
{
	m_bLoaded = bLoaded;
}


// 数据变更了,定时或调用SaveData() 时会触发保存
void CRoom::OnDataChanged()
{
	if( !IsSupportDB() )
	{
		return;
	}
	// 是否已保存数据
	m_bSaved = false;
}

// 冷却请求保存数据
bool CRoom::CanSaveData()
{
	// 取得当前Tick
	DWORD dwTickCount=::GetTickCount();

	// 是否是定时调用,是就不作为新请求，并把有排队的请求放行
	if (m_saveCoolInfo.bOnTimer)
	{
		m_saveCoolInfo.bOnTimer = false;
		// 有排队的请求
		if (m_saveCoolInfo.nCounts>0)
		{
			// 放行排队的请求
			m_saveCoolInfo.nCounts		= 0;
			m_saveCoolInfo.dwLastTimes	= dwTickCount;
			return true;
		}
		else
		{
#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn( "CRoom::CanSaveData m_saveCoolInfo.bOnTimer m_saveCoolInfo.nCounts <= 0 无排队的请求" );
#endif
			return false;
		}
	}

	// 请求次数加1
	m_saveCoolInfo.nCounts++;
#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn( "CRoom::CanSaveData m_saveCoolInfo.nCounts=" << m_saveCoolInfo.nCounts );
#endif

	// 能否请求
	if ( (dwTickCount - m_saveCoolInfo.dwLastTimes< VOICE_COOL_SAVE_TIMES ) && (m_saveCoolInfo.nCounts< VOICE_COOL_SAVE_COUNTS ) )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn( "CRoom::CanSaveData 时间冷却中" );
#endif
		return false;
	}
	// 新请求
	m_saveCoolInfo.nCounts		= 0;
	m_saveCoolInfo.dwLastTimes	= dwTickCount;

	return true;
}


// 请求保存数据,有缓冲机制,数据变化都调用 OnDataChanged()
bool CRoom::SaveData(void)
{
	// 不用保存
	if( !IsSupportDB() )
	{
		return false;
	}
	// 检查能否保存数据
	if( ! CanSaveData() )
	{
		return false;
	}

	// 社会服务器保存数据到数据库
	SaveDataToDB();

	return true;
}

// 定时检查是否要保存数据
void CRoom::OnTimerCheckSaveDB(void)
{
	m_saveCoolInfo.bOnTimer = true;
	SaveData();
	m_saveCoolInfo.bOnTimer = false;
}

// 数据是否已经保存
bool CRoom::IsDBSaved(void)
{
	if( !IsSupportDB() )
	{
		return true;
	}

	return m_bSaved;
}

// 设置房间名称
void CRoom::SetName( const char* szRoomName )
{
	if( NULL == szRoomName )
	{
		return;
	}

	// 名称一样不做修改
	if( strcmp( m_szName, szRoomName ) == 0 )
	{
		return;
	}

	sstrcpyn( m_szName, szRoomName, sizeof(m_szName) );

	OnDataChanged();
	SaveData();

	// 取得频道里所有用户ID
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(Count());

	DWORD dwCount = 0;
	m_pChannel->GetClientIDList( pClientIDArray, dwBuffSize, dwCount );

	if( dwCount > 0 )
	{
		m_pChannel->UpdateChannelInfoToClient( pClientIDArray, dwCount, VoiceUpdateChannelType_RoomName, m_dwID );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;
}

// 获取房间名称
const char * CRoom::GetName(void)
{
	return m_szName;
}

// 设置文字聊天冷却时间 秒，如果输入值小于标准值，则表示关闭文字聊天  返回false，表明关闭
DWORD CRoom::SetChatCoolDown( DWORD dwTime )
{
	if( dwTime == m_dwChatCoolDown )
	{
		return dwTime;
	}

	if( dwTime <= 1 )
	{
		m_dwChatCoolDown = 1;
	}
	else if( dwTime > 900 )
	{
		m_dwChatCoolDown = 900;
	}
	else
	{
		m_dwChatCoolDown = dwTime;
	}
	
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(Count());

	// 取用户ID列表
	DWORD dwCount = 0;
	GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
	if( dwCount > 0 )
	{
		// 同步房间信息
		UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_Base );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;

	return m_dwChatCoolDown;
}

// 获取文字聊天冷却时间
DWORD CRoom::GetChatCoolDown(void)
{
	return m_dwChatCoolDown;
}

// 是否正在视频
bool CRoom::IsVideoShow(void)
{
	if( m_MicInfo.dwCurMicUser <= 0 )
	{
		return false;
	}

	CClientUser * pUser = CClientList::getInstance().GetUser(m_MicInfo.dwCurMicUser);
	if( NULL == pUser )
	{
		return false;
	}

	return pUser->GetFlag(VoiceUserStateFlag_Video);
}

// 房间峰值统计处理
void CRoom::StatsMaxMemberNum(bool bOn)
{
	m_bStatsMaxMember = bOn;

	if(bOn)
	{
		m_dwMaxMemberNum = m_mapList.size();
	}
	else
	{
		m_dwMaxMemberNum = 0;
	}
}

// 获取统计期间的房间峰值
DWORD CRoom::GetStatsMaxMemberNum(void)
{
	return m_dwMaxMemberNum;
}

// 房间广播
void CRoom::broadCast(void* pData, int len)
{
    int nCount = m_mapList.size();
    __VOICE_GET_ID_BUFFER_START(nCount);

    int i = 0;
    for(TMAP_UserList::iterator it = m_mapList.begin(); it != m_mapList.end(); ++it)
    {
        pClientIDArray[i++] = it->first;
    }

    gGlobalServer->BroadcastDataToClient( pClientIDArray, nCount, (const char*)pData, len );

    __VOICE_GET_ID_BUFFER_END;
}

// 更新房间内用户信息列表到客户端
template<typename T>
void CRoom::updateUserList2Client(DWORD dwClientID, void* p)
{
    SGameMsgHead header;
    header.SrcEndPoint = MSG_ENDPOINT_VOICE;
    header.DestEndPoint= MSG_ENDPOINT_CLIENT;
    header.byKeyModule  = MSG_MODULEID_VOICE;
    header.byKeyAction  = MSG_VOICE_CHANNELUSERLIST_UPDATE;			// 游戏玩家信息更新

    GlobalVoiceServer* pVoiceServer = (GlobalVoiceServer*)gGlobalServer;
    ofixbuf obufSend(pVoiceServer->m_szSendBuf, sizeof(pVoiceServer->m_szSendBuf));

    obufSend.push_back( &header, sizeof(SGameMsgHead) );

    CClientList & clientList = CClientList::getInstance();

    bool bMask[VOICE_FIELD_MAX];
    memset(bMask, 1, sizeof(bMask));
    for(T::iterator it = ((T*)p)->begin(); it != ((T*)p)->end(); ++it)
    {
        CClientUser * pUser = clientList.GetUser(it->first);
        if( pUser != NULL )
        {
            obuf ob;
            pUser->m_UserInfo.getFields(ob, bMask);
            if(obufSend.space() < ob.size()) // 缓冲区满 发送
            {
                gGlobalServer->SendDataToClient( dwClientID, obufSend.data(), obufSend.size() );
                obufSend.reset();
                obufSend.push_back( &header, sizeof(SGameMsgHead) );
            }
            else
            {
                obufSend.push_back(ob.data(), ob.size());
            }
        }
    }

    if(obufSend.size() > sizeof(SGameMsgHead))
    {
        gGlobalServer->SendDataToClient( dwClientID, obufSend.data(), obufSend.size() );
    }
}

// 保存数据到数据库,只能SaveData()内部调用,其他地方不要调用
void CRoom::SaveDataToDB(void)
{
	// 没有保存数据的可设为m_bSaved为true,否则停止服务器投票事件通不过导致不能关闭
	// 有存数据库的在数据请求返回时设为m_bSaved为true

	// 如果已保存数据或没加载数据，返回
	if (m_bSaved || ! m_bLoaded)
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn( "CRoom::SaveDataToDB 已保存数据或没加载数据，返回" );
#endif
		m_bSaved =true;
		return;
	}

	// 保存到数据库

	if( !IsSupportDB() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn( "CRoom::SaveDataToDB 不支持数据库，返回" );
#endif
		return;
	}

	// 保存语音频道房间信息
	DB_Param_VoiceRoom_Save dbParam;
	// 条件参数,找到nChannelID的记录来修改，否则不修改
	dbParam.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
	dbParam.bIsNew			= false;					// 是否是新增,如是新增，如已有记录就失败不操作，以免覆盖,如是修改，没有记录时可加一条
	// 要保存的数据
	dbParam.nChannelID		= m_pChannel->GetID();		// 频道ID
	dbParam.dwRoomID		= m_dwID;					// 房间ID
	dbParam.dwFlag			= m_dwStateFlag;			// 状态标识
	dbParam.dwMicTimes		= m_MicInfo.dwMicOrderTime;	// 麦时间
	dbParam.dwMicState		= m_dwMicState;				// 默认麦序模式
	dbParam.dwSpare1		= m_dwSpare1;				// 备用1
	dbParam.dwSpare2		= m_dwSpare1;				// 备用2
	dbParam.dwSpare3		= m_dwSpare1;				// 备用3
	sstrcpyn( dbParam.szName, m_szName, sizeof(dbParam.szName) );		// 房间名称
	sstrcpyn( dbParam.szPassword, m_szPassword, sizeof(dbParam.szPassword) );		// 房间密码
	sstrcpyn( dbParam.szTitle, m_strRemark.c_str(), sizeof(dbParam.szTitle) );		// 房间公告

	CDBRequester::getInstance().DB_VoiceRoom_Save(dbParam,static_cast<IDBRetSink *>(this));

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn( "CRoom::SaveDataToDB 已提交数据库！频道ID=" << m_pChannel->GetID() << "房间ID=" << m_dwID );
#endif

}



