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
#include "ChannelList.h"
#include "ClientList.h"
#include "GlobalVoiceServer.h"

// 构造函数
CChannel::CChannel(LONGLONG nID)
	: m_bObserverChange(false)
	, m_bDestroyTimerStart(false)	// 销毁定时器是否已经起来
	, m_dwCloseTimeDelay(0)
	, m_dwCloseTimeStart(0)
	, m_dwLastAudioTime(0)
	, m_dwMaxMemberCount(0)
	, m_dwTopListTime(0)
	, m_dwWorldID(0)
	, m_dwCountryID(0)
	, m_bLoaded(false)
	, m_bSaved(true)
	, m_bActive(false)
	, m_bPushed(false)
	, m_bActiveTimerStart(false)
	, m_dwActiveTimerStartTime(0)
	, m_dwActiveTimerEndTime(0)
	, m_dwAudioCount(0)
	, m_dwLastActiveTime(0)
	, m_dwAudioCheckTime(0)
	, m_dwActiveTimerCheckTime(0)
	, m_dwActiveRoomID(0)
	, m_dwRunFlag(0)
	, m_dwMaxCount(0)
{
	m_nID = nID;

	SVoiceChannelID *pIdInfo = (SVoiceChannelID *)(&nID);
	m_IDInfo = *pIdInfo;

	m_dwType = pIdInfo->dwType;

	memset( m_szOwnerName, 0, sizeof(m_szOwnerName) );

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn( "创建频道，nID=" << nID << "，类型=" << m_dwType << "，世界ID=" << pIdInfo->dwWorldID << "，频道ID=" << pIdInfo->dwID );
#endif

}

// 析构函数
CChannel::~CChannel(void)
{

}

// 创建
bool CChannel::Create(void)
{
	const SVoiceChannelConfig & config = GetChannelConfig(m_dwType);

	m_baseInfo.dwChatCoolDown = config.dwChatCoolDown;

	// 没有开放
	if( !(config.bOpen) )
	{
		SetFlag( VoiceChannelState_Close, true, false );

		m_strCloseInfo = "暂未开放！";
	}
	
	// 不支持用户列表
	if( !(config.bUserList) )
	{
		SetFlag( VoiceChannelState_NoUserList, true, false );
	}

	// 不支持麦序模式
	if( !(config.bMicOrder) )
	{
		SetFlag( VoiceChannelState_NoMicOrder, true, false );
	}

	// 不支持自由模式
	if( !(config.bMicFree) )
	{
		SetFlag( VoiceChannelState_NoMicFree, true, false );
	}

	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	{
		m_roomList[i].Create( this, i );
	}

	// 初始化
#ifdef VOICE_CLIENTID_BUFFLIST
	m_ObserverListInfo.pData	= new DWORD[VOICE_ROOM_MIN_BUFF_SIZE];
	m_ObserverListInfo.dwSize	= VOICE_ROOM_MIN_BUFF_SIZE;
	m_ObserverListInfo.dwCount	= 0;
#endif

	// 如果是个人频道，可以初始化拥有者的UDBID
	if( m_dwType == VoiceChannel_Personal )
	{
		m_baseInfo.dwOwnerUDBID = m_IDInfo.dwID;

		// 默认频道是开放的
		SetFlag( VoiceChannelState_Public, true, false );
	}

	// 最大人数
	m_dwMaxCount = config.dwMaxCount;

	// 语音品质
	m_baseInfo.dwQuality = config.dwVoiceQuality;


	// 加载数据库信息
	// 是否支持数据库存储,团队不需要存储数据
	if (IsSupportDB())
	{
		DWORD dwSvrWorldID = getThisGameWorldID();
		// 读取语音频道信息
		DB_Param_VoiceChannel_Read dbParam;
		dbParam.dwSvrWorldID	= dwSvrWorldID;
		dbParam.nChannelID		= m_nID;
		CDBRequester::getInstance().DB_VoiceChannel_Read(dbParam,static_cast<IDBRetSink *>(this));

		// 读取马甲权限用户信息
		DB_Param_VoiceVestInfo_Read dbParam3;
		dbParam3.dwSvrWorldID	= dwSvrWorldID;
		dbParam3.nChannelID		= m_nID;
		CDBRequester::getInstance().DB_VoiceVestInfo_Read(dbParam3,static_cast<IDBRetSink *>(this));

		// 读取语音频道房间信息
		DB_Param_VoiceRoom_Read dbParam2;
		dbParam2.dwSvrWorldID	= dwSvrWorldID;
		dbParam2.nChannelID		= m_nID;
		CDBRequester::getInstance().DB_VoiceRoom_Read(dbParam2,static_cast<IDBRetSink *>(this));
	}

	gGlobalServer->SetTimer( ETimerEventID_DB, VOICE_SAVE_DB_INTERVAL, this, "CChannel::Create" );

	return true;
}

// 释放
void CChannel::Release(void)
{
	// 关闭数据库回调
	GameDatabaseAccessor::getInstance().RemoveDBRetSink(static_cast<IDBRetSink *>(this));
	// 通知逻辑层房间销毁
	gGlobalServer->OnChannelDestroy(this);

	CChannelList::getInstance().Del(m_nID);

	gGlobalServer->KillTimer( ETimerEventID_Destroy, this );
	gGlobalServer->KillTimer( ETimerEventID_Close, this );
	gGlobalServer->KillTimer( ETimerEventID_Active, this );
	gGlobalServer->KillTimer( ETimerEventID_DB, this );
	gGlobalServer->KillTimer( ETimerEventID_Kill, this );
	
	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	{
		m_roomList[i].Release();
	}

#ifdef VOICE_CLIENTID_BUFFLIST
	m_ObserverListInfo.dwCount = 0;
	delete [] m_ObserverListInfo.pData;
	m_ObserverListInfo.pData = NULL;
#endif

#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("CChannel::Release 频道没有人了，销毁掉，频道ID=" << m_nID );
#endif
	delete this;
}

///////////////////// TimerHandler /////////////////////////////////////////////////////
// 定时器，计算麦序时间等
void CChannel::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_DB:	// 保存数据库定时器
		{
			OnTimerCheckSaveDB();
		}
		break;

	case ETimerEventID_Kill:				// 释放自己定时器，顺便检查是否保存数据库
		{
			m_dwCloseTimeDelay ++;

			// 如果数据已经保存或者已经保存超时，强制释放
			if( IsDBSaved() || m_dwCloseTimeDelay > 600 )
			{
				Release();
			}
		}
		break;

	case ETimerEventID_Destroy:		// 无人时延时关闭定时器
		{
			m_bDestroyTimerStart = false;
			DWORD dwTime = 0;
			if( CanDestroy(dwTime) )
			{
				// 立即保存数据
				OnTimerCheckSaveDB();

				// 关闭频道，设置标记即可
				SetRunFlag( VoiceChannelRunFlag_Saving, true );

				m_dwCloseTimeDelay = 0;
				gGlobalServer->SetTimer( ETimerEventID_Kill, 3000, this, "CChannel::OnTimer" );
			}
		}
		break;

	case ETimerEventID_Close:	// 频道关闭定时器
		{
			DWORD dwNow = (DWORD)time(NULL);
			
			// 时间到了，把人踢光
			if( dwNow >= m_dwCloseTimeStart + m_dwCloseTimeDelay )
			{
				StopCloseTimer();
				KickUser();
			}
			else	// 没到时间，提示频道里的用户
			{
				char szText[512] = {0};
				DWORD dwDiffTime = m_dwCloseTimeStart + m_dwCloseTimeDelay - dwNow;
				sprintf_s(szText, 512, "频道将在%s后关闭！原因：%s", GetRemainTimeString(dwDiffTime), m_strCloseInfo.c_str() );

				if ( dwDiffTime <= 15 )		// 10秒内，每秒提示
				{
					SendChatMsg(szText);
				}
			}
		}
		break;

	case ETimerEventID_Active:
		{
			DWORD dwNow = ::GetTickCount();

			// 如果两分钟内，都没有语音来了，杀掉定时器
			if( dwNow >= m_dwLastAudioTime + 120000 )
			{
#ifdef VOICE_PRINT_DEBUG_INF
				TraceLn("CChannel::OnTimer 两分钟内，都没有语音来了");
#endif
				gGlobalServer->KillTimer( ETimerEventID_Active, this );
				m_bActiveTimerStart = false;
				m_dwActiveTimerEndTime = dwNow;

				// 如果已经推送过了，要关闭推送
				if( m_bActive )
				{
#ifdef VOICE_PRINT_DEBUG_INF
					TraceLn("CChannel::OnTimer 已经推送过了，要关闭推送");
#endif
					m_bActive = false;
					OnActiveChange();
				}
			}
			else
			{
				// 5秒内有语音过
				if( (m_dwLastAudioTime >= m_dwActiveTimerCheckTime) && (m_dwLastAudioTime <= m_dwActiveTimerCheckTime + 5000) )
				{
					m_dwAudioCount ++;
#ifdef VOICE_PRINT_DEBUG_INF
					TraceLn("CChannel::OnTimer 5秒内有语音过 m_dwAudioCount=" << m_dwAudioCount);
#endif
				}
				m_dwActiveTimerCheckTime = dwNow;

				// 3分钟内，语音满30次，则触发推送
				if( dwNow <= m_dwActiveTimerStartTime + 180000 )
				{
					if( m_dwAudioCount >= 30 )
					{
#ifdef VOICE_PRINT_DEBUG_INF
						TraceLn("CChannel::OnTimer 3分钟内，语音满30次 m_dwAudioCount=" << m_dwAudioCount);
#endif
						if( !m_bActive )
						{
							m_bActive = true;
							m_dwLastActiveTime = dwNow;
							OnActiveChange();
						}
					}
				}
				else
				{
					if( m_dwAudioCount < 30 )
					{
#ifdef VOICE_PRINT_DEBUG_INF
						TraceLn("CChannel::OnTimer 3分钟内，语音满30次,条件不满足，杀掉定时器 m_dwAudioCount=" << m_dwAudioCount);
#endif
						// 如果条件不满足，杀掉定时器
						gGlobalServer->KillTimer( ETimerEventID_Active, this );
						m_bActiveTimerStart = false;
						m_dwActiveTimerEndTime = dwNow;
					}
				}
			}
		}
		break;
	}
}


//数据库请求返回回调方法
void CChannel::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	// 出错了就报一下
	if ( nDBRetCode!=DBRET_CODE_SUCCEED )
	{
		ErrorLn("CChannel::OnReturn() 出错了 nCmdID=" << nCmdID << " nDBRetCode=" << nDBRetCode);
		if ( pszDBRetDesc )
		{
			ErrorLn(pszDBRetDesc);
		}

		return;
	}

	switch(nCmdID)
	{
	case VOICEDB_REQUEST_VOICECHANNEL_READ:		// 读取语音频道信息
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CChannel::OnReturn() 读取语音频道信息 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			if(pOutData == NULL || nOutLen < sizeof(DB_Result_VoiceChannel_Read))
			{
				ErrorLn("读取语音频道信息 数据大小非法，nOutLen = "<<nOutLen);
				return;	
			}
			DB_Result_VoiceChannel_Read * pResult = (DB_Result_VoiceChannel_Read *)(pOutData);

			if(nOutLen != pResult->nRecordCount * sizeof(DB_Table_VoiceChannel) + sizeof(DB_Result_VoiceChannel_Read))
			{
				ErrorLn("读取语音频道信息 数据条数与大小不符，RecordCount="<<pResult->nRecordCount<<",nOutLen = "<<nOutLen);
				return;
			}

			// 是否已加载数据
			m_bLoaded = true;
			m_bSaved = true;

			// 设一下保存时间,否则
			m_saveCoolInfo.dwLastTimes	= ::GetTickCount();

			// 有数据
			if (pResult->nRecordCount>0)
			{
				DB_Table_VoiceChannel * pInfo = (DB_Table_VoiceChannel *)(pOutData + sizeof(DB_Result_VoiceChannel_Read));

				// 更新频道配置信息
				m_baseInfo.dwOwnerUDBID		= pInfo->dwOwnerUDBID;		// 主人通行证UDBID
				m_baseInfo.dwDefautRoomID	= pInfo->dwRoomID;			// 默认进入的房间ID
				m_baseInfo.dwStateFlag		= pInfo->dwFlag;			// 状态标记 EMVoiceChannelStateFlag
				m_baseInfo.dwUseType		= pInfo->dwUseType;			// 应用类型
				m_baseInfo.dwSpare1			= pInfo->dwSpare1;			// 备用1
				m_baseInfo.dwSpare2			= pInfo->dwSpare2;			// 备用2
				m_baseInfo.dwSpare3			= pInfo->dwSpare3;			// 备用3
				m_baseInfo.dwOwnerType		= pInfo->dwOwnerType;		// 拥有者帐号类型

				m_dwMaxMemberCount			= pInfo->dwMaxOnline;		// 最高在线

				sstrcpyn( m_szOwnerName, pInfo->szName, sizeof(m_szOwnerName) );

				// 官方和世界频道的语音品质不使用数据库中的,用默认的
				//if (VoiceChannel_GM!=m_dwType && VoiceChannel_World!=m_dwType)
				//{
					m_baseInfo.dwQuality		= pInfo->dwQuality;			// 语音品质
				//}

#ifdef VOICE_PRINT_DEBUG_INF
				// 调式代码
				sprintf_s(szBuf, _countof(szBuf),"UDBID=%d,语音品质=%d,默认房间ID=%d,状态标记=%d,应用类型=%d,最高在线=%d,名称:%s",
					m_baseInfo.dwOwnerUDBID,m_baseInfo.dwQuality,m_baseInfo.dwDefautRoomID,m_baseInfo.dwStateFlag,m_baseInfo.dwUseType,m_dwMaxMemberCount,m_szOwnerName);
				TraceLn(szBuf);
#endif
			}
			else
			{
				// 没有就新加一条

				// 保存语音频道信息
				DB_Param_VoiceChannel_Save dbParam;
				// 条件参数,找到nChannelID的记录来修改，否则不修改
				dbParam.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
				dbParam.bIsNew			= true;						// 是否是新增,如是新增，如已有记录就失败不操作，以免覆盖,如是修改，没有记录时可加一条
				// 要保存的数据
				dbParam.nChannelID		= m_nID;					// 频道ID
				dbParam.dwType			= m_IDInfo.dwType;			// 频道类型
				dbParam.dwWorldID		= m_IDInfo.dwWorldID;		// 世界ID
				dbParam.dwDataID		= m_IDInfo.dwID;			// 频道数值ID
				dbParam.dwOwnerUDBID	= m_baseInfo.dwOwnerUDBID;	// 主人通行证UDBID
				dbParam.dwMaxOnline		= m_dwMaxMemberCount;		// 最高在线
				dbParam.dwQuality		= m_baseInfo.dwQuality;		// 语音品质
				dbParam.dwRoomID		= m_baseInfo.dwDefautRoomID;// 默认进入的房间ID
				dbParam.dwFlag			= m_baseInfo.dwStateFlag;	// 状态标识
				dbParam.dwUseType		= m_baseInfo.dwUseType;		// 应用类型
				dbParam.dwSpare1		= m_baseInfo.dwSpare1;		// 备用1
				dbParam.dwSpare2		= m_baseInfo.dwSpare2;		// 备用2
				dbParam.dwSpare3		= m_baseInfo.dwSpare3;		// 备用3
				sstrcpyn( dbParam.szName, m_szOwnerName, sizeof(dbParam.szName) );		// 频道名称，默认情况下是拥有者名称

				CDBRequester::getInstance().DB_VoiceChannel_Save(dbParam);
			}
		}
		break;

	case VOICEDB_REQUEST_VOICEROOM_READ:		// 读取语音频道房间信息
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[1024]={0};
			sprintf_s(szBuf, _countof(szBuf),"CChannel::OnReturn() 读取语音频道房间信息 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			if(pOutData == NULL || nOutLen < sizeof(DB_Result_VoiceRoom_Read))
			{
				ErrorLn("读取语音频道房间信息 数据大小非法，nOutLen = "<<nOutLen);
				return;	
			}
			DB_Result_VoiceRoom_Read * pResult = (DB_Result_VoiceRoom_Read *)(pOutData);

#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn( "CChannel::OnReturn() 读取语音频道房间信息 RecordCount=" << pResult->nRecordCount );
#endif

			if(nOutLen != pResult->nRecordCount * sizeof(DB_Table_VoiceRoom) + sizeof(DB_Result_VoiceRoom_Read))
			{
				ErrorLn("读取语音频道房间信息 数据条数与大小不符，RecordCount="<<pResult->nRecordCount<<",nOutLen = "<<nOutLen);
				return;
			}

			DB_Table_VoiceRoom * pInfo = (DB_Table_VoiceRoom *)(pOutData + sizeof(DB_Result_VoiceRoom_Read));

			for (int i=0;i<pResult->nRecordCount;i++)
			{
				if (pInfo->dwRoomID<VoiceRoomType_Max)
				{
					CRoom & room = GetRoom(pInfo->dwRoomID);
					// 收到数据库数据
					room.ImportDBInfo(*pInfo);

#ifdef VOICE_PRINT_DEBUG_INF
					// 调式代码
					sprintf_s(szBuf, _countof(szBuf),"房间ID=%d,状态标识=%d,麦时间=%d,默认麦序模式=%d,备用1=%d,,备用2=%d,,备用3=%d,房间名称=%s,房间密码=%s,房间公告=%s",
						pInfo->dwRoomID,pInfo->dwFlag,pInfo->dwMicTimes,pInfo->dwMicState,pInfo->dwSpare1,pInfo->dwSpare2,pInfo->dwSpare3,pInfo->szName,pInfo->szPassword,pInfo->szTitle);
					TraceLn(szBuf);
#endif
				}

				pInfo++;
			}

			for( DWORD nRoomID = VoiceRoomType_Main; nRoomID < VoiceRoomType_Max; nRoomID ++ )
			{
				CRoom & room = GetRoom(nRoomID);
				if( !room.IsLoaded() )
				{
					// 如果到这里还没有加载完成，说明是没有记录的，手动设置加载完成
					room.SetLoaded(true);
#ifdef VOICE_PRINT_DEBUG_INF
					TraceLn( "CChannel::OnReturn() 读取语音频道房间信息 房间ID" << nRoomID << "没有数据，手动添加一条记录" );
#endif
				}

				MoveUser( nRoomID, nRoomID );
			}

			// 取得频道里所有用户ID
			// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
			__VOICE_GET_ID_BUFFER_START(Count());
			DWORD dwCount = 0;
			GetClientIDList( pClientIDArray, dwBuffSize, dwCount );

			if( dwCount > 0 )
			{
				UpdateChannelInfoToClient( pClientIDArray, dwCount, VoiceUpdateChannelType_All );
			}

			// 是new的内存,用完释放
			// ID缓存分配结束
			__VOICE_GET_ID_BUFFER_END;

		}
		break;

	case VOICEDB_REQUEST_VOICEVEST_READ:		// 读取马甲权限用户信息
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CChannel::OnReturn() 读取马甲权限用户信息 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			if(pOutData == NULL || nOutLen < sizeof(DB_Result_VoiceVestInfo_Read))
			{
				ErrorLn("语音角色用户登录数据大小非法，nOutLen = "<<nOutLen);
				return;	
			}
			DB_Result_VoiceVestInfo_Read * pResult = (DB_Result_VoiceVestInfo_Read *)(pOutData);

			if(nOutLen != pResult->nRecordCount * sizeof(DB_Table_VoiceVestInfo) + sizeof(DB_Result_VoiceVestInfo_Read))
			{
				ErrorLn("语音角色用户登录数据条数与大小不符，RecordCount="<<pResult->nRecordCount<<",nOutLen = "<<nOutLen);
				return;
			}

			m_vestList.clear();

			DB_Table_VoiceVestInfo * pInfo = (DB_Table_VoiceVestInfo *)(pOutData + sizeof(DB_Result_VoiceVestInfo_Read));

			for (int i=0;i<pResult->nRecordCount;i++)
			{
				// 计算权限
				DWORD dwNewChannelRight = VoiceRight_None;
				switch(pInfo->nUserType)
				{
				case VoiceUserType_VIP:					// 嘉宾主持
					dwNewChannelRight = VOICE_USER_RIGHT_VIP;
					break;

				case VoiceUserType_Host:					// 主播
					dwNewChannelRight = VOICE_USER_RIGHT_HOST;
					break;

				case VoiceUserType_Admin:				// 频道管理员
					dwNewChannelRight = VOICE_USER_RIGHT_ADMIN;
					break;
				}

				SVoiceVestInfo addNode;
				addNode.dwUDBID			= pInfo->dwPDBID;		// 用户角色ID
				addNode.nSex			= pInfo->nSex;			// 性别
				addNode.nNewUserType	= pInfo->nUserType;		// 新用户类型
				sstrcpyn(addNode.szName, pInfo->szName, sizeof(addNode.szName));

				m_vestList.insert(TMAP_VestList::value_type(addNode.dwUDBID, addNode));


#ifdef VOICE_PRINT_DEBUG_INF
				// 调式代码
				sprintf_s(szBuf, _countof(szBuf),"%d.角色ID=%d,性别=%d,用户类型=%d,角色=%s",
					i,addNode.dwPDBID,addNode.nSex,addNode.nNewUserType,addNode.szName);
				TraceLn(szBuf);
#endif

				pInfo++;
			}
			
			// 有数据才更新所有房间的人的权限信息
			if (pResult->nRecordCount>0)
			{
				for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
				{
					// 更新一下当前所有用户房间信息,主要是用来数据库加载数据后,需刷新一下相关状态和权限
					m_roomList[i].UpdateAllUserCurRoomInf();
				}
			}

		}
		break;


	case VOICEDB_REQUEST_VOICECHANNEL_SAVE:		// 保存语音频道信息
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CChannel::OnReturn() 保存语音频道信息 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
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


// 获取频道类型
const SVoiceChannelID & CChannel::GetIDInfo(void)
{
	return m_IDInfo;
}

// 获取频道ID
LONGLONG CChannel::GetID(void)
{
	return m_nID;
}

// 获得频道内总人数
DWORD CChannel::Count(void)
{
	DWORD dwCount = 0;
	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	{
		dwCount += m_roomList[i].Count();
	}

	return dwCount;
}


// 获得频道内总人数,可排除国家频道中挂机的人数
DWORD CChannel::CountEx(bool bNoCountryHang)
{
	DWORD dwCount = 0;
	if (bNoCountryHang && GetType()==VoiceChannel_Country)
	{
		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			if (VoiceRoomType_Hang==i)
			{
				continue;
			}
			dwCount += m_roomList[i].Count();
		}
	}
	else
	{
		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			dwCount += m_roomList[i].Count();
		}
	}

	return dwCount;
}

// 获取房间, 如果找不到，返回默认房间
CRoom & CChannel::GetRoom( DWORD dwRoomID )
{
	if( dwRoomID >= VoiceRoomType_Max )
	{
		return m_roomList[0];	// 默认返回主房间
	}

	return m_roomList[dwRoomID];
}

// 获取子房间指针, 如果找不到房间，返回空
CRoom * CChannel::GetRoomPtr( DWORD dwRoomID )
{
	if( dwRoomID >= VoiceRoomType_Max )
	{
		return NULL;	// 空
	}

	return &(m_roomList[dwRoomID]);
}

// 获取类型
DWORD CChannel::GetType(void)
{
	return m_dwType;
}

// 用户数变化
void CChannel::OnUsersChange( CRoom & room, CClientUser & user, bool bAdd )
{
	DWORD dwClientID = user.GetClientID();

	// 更新最高在线人数
	DWORD dwCurMemberCount = Count();
	if( dwCurMemberCount > m_dwMaxMemberCount )		// 如果比历史在线高，就更新一下
	{
		m_dwMaxMemberCount = dwCurMemberCount;

		// 保存数据到数据库，历史在线更新频率不高，可以保存数据
		OnDataChanged();
		SaveData();
	}

	// 列表更新类型
	DWORD dwUpdateType = VoiceUpdateType_Add;

	// 如果有人离开房间，定时销毁房间
	if( !bAdd )
	{
		dwUpdateType = VoiceUpdateType_Delete;

		// 频道人数小于1，才启动启动销毁定时器，并且是队伍，团频道才销毁
		DWORD dwDestroyTime = 0;
		if( CanDestroy(dwDestroyTime) && !m_bDestroyTimerStart )
		{
#ifdef VOICE_PRINT_DEBUG_INF
			ErrorLn( "CChannel::OnUsersChange 频道没有人了，启动销毁定时器，频道ID=" << m_nID );
#endif

			m_bDestroyTimerStart = true;

			gGlobalServer->SetTimer( ETimerEventID_Destroy, dwDestroyTime*60000+100, this, "CChannel::OnUsersChange", 1 );

		}
	}
	else
	{
		// 有人进入，把定时器杀掉
		if( m_bDestroyTimerStart )
		{
			m_bDestroyTimerStart = false;
			gGlobalServer->KillTimer( ETimerEventID_Destroy, this );
		}
	}

	// 取得所有房间观察者数量
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(GetObserversCount());

	// 得到观察者列表
	DWORD dwCount = 0;
	GetObserverIDList( pClientIDArray, dwBuffSize, dwCount );

	if( dwCount > 0 )
	{
		// 支持列表，更新用户信息添加到观察者列表
		if( !GetFlag(VoiceChannelState_NoUserList) )
		{
            //BroadcastDataToObserver(ob.data(), ob.size());
		}
		else	// 不支持列表，更新房间基本信息到客户端，冷却5秒钟
		{
			DWORD dwNow = ::GetTickCount();
			if( dwNow >= room.m_dwCountUpdateTime + 5000 )
			{
				room.m_dwCountUpdateTime = dwNow;
				room.UpdateRoomInfoToClient( pClientIDArray, dwCount, VoiceUpdateRoomType_Base );
			}
		}
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;

	// 如果用户是管理员，则需要更新当前频道在线管理员列表
	DWORD dwUserType = user.GetType(m_dwType);
	if( dwUserType > VoiceUserType_Normal && dwUserType < VoiceUserType_Max )
	{
		// 除了GM其他管理员进入了都要通知
		if( dwUserType < VoiceUserType_GM )
		{
			UpdateAdminEnterInfoToOther( user, room.GetID(), bAdd );
		}

		AddCurAdmin( dwClientID, bAdd );
	}
}

// 广播数据给观察者 dwBanClientID 排除ID
void CChannel::BroadcastDataToObserver( const char *data, DWORD dwLen, DWORD dwBanClientID )
{
	// 取得所有房间观察者数量
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(GetObserversCount());

	// 得到观察者列表
	DWORD dwCount = 0;
	GetObserverIDList( pClientIDArray, dwBuffSize, dwCount );

	if( dwCount > 0 )
	{
		gGlobalServer->BroadcastDataToClient( pClientIDArray, dwCount, data, dwLen, dwBanClientID );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;
}



// 添加一个观察者，新增的返回TRUE
bool CChannel::AddObserver( DWORD dwClientID, DWORD dwRoomID )
{
	DWORD dwTimeNow = ::GetTickCount();

	// 房间ID无效
	if( dwRoomID >= VoiceRoomType_Max )
	{
		ErrorLn( "CChannel::AddObserver 房间ID无效，dwRoomID=" << dwRoomID );
		return false;
	}

	TMAP_ChannelObserver & observers = m_ChannelObserver[dwRoomID];

	TMAP_ChannelObserver::iterator it = observers.find(dwClientID);
	// 找到了
	if( it != observers.end() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("观察者已经存在了，不需要再添加，dwClientID=" << dwClientID);
#endif
		it->second.dwTimeout = dwTimeNow;
		return false;
	}

	// 修改标志
	m_bObserverChange = true;

	SChannelObserver observer;
	observer.dwClientID = dwClientID;	
	observer.dwTimeout = dwTimeNow;		// 超时时间

	// 加入观察者列表
	observers.insert( TMAP_ChannelObserver::value_type(dwClientID, observer) );

#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("CChannel::AddObserver 添加观察者，dwRoomID=" << dwRoomID << ", dwClientID=" << dwClientID);
#endif

	return true;
}

// 删除一个观察者
bool CChannel::DelObserver( DWORD dwClientID, DWORD dwRoomID )
{
	if( dwRoomID >= VoiceRoomType_Max )
	{
		ErrorLn( "房间ID无效，dwRoomID=" << dwRoomID );
		return false;
	}
	
	TMAP_ChannelObserver & observers = m_ChannelObserver[dwRoomID];

	TMAP_ChannelObserver::iterator it = observers.find(dwClientID);
	// 找到了才删除
	if( it != observers.end() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CChannel::DelObserver 移除观察者，dwRoomID=" << dwRoomID << ", dwClientID=" << dwClientID );
#endif

		observers.erase(it);

		// 设置列表改变标记
		m_bObserverChange = true;

		return true;
	}
#ifdef VOICE_PRINT_DEBUG_INF
	else
	{
		ErrorLn("CChannel::DelObserver 移除观察者时，没找到观察者。dwRoomID=" << dwRoomID << ", dwClientID=" << dwClientID);
	}
#endif

	return false;
}

// 移动一个观察者，dwSrcRoomID原房间ID，dwDesRoomID目的房间ID
bool CChannel::MoveObserver( DWORD dwClientID, DWORD dwSrcRoomID, DWORD dwDesRoomID )
{
	if( dwSrcRoomID >= VoiceRoomType_Max || dwDesRoomID >= VoiceRoomType_Max )
	{
		ErrorLn( "房间ID无效，dwSrcRoomID=" << dwSrcRoomID << ", dwDesRoomID=" << dwDesRoomID );
		return false;
	}
	// 一样的就不处理了
	if(dwSrcRoomID==dwDesRoomID)
	{
		return false;
	}

	// 查找源房间，从里面移除
	TMAP_ChannelObserver & srcObservers = m_ChannelObserver[dwSrcRoomID];
	TMAP_ChannelObserver::iterator itSrc = srcObservers.find(dwClientID);
	if( itSrc == srcObservers.end() )		// 没找到，返回false
	{
#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CChannel::MoveObserver 源房间里没找到观察者，dwClientID=" << dwClientID << ", dwSrcRoomID=" << dwSrcRoomID );
#endif
		return false;
	}

	// 找到了，移除
	srcObservers.erase(itSrc);

	// 查找目的房间，添加
	TMAP_ChannelObserver & desObservers = m_ChannelObserver[dwDesRoomID];
	TMAP_ChannelObserver::iterator itDes = desObservers.find(dwClientID);
	if( itDes != desObservers.end() )		// 找到了，不再添加
	{
#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CChannel::MoveObserver 目的房间已经有观察者，dwClientID=" << dwClientID << ", dwDesRoomID=" << dwDesRoomID );
#endif
		return false;
	}

	SChannelObserver observer;
	observer.dwClientID = dwClientID;	
	observer.dwTimeout = ::GetTickCount();		// 超时时间

	// 加入观察者列表
	desObservers.insert( TMAP_ChannelObserver::value_type(dwClientID, observer) );

#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("CChannel::MoveObserver 移动观察者，dwClientID=" << dwClientID << "源房间=" << dwSrcRoomID << "，目的房间=" << dwDesRoomID );
#endif

	m_bObserverChange = true;

	return true;
}

// 清空观察者
void CChannel::ClearObserver( DWORD dwRoomID )
{
	// 清除所有
	if( dwRoomID >= VoiceRoomType_Max )
	{
		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			m_ChannelObserver[i].clear();
		}
	}
	else
	{
		m_ChannelObserver[dwRoomID].clear();
	}
}


// 是否是观察者
bool CChannel::IsObserver( DWORD dwClientID, DWORD dwRoomID )
{
	if( dwRoomID >= VoiceRoomType_Max )
	{
		ErrorLn( "房间ID无效，dwRoomID=" << dwRoomID );
		return false;
	}

	TMAP_ChannelObserver & observers = m_ChannelObserver[dwRoomID];

	TMAP_ChannelObserver::iterator it = observers.find(dwClientID);
	if( it != observers.end() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CChannel::IsObserver [" << dwClientID << "]是观察者，房间ID=" << dwRoomID );
#endif
		return true;		// 找到了，返回true
	}
#ifdef VOICE_PRINT_DEBUG_INF
	else
	{
		TraceLn("CChannel::IsObserver [" << dwClientID << "]不是观察者，房间ID=" << dwRoomID );
	}
#endif

	return false;
}

/*
// 获取观察者ID列表 暂时关闭此功能，好像有问题
const DWORD * CChannel::GetObserverList( DWORD & dwCount, DWORD dwRoomID )
{
	// 暂时关闭此功能，好像有问题
	dwCount = 0;
	return NULL;

	DWORD dwTimeNow = ::GetTickCount();

	// 定期清理超时观察者
	if( dwTimeNow > m_ObserverListInfo.dwClearTime + VOICE_CHANNEL_OBSERVER_TIMEOUT/2 )
	{
		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			TMAP_ChannelObserver & observers = m_ChannelObserver[i];

			for( TMAP_ChannelObserver::iterator it = observers.begin(); it != observers.end(); )
			{
				// 移除超时的
				if( dwTimeNow > it->second.dwTimeout + VOICE_CHANNEL_OBSERVER_TIMEOUT  )
				{
#ifdef VOICE_PRINT_DEBUG_INF
					ErrorLn("CChannel::GetObserverList 观察者超时了，移除观察者，dwClientID=" << it->second.dwClientID );
#endif
					observers.erase(it++);
					m_bObserverChange = true;

				}
				else
				{
					it ++;
				}
			}
		}

		m_ObserverListInfo.dwClearTime = dwTimeNow;
	}
	

	// 需要重新计算
	if( m_bObserverChange )
	{
		m_bObserverChange = false;

		// 分配ObserverList内存，返回true表示重新分配，
		GetObserverListBuff();

		m_ObserverListInfo.dwCount = 0;

		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			m_ObserverListInfo.dwRoomCount[i] = 0;
			m_ObserverListInfo.pRoomData[i] = &(m_ObserverListInfo.pData[m_ObserverListInfo.dwCount]);

			TMAP_ChannelObserver & observers = m_ChannelObserver[i];

			// 遍历整个房间的观察者列表
			for( TMAP_ChannelObserver::iterator it = observers.begin(); it != observers.end(); it ++ )
			{
				// 保存ID
				m_ObserverListInfo.pData[m_ObserverListInfo.dwCount] = it->second.dwClientID;

				// 总数增加
				m_ObserverListInfo.dwCount ++;

				// 对应的房间观察者数增加
				m_ObserverListInfo.dwRoomCount[i] ++;
			}
#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn("CChannel::GetObserverList 房间[" << i << "]的观察者数为[" << m_ObserverListInfo.dwRoomCount[i] << "]");
#endif
		}
	}

	// 如果房间ID为VoiceRoomType_Max，则返回整个频道的的观察者
	if( dwRoomID >= VoiceRoomType_Max )
	{
		dwCount = m_ObserverListInfo.dwCount;

#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CChannel::GetObserverList 频道观察者数=" << dwCount);
#endif

		return m_ObserverListInfo.pData;
	}
	else
	{
		dwCount = m_ObserverListInfo.dwRoomCount[dwRoomID];

#ifdef VOICE_PRINT_DEBUG_INF
		ErrorLn("CChannel::GetObserverList 房间观察者数=" << dwCount);
#endif

		return m_ObserverListInfo.pRoomData[dwRoomID];
	}
	
}
*/


// 获取频道里指定房间观察者ID列表
DWORD CChannel::GetObserverIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, DWORD dwRoomID )
{
	// 如果为空，直接返回
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;

	// 取频道里所有观察者
	if( dwRoomID >= VoiceRoomType_Max )
	{
		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			// 如果满了要退出
			bool bFull = false;

			TMAP_ChannelObserver & observers = m_ChannelObserver[i];

			// 遍历整个房间的观察者列表
			for( TMAP_ChannelObserver::iterator it = observers.begin(); it != observers.end(); ++it  )
			{
				if (dwFindCounts >= dwIDArrayCount )
				{
					bFull = true;
					break;
				}

				(*pClientID) = it->second.dwClientID;

				pClientID ++;
				dwFindCounts++;
			}

			// 满了，不再找了
			if( bFull )
			{
				break;
			}
		}
	}
	else	// 取房间里观察者
	{
		TMAP_ChannelObserver & observers = m_ChannelObserver[dwRoomID];

		// 遍历整个房间的观察者列表
		for( TMAP_ChannelObserver::iterator it = observers.begin(); it != observers.end(); ++it  )
		{
			if (dwFindCounts >= dwIDArrayCount )
			{
				break;
			}

			(*pClientID) = it->second.dwClientID;

			pClientID ++;
			dwFindCounts++;
		}
	}

	dwGetCounts = dwFindCounts;

	return dwFindCounts;
}


// 取得某个房间的观察者数量
DWORD CChannel::GetObserversCount( DWORD dwRoomID )
{
	// 定期清理超时观察者
	DWORD dwTimeNow = ::GetTickCount();
	if( dwTimeNow > m_ObserverListInfo.dwClearTime + VOICE_CHANNEL_OBSERVER_TIMEOUT/2 )
	{
		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			TMAP_ChannelObserver & observers = m_ChannelObserver[i];

			for( TMAP_ChannelObserver::iterator it = observers.begin(); it != observers.end(); )
			{
				// 移除超时的
				if( dwTimeNow > it->second.dwTimeout + VOICE_CHANNEL_OBSERVER_TIMEOUT  )
				{
#ifdef VOICE_PRINT_DEBUG_INF
					ErrorLn("CChannel::GetObserversCount 观察者超时了，移除观察者，dwClientID=" << it->second.dwClientID );
#endif
					observers.erase(it++);
					m_bObserverChange = true;
				}
				else
				{
					it ++;
				}
			}
		}

		m_ObserverListInfo.dwClearTime = dwTimeNow;
	}

	// 取频道里所有观察者
	if( dwRoomID >= VoiceRoomType_Max )
	{
		DWORD dwCount = 0;

		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			dwCount += m_ChannelObserver[i].size();
		}

		return dwCount;
	}
	else	// 取房间里观察者
	{
		return m_ChannelObserver[dwRoomID].size();
	}
}



// 获取频道里所有房间人ID列表
DWORD CChannel::GetClientIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, bool bNoMute )
{
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;
	DWORD dwBuffCounts = dwIDArrayCount;
	
	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	{
		DWORD dwCount = 0;
		CRoom & room = GetRoom(i);
		room.GetUserIDList( pClientID, dwBuffCounts, dwCount, bNoMute );

		dwFindCounts += dwCount;		// 增加数量
		pClientID += dwCount;		// 指向后一段空间

		if( dwBuffCounts > dwCount )	// 剩余空间比当前获取的数量大
		{
			dwBuffCounts -= dwCount;
		}
		else
		{
			break;
		}
	}

	dwGetCounts = dwFindCounts;

	return dwFindCounts;
}

// 获取当前在频道里的所有管理员列表
DWORD CChannel::GetCurAdminIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, DWORD dwBanClientID )
{
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;
	for(TMAP_CurAdminList::iterator it = m_CurAdminList.begin(); it != m_CurAdminList.end(); ++it)
	{
		if (dwFindCounts >= dwIDArrayCount )
		{
			break;
		}

		DWORD dwClientID = it->second;

		if( dwClientID == dwBanClientID )
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

// 获取频道的所有在线管理员列表
DWORD CChannel::GetAdminIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts )
{	
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;

	CClientList & clientList = CClientList::getInstance();

	for( TMAP_VestList::iterator it = m_vestList.begin(); it != m_vestList.end(); ++it  )
	{
		if (dwFindCounts >= dwIDArrayCount )
		{
			break;
		}

		// 查找是否在线
		DWORD dwClientID = clientList.GetClientIDByUDBID(it->second.dwUDBID);
		if( dwClientID <= 0 )
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


// 更新频道信息到客户端
void CChannel::UpdateChannelInfoToClient( DWORD * pDesClientArray, DWORD wDesClientNum, BYTE nUpdateType, DWORD dwRoomID )
{
	// 组个头
	obuf1024 obufData;
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_CHANNELINFO_UPDATE;
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	SMsgVoiceUpdateChannelInfoHead_VC listData;
	listData.nUpdateType = nUpdateType;
	obufData.push_back( &listData, sizeof(SMsgVoiceUpdateChannelInfoHead_VC) );

	switch(nUpdateType)
	{
	case VoiceUpdateChannelType_All:	// 全部频道信息，以后扩展用
		{
			SMsgVoiceUpdateChannelAll_VC allInfo;
			allInfo.baseInfo = m_baseInfo;

			allInfo.nRoomLockInfo = 0;
			allInfo.nAudioInfo = 0;

			obuf512 obufName; 
			DWORD dwNow = ::GetTickCount();
			for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
			{
				CRoom & room = GetRoom(i);
				// 房间有锁
				if( room.GetFlag(VoiceRoomState_Lock) )
				{
					WORD nLockFlag = (1 << i);
#ifdef VOICE_PRINT_DEBUG_INF
					TraceLn("CChannel::UpdateChannelInfoToClient 房间[" << i << "]加锁信息=" << nLockFlag);
#endif
					allInfo.nRoomLockInfo |= nLockFlag;
				}

				// 最后说话信息 
				if( dwNow < room.m_dwLastAudioTime + VOICE_ACTIVE_CHANNEL_TIME )
				{
					WORD nAudioFlag = (1 << i);
					allInfo.nAudioInfo |= nAudioFlag;
				}

				// 房间名称长度
				allInfo.nRoomNameLen[i] = strlen(room.GetName());
				if( allInfo.nRoomNameLen[i] > 0 )
				{
					obufName.push_back( room.GetName(), allInfo.nRoomNameLen[i] );
				}
			}

			allInfo.nOwnerNameLen = strlen(m_szOwnerName);

			obufData.push_back( &allInfo, sizeof(SMsgVoiceUpdateChannelAll_VC) );

			if( allInfo.nOwnerNameLen > 0 )
			{
				obufData.push_back( m_szOwnerName, allInfo.nOwnerNameLen );
			}

			// 压入名称
			if( obufName.size() > 0 )
			{
				obufData.push_back( obufName.data(), obufName.size() );
			}
		}
		break;

	case VoiceUpdateChannelType_Base:	// 频道基础信息
		{
			SMsgVoiceUpdateChannelBase_VC base;
			base.baseInfo = m_baseInfo;		// 基本信息
			
			base.nRoomLockInfo = 0;
			base.nAudioInfo = 0;
			DWORD dwNow = ::GetTickCount();
			for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
			{
				// 房间有锁
				if( GetRoom(i).GetFlag(VoiceRoomState_Lock) )
				{
					WORD nLockFlag = (1 << i);
#ifdef VOICE_PRINT_DEBUG_INF
					TraceLn("CChannel::UpdateChannelInfoToClient 房间[" << i << "]加锁信息=" << nLockFlag);
#endif
					base.nRoomLockInfo |= nLockFlag;
				}

				// 最后说话信息 
				if( dwNow >= GetRoom(i).m_dwLastAudioTime + VOICE_ACTIVE_CHANNEL_TIME )
				{
					continue;
				}

				WORD nAudioFlag = (1 << i);
				base.nAudioInfo |= nAudioFlag;
			}

			obufData.push_back( &base, sizeof(SMsgVoiceUpdateChannelBase_VC) );
		}
		break;

	case VoiceUpdateChannelType_Audio:	// 频道里房间是否有人语音信息
		{
			SMsgVoiceUpdateChannelAudio_VC audio;
			audio.nAudioInfo = 0;
			
			DWORD dwNow = ::GetTickCount();
			for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
			{
				if( dwNow >= GetRoom(i).m_dwLastAudioTime + VOICE_ACTIVE_CHANNEL_TIME )
				{
					continue;
				}

				WORD nAudioFlag = (1 << i);
				audio.nAudioInfo |= nAudioFlag;
			}

			obufData.push_back( &audio, sizeof(SMsgVoiceUpdateChannelAudio_VC) );
		}
		break;

	case VoiceUpdateChannelType_Name:
		{
			SMsgVoiceUpdateChannelName_VC nameData;
			sstrcpyn( nameData.szChannelName, m_szOwnerName, sizeof(nameData.szChannelName) );
			obufData.push_back( &nameData, sizeof(SMsgVoiceUpdateChannelName_VC) );
		}
		break;

	case VoiceUpdateChannelType_RoomName:
		{
			if( dwRoomID >= VoiceRoomType_Max )
			{
				return;
			}

			SMsgVoiceUpdateRoomName_VC roomName;
			roomName.nRoomID = (BYTE)dwRoomID;

			if( strlen(GetRoom(dwRoomID).GetName()) > 0 )
			{
				sstrcpyn( roomName.szRoomName, GetRoom(dwRoomID).GetName(), sizeof(roomName.szRoomName) );
			}
			else
			{
				memset( roomName.szRoomName, 0, sizeof(roomName.szRoomName) );
			}
			
			obufData.push_back( &roomName, sizeof(SMsgVoiceUpdateRoomName_VC) );
		}
		break;
	}

	if( 1 == wDesClientNum )
	{
		gGlobalServer->SendDataToClient( pDesClientArray[0], obufData.data(), obufData.size() );
	}
	else
	{
		gGlobalServer->BroadcastDataToClient( pDesClientArray, wDesClientNum, obufData.data(), obufData.size() );
	}
}

// 获取观察者ID信息
const SObserverListInfo & CChannel::GetObserverListInfo(void)
{
	return m_ObserverListInfo;
}

// 移动一个房间里的人到另外一个房间
bool CChannel::MoveUser( DWORD dwSrcRoomID, DWORD dwDesRoomID )
{
	if( dwSrcRoomID >= VoiceRoomType_Max || dwDesRoomID >= VoiceRoomType_Max )
	{
		return false;
	}

	// 取目的房间ID
	CRoom & srcRoom = GetRoom(dwSrcRoomID);
	LONGLONG nDesRoomID = GetRoom(dwDesRoomID).GetIDEx();

	// 取得房间里所有用户ID
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(srcRoom.Count());

	DWORD dwGetCounts = 0;
	srcRoom.GetUserIDList( pClientIDArray, dwBuffSize, dwGetCounts );
	
	if( dwGetCounts > 0 )
	{
		CClientList & clientList = CClientList::getInstance();
		for( DWORD i = 0; i < dwGetCounts; i ++ )
		{
			CClientUser * pUser = clientList.GetUser(pClientIDArray[i]);
			if( pUser != NULL )
			{
				// 强制进入房间
				pUser->EnterRoom( nDesRoomID, NULL, true );
			}
		}
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;

	return true;
}

// 踢出所有用户
void CChannel::KickUser(void)
{
	// 取得频道里所有用户ID
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(Count());

	DWORD dwCount = 0;
	GetClientIDList( pClientIDArray, dwBuffSize, dwCount );

	if( dwCount > 0 )
	{
		CClientList & clientList = CClientList::getInstance();
		for( DWORD i = 0; i < dwCount; i ++ )
		{
			CClientUser * pUser = clientList.GetUser(pClientIDArray[i]);
			if( pUser != NULL )
			{
				// GM跳过
				if( pUser->GetType() == VoiceUserType_GM )
				{
					continue;
				}

				// 进入空房间
				pUser->EnterRoom( 0, NULL );
			}
		}
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;
}


// 设置位状态标记
void CChannel::SetFlag( DWORD dwFlag, bool bSet, bool bSync )
{
	// 清除指定标识
	DWORD dwMyNewFlag = (m_baseInfo.dwStateFlag) & (~dwFlag);
	// 如果指定标识为true,设回状态
	if (bSet)
	{
		dwMyNewFlag =  dwMyNewFlag | dwFlag;
	}

	m_baseInfo.dwStateFlag = dwMyNewFlag;

	if( bSync )
	{
		OnDataChanged();
		SaveData();

		// 取得频道里所有用户ID
		// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
		__VOICE_GET_ID_BUFFER_START(Count());

		DWORD dwCount = 0;
		GetClientIDList( pClientIDArray, dwBuffSize, dwCount );

		if( dwCount > 0 )
		{
			UpdateChannelInfoToClient( pClientIDArray, dwCount, VoiceUpdateChannelType_Base );
		}

		// 是new的内存,用完释放
		// ID缓存分配结束
		__VOICE_GET_ID_BUFFER_END;
	}

}

// 获取位状态标记
bool CChannel::GetFlag( DWORD dwFlag )
{
	if ( ((m_baseInfo.dwStateFlag) & dwFlag) != 0 )
	{
		return true;
	}

	return false;
}


// 设置关闭信息
void CChannel::SetCloseInfo( const char * szCloseInfo, DWORD dwTimeDelay )
{
	if( szCloseInfo == NULL || strlen(szCloseInfo) <= 0 )
	{
		return ;
	}

	m_strCloseInfo = szCloseInfo;
	m_dwCloseTimeDelay = dwTimeDelay;
}

// 获得关闭文字信息
const char* CChannel::GetCloseMsg(void)
{
	return m_strCloseInfo.c_str();
}


// 发送聊天信息
void CChannel::SendChatMsg( const char * pMsg, DWORD dwRoomID, WORD nPos, WORD nTipsType, DWORD dwUserData1, DWORD dwUserData2, DWORD dwUserData3 )
{
	// 取得频道里所有用户ID
	DWORD dwMaxCount = 0;
	if( dwRoomID >= VoiceRoomType_Max )
	{
		dwMaxCount = Count();		// 取频道用户
	}
	else
	{
		dwMaxCount = GetRoom(dwRoomID).Count();		// 取房间用户
	}

	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(dwMaxCount);

	DWORD dwCount = 0;
	if( dwRoomID >= VoiceRoomType_Max )
	{
		GetClientIDList( pClientIDArray, dwBuffSize, dwCount );
	}
	else
	{
		GetRoom(dwRoomID).GetUserIDList( pClientIDArray, dwBuffSize, dwCount );
	}
	

	if( dwCount > 0 )
	{
		gGlobalServer->BroadcastChatMsgToClient( pClientIDArray, dwCount, nPos, nTipsType, pMsg, dwUserData1, dwUserData2, dwUserData3 );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;
}


// 启动频道关闭定时器
void CChannel::StartCloseTimer(void)
{
	// 最少要10秒钟
	if( m_dwCloseTimeDelay < 10 )
	{
		m_dwCloseTimeDelay = 10;
	}

	m_dwCloseTimeStart = (DWORD)time(NULL);

	// 启动踢人定时器
	gGlobalServer->SetTimer( ETimerEventID_Close, 1000, this, "CChannel::StartKickUserTimer" );
}

// 关闭频道关闭定时器
void CChannel::StopCloseTimer(void)
{
	gGlobalServer->KillTimer( ETimerEventID_Close, this );
}

// 设置频道拥有者名称
void CChannel::SetOwnerName( const char * szOwnerName, bool bSync, bool bSaveDB )
{
	if( szOwnerName == NULL || strlen(szOwnerName) <= 0 )
	{
		return;
	}

	// 如果名称和以前的不同才更新
	if( strcmp( m_szOwnerName, szOwnerName ) == 0 )
	{
		return;
	}

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn( "CChannel::SetOwnerName old=" << m_szOwnerName << ", new=" << szOwnerName );
#endif

	sstrcpyn( m_szOwnerName, szOwnerName, sizeof(m_szOwnerName) );

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn( "CChannel::SetOwnerName changed=" << m_szOwnerName << ", new=" << szOwnerName );
#endif

	if( bSaveDB )
	{
		OnDataChanged();
		SaveData();
	}
	
	if( bSync )	// 要更新到客户端
	{
		// 取得频道里所有用户ID
		// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
		__VOICE_GET_ID_BUFFER_START(Count());

		DWORD dwCount = 0;
		GetClientIDList( pClientIDArray, dwBuffSize, dwCount );

		if( dwCount > 0 )
		{
			UpdateChannelInfoToClient( pClientIDArray, dwCount, VoiceUpdateChannelType_Name );
		}

		// 是new的内存,用完释放
		// ID缓存分配结束
		__VOICE_GET_ID_BUFFER_END;
	}
}

// 获取频道拥有者名称
const char * CChannel::GetOwnerName(void)
{
	return m_szOwnerName;
}

// 添加马甲（临时管理）
bool CChannel::AddVest( CClientUser & user, DWORD nNewUserType, DWORD & dwError )
{
	SVoiceUserInfo & userInfo = user.GetUserInfo();
	DWORD dwClientID = user.GetClientID();
	SVoiceVestInfo * pVest = GetVest(userInfo.dwUDBID);
	DWORD dwUDBID = userInfo.dwUDBID;

	// 马甲不存在，需要新增一个
	if( NULL == pVest )
	{
		DWORD dwVestCount = GetChannelConfig(m_dwType).dwVestCount;
		if( m_vestList.size() >= dwVestCount )
		{
			dwError = VoiceAddVestError_Full;
			return false;
		}

		// 如果添加的比自身权限还小
		if( nNewUserType <= user.GetType() )
		{
			dwError = VoiceAddVestError_Exist;
			return false;
		}

		SVoiceVestInfo vest;
		vest.dwUDBID = dwUDBID;
		sstrcpyn( vest.szName, userInfo.szName, sizeof(vest.szName) );
		vest.nNewUserType = (BYTE)nNewUserType;
		m_vestList.insert( TMAP_VestList::value_type(dwUDBID, vest) );

		AddCurAdmin( dwClientID, true );
	}
	else	// 马甲存在
	{
		// 如果添加的比原来的马甲相等
		if( nNewUserType == pVest->nNewUserType )
		{
			dwError = VoiceAddVestError_Exist;
			return false;
		}

		pVest->nNewUserType = (BYTE)nNewUserType;
	}

	// 是否支持数据库存储,团队不需要存储数据
	if (IsSupportDB())
	{
		// 新增马甲权限用户信息
		DB_Param_VoiceVestInfo_Add dbParam;
		// 如找到为nChannelID,dwPDBID的记录就修改，否则新增
		dbParam.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		// 要保存的数据
		dbParam.nChannelID		= m_nID;			// 频道ID
		dbParam.dwPDBID			= dwUDBID;			// 用户角色ID
		dbParam.nUserType		= (BYTE)nNewUserType;		// 用户类型
		dbParam.dwFlag			= 0;				// 状态标识
		dbParam.dwSpare1		= 0;			// 备用1
		dbParam.dwSpare2		= 0;			// 备用2
		dbParam.dwSpare3		= 0;			// 备用3
		sstrcpyn( dbParam.szName, userInfo.szName, sizeof(dbParam.szName) );
		// 不用处理返回结果
		CDBRequester::getInstance().DB_VoiceVestInfo_Add(dbParam);
	}

	user.SetVestUserType( (BYTE)nNewUserType, true, true );

	CRoom * pRoom = user.GetCurRoom();
	if( NULL == pRoom )
	{
		dwError = VoiceAddVestError_None;
		return true;
	}

    // 同步给频道观察者
    // BroadcastDataToObserver(ob.data(), ob.size());

	dwError = VoiceAddVestError_None;
	return true;
}

// 修改马甲
bool CChannel::EditVest( DWORD dwUDBID, DWORD nNewUserType, DWORD & dwError )
{
	SVoiceVestInfo * pVest = GetVest(dwUDBID);
	if( pVest == NULL )
	{
		dwError = VoiceAddVestError_Normal;
		return false;
	}

	pVest->nNewUserType = (BYTE)nNewUserType;

	// 是否支持数据库存储,团队不需要存储数据
	if (IsSupportDB())
	{
		// 新增马甲权限用户信息
		DB_Param_VoiceVestInfo_Add dbParam;
		// 如找到为nChannelID,dwPDBID的记录就修改，否则新增
		dbParam.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		// 要保存的数据
		dbParam.nChannelID		= m_nID;			// 频道ID
		dbParam.dwPDBID			= dwUDBID;			// 用户角色ID
		dbParam.nUserType		= (BYTE)nNewUserType;		// 用户类型
		dbParam.nSex			= pVest->nSex;	// 性别
		dbParam.dwFlag			= 0;				// 状态标识
		dbParam.dwSpare1		= 0;			// 备用1
		dbParam.dwSpare2		= 0;			// 备用2
		dbParam.dwSpare3		= 0;			// 备用3
		sstrcpyn( dbParam.szName, pVest->szName, sizeof(dbParam.szName) );
		// 不用处理返回结果
		CDBRequester::getInstance().DB_VoiceVestInfo_Add(dbParam);
	}

	return true;
}

// 删除马甲（临时管理）
bool CChannel::DelVest( DWORD dwUDBID, CClientUser * pUser )
{
	// 找到是否有记录
	TMAP_VestList::iterator iter = m_vestList.find(dwUDBID);
	if (iter!=m_vestList.end())
	{
		m_vestList.erase(iter);	// 删除记录

		// 是否支持数据库存储,团队不需要存储数据
		if (IsSupportDB())
		{
			// 删除马甲权限用户信息
			DB_Param_VoiceVestInfo_Del dbParam;
			// 如找到为nChannelID,dwPDBID的记录就修改，否则新增
			dbParam.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
			// 要保存的数据
			dbParam.nChannelID		= m_nID;			// 频道ID
			dbParam.dwPDBID			= dwUDBID;			// 用户角色ID
			// 不用处理返回结果
			CDBRequester::getInstance().DB_VoiceVestInfo_Del(dbParam);
		}
	}

	if( pUser != NULL )
	{
		DWORD dwClientID = pUser->GetClientID();
		AddCurAdmin( dwClientID, false );

		CRoom * pRoom = pUser->GetCurRoom();
		if( pRoom == NULL )
		{
			return true;
		}

		// 如果被删除的用户不是本频道的，不更新用户信息
		if( pRoom->GetChannel().GetID() != m_nID )
		{
			return true;
		}

		pUser->SetVestUserType( 0, false, true );

        // 同步给频道观察者
        // BroadcastDataToObserver(ob.data(), ob.size());
	}

	return true;
}

// 获取马甲（临时管理）
SVoiceVestInfo * CChannel::GetVest( DWORD dwUDBID )
{
	TMAP_VestList::iterator it = m_vestList.find(dwUDBID);
	// 没有找到
	if( it == m_vestList.end() )
	{
		return NULL;
	}
	
	return &(it->second);
}


// 获取马甲（临时管理）列表
void CChannel::UpdateChannelVestListToClient( DWORD dwClientID, BYTE nUpdateType )
{
#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("CChannel::UpdateChannelVestListToClient nUpdateType=" << nUpdateType);
#endif

	switch(nUpdateType)
	{
	case VoiceUpdateType_All:
		{
			// 使用全局缓冲内存
			GlobalVoiceServer* pVoiceServer = (GlobalVoiceServer*)gGlobalServer;
			ofixbuf obufData(pVoiceServer->m_szDataBuf, sizeof(pVoiceServer->m_szDataBuf));
			ofixbuf obufName(pVoiceServer->m_szNameBuf, sizeof(pVoiceServer->m_szNameBuf));

			// 组个头
			SGameMsgHead header;
			header.SrcEndPoint = MSG_ENDPOINT_VOICE;
			header.DestEndPoint= MSG_ENDPOINT_CLIENT;
			header.byKeyModule  = MSG_MODULEID_VOICE;
			header.byKeyAction  = MSG_VOICE_VESTLIST;			// 马甲（临时管理）列表
			obufData.push_back( &header, sizeof(SGameMsgHead) );

			SVoiceVestList_VC listData;
			listData.nUpdateType = nUpdateType;
			listData.nCount = m_vestList.size();
			obufData.push_back( &listData, sizeof(SVoiceVestList_VC) );

			for( TMAP_VestList::iterator it = m_vestList.begin(); it != m_vestList.end(); ++it  )
			{
				SVoiceVestInfo & vestInfo = it->second;

				SVoiceVestNode_VC nodeData;
				nodeData.dwUDBID = vestInfo.dwUDBID;					// PDBID
				nodeData.nSex = vestInfo.nSex;
				nodeData.nNewUserType = vestInfo.nNewUserType;			// 用户类型	
				nodeData.nNameLen = strlen(vestInfo.szName);			// 名称长度

				obufData.push_back( &nodeData, sizeof(SVoiceVestNode_VC) );

				if( nodeData.nNameLen > 0 )	// 名称
				{
					obufName.push_back( vestInfo.szName, nodeData.nNameLen );
				}
			}

			obufData.push_back( obufName.data(), obufName.size() );

			// 发送
			gGlobalServer->SendDataToClient( dwClientID, obufData.data(), obufData.size() );
		}
		break;

	case VoiceUpdateType_Add:
	case VoiceUpdateType_Edit:
		{
			
		}
		break;
	}
}

// 是否可以参加个人排行榜
bool CChannel::CanInsertPersonalTopList(void)
{
	//if( m_dwType == VoiceChannel_Personal )
	//{
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}

	if( m_dwType != VoiceChannel_Personal )		// 不是个人频道
	{
		return false;
	}
	else if( !GetFlag(VoiceChannelState_Public) )	// 频道未公开
	{
		return false;
	}
	else if( strlen(m_szOwnerName) <= 0 )	// 没有名称
	{
		return false;
	}
	//else if( m_dwWorldID <= 0 )	// 世界ID为空也不参与排行
	//{
	//	return false;
	//}

	bool bValid = false;
	DWORD dwUserCount = Count();
	DWORD dwTimeNow = (DWORD)time(NULL);

	// 总人数没有超过6个，需要判断历史在线是否超过20个
	if( dwUserCount >= 1 )
	{
		m_dwTopListTime = dwTimeNow;
		bValid = true;
	}
	/*
	else if( m_dwMaxMemberCount >= 20 && dwUserCount >= 2 )	// 如果历史在线超过20，且人数达到2人
	{
		m_dwTopListTime = dwTimeNow;
		bValid = true;
	}
	else if( (dwTimeNow <= m_dwTopListTime + 1800) && dwUserCount >= 1 )	// 如果半个小时内有上榜，且人数满足1人
	{
		bValid = true;
	}
	*/

	return bValid;
}


// 获取历史最高在线人数
DWORD CChannel::GetMaxMemberCount(void)
{
	return m_dwMaxMemberCount;
}

// 设置频道拥有者信息
void CChannel::SetOwnerInfo( DWORD dwUDBID, DWORD dwWorldID, DWORD dwCountryID, bool bSaveDB )
{
#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("CChannel::SetOwnerInfo 传入参数 dwUDBID=" << dwUDBID << ",dwWorldID=" << dwWorldID << ",dwCountryID=" << dwCountryID );
	TraceLn("CChannel::SetOwnerInfo 原有值 dwUDBID=" << m_baseInfo.dwOwnerUDBID << ",dwWorldID=" << m_dwWorldID << ",dwCountryID=" << m_dwCountryID );
#endif

	// 不一致才修改
	bool bChange = false;
	if( m_baseInfo.dwOwnerUDBID != dwUDBID )
	{
		m_baseInfo.dwOwnerUDBID = dwUDBID;
		bChange = true;
	}

	if( m_dwCountryID != dwCountryID && dwCountryID != 0 )
	{
		m_dwCountryID = dwCountryID;
		bChange = true;
	}

	if( m_dwWorldID != dwWorldID && dwWorldID != 0 )
	{
		m_dwWorldID = dwWorldID;
		bChange = true;
	}

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("CChannel::SetOwnerInfo 修改后 dwUDBID=" << m_baseInfo.dwOwnerUDBID << ",dwWorldID=" << m_dwWorldID << ",dwCountryID=" << m_dwCountryID );
#endif

	if( !bChange )
	{
		return;
	}
	
	if( bSaveDB )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CChannel::SetOwnerInfo 保存数据库" );
#endif
		OnDataChanged();
		SaveData();
	}
}

// 获取频道拥有者世界信息
void CChannel::GetOwnerWorldInfo( DWORD & dwWorldID, DWORD & dwCountryID )
{
	dwWorldID = m_dwWorldID;
	dwCountryID = m_dwCountryID;
}

// 语音检测
void CChannel::OnAudioCheck( DWORD dwNow, DWORD dwAudioRoomID )
{
	// 激活过后，15分钟内不再激活
	if( dwNow < m_dwLastActiveTime + 900000 )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CChannel::OnAudioCheck dwNow < m_dwLastActiveTime + 900000");
#endif
		return;
	}

	// 没有激活过，在定时器关闭5分钟内，不再激活
	if( dwNow < m_dwActiveTimerEndTime + 300000 )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CChannel::OnAudioCheck dwNow < m_dwActiveTimerEndTime + 300000");
#endif
		return;
	}

	if( !m_bActiveTimerStart )
	{
		m_bActiveTimerStart = true;

		m_dwActiveTimerStartTime = dwNow;
		m_dwActiveTimerCheckTime = dwNow;
		m_dwAudioCount = 0;
		m_dwActiveRoomID = dwAudioRoomID;
		gGlobalServer->SetTimer( ETimerEventID_Active, 5000, this, "CChannel::OnAudioCheck" );
	}
#ifdef VOICE_PRINT_DEBUG_INF
	else
	{
		TraceLn("CChannel::OnAudioCheck m_bActiveTimerStart is true");
	}
#endif
}

void CChannel::OnActiveChange(void)
{
	OnPush(m_bActive);
}

// 频道是否有推送
bool CChannel::IsPushed(void)
{
	return m_bPushed;
}

// 推送
void CChannel::OnPush( bool bActive )
{
	if( !GetChannelConfig(m_dwType).bPush )	// 不需要推送的频道
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CChannel::OnPush 不需要推送的频道");
#endif
		m_bPushed = false;
		return;
	}

	if( GetRoomConfig(m_dwType, m_dwActiveRoomID).nPush != 1 )	// 不需要推送的房间
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CChannel::OnPush 不需要推送的房间");
#endif

		m_bPushed = false;
		return;
	}

	m_bPushed = bActive;

	SendVoiceTipsToClient(bActive);
}

// 发送语音提醒到客户端
void CChannel::SendVoiceTipsToClient( bool bShow )
{
	// 先获取所有有权限进入此频道的人
	CClientList & clientList = CClientList::getInstance();

	DWORD * pClientIDArray = ((GlobalVoiceServer *)gGlobalServer)->m_dwClientIDBuff;

	// 得到观察者列表
	DWORD dwCount = 0;
	clientList.GetClientIDList( pClientIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount, m_dwType, m_nID );
#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("CChannel::SendVoiceTipsToClient dwCount=" << dwCount << "bShow=" << bShow );
#endif
	if( dwCount > 0 )
	{
		LONGLONG nID = m_nID;
		SVoiceChannelID * pIDInfo = (SVoiceChannelID*)(&nID);
		pIDInfo->dwRoomID = m_dwActiveRoomID;
		gGlobalServer->BroadcastVoiceDataToClient( pClientIDArray, dwCount, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_ChannelAudio, bShow ? 1 : 0, 0, 0, (char*)(&nID), sizeof(LONGLONG) );
	}
}

// 是否支持数据库存储,团队不需要存储数据
bool CChannel::IsSupportDB()
{
	if (VoiceChannel_Group==m_dwType||VoiceChannel_Team==m_dwType||VoiceChannel_War==m_dwType)
	{
		return false;
	}
	return true;
}



// 数据是否加载完成
bool CChannel::IsLoaded(void)
{
	return m_bLoaded;
}

// 数据变更了,定时或调用SaveData() 时会触发保存
void CChannel::OnDataChanged()
{
	if( !IsSupportDB() )
	{
		return;
	}
	// 是否已保存数据
	m_bSaved = false;
}



// 冷却请求保存数据
bool CChannel::CanSaveData()
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
			TraceLn( "CChannel::CanSaveData m_saveCoolInfo.bOnTimer m_saveCoolInfo.nCounts <= 0 无排队的请求" );
#endif
			return false;
		}
	}

	// 请求次数加1
	m_saveCoolInfo.nCounts++;

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn( "CChannel::CanSaveData m_saveCoolInfo.nCounts=" << m_saveCoolInfo.nCounts );
#endif

	// 能否请求
	if ( (dwTickCount - m_saveCoolInfo.dwLastTimes< VOICE_COOL_SAVE_TIMES ) && (m_saveCoolInfo.nCounts< VOICE_COOL_SAVE_COUNTS ) )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn( "CChannel::CanSaveData 时间冷却中" );
#endif
		return false;
	}
	// 新请求
	m_saveCoolInfo.nCounts		= 0;
	m_saveCoolInfo.dwLastTimes	= dwTickCount;

	return true;
}


// 请求保存数据,有缓冲机制,数据变化都调用 OnDataChanged()
bool CChannel::SaveData(void)
{
	// 不用保存
	if( !IsSupportDB() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn( "CChannel::SaveData 不支持数据库 返回" );
#endif
		return false;
	}
	// 检查能否保存数据
	if( ! CanSaveData() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn( "CChannel::SaveData 不需要保存数据 返回" );
#endif
		return false;
	}

	// 社会服务器保存数据到数据库
	SaveDataToDB();

	return true;
}

// 定时检查是否要保存数据
void CChannel::OnTimerCheckSaveDB(void)
{
	m_saveCoolInfo.bOnTimer = true;
	SaveData();
	m_saveCoolInfo.bOnTimer = false;

	// 遍历房间，保存数据
	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	{
		m_roomList[i].OnTimerCheckSaveDB();
	}
}

// 数据是否已经保存
bool CChannel::IsDBSaved(void)
{
	if( !IsSupportDB() )
	{
		return true;
	}

	// 遍历房间，保存数据
	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	{
		// 如果有一个房间没有保存，直接返回false
		if( !m_roomList[i].IsDBSaved() )
		{
			return false;
		}
	}

	return m_bSaved;
}

// 设置运行位状态标记
void CChannel::SetRunFlag( DWORD dwFlag, bool bSet )
{
	// 清除指定标识
	DWORD dwMyNewFlag = (m_dwRunFlag) & (~dwFlag);
	// 如果指定标识为true,设回状态
	if (bSet)
	{
		dwMyNewFlag =  dwMyNewFlag | dwFlag;
	}

	m_dwRunFlag = dwMyNewFlag;
}

// 获取运行位状态标记
bool CChannel::GetRunFlag( DWORD dwFlag )
{
	if ( (m_dwRunFlag & dwFlag) != 0 )
	{
		return true;
	}

	return false;
}


// 获取频道人数上限
DWORD CChannel::GetMaxCount(void)
{
	return m_dwMaxCount;
}

// 设置频道人数上限
void CChannel::SetMaxCount( DWORD dwCount )
{
	m_dwMaxCount = dwCount;
}

// 设置频道语音品质类型  EMVoiceQualityType
bool CChannel::SetQuality( DWORD dwQuality )
{
	DWORD dwNewQuality = dwQuality;	
	// 默认或无效用服务器定义
	if (dwNewQuality>VoiceQuality_High)
	{
		// 要设置的新语音品质类型
		DWORD dwDefaultQuality = VoiceServerSetting::getInstance().m_dwQualityType;	//  默认语音品质

		// 无效的默认类型就用普通语音品质
		if (dwDefaultQuality<VoiceQuality_Low || dwDefaultQuality>VoiceQuality_High)
		{
			dwDefaultQuality = VoiceQuality_Normal;
		}

		dwNewQuality = dwDefaultQuality;
	}

	// 没变化
	if (dwNewQuality==m_baseInfo.dwQuality)
	{
		return false;
	}
	m_baseInfo.dwQuality = dwNewQuality;

	OnDataChanged();
	SaveData();

	// 取得频道里所有用户ID
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(Count());

	DWORD dwCount = 0;
	GetClientIDList( pClientIDArray, dwBuffSize, dwCount );

	if( dwCount > 0 )
	{
		UpdateChannelInfoToClient( pClientIDArray, dwCount, VoiceUpdateChannelType_Base );
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;

	return true;
}


// 频道是否可进
bool CChannel::CanChannelEnter( DWORD & dwResult )
{
	if( GetFlag(VoiceChannelState_Close) )		// 频道已关闭
	{
		dwResult = 1;
		return false;
	}
	else if( GetRunFlag(VoiceChannelRunFlag_Saving) )	 // 频道正在保存数据中
	{
		dwResult = 2;
		return false;
	}
	else if( Count() > m_dwMaxCount )	// 人数已达上限
	{
		dwResult = 3;
		return false;
	}

	return true;
}

// 获取频道基本信息
SVoiceChannelBaseInfo & CChannel::GetBaseInfo(void)
{
	return m_baseInfo;
}

// 更新管理员进入房间信息给其他管理员
void CChannel::UpdateAdminEnterInfoToOther( CClientUser & admin, DWORD dwRoomID, bool bEnter )
{
	DWORD dwCurAdminCount = GetCurAdminCount();
	if( dwCurAdminCount <= 0 )
	{
		return;
	}

	SVoiceUserInfo & userInfo = admin.GetUserInfo();

	obuf obufData;
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_ADMINENTERINFO;
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	SMsgVoiceAdminEnterInfo_VC enterInfo;
	enterInfo.bEnter = bEnter;		// 是否进入 true进，false 出
	enterInfo.dwClientID = admin.GetClientID();	// 客户端ID
	enterInfo.nWorldID = (WORD)userInfo.dwFromWorldID;	// 来源世界ID
	enterInfo.dwPDBID = userInfo.dwPDBID;
	enterInfo.nRoomID = (BYTE)dwRoomID;		// 进出的房间ID
	enterInfo.nNameLen = strlen(userInfo.szName);		// 角色名称
	obufData.push_back( &enterInfo, sizeof(SMsgVoiceAdminEnterInfo_VC) );

	if( enterInfo.nNameLen > 0 )
	{
		obufData.push_back( userInfo.szName, enterInfo.nNameLen );
	}

	// 取得所有房间观察者数量
	// ID缓存分配开始，在后续代码中必须使用 pClientIDArray  dwBuffSize 访问 必须要与__VOICE_GET_ID_BUFFER_END配对出现，否则会导致内存泄漏
	__VOICE_GET_ID_BUFFER_START(dwCurAdminCount);

	// 得到观察者列表
	DWORD dwCount = 0;
	GetCurAdminIDList( pClientIDArray, dwBuffSize, dwCount, enterInfo.dwClientID );

	if( dwCount > 0 )
	{
		// 发送
		if( 1 == dwCount )
		{
			gGlobalServer->SendDataToClient( pClientIDArray[0], obufData.data(), obufData.size() );
		}
		else
		{
			gGlobalServer->BroadcastDataToClient( pClientIDArray, dwCount, obufData.data(), obufData.size() );
		}
	}

	// 是new的内存,用完释放
	// ID缓存分配结束
	__VOICE_GET_ID_BUFFER_END;
}




// 保存数据到数据库,只能SaveData()内部调用,其他地方不要调用
void CChannel::SaveDataToDB(void)
{
	// 没有保存数据的可设为m_bSaved为true,否则停止服务器投票事件通不过导致不能关闭
	// 有存数据库的在数据请求返回时设为m_bSaved为true

	// 如果已保存数据或没加载数据，返回
	if (m_bSaved || ! m_bLoaded)
	{
		m_bSaved =true;
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn( "CChannel::SaveDataToDB 已保存数据或没加载数据，返回" );
#endif
		return;
	}

	// 保存到数据库

	if( !IsSupportDB() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn( "CChannel::SaveDataToDB 不支持数据，返回" );
#endif
		return;
	}

	// 保存语音频道信息
	DB_Param_VoiceChannel_Save dbParam;
	// 条件参数,找到nChannelID的记录来修改，否则不修改
	dbParam.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
	dbParam.bIsNew			= false;					// 是否是新增,如是新增，如已有记录就失败不操作，以免覆盖,如是修改，没有记录时可加一条
	// 要保存的数据
	dbParam.nChannelID		= m_nID;					// 频道ID
	dbParam.dwType			= m_IDInfo.dwType;			// 频道类型
	dbParam.dwWorldID		= m_IDInfo.dwWorldID;		// 世界ID
	dbParam.dwDataID		= m_IDInfo.dwID;			// 频道数值ID
	dbParam.dwOwnerUDBID	= m_baseInfo.dwOwnerUDBID;	// 主人通行证UDBID
	dbParam.dwMaxOnline		= m_dwMaxMemberCount;		// 最高在线
	dbParam.dwQuality		= m_baseInfo.dwQuality;		// 语音品质
	dbParam.dwRoomID		= m_baseInfo.dwDefautRoomID;// 默认进入的房间ID
	dbParam.dwFlag			= m_baseInfo.dwStateFlag;	// 状态标识
	dbParam.dwUseType		= m_baseInfo.dwUseType;		// 应用类型
	dbParam.dwSpare1		= m_baseInfo.dwSpare1;		// 备用1
	dbParam.dwSpare2		= m_baseInfo.dwSpare2;		// 备用2
	dbParam.dwSpare3		= m_baseInfo.dwSpare3;		// 备用3
	sstrcpyn( dbParam.szName, m_szOwnerName, sizeof(dbParam.szName) );		// 频道名称，默认情况下是拥有者名称

	CDBRequester::getInstance().DB_VoiceChannel_Save(dbParam,static_cast<IDBRetSink *>(this));

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn( "CChannel::SaveDataToDB 已提交数据库" );
#endif

}







//////////////////////////////////////////////////////////////////////////
// 分配ObserverList内存，返回true表示重新分配，
bool CChannel::GetObserverListBuff(void)
{
	DWORD dwObserverCount = 0;
	for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
	{
		dwObserverCount += m_ChannelObserver[i].size();
	}
	
	DWORD dwNewBuffSize = m_ObserverListInfo.dwSize;

	bool bReSize = false;
	// 要扩容
	if( dwObserverCount >= m_ObserverListInfo.dwSize*0.95 )
	{
		dwNewBuffSize *= 2;
		bReSize = true;
	}
	// 缩小
	else if( VOICE_ROOM_MIN_BUFF_SIZE+dwObserverCount < m_ObserverListInfo.dwSize*0.3 )
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
	//memcpy( pNewBuf, m_ObserverList.pData, m_ObserverList.dwCount*sizeof(DWORD) );

	delete [] m_ObserverListInfo.pData;

	// 存新的
	m_ObserverListInfo.pData = pNewBuf;
	m_ObserverListInfo.dwSize = dwNewBuffSize;

	return true;
}

// 是否能销毁 dwDestroyTime多少分钟后销毁
bool CChannel::CanDestroy( DWORD & dwDestroyTime )
{
	// 如果频道里有人，什么都不处理
	if( Count() > 0 )
	{
		dwDestroyTime = 0;
		return false;
	}
	
	// 默认都是5分钟
	dwDestroyTime = 5;
	bool bRes = false;
	switch(m_dwType)
	{
	case VoiceChannel_Team:
	case VoiceChannel_Group:
		bRes = true;
		break;

	case VoiceChannel_Personal:
		dwDestroyTime = 60;
		bRes = true;
		break;

	default:
		break;
	}

	return bRes;
}

// 添加管理员到当前频道在线管理员列表，方便查询使用
bool CChannel::AddCurAdmin( DWORD dwClientID, bool bAdd )
{
	if( dwClientID <= 0 )
	{
		return false;
	}

	if( bAdd )
	{
		m_CurAdminList[dwClientID] = dwClientID;
	}
	else
	{
		m_CurAdminList.erase(dwClientID);
	}

	return true;
}

// 获取当前频道所有在线管理列表个数
DWORD CChannel::GetCurAdminCount(void)
{
	return m_CurAdminList.size();
}