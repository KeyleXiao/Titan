/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ClientUser.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2012-11-21
** 版  本:	1.0
** 描  述:	语音客户端用户
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ClientUser.h"
#include "ClientList.h"
#include <strstream>
using namespace std;
#include "Winsock2.h"
#include "GlobalVoiceServer.h"
#include "VoiceCallList.h"

CClientUser::CClientUser()
	: m_bLogin(false)		// 是否验证
	, m_nKeyLen(0)			// 密钥长度
	, m_dwClientID(0)		// 客户端ID	
	, m_dwWorldID(0)		/// 世界ID
	, m_dwServerID(0)		/// 所属社会服务器ID
	, m_dwKickReason(0)		// 踢出理由
	, m_bOnline(false)		// 是否在线
	, m_dwState(0)			// 用户网络状态 EMVoiceUserState
	, m_bCanSpeak(false)		// 是否可以说话
	, m_bBroadcast(false)	// 语音数据是否频道广播
	, m_dwConnectTime((DWORD)time(NULL))	// 连入时间
	, m_dwRemoteIP(0)		// 该客户端IP
	, m_bNeedOpenMic(false)
	, m_dwLastAudioTime(0)
	, m_dwLastChatChannel(0)
	, m_dwLastChatTime(0)
	, m_dwLastUpdateFansTime(0)	// 上一次更新粉丝数字时间
	, m_bLoaded(false)
	, m_bSaved(false)
	, m_dwLastCallFansTime(0)
	, m_dwFansAcceptCount(0)
	, m_dwFansCancelCount(0)
	, m_dwFansCountYesterday(0)
    , m_nUpdateFlag(0)
    , m_nRoomID(0)
{
	memset( m_pKeyData, 0, sizeof(m_pKeyData) );
	memset( m_dwLastADTime, 0, sizeof(m_dwLastADTime) );

	CreateKeyData();
}


CClientUser::~CClientUser(void)
{

}

// 定时器
void CClientUser::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_LoginTimeout:
		{
			// 关闭定时器
			if( !m_bLogin )
			{
				// 登录超时了要踢掉用户
				SendChatMsg("连接语音系统超时！", VoiceInfoPos_ActorRight|VoiceInfoPos_ChatWindow, VoiceTipType_Operate, 1);
				gGlobalServer->KickUser(this, VoiceKickReason_LoginTimeout);
			}
		}
		break;

	case ETimerEventID_Kick:
		{
			// 通知网关
			gGlobalServer->SendVoiceDataToGateway( GetGatewayID(), MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_KickUser, m_dwClientID, m_dwKickReason );
		}
		break;
	}
}


//数据库请求返回回调方法
void CClientUser::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	switch(nCmdID)
	{
	case VOICEDB_REQUEST_VOICEUSER_LOGIN:		// 语音角色用户登录
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CClientUser::OnReturn() 语音角色用户登录 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			if(pOutData == NULL || nOutLen < sizeof(DB_Result_VoiceUser_Login))
			{
				ErrorLn("语音角色用户登录数据大小非法，nOutLen = "<<nOutLen);
				return;	
			}
			DB_Result_VoiceUser_Login * pResult = (DB_Result_VoiceUser_Login *)(pOutData);

			if(nOutLen != pResult->nRecordCount * sizeof(DB_Table_VoiceUser) + sizeof(DB_Result_VoiceUser_Login))
			{
				ErrorLn("语音角色用户登录数据条数与大小不符，RecordCount="<<pResult->nRecordCount<<",nOutLen = "<<nOutLen);
				return;
			}

			if (pResult->nRecordCount>0)
			{
				DB_Table_VoiceUser * pDBUser = (DB_Table_VoiceUser *)(pOutData + sizeof(DB_Result_VoiceUser_Login));

				// 更新数据
				m_UserInfo.context.dwFansCount	= pDBUser->dwFansCount;		// 所有粉丝数，在加粉丝时+1,删除时-1,这样可提高登录的性能
				m_UserInfo.context.dwDBUserType	= pDBUser->dwUserType;		// DB用户类型
				m_dwFansCountYesterday = pDBUser->dwOldFansCount;	// 昨日粉丝数

                UpdateUserInfoToClient(true, 3, VOICE_FIELD_DBUSER_TYPE, VOICE_FIELD_FANSCOUNT, VOICE_FIELD_FANSCOUNT_YESTERDAY);
			}
			// 是否已加载数据
			m_bLoaded=true;

		}
		break;

	case VOICEDB_REQUEST_VOICEFANS_READ:		// 读取粉丝关系信息
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CClientUser::OnReturn() 读取粉丝关系信息 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			if(pOutData == NULL || nOutLen < sizeof(DB_Result_VoiceFansUser_Read))
			{
				ErrorLn("读取粉丝关系信息 数据大小非法，nOutLen = "<<nOutLen);
				return;	
			}
			DB_Result_VoiceFansUser_Read * pResult = (DB_Result_VoiceFansUser_Read *)(pOutData);

			if(nOutLen != pResult->nRecordCount * sizeof(DWORD) + sizeof(DB_Result_VoiceFansUser_Read))
			{
				ErrorLn("读取粉丝关系信息 数据条数与大小不符，RecordCount="<<pResult->nRecordCount<<",nOutLen = "<<nOutLen);
				return;
			}

			// 清空粉丝关系表
			m_mapFansUser.clear();

			DWORD * pUDBID = (DWORD *)(pOutData + sizeof(DB_Result_VoiceFansUser_Read));

			for (int i=0;i<pResult->nRecordCount;i++)
			{
				DWORD dwUDBID = *pUDBID;
				// 存入粉丝关系列表中
				m_mapFansUser.insert(TMAP_VFansUser::value_type(dwUDBID, dwUDBID));

#ifdef VOICE_PRINT_DEBUG_INF
				// 调式代码
				sprintf_s(szBuf, _countof(szBuf),"%d.%d",i,dwUDBID);
				TraceLn(szBuf);
#endif
				pUDBID++;
			}

			OnLoadDBFansList();
		}
		break;

	case VOICEDB_REQUEST_VOICEFANS_ADD:		// 新增粉丝关系
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CClientUser::OnReturn() 新增粉丝关系 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			if(pOutData == NULL || nOutLen != sizeof(DB_Result_VoiceFansUser_Add))
			{
				ErrorLn("语音角色新增粉丝关系非法，nOutLen = "<<nOutLen);
				return;	
			}
			DB_Result_VoiceFansUser_Add * pResult = (DB_Result_VoiceFansUser_Add *)(pOutData);

			SetFansCountYesterday( pResult->OldFansCount );	// 昨天粉丝总数
		}
		break;

	case VOICEDB_REQUEST_VOICEFANS_DEL:		//  删除粉丝关系
		{

#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CClientUser::OnReturn() 删除粉丝关系 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif

			if(pOutData == NULL || nOutLen != sizeof(DB_Result_VoiceFansUser_Del))
			{
				ErrorLn("语音角色删除粉丝关系非法，nOutLen = "<<nOutLen);
				return;	
			}
			DB_Result_VoiceFansUser_Del * pResult = (DB_Result_VoiceFansUser_Del *)(pOutData);

			SetFansCountYesterday( pResult->OldFansCount );	// 昨天粉丝总数
		}
		break;

	case VOICEDB_REQUEST_VOICENUMVALUE_READ:	// 读取蓝钻信息
		{
#ifdef VOICE_PRINT_DEBUG_INF
			// 调式代码
			char szBuf[512]={0};
			sprintf_s(szBuf, _countof(szBuf),"CClientUser::OnReturn() 读取蓝钻信息 nCmdID=%d,nDBRetCode=%d,nQueueIndex=%d,nOutLen=%d",
				nCmdID,nDBRetCode,nQueueIndex,nOutLen);
			TraceLn(szBuf);
#endif
			if(pOutData == NULL || nOutLen < sizeof(DB_Result_VoiceNumValue_Read))
			{
				ErrorLn("CClientUser::OnReturn() 读取蓝钻信息 数据大小非法，nOutLen = "<<nOutLen);
				return;	
			}

			DB_Result_VoiceNumValue_Read * pResult = (DB_Result_VoiceNumValue_Read *)(pOutData);

			if( nOutLen != pResult->nRecordCount * sizeof(DB_Table_VoiceNumValue) + sizeof(DB_Result_VoiceNumValue_Read) )
			{
				ErrorLn("CClientUser::OnReturn() 读取蓝钻信息 数据条数与大小不符，RecordCount="<<pResult->nRecordCount<<",nOutLen = "<<nOutLen);
				return;
			}

			if ( pResult->nRecordCount > 0 )
			{
				DB_Table_VoiceNumValue * pNumValue = (DB_Table_VoiceNumValue *)(pOutData + sizeof(DB_Result_VoiceNumValue_Read));
#ifdef VOICE_PRINT_DEBUG_INF
				char szBuf[512]={0};
				sprintf_s(szBuf, _countof(szBuf),"CClientUser::OnReturn() 读取蓝钻信息 dwUDBID=%d,蓝钻总数=%d,当前蓝钻数=%d,当月之前的总数=%d,今日获得蓝钻数=%d, 红钻总=%d",
					pNumValue->dwUDBID, pNumValue->dwBlueDiamond, pNumValue->dwCurBlueDiamond, pNumValue->dwBlueDiamondOld, pNumValue->dwBlueDiamondToday, pNumValue->dwRedDiamond );
				TraceLn(szBuf);
#endif
// 				m_DiamondInfo.dwUDBID = pNumValue->dwUDBID;				// 通行证UDBID,唯一
// 				m_DiamondInfo.dwBlueDiamond = pNumValue->dwBlueDiamond;		// 蓝钻总数，一直累加
// 				m_DiamondInfo.dwCurBlueDiamond = pNumValue->dwCurBlueDiamond;		// 当前蓝钻数，只有这个数值才可结算成红钻
// 				m_DiamondInfo.dwBlueDiamondOld = pNumValue->dwBlueDiamondOld;		// 当月之前的总数
// 				m_DiamondInfo.dwBlueDiamondToday = pNumValue->dwBlueDiamondToday;	// 今日获得蓝钻数，第二天自动把今天的这个值添加到dwCurBlueCount中，然后清零
// 				m_DiamondInfo.dwRedDiamond = pNumValue->dwRedDiamond;			// 红钻总数，一直累加，这部分可结算成金币/RMB
			}
			else
			{
// 				m_DiamondInfo.dwUDBID = 0;				// 通行证UDBID,唯一
// 				m_DiamondInfo.dwBlueDiamond = 0;		// 蓝钻总数，一直累加
// 				m_DiamondInfo.dwCurBlueDiamond = 0;		// 当前蓝钻数，只有这个数值才可结算成红钻
// 				m_DiamondInfo.dwBlueDiamondOld = 0;		// 当月之前的总数
// 				m_DiamondInfo.dwBlueDiamondToday = 0;	// 今日获得蓝钻数，第二天自动把今天的这个值添加到dwCurBlueCount中，然后清零
// 				m_DiamondInfo.dwRedDiamond = 0;			// 红钻总数，一直累加，这部分可结算成金币/RMB

#ifdef VOICE_PRINT_DEBUG_INF
				TraceLn("CClientUser::OnReturn() 读取蓝钻信息 无数据返回");
#endif
			}

			// 更新到客户端
			// UpdateDiamondInfoToClient();
		}
		break;

	default:
		break;
	}
}

// 设置IP
void CClientUser::SetClientIP( DWORD dwIP )
{
	m_dwRemoteIP = dwIP;
}

const char *CClientUser::GetClientIP()
{
	in_addr addr;
	addr.s_addr = m_dwRemoteIP;
	return inet_ntoa(addr);
}

// 获得连入时间
DWORD CClientUser::GetConnectTime(void)
{
	return m_dwConnectTime;
}

DWORD CClientUser::GetGatewayID()
{
	return ((VClientIDInfo *)&m_dwClientID)->dwGatewayID;
}


// 释放
void CClientUser::Release()
{
	// 关闭数据库回调
	GameDatabaseAccessor::getInstance().RemoveDBRetSink(static_cast<IDBRetSink *>(this));
	// 告诉游戏服务器该连接断开了
	IGlobalVoiceServer* pGlobalServer = gGlobalServer;
	if (pGlobalServer!=NULL)
	{
		pGlobalServer->OnClientUserLeave(this);
	}

	CClientList::getInstance().DelUser( m_dwClientID, m_UserInfo.context.dwPDBID, m_UserInfo.context.dwUDBID );

	// 关闭定时器
	gGlobalServer->KillTimer( ETimerEventID_Kick, this );
	gGlobalServer->KillTimer( ETimerEventID_LoginTimeout, this );

	delete this;
}


/// purpose: 客户端信息
std::string CClientUser::ToString()
{

	ostrstream buffer;
	buffer << "客户端ID[" << m_dwClientID
		<< "]，网关[" << GetGatewayID() 
		<< "]，IP[" << GetClientIP()
		<< "]，世界ID[" <<  m_dwWorldID
		<< "]，昵称[" << m_UserInfo.context.szName
		<< "]，PDBID[" << m_UserInfo.context.dwPDBID
		<< "]，UDBID[" << m_UserInfo.context.dwUDBID
		<< "]" << ends;

	return buffer.str();
}


// 获得客户端ID
DWORD CClientUser::GetClientID(void)
{
	return m_dwClientID;
}


// 设置客户端ID
void CClientUser::SetClientID( DWORD dwClientID )
{
	m_dwClientID = dwClientID;
}

// 生成密钥
void CClientUser::CreateKeyData(void)
{
	m_nKeyLen = VOICE_RC4_KEY_SIZE;
	for ( int i=0;i<m_nKeyLen;++i )
	{
		m_pKeyData[i] = rand()% ('z'-'A') + 'A';
	}
}

// 校验数据
bool CClientUser::CheckKeyData( char* szKeyData, DWORD dwKeyLen )
{
	if( szKeyData == NULL )
	{
		return false;
	}

	if( m_nKeyLen != (BYTE)dwKeyLen )
	{
		return false;
	}

	for ( int i=0; i<m_nKeyLen; ++i )
	{
		if( m_pKeyData[i] != szKeyData[i] )
		{
			return false;
		}
	}

	return true;
}

// 设置登录
void CClientUser::SetLogin( bool bLogin )
{
	m_bLogin = bLogin;

	// 已经登录了，需要停止定时器
	if( m_bLogin )
	{
		gGlobalServer->KillTimer( ETimerEventID_LoginTimeout, this );
	}
	else	// 重置时，要启动定时器
	{
		gGlobalServer->SetTimer( ETimerEventID_LoginTimeout, 10000, this, "CClientUser::SetLogin", 1 );
	}
}

// 获取是否登录
bool CClientUser::GetLogin(void)
{
	return m_bLogin;
}

// 获取用户详细信息
SVoiceUserInfo & CClientUser::GetUserInfo(void)
{
	return m_UserInfo.context;
}

// 设置服务器信息，游戏世界ID，服务器ID
void CClientUser::SetServerInfo( DWORD dwServerID, DWORD dwWorldID )
{
	m_dwServerID = dwServerID;
	m_dwWorldID = dwWorldID;
}

// 获得所属游戏服务器的ID
DWORD CClientUser::GetServerID(void)
{
	return m_dwServerID;
}

// 获得所属游戏世界ID
DWORD CClientUser::GetWorldID(void)
{
	return m_dwWorldID;
}

// 踢出操作
void CClientUser::Kick( DWORD dwReason )
{
	m_dwKickReason = dwReason;
	gGlobalServer->SetTimer( ETimerEventID_Kick, 500, this, "CClientUser::Kick", 1 );
}

// 设置在线
void CClientUser::SetOnline( bool bOnline )
{
	m_bOnline = bOnline;
}

// 是否在线
bool CClientUser::IsOnline(void)
{
	return m_bOnline;
}


// 获取当前所在频道权限
bool CClientUser::GetRight( DWORD dwRightFlag  )
{
	DWORD dwRight = GetChannelRight(VoiceChannel_Max);

	if ( (dwRight & dwRightFlag) != 0 )
	{
		return true;
	}

	return false;
}

// 设置位状态标记
void CClientUser::SetFlag( DWORD dwStateFlag, bool bSet, bool bSyncOthers )
{
	// 清除指定标识
	DWORD dwMyNewFlag = m_UserInfo.context.dwStateFlag & (~dwStateFlag);

	// 如果指定标识为true,设回状态
	if (bSet)
	{
		dwMyNewFlag =  dwMyNewFlag | dwStateFlag;
	}

	m_UserInfo.context.dwStateFlag = dwMyNewFlag;

	switch(dwStateFlag)
	{
	case VoiceUserStateFlag_Mic:			// 1	:开麦
		{
			m_bCanSpeak = bSet;		// 存变量，提高判断效率 语音时用
		}
		break;

	case VoiceUserStateFlag_Broadcast:		// 8	:可频道广播
		{
			m_bBroadcast = bSet;	// 存变量，提高判断效率，广播时用
		}
		break;

	case VoiceUserStateFlag_NoListen:	// 听筒操作
		{
			// 去当前房间对象
			CRoom * pRoom = GetCurRoom();
			if( pRoom != NULL )
			{
				pRoom->UpdateUserMuteState( m_dwClientID, bSet );
			}
		}
		break;

	case VoiceUserStateFlag_ChatMic:		// 私聊开麦
		{
			m_ChatInfo.bMicOpen = bSet;		// 存变量，提高判断效率 语音时用
		}
		break;

	case VoiceUserStateFlag_Video:	// 视频
		{
			// 去当前房间对象
			CRoom * pRoom = GetCurRoom();
			if( pRoom != NULL )
			{
				// 如果该用户是正在麦上说话的人，要同步到房间所有人
				if( pRoom->IsMicUser(m_dwClientID) )
				{
                    UpdateUserInfoToClient(true, -1);
				}
			}
		}
		break;
	};

	// 同步状态标记到客户端
    UpdateUserInfoToClient(false, 1, VOICE_FIELD_STATE_FLAG);
}

// 获取位状态标记
bool CClientUser::GetFlag( DWORD dwStateFlag )
{
	if ( (m_UserInfo.context.dwStateFlag & dwStateFlag) != 0 )
	{
		return true;
	}

	return false;
}

// 获得状态标记值
DWORD CClientUser::GetFlagValue(void)
{
	return m_UserInfo.context.dwStateFlag;
}

// 设置网络状态
void CClientUser::SetState( DWORD dwState )
{
	m_dwState = dwState;
}

// 获取网络状态
DWORD CClientUser::GetState(void)
{
	return m_dwState;
}

// 获取用户类型
BYTE CClientUser::GetType( DWORD dwChannelType )
{	
    LONGLONG nCurChannelID = m_nRoomID;
    ((SVoiceChannelID*)&nCurChannelID)->dwRoomID = 0;

    if(dwChannelType == VoiceChannel_Max)  // 取当前频道的用户类型
    {
        SVoiceChannel* pCurChannel = NULL;
        for(int i = 0; i < VoiceChannel_Max; ++i)
        {
            SVoiceChannel* pInfo = &(m_UserInfo.context.channelList[i]);
            if(pInfo->channleID == nCurChannelID)
            {
                pCurChannel = pInfo;
                break;
            }
        }
        if(pCurChannel == NULL)
        {
            if(m_UserInfo.context.exChannel.channleID == nCurChannelID)
            {
                pCurChannel = &(m_UserInfo.context.exChannel);
            }
        }

        if(pCurChannel == NULL)
        {
            return VoiceUserType_None; 
        }

        BYTE nUserType = max(pCurChannel->nUserType, m_UserInfo.context.nVestUserType);
        return max(nUserType, (BYTE)GetDBUserType(((SVoiceChannelID*)&nCurChannelID)->dwType));
    }

    if(dwChannelType <= VoiceChannel_None || dwChannelType >= VoiceChannel_Max)
    {
        return VoiceUserType_None; 
    }
    
    SVoiceChannel& channel = m_UserInfo.context.channelList[dwChannelType];
    if(channel.channleID != nCurChannelID)  // 并不是当前频道
    {
        return channel.nUserType;
    }
    else
    {
        int nUserType = max(channel.nUserType, m_UserInfo.context.nVestUserType);
        return max(nUserType, (BYTE)GetDBUserType(dwChannelType));
    }
}

// 获取用户频道权限VoiceChannel_Max为获取当前频道
DWORD CClientUser::GetChannelRight( DWORD dwChannelType )
{
	DWORD dwType = GetType(dwChannelType);
	DWORD dwRight = VoiceRight_None;
	switch(dwType)
	{
	case VoiceUserType_VIP:					// 嘉宾主持
		dwRight = VOICE_USER_RIGHT_VIP;
		break;

	case VoiceUserType_Host:					// 主播
		dwRight = VOICE_USER_RIGHT_HOST;
		break;

	case VoiceUserType_Admin:				// 频道管理员
		dwRight = VOICE_USER_RIGHT_ADMIN;
		break;

	case VoiceUserType_OW:					// 频道拥有者
		dwRight = VOICE_USER_RIGHT_OW;
		break;

	case VoiceUserType_GM:					// 官方管理员
		dwRight = VOICE_USER_RIGHT_ALL;
		break;

	default:
		break;
	}

	return dwRight;
}

bool CClientUser::OnUpdateUserInfo(void* data, int len)
{
    // 更新缓存
    bool bMask[VOICE_FIELD_MAX];
    memset(bMask, 0, sizeof(bMask));
    m_UserInfo.updateFields(ibuffer(data, len), bMask);

    // 更新给客户端
    UpdateUserInfoToClient(true, data, len);

    // 当前频道信息有更新 可能需要退出当前频道
    if(m_nRoomID != 0)
    {
        if(bMask[VOICE_FIELD_CHANNEL_None + ((SVoiceChannelID*)&m_nRoomID)->dwType])
        {
            LONGLONG nCurrentChannel = m_nRoomID;
            ((SVoiceChannelID*)&nCurrentChannel)->dwRoomID = 0;

            bool bCurChannelValid = (m_UserInfo.context.channelList[((SVoiceChannelID*)&nCurrentChannel)->dwType].channleID == nCurrentChannel
                || m_UserInfo.context.exChannel.channleID == nCurrentChannel); 
            if(!bCurChannelValid)
            {
                // 退出当前房间
                EnterRoom(0, NULL);
            }
        }
    }

    // 如果是战场频道有更新 自动进入战场频道
    LONGLONG nCurChannel = m_nRoomID;
    ((SVoiceChannelID*)&nCurChannel)->dwRoomID = 0;
    if(bMask[VOICE_FIELD_CHANNEL_War] 
        && ((SVoiceChannelID*)&(m_UserInfo.context.channelList[VoiceChannel_War].channleID))->dwID != 0
        && nCurChannel != m_UserInfo.context.channelList[VoiceChannel_War].channleID)
    {
        EnterRoom(m_UserInfo.context.channelList[VoiceChannel_War].channleID, NULL);
    }
	return true;
}

bool CClientUser::UpdateUserInfoToClient(bool bBroadcast, void* data, int len)
{
    if(len <= 0)
    {
        return false;
    }

    obuf obufData;

    SGameMsgHead header;
    header.SrcEndPoint = MSG_ENDPOINT_VOICE;
    header.DestEndPoint= MSG_ENDPOINT_CLIENT;
    header.byKeyModule  = MSG_MODULEID_VOICE;
    header.byKeyAction  = MSG_VOICE_USERINFO_UPDATE;
    obufData.push_back( &header, sizeof(SGameMsgHead) );
    obufData << m_dwClientID;

    obufData.push_back(data, len);

    CRoom* pRoom = GetCurRoom();
    if(pRoom && bBroadcast)
    {
        pRoom->broadCast(obufData.data(), obufData.size());
    }
    else
    {
        gGlobalServer->SendDataToClient( m_dwClientID, obufData.data(), obufData.size() );
    }

    return true;
}

// 更新用户信息到客户端
bool CClientUser::UpdateUserInfoToClient(bool bBroadcast, int nFieldNum, ...)
{
	obuf obufData;

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_USERINFO_UPDATE;
	obufData.push_back( &header, sizeof(SGameMsgHead) );
    obufData << m_dwClientID;

    bool bMask[VOICE_FIELD_MAX];
    memset(bMask, 0, sizeof(bMask));

    // 获取需要更新字段
    if(nFieldNum == -1)
    {
        for(int i = 0; i < VOICE_FIELD_MAX; ++i)
        {
            bMask[i] = true;
        }
    }
    else
    {
        va_list   arg_ptr;   
        va_start(arg_ptr, nFieldNum);

        for(int i = 0; i < nFieldNum; ++i)
        {
            int nFieldID = va_arg(arg_ptr, int);
            if(nFieldID < 0 || nFieldID > VOICE_FIELD_MAX)
            {
                continue;
            }
            bMask[nFieldID] = true;
        }

        va_end(arg_ptr);
    }

    m_UserInfo.getFields(obufData, bMask);

    CRoom* pRoom = GetCurRoom();
    if(pRoom && bBroadcast)
    {
        pRoom->broadCast(obufData.data(), obufData.size());
    }
    else
    {
        gGlobalServer->SendDataToClient( m_dwClientID, obufData.data(), obufData.size() );
    }

    return true;
}


// 根据频道类型获取ID dwType为EMVoiceChannelType
LONGLONG CClientUser::GetChannelID( DWORD dwType )
{
	if( dwType <= VoiceChannel_None || dwType >= VoiceChannel_Max )
	{
		return 0;
	}

    return m_UserInfo.context.channelList[dwType].channleID;
}

// 切换频道
bool CClientUser::EnterRoom( LONGLONG nRoomID, const char * szPassword, bool bForce )
{
    // 只退出当前房间
    CChannelList& channelList = CChannelList::getInstance();
    if(nRoomID == 0)
    {
        CRoom* pCurRoom = channelList.GetRoom(m_nRoomID);	
        if(pCurRoom != NULL)
        {
            if(pCurRoom->Del(*this))
            {
                gGlobalServer->SendVoiceDataToClient( m_dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_EnterRoomResult, 0, 0, 0, (char *)(&m_nRoomID), sizeof(m_nRoomID) );
            }
        }
        return true;
    }

    if(nRoomID == m_nRoomID)
    {
        return true;
    }
    
    CRoom* pNewRoom = channelList.GetRoom(nRoomID);  // 新房间
    SVoiceChannelID* pNewIDInfo = (SVoiceChannelID*)&nRoomID; 
    if(pNewIDInfo->dwID == 0)  // 频道ID为0为无效频道
    {
        return false;
    }

    CChannel *pNewChannel = NULL;	// 新频道
    if(pNewRoom == NULL)  // 创建新的频道
    {
        bool bNew = false;
        pNewChannel = channelList.Add( pNewIDInfo->dwType, pNewIDInfo->dwWorldID, pNewIDInfo->dwID, bNew );
        if(pNewChannel == NULL)
        {
            TraceLn(__FUNCTION__ << ", pNewChannel == NULL");
            return false;
        }
        pNewRoom = &(pNewChannel->GetRoom(pNewIDInfo->dwRoomID));
    }
    else
    {
        pNewChannel = &(pNewRoom->GetChannel());
    }

    if(pNewChannel == NULL || pNewRoom == NULL)
    {
        TraceLn(__FUNCTION__ << ", pNewChannel == NULL");
        return false;
    }

    // 频道是否可以进
    DWORD dwResult = 0;
    if( !(pNewChannel->CanChannelEnter(dwResult)) )		// 如果频道不能进入
    {
        bool bContinue = true;
        switch(dwResult)
        {
        case 1:		// 频道已关闭
            if( VoiceUserType_GM != GetType(pNewIDInfo->dwType) )		// 只有GM才可以进入，否则告诉用户频道已关闭
            {
                char szCloseMsg[512] = {0};
                sprintf_s( szCloseMsg, sizeof(szCloseMsg), "频道已关闭！原因：%s", pNewChannel->GetCloseMsg() );
                SendChatMsg(szCloseMsg);
                bContinue = false;
            }
            break;

        case 2:		// 频道正在保存数据中
            SendChatMsg("频道正在保存数据，请稍后再试！");
            bContinue = false;
            break;

        case 3:		// 人数已达上限
            if( VoiceUserType_GM != GetType(pNewIDInfo->dwType) )		// 只有GM才可以进入，否则告诉用户频道人数已满
            {
                SendChatMsg("频道人数已满，请稍后再试！");
                bContinue = false;
            }
            break;
        }

        if( !bContinue )
        {
            // 发送进入房间结果给客户端 dwUserData1：是否成功 0成功，1 密码校验失败，2 频道不存在 
            gGlobalServer->SendVoiceDataToClient( m_dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_EnterRoomResult, 2, 0, 0, (char *)(&nRoomID), sizeof(nRoomID) );
            return false;
        }
    }
    
    // 房间是否可以进
    if( !bForce && !(pNewRoom->CheckPassword(szPassword)) )
    {
        // 发送进入房间结果给客户端 dwUserData1：是否成功 0成功，1 密码校验失败，2 频道不存在 
        gGlobalServer->SendVoiceDataToClient( m_dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_EnterRoomResult, 1, 0, 0, (char *)(&nRoomID), sizeof(nRoomID) );
        return false;
    }

    CRoom* pOldRoom = channelList.GetRoom(m_nRoomID);	
    // 离开旧房间
    if(pOldRoom != NULL)
    {
        pOldRoom->Del(*this);
    }

    // 进入新房间
    pNewRoom->Add(*this);

    // 通知逻辑层
    gGlobalServer->OnUserEnterRoom( pNewRoom, this );

	return true;
}

// 清空当前房间对象
void CClientUser::ClearCurRoom(void)
{
    m_UserInfo.context.exChannel.Clear();
    m_nRoomID = 0;

	SetVestUserType( 0, false, false );

#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("CClientUser::ClearCurRoom 用户离开房间，" << ToString() );
#endif
}

// 设置当前房间对象
void CClientUser::SetCurRoom( CRoom & room )
{
    m_nRoomID = room.GetIDEx(); 

    // 清空扩展频道信息
    m_UserInfo.context.exChannel.Clear();

    LONGLONG nChannelID = m_nRoomID;
    ((SVoiceChannelID*)&nChannelID)->dwRoomID = 0;

    // 找出进入了哪个频道
    SVoiceChannelID* pID = (SVoiceChannelID*)&m_nRoomID;
    int nChannelType = VoiceChannel_Max; 
    for(int i = VoiceChannel_None; i < VoiceChannel_Max; ++i)
    {
        if(nChannelID == m_UserInfo.context.channelList[i].channleID)
        {
            nChannelType = i;
            break;
        }
    }

    // 没找到 则进入了扩展频道
    if(nChannelType == VoiceChannel_Max)
    {
        m_UserInfo.context.exChannel.channleID = nChannelID;
    }

	CChannel & channel = room.GetChannel();
	
	// 查询是否有附加权限
	SVoiceVestInfo * pVest = channel.GetVest( m_UserInfo.context.dwUDBID );
	if( pVest != NULL )
	{
		SetVestUserType( pVest->nNewUserType, true, false );
	}

	// 更新禁言标志
	DWORD dwBanTimeLeft = 0;
	if( ((GlobalVoiceServer *)gGlobalServer)->IsPDBIDBan( room.GetChannel().GetID(), m_UserInfo.context.dwPDBID, dwBanTimeLeft ) )
	{
		SetFlag( VoiceUserStateFlag_Mute, true );
	}
	else
	{
		SetFlag( VoiceUserStateFlag_Mute, false );
	}

	// 是否静音
	room.UpdateUserMuteState( m_dwClientID, GetFlag(VoiceUserStateFlag_NoListen) );

	// 关掉麦克风
	SetFlag( VoiceUserStateFlag_Mic, false );

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("CClientUser::SetCurRoom 房间ID=" << m_nCurRoomID << ", 频道类型=" << m_dwCurChannelType);
#endif

	// 语音品质  EMVoiceQualityType
	DWORD dwQuality = channel.GetBaseInfo().dwQuality;

	// 语音角色用户进入频道,保存DB,暂时不用,有用时可调用一下来记录,这个调用频繁,所有没有必要,不要打开
	// DBEnterChannel();

	// 发送进入房间结果给客户端 dwUserData1：是否成功 0成功，1 密码校验失败，2 频道不存在  dwUserData2: 1 同频道切换，0 不同频道切换
	gGlobalServer->SendVoiceDataToClient( m_dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_EnterRoomResult, 0, GetChannelRight(), dwQuality, (char *)(&m_nRoomID), sizeof(m_nRoomID) );
}

// 获得当前所在房间对象
CRoom * CClientUser::GetCurRoom(void)
{
	return CChannelList::getInstance().GetRoom(m_nRoomID);
}

// 获取当前房间ID
LONGLONG CClientUser::GetCurRoomID(void)
{
    return m_nRoomID;
}

// 获取当前频道类型
DWORD CClientUser::GetCurChannelType(void)
{
	SVoiceChannelID* pIDInfo = (SVoiceChannelID*)&m_nRoomID;
    
    return pIDInfo->dwType;
}

// 获取频道值ID
DWORD CClientUser::GetChannelTypeID( DWORD dwChannelType )
{
	if( dwChannelType >= VoiceChannel_Max )
	{
		return 0;
	}

	return ((SVoiceChannelID*)&(m_UserInfo.context.channelList[dwChannelType].channleID))->dwID;
}

// 是否有权限说话
bool CClientUser::CanSpeak(void)
{
	return m_bCanSpeak;
}

// 是否可以私聊
bool CClientUser::CanPrivateChat(void)
{
	return m_ChatInfo.bMicOpen;
}

// 语音数据是否要广播
bool CClientUser::NeedBroadcast(void)
{
	return m_bBroadcast;
}

// 设置麦序
void CClientUser::SetMicIndex( DWORD dwIndex )
{
	m_UserInfo.context.nMicIndex = dwIndex;

    // 广播给房间所有用户
    UpdateUserInfoToClient(true, 1, VOICE_FIELD_MIC_INDEX);
}

// 获得麦序
DWORD CClientUser::GetMicIndex(void)
{
	return m_UserInfo.context.nMicIndex;
}

// 发送聊天信息
void CClientUser::SendChatMsg( const char * pMsg, WORD nPos, WORD nTipsType, DWORD dwUserData1, DWORD dwUserData2, DWORD dwUserData3 )
{
	gGlobalServer->BroadcastChatMsgToClient( &m_dwClientID, 1, nPos, nTipsType, pMsg, dwUserData1, dwUserData2, dwUserData3 );
}

// 操作话筒
bool CClientUser::OpenMic( bool bOpen )
{
	CRoom * pRoom = GetCurRoom();
	if( NULL == pRoom )
	{
		ErrorLn( "CClientUser::OpenMic 当前房间对象为空？ClientID=" << m_dwClientID << "，当前房间ID" << m_nRoomID );

		SendChatMsg("你不在语音频道中！");
		return false;
	}

	bool bRet = false;

	if( bOpen )		// 开麦
	{
		DWORD dwBanTimeLeft = 0;
		LONGLONG nChannelID = pRoom->GetChannel().GetID();
		// 要判断是不是被禁言了
		if( ((GlobalVoiceServer *)gGlobalServer)->IsPDBIDBan( nChannelID, m_UserInfo.context.dwPDBID, dwBanTimeLeft ) )
		{
			char szText[64] = {0};
			sprintf_s( szText, sizeof(szText), "你已被禁言，剩余时间%s", GetRemainTimeString(dwBanTimeLeft) );

			SendChatMsg(szText);

			return false;
		}

		// 已经开麦了，不处理
		if( GetFlag(VoiceUserStateFlag_Mic) )
		{
			SendChatMsg("你已经开启了麦克风！");
			return false;
		}

		// 有权限的，可以开麦
		if( GetRight(VoiceRight_Speak) )
		{
			SetFlag(VoiceUserStateFlag_Mic, true);
			return true;
		}
		else	// 如果没有权限的话，就要判断是否有开启的条件
		{
			bool bCanOpen = false;

			DWORD dwMicState = pRoom->GetMicState();
			
			// 各种模式处理
			switch(dwMicState)
			{
			case VoiceMicState_MicOrder:	/// 麦序模式
				{
					// 如果是控麦中，不能开麦
					if( pRoom->GetFlag(VoiceRoomState_HoldMic) )
					{
						SendChatMsg("管理员控麦中，请稍后！");
						return false;
					}

					// 麦上的人才能开麦
					if (pRoom->IsMicUser(m_dwClientID))
					{
						bRet = true;
					}
					else
					{
						// 有连麦，也可以开麦
						if( GetFlag(VoiceUserStateFlag_ShareMic) )
						{
							bRet = true;
						}
						else
						{
							SendChatMsg("你不在麦上，开启麦克风失败！");
						}
					}
				}
				break;
			case VoiceMicState_MicFree:		// 如果当前房间是自由模式
				{
					// 正在说的人数没超过上限,不可以开麦
					if (pRoom->AddSpeak(*this))
					{
						bRet = true;
					}
					else
					{
						SendChatMsg("当前开麦说话人过多，请稍候再试！");
					}
				}
				break;
			}

			if( bRet )
			{
				SetFlag(VoiceUserStateFlag_Mic, true);
			}
		}
	}
	else
	{

		DWORD dwMicState = pRoom->GetMicState();

		// 各种模式处理
		switch(dwMicState)
		{
		case VoiceMicState_MicFree:		// 如果当前房间是自由模式
			{
				// 是自由模式,从说话列表中删除,不删除别人就开不了
				pRoom->DelSpeak(*this);

			}
			break;
		}

		// 已经关麦了，不处理
		if( !GetFlag(VoiceUserStateFlag_Mic) )
		{
			return false;
		}

		bRet = true;

		SetFlag(VoiceUserStateFlag_Mic, false);
	}

	return bRet;
}

// 操作私聊话筒
bool CClientUser::OpenChatMic( bool bOpen )
{
	CChat * pChat = GetChatInstance();
	if( NULL == pChat )
	{
		SendChatMsg("您未在私聊中");
		return false;
	}

	if( pChat->GetState() != VoiceChatState_Run )
	{
		SendChatMsg("当前状态不能开麦");
		return false;
	}

	if( bOpen )
	{
		if( !GetFlag(VoiceUserStateFlag_ChatMic) )
		{
			SetFlag( VoiceUserStateFlag_ChatMic, true );
		}
	}
	else
	{
		if( GetFlag(VoiceUserStateFlag_ChatMic) )
		{
			SetFlag( VoiceUserStateFlag_ChatMic, false );
		}
	}

	return true;
}


// 操作开启视频
bool CClientUser::OpenVideo( bool bOpen )
{
	if( bOpen )
	{
		if( !GetFlag(VoiceUserStateFlag_Video) )
		{
			SetFlag( VoiceUserStateFlag_Video, true );
		}
	}
	else
	{
		if( GetFlag(VoiceUserStateFlag_Video) )
		{
			SetFlag( VoiceUserStateFlag_Video, false );
		}
	}

	return true;
}



// 操作听筒
bool CClientUser::OpenHeadset( bool bOpen )
{
	if( bOpen )		// 开听筒
	{
		// 已经开启听筒了，不执行
		if( !GetFlag(VoiceUserStateFlag_NoListen) )
		{
			return false;
		}

		SetFlag(VoiceUserStateFlag_NoListen, false, true);
	}
	else
	{
		// 已经关闭听筒了，不执行
		if( GetFlag(VoiceUserStateFlag_NoListen) )
		{
			return false;
		}

		SetFlag(VoiceUserStateFlag_NoListen, true, true);
	}

	return true;
}

// 请求观察
bool CClientUser::RequestVisitor( bool bForce )
{
	CRoom * pRoom = GetCurRoom();
	if( NULL == pRoom )
	{
		//ErrorLn( "CClientUser::OnClientRequestVisitor 当前房间对象为空？ClientID=" << m_dwClientID << "，当前房间ID" << m_nCurRoomID );
		return false;
	}

	CChannel & channel = pRoom->GetChannel();

	// 如果是新增的观察者，要把房间列表发送给他
	// 需要强制请求观察的也发
	if( channel.AddObserver( m_dwClientID, pRoom->GetID() ) || bForce )
	{
        // 更新 todo.
	}

	// 房间是否正在语音的信息发送过去
	channel.UpdateChannelInfoToClient( &m_dwClientID, 1, VoiceUpdateChannelType_Audio );

	return true;
}

// 抢/下麦
bool CClientUser::AddMicOrder( bool bAdd )
{
	CRoom * pRoom = GetCurRoom();
	if( NULL == pRoom )
	{
		ErrorLn( "CClientUser::OnClientAddMicOrder 当前房间对象为空？ClientID=" << m_dwClientID << "，当前房间ID" << m_nRoomID );
		return false;
	}

	if( bAdd )
	{
		// 如果被禁言了，不能抢麦
		DWORD dwBanTimeLeft = 0;
		LONGLONG nChannelID = pRoom->GetChannel().GetID();
		// 要判断是不是被禁言了
		if( ((GlobalVoiceServer *)gGlobalServer)->IsPDBIDBan( nChannelID, m_UserInfo.context.dwPDBID, dwBanTimeLeft ) )
		{
			char szText[64] = {0};
			sprintf_s( szText, sizeof(szText), "不能抢麦，你已被禁言，剩余时间%s", GetRemainTimeString(dwBanTimeLeft) );

			SendChatMsg(szText);

			return false;
		}

		if( pRoom->GetMicState() != VoiceMicState_MicOrder )
		{
			SendChatMsg("非麦序模式，不能抢麦！");
			return false;
		}

		if( !pRoom->GetFlag(VoiceRoomState_AddMicOrder) )
		{
			SendChatMsg("管理员已控麦，不能抢麦！");
			return false;
		}

		if( pRoom->GetMicOrderCount() >= VOICE_ROOM_MAX_MICORDER )
		{
			SendChatMsg("麦序已满，请等待！");
			return false;
		}

		return pRoom->AddMicOrder(*this);
	}
	else	// 下麦
	{
		DWORD dwError = 0;
		return pRoom->DelMicOrder( *this, dwError );
	}
}

// 成员调度
bool CClientUser::Move( bool bMoveRoom, DWORD dwData )
{
	CRoom * pRoom = GetCurRoom();
	if( pRoom == NULL )
	{
		ErrorLn( "CClientUser::Move 当前房间对象为空？ClientID=" << m_dwClientID << "，当前房间ID" << m_nRoomID );
		return false;
	}

	// 判断是否有权限
	if( !GetRight(VoiceRight_Move) )
	{
		SendChatMsg("你没有成员调度权限！");
		return false;
	}

	// 返回结果
	bool bResult = false;

	// 调度整个房间里的人
	if( bMoveRoom )
	{
		if( pRoom->GetID() == dwData )
		{
			SendChatMsg("不能调度本房间里的成员！");
			return false;
		}

		// 调度成员
		bResult = pRoom->GetChannel().MoveUser( dwData, pRoom->GetID() );
		if( !bResult )
		{
			SendChatMsg("调度房间里的成员失败！");
		}
	}
	else
	{
		// 不能调度自己
		if( dwData == m_UserInfo.context.dwPDBID )
		{
			SendChatMsg("不能调度自己！");
			return false;
		}

		// 找到用户 
		CClientUser * pUser = CClientList::getInstance().GetClientByPDBID(dwData);
		if( NULL == pUser )
		{
			SendChatMsg("此用户未连入语音服务器！");
			return false;
		}

		// 判断是否在同一个频道
		LONGLONG nMyChannelID = pRoom->GetChannel().GetID();
		LONGLONG nOtherChannelID = 0;
		CRoom * pOtherRoom = pUser->GetCurRoom();
		if( pOtherRoom != NULL )
		{
			nOtherChannelID = pOtherRoom->GetChannel().GetID();
		}

		// 不在一个频道，不能调度
		if( nOtherChannelID != nMyChannelID )
		{
			SendChatMsg("不能调度其他频道成员！");
			return false;
		}

		bResult = pUser->EnterRoom( pRoom->GetIDEx(), NULL, true );
	}

	return bResult;
}

// 开启频道广播
bool CClientUser::OpenBroadcast( bool bOpen )
{
	CRoom * pRoom = GetCurRoom();
	if( pRoom == NULL )
	{
		ErrorLn( "CClientUser::OpenBroadcast 当前房间对象为空？ClientID=" << m_dwClientID << "，当前房间ID" << m_nRoomID );
		return false;
	}

	// 判断是否有权限
	if( !GetRight(VoiceRight_Broadcast) )
	{
		SendChatMsg("你没有频道广播权限！");
		return false;
	}

	if( bOpen )
	{
		if( GetFlag(VoiceUserStateFlag_Broadcast) )
		{
			SendChatMsg("你已开启频道广播！");
			return false;
		}

		SetFlag(VoiceUserStateFlag_Broadcast, true);
	}
	else
	{
		if( !GetFlag(VoiceUserStateFlag_Broadcast) )
		{
			SendChatMsg("你已关闭频道广播！");
			return false;
		}

		SetFlag(VoiceUserStateFlag_Broadcast, false);
	}

	return true;
}

// 禁言操作
bool CClientUser::BanPDBID( DWORD dwPDBID, bool bBan )
{
	CRoom * pRoom = GetCurRoom();
	if( pRoom == NULL )
	{
		ErrorLn( "CClientUser::BanPDBID 当前房间对象为空？ClientID=" << m_dwClientID << "，当前房间ID" << m_nRoomID );
		return false;
	}

	if( !GetRight(VoiceRight_Ban) )
	{
		SendChatMsg("你没有禁言权限！");
		return false;
	}

	// 不能禁言自己
	if( dwPDBID == m_UserInfo.context.dwPDBID )
	{
		SendChatMsg("不能禁言自己！");
		return false;
	}

	// 找到用户 
	CClientUser * pUser = CClientList::getInstance().GetClientByPDBID(dwPDBID);
	if( NULL == pUser )
	{
		SendChatMsg("此用户未连入语音服务器！");
		return false;
	}

	// 判断是否在同一个频道
	LONGLONG nMyChannelID = pRoom->GetChannel().GetID();	// 获取我的频道ID
	LONGLONG nOtherChannelID = 0;							// 对方所在频道ID
	CRoom * pOtherRoom = pUser->GetCurRoom();
	if( pOtherRoom != NULL )
	{
		nOtherChannelID = pOtherRoom->GetChannel().GetID();
	}
	
	// 判断用户类型
	if( GetType() <= pUser->GetType() )
	{
		SendChatMsg("你的权限等级比此用户低，操作失败！");
		return false;
	}

	// 不在一个频道，不能调度，有超级权限的可以跳过进行处理
	if( nOtherChannelID != nMyChannelID && !GetRight(VoiceRight_Supper))
	{
		SendChatMsg("此用户不在你所在频道，操作失败！");
		return false;
	}

	if( bBan )
	{
		// 禁止PDBID
		if( ((GlobalVoiceServer*)gGlobalServer)->BanPDBID( nMyChannelID, dwPDBID ) )
		{
			pUser->SetFlag( VoiceUserStateFlag_Mic, false );	// 关MIC
			pUser->SetFlag( VoiceUserStateFlag_Mute, true, true );

			// 如果在麦序上，要移除
			if( pRoom->GetMicState() == VoiceMicState_MicOrder )
			{
				DWORD dwError = 0;
				pRoom->DelMicOrder( *pUser, dwError );
			}

			// pUser->SendChatMsg("你已被管理员禁言!");
#ifdef VOICE_PRINT_DEBUG_INF
			TraceLn("CClientUser::BanPDBID 管理员禁言 用户=" << dwPDBID );
#endif
			return true;
		}
	}
	else
	{
		// 解禁PDBID
		if( ((GlobalVoiceServer*)gGlobalServer)->UnbanPDBID( nMyChannelID, dwPDBID ) )
		{
			pUser->SetFlag( VoiceUserStateFlag_Mute, false, true );
			//pUser->SendChatMsg("你已被管理员解除禁言!");
			return true;
		}
	}

	return false;
}


// 踢出用户
bool CClientUser::KickPDBID( DWORD dwPDBID )
{
	CRoom * pRoom = GetCurRoom();
	if( pRoom == NULL )
	{
		ErrorLn( "CClientUser::BanPDBID 当前房间对象为空？ClientID=" << m_dwClientID << "，当前房间ID" << m_nRoomID );
		return false;
	}

	if( !GetRight(VoiceRight_Kick) )
	{
		SendChatMsg("你没有踢人权限！");
		return false;
	}

	// 不能把自己踢出频道
	if( dwPDBID == m_UserInfo.context.dwPDBID )
	{
		SendChatMsg("不能踢自己！");
		return false;
	}

	// 找到用户 
	CClientUser * pUser = CClientList::getInstance().GetClientByPDBID(dwPDBID);
	if( NULL == pUser )
	{
		SendChatMsg("此用户未连入语音服务器！");
		return false;
	}

	// 判断用户类型
	if( GetType() <= pUser->GetType() )
	{
		SendChatMsg("你的权限等级比此用户低，操作失败！");
		return false;
	}

	// 判断是否在同一个频道
	LONGLONG nMyChannelID = pRoom->GetChannel().GetID();	// 获取我的频道ID
	LONGLONG nOtherChannelID = 0;							// 对方所在频道ID
	CRoom * pOtherRoom = pUser->GetCurRoom();
	if( pOtherRoom != NULL )
	{
		nOtherChannelID = pOtherRoom->GetChannel().GetID();
	}

	// 不在一个频道，不能调度
	if( nOtherChannelID != nMyChannelID )
	{
		SendChatMsg("此用户不在你所在频道，操作失败！");
		return false;
	}

	// 把用户扔到一个空频道
	pUser->EnterRoom(0, NULL);
	pUser->SendChatMsg("你已被管理员踢出频道!");

	return true;
}


// 设置是否开麦，在控麦下使用 在控麦下使用，临时保存
void CClientUser::SetNeedOpenMic( bool bNeed )
{
	m_bNeedOpenMic = bNeed;
}

// 是否要开麦，在控麦下使用，临时保存
bool CClientUser::IsNeedOpenMic(void)
{
	return m_bNeedOpenMic;
}

// 在当前房间是否可以文字聊天
bool CClientUser::CanChat( CRoom & curRoom, DWORD & dwTimeLeft )
{
	dwTimeLeft = 0;

	DWORD dwNow = (DWORD)time(NULL);
	DWORD dwChatCoolDown = curRoom.GetChatCoolDown();

	// 如果是不同频道，直接可以说话
	if( curRoom.GetType() != m_dwLastChatChannel )
	{
		m_dwLastChatChannel = curRoom.GetType();	// 保留说话频道
		m_dwLastChatTime = dwNow;				// 保留说话时间
		return true;
	}

	if( dwNow >= m_dwLastChatTime + dwChatCoolDown )	// 冷却时间过了，可以说话
	{
		m_dwLastChatTime = dwNow;			// 保留说话时间
		return true;
	}

	dwTimeLeft = (m_dwLastChatTime + dwChatCoolDown) - dwNow;

	return false;
}

// 获取个人频道ID
LONGLONG CClientUser::GetPersonalChannelID(void)
{
    return m_UserInfo.context.channelList[VoiceChannel_Personal].channleID;
}

// 发送AD数据到社会服
void CClientUser::SendADToSocial( DWORD dwBroadcastType, DWORD dwADType, char * pTextData, DWORD dwTextLen )
{
	/*
	if( pTextData == NULL || dwTextLen <= 0 || dwTextLen >= VOICE_TEXTSTR_MAXSIZE )
	{
		return;
	}

	// 判断是否有权限
	if( !GetRight(VoiceRight_AD) )
	{
		SendChatMsg("你没有语音召集权限！");
		return;
	}

	CRoom * pRoom = GetCurRoom();
	if( pRoom == NULL )
	{
		return;
	}

	// 私聊频道不能发送频道拉人
	if( pRoom->GetType() == VoiceChannel_Personal )
	{
		SendChatMsg("此频道不能进行语音召集！");
		return;
	}

	obuf2048 obufData;

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_SOCIAL;
	header.byKeyModule  = MSG_MODULEID_COMMON;
	header.byKeyAction  = MSG_VOICE_AD;
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	// 拉人数据
	SMsgVoiceAD_VS ADData;
	ADData.dwPDBID = m_UserInfo.dwPDBID;
	ADData.dwUDBID = m_UserInfo.dwUDBID;
	ADData.nSex = m_UserInfo.nSex;
	ADData.nADType = (BYTE)dwADType;
	ADData.nChannelType = (BYTE)pRoom->GetType();
	ADData.nRoomID = (BYTE)pRoom->GetID();
	ADData.dwTextLen = dwTextLen;

	obufData.push_back( &ADData, sizeof(SMsgVoiceAD_VS) );

	// 附加文本
	obufData.push_back( pTextData, dwTextLen );
		
	// 只发自己区
	bool bSelfWorld		= ( dwBroadcastType & VoiceBroadcast_SelfWorld ) > 0 ? true:false;
	// 全区
	bool bAllWorld		= ( dwBroadcastType & VoiceBroadcast_AllWorld ) > 0 ? true:false;
	// 除自己区
	bool bExcepSelfWorld = ( dwBroadcastType & VoiceBroadcast_ExceptSelfWorld ) > 0 ? true:false;

	// 全区
	if ( bAllWorld )
	{
		// 发送 除了自己区的 其他所有区
		if ( bExcepSelfWorld  )
		{
			if( pRoom->GetType() == VoiceChannel_GM )
			{
				DWORD dwServerID = GetServerID();
				DWORD dwServerList[1024] = {0};
				WORD wCount = 0;
				for(ServerList::SERVER_LIST::iterator iter = ServerList::getInstance().m_list.begin(); iter!= ServerList::getInstance().m_list.end();++iter)
				{	
					// 如果服务器ID与自身的ID不相等，放入集合中
					if( iter->first != dwServerID )
					{
						if( wCount >= 1024 )
						{
							break;
						}

						dwServerList[wCount] = iter->first;
						wCount ++;
						
					}
				}

				gGlobalServer->BroadcastToServer( dwServerList, wCount, obufData.data(), obufData.size() );
				// TraceLn("发送到除自己区的:"<<m_UserInfo.dwFromWorldID);
			}
		}
		// 所有区
		else
		{
			if( pRoom->GetType() == VoiceChannel_GM )
			{
				gGlobalServer->BroadcastToServer( NULL, VOICE_BROADCASTALL_NUM, obufData.data(), obufData.size() );
				// TraceLn("全区广播")
			}
		}
		
	}
	// 非全区 只发自己区
	else if ( bSelfWorld )
	{
		gGlobalServer->SendDataToServer( GetServerID(), obufData.data(), obufData.size() );
		//TraceLn("发送到自己区:"<<GetServerID());
	}
	*/
}

// 发送AD数据到客户端
void CClientUser::SendADToClient(DWORD dwChannelType, DWORD dwRoomID, DWORD dwBroadcastType, DWORD dwADType, char * pTextData, DWORD dwTextLen)
{
	if( pTextData == NULL || dwTextLen <= 0 || dwTextLen >= VOICE_TEXTSTR_MAXSIZE )
	{
		return;
	}

	// 判断是否有权限
	if(GetChannelRight(dwChannelType) & VoiceRight_AD == 0)
	{
		SendChatMsg("你没有语音召集权限！");
		return;
	}

	if(dwChannelType >= VoiceChannel_Max)
	{
		return;
	}

	// 判断冷却时间
	const SVoiceChannelConfig & config = GetChannelConfig(dwChannelType);
	DWORD dwTimeNow = (DWORD)time(NULL);
	if( dwTimeNow < m_dwLastADTime[dwChannelType] + config.dwADCoolTime )
	{
		char szMsg[256] = {0};
		sprintf_s( szMsg, sizeof(szMsg), "语音召集令冷却中，请在%s后再试！", GetRemainTimeString(m_dwLastADTime[dwChannelType] + config.dwADCoolTime-dwTimeNow) );
		SendChatMsg(szMsg);
		return;
	}

	// 更新召集时间
	m_dwLastADTime[dwChannelType] = dwTimeNow;
	((GlobalVoiceServer*)gGlobalServer)->AddADInfo(m_UserInfo.context.dwUDBID, dwChannelType, dwTimeNow);
	DWORD dwChannelID = GetChannelTypeID(dwChannelType); 
	CChannel* pChannel = CChannelList::getInstance().Get(dwChannelType, GetWorldID(), dwChannelID); 
	if(pChannel == NULL)
	{
		bool bNew = false;
		pChannel = CChannelList::getInstance().Add(dwChannelType, GetWorldID(), dwChannelID, bNew);
	}

	if(pChannel == NULL)
	{
		SendChatMsg("语音召集失败，找不到指定频道");
		return;
	}

	CRoom* pRoom = &(pChannel->GetRoom(dwRoomID));
	if(pRoom == NULL)
	{
		SendChatMsg("语音召集失败，找不到指定房间");
		return;
	}

	obuf2048 obufData;

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_AD;
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	// 拉人数据
	SMsgVoiceAD_SC ADData;
	ADData.dwTime = 0;
	ADData.dwPDBID = m_UserInfo.context.dwPDBID;
	ADData.dwUDBID = m_UserInfo.context.dwUDBID;
	ADData.nADType = (BYTE)dwADType;
	ADData.nChannelType = dwChannelType;
	ADData.dwValueID = pRoom->GetChannel().GetIDInfo().dwID;
	ADData.nRoomID = dwRoomID;
	ADData.nTextLen = (WORD)dwTextLen;
	obufData.push_back( &ADData, sizeof(SMsgVoiceAD_SC) );

	// 附加文本
	obufData.push_back( pTextData, dwTextLen );


	// 指定缓存
	DWORD * pClientIDArray = ((GlobalVoiceServer *)gGlobalServer)->m_dwClientIDBuff;
	DWORD dwCount = 0;
	CClientList & clientList = CClientList::getInstance();

	switch(ADData.nChannelType)
	{
	case VoiceChannel_GM:	// 官方
		{
			// 只发自己区
			bool bSelfWorld		= ( dwBroadcastType & VoiceBroadcast_SelfWorld ) > 0 ? true:false;
			// 全区
			bool bAllWorld		= ( dwBroadcastType & VoiceBroadcast_AllWorld ) > 0 ? true:false;
			// 除自己区
			bool bExcepSelfWorld = ( dwBroadcastType & VoiceBroadcast_ExceptSelfWorld ) > 0 ? true:false;

			// 全区
			if ( bAllWorld )
			{
				// 发送 除了自己区的 其他所有区
				if ( bExcepSelfWorld  )
				{
					clientList.GetClientIDListByServer( pClientIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount, VOICE_INVALID_ID, m_dwServerID, false );
				}
				// 所有区
				else
				{
					clientList.GetClientIDList( pClientIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount );
				}
			}
			// 非全区 只发自己区
			else if ( bSelfWorld )
			{
				clientList.GetClientIDListByServer( pClientIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount, VOICE_INVALID_ID, m_dwServerID, true );
			}
		}
		break;

	case VoiceChannel_Personal:		// 个人频道
		{
			// 个人频道只要获取频道的所有在线管理员列表
			pRoom->GetChannel().GetAdminIDList( pClientIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount );
		}
		break;

	default:	// 其他频道
		{
			LONGLONG nID = pRoom->GetChannel().GetID();
			clientList.GetClientIDList( pClientIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount, ADData.nChannelType, nID );
		}
		break;
	}

	if( dwCount > 0 )
	{
		// 发送消息到客户端
		if( 1 == dwCount )
		{
			gGlobalServer->SendDataToClient( pClientIDArray[0], obufData.data(), obufData.size() );
		}
		else
		{
			gGlobalServer->BroadcastDataToClient( pClientIDArray, dwCount, obufData.data(), obufData.size() );
		}
	}

	// 加入到缓存中
	SVoiceADCacheData cacheData;
	cacheData.dwTime = (DWORD)time(NULL);			// 发送邀请的时间
	cacheData.dwPDBID = m_UserInfo.context.dwPDBID;
	cacheData.dwUDBID = m_UserInfo.context.dwUDBID;			// 发送邀请的人
	cacheData.dwADType = dwADType;					// 邀请类型
	cacheData.dwChannelType = ADData.nChannelType;	// 频道类型
	cacheData.dwRoomID = ADData.nRoomID;			// 房间ID
	cacheData.dwValueID = ADData.dwValueID;			// 值ID
	cacheData.dwBroadcastType = dwBroadcastType;	// 广播类型
	cacheData.nID = pRoom->GetIDEx();

	DWORD dwMsgLen = dwTextLen + 1;
	if( dwTextLen + 1 > VOICE_TEXTSTR_MAXSIZE )
	{
		dwMsgLen = VOICE_TEXTSTR_MAXSIZE - 1;
	}
	sstrcpyn( cacheData.szText, pTextData, dwMsgLen );

	CVoiceCallList::getInstance().AddVoiceADCacheData(cacheData);
}

// 设置当前频道赋予用户类型
void CClientUser::SetVestUserType( BYTE nType, bool bVest, bool bUpdate )
{
	m_UserInfo.context.nVestUserType = nType;

	SetFlag( VoiceUserStateFlag_Vest, bVest );

	if( bUpdate )
	{
		UpdateUserInfoToClient(true, 1, VOICE_FIELD_VESTTYPE);
	}
}

// 添加/删除马甲
void CClientUser::AddVest( DWORD dwUDBID, bool bAdd, DWORD dwAddType )
{
	CRoom * pRoom = GetCurRoom();
	if( pRoom == NULL )
	{
		ErrorLn( "CClientUser::AddVest 当前房间对象为空？ClientID=" << m_dwClientID << "，当前房间ID" << m_nRoomID );
		return ;
	}

	CChannel & channel = pRoom->GetChannel();

	// 查找被操作的用户
	CClientUser * pUser = CClientList::getInstance().GetClientByUDBID(dwUDBID);

	BYTE nMyUserType = GetType();

	// 如果是添加马甲
	if( bAdd )
	{
		// 判断是否有权限
		if( !GetRight(VoiceRight_Vest) )
		{
			SendChatMsg("你没有频道权限管理的权限！");
			return ;
		}

		// 不能操作自己
		if( dwUDBID == m_UserInfo.context.dwUDBID )
		{
			SendChatMsg("不能操作自己！");
			return ;
		}

		// 如果有权限了，判断权限
		SVoiceVestInfo * pVest = channel.GetVest(dwUDBID);
		if( NULL != pVest )
		{
			// 只能任命比自己权限低的人
			if( nMyUserType <= pVest->nNewUserType )
			{
				SendChatMsg("你只能操作权限等级比你低的成员！");
				return;
			}
		}
		else if( NULL == pUser )		// 如果用户不在线，并且马甲也为空
		{
			SendChatMsg("操作失败，请稍后再试！");
			return;
		}

		if( nMyUserType <= dwAddType )
		{
			SendChatMsg("你只能添加权限等级比你低的管理员！");
			return;
		}

		DWORD dwError = 0;
		bool bRes = false;

		// 如果用户在线，可以添加新马甲，也可以修改马甲
		if( NULL != pUser )
		{
			// 如果是新增的话，就需要判断是否在同一个频道
			if( NULL == pVest )
			{
				LONGLONG nMyChannelID = channel.GetID();
				LONGLONG nOtherChannelID = 0;
				CRoom * pOtherRoom = pUser->GetCurRoom();
				if( pOtherRoom != NULL )
				{
					nOtherChannelID = pOtherRoom->GetChannel().GetID();
				}

				// 不在一个频道，不能操作
				if( nOtherChannelID != nMyChannelID )
				{
					SendChatMsg("不能操作其他频道成员！");
					return ;
				}
			}

			bRes = channel.AddVest( *pUser, dwAddType, dwError );
		}
		else if( NULL != pVest )	// 如果用户不在线，有马甲的话，可以修改
		{
			bRes = channel.EditVest( dwUDBID, dwAddType, dwError );
		}

		// 添加失败
		if( !bRes )
		{
			switch(dwError)
			{
			case VoiceAddVestError_Full:			// 管理员满
				{
					char szText[128] = {0};
					sprintf_s( szText, sizeof(szText), "管理员已达到%d个，请先删除部分管理！", GetChannelConfig(channel.GetType()).dwVestCount );
					SendChatMsg(szText);
				}
				break;

			case VoiceAddVestError_Exist:		// 已有管理
				{
					SendChatMsg( "此用户已有该管理权限！" );
				}
				break;

			case VoiceAddVestError_Normal:		// 一般错误
				{
					SendChatMsg( "操作非法，请稍后再试！" );
				}
				break;

			default:
				break;
			}
		}
		else
		{	
			char szText[128] = {0};
			if( NULL != pUser )
			{
				ssprintf_s( szText, sizeof(szText), "你已被[%s]授为频道%s", utf82a(m_UserInfo.context.szName), GetNameByUserType(dwAddType) );
				pUser->SendChatMsg(szText);

				sprintf_s( szText, sizeof(szText), "成功设置%s为频道%s", utf82a(pUser->GetUserInfo().szName), GetNameByUserType(dwAddType) );
				SendChatMsg(szText);
			}
			else if( NULL != pVest )
			{
				sprintf_s( szText, sizeof(szText), "成功设置%s为频道%s", utf82a(pVest->szName), GetNameByUserType(dwAddType) );
				SendChatMsg(szText);
			}
		}
	}
	else
	{
		char szText[128] = {0};

		// 如果操纵的是别人
		if( dwUDBID != m_UserInfo.context.dwUDBID )
		{
			// 判断是否有权限
			if( !GetRight(VoiceRight_Vest) )
			{
				SendChatMsg("你没有频道权限管理的权限！");
				return ;
			}

			SVoiceVestInfo * pVest = channel.GetVest(dwUDBID);
			if( pVest == NULL )
			{
				SendChatMsg("此用户不是普通管理员，无法删除！");
				return;
			}

			if( nMyUserType <= pVest->nNewUserType )
			{
				SendChatMsg("你只能删除权限等级比你低的管理员！");
				return;
			}
			sprintf_s( szText, sizeof(szText), "管理员已收回你%s身份", GetNameByUserType(pVest->nNewUserType) );

			channel.DelVest( dwUDBID, pUser );

			if( pUser != NULL )
			{
				CRoom * pCurRoom = pUser->GetCurRoom();
				if( NULL != pCurRoom )
				{
					// 如果当前用户已经是普通用户，并且在当前频道，有说话的话，就要关闭麦
					if( pCurRoom->GetChannel().GetID() == channel.GetID() )
					{
						// 没有说话权限
						if( !pUser->GetRight(VoiceRight_Speak) )
						{
							pUser->OpenMic(false);
						}
					}
				}

				pUser->SendChatMsg(szText);
			}

			SendChatMsg("成功删除管理员！");
		}
		else
		{
			SVoiceVestInfo * pVest = channel.GetVest(dwUDBID);
			if( pVest == NULL )
			{
				SendChatMsg("你不是管理员！");
				return;
			}

			channel.DelVest( dwUDBID, this );

            if( GetRight(VoiceRight_Speak) )
            {
                OpenMic(false);
            }

            SendChatMsg("你已辞去管理员身份！");
        }

        channel.UpdateChannelVestListToClient( m_dwClientID, VoiceUpdateType_All );
    }
}

// 设置私聊配置，接受配置
void CClientUser::SetChatConfig( DWORD dwConfigType )
{
	switch(dwConfigType)
	{
	case VoiceChatConfig_Defualt:	// 默认设置，接受任何人的私聊
		{
			if( GetFlag(VoiceUserStateFlag_BanChat) )
			{
				SetFlag( VoiceUserStateFlag_BanChat, false );
			}
			
			if( GetFlag(VoiceUserStateFlag_BanStrangerChat) )
			{
				SetFlag( VoiceUserStateFlag_BanStrangerChat, false );
			}
		}
		break;

	case VoiceChatConfig_Ban:		// 不接受任何私聊
		{
			if( !GetFlag(VoiceUserStateFlag_BanChat) )		// 没设置过的才设置下
			{
				SetFlag( VoiceUserStateFlag_BanChat, true );
			}
			
			if( GetFlag(VoiceUserStateFlag_BanStrangerChat) )
			{
				SetFlag( VoiceUserStateFlag_BanStrangerChat, false );
			}
		}
		break;
	
	case VoiceChatConfig_BanStranger:	// 不接受陌生人私聊
		{
			if( GetFlag(VoiceUserStateFlag_BanChat) )
			{
				SetFlag( VoiceUserStateFlag_BanChat, false );
			}

			if( !GetFlag(VoiceUserStateFlag_BanStrangerChat) )
			{
				SetFlag( VoiceUserStateFlag_BanStrangerChat, true );
			}
		}
		break;

	default:
		break;
	}
}

// 设置私聊对象ID和私聊用户类型
void CClientUser::SetChatIDAndType( DWORD dwID, DWORD dwUserType )
{
	m_ChatInfo.dwChatID = dwID;
	m_ChatInfo.dwUserType = dwUserType;

#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn( "m_ChatInfo.dwChatID=" << m_ChatInfo.dwChatID );
	ErrorLn( "m_ChatInfo.dwUserType=" << m_ChatInfo.dwUserType );
#endif
}

// 获取私聊用户类型
DWORD CClientUser::GetChatUserType(void)
{
	return m_ChatInfo.dwUserType;
}

// 获取私聊对象
CChat * CClientUser::GetChatInstance(void)
{
	return CChatList::getInstance().Get(m_ChatInfo.dwChatID);
}

// 邀请私聊
void CClientUser::ChatInvite( DWORD dwPDBIDAcceptor, bool bStranger )
{
	if( GetFlag(VoiceUserStateFlag_Chat) )
	{
		SendChatMsg("您正在私聊中，请挂断后再邀请！");
		return;
	}

	CClientUser * pAcceptor = CClientList::getInstance().GetClientByPDBID(dwPDBIDAcceptor);
	if( NULL == pAcceptor )
	{
		SendChatMsg("此用户未登录语音服务器，请稍后重试！");
		return;
	}
	
	// 判断私聊设置
	if( pAcceptor->GetFlag(VoiceUserStateFlag_BanChat) )
	{
		SendChatMsg("对方拒绝与任何人私聊！");
		return;
	}
	else if( pAcceptor->GetFlag(VoiceUserStateFlag_BanStrangerChat) && bStranger )
	{
		SendChatMsg("对方拒绝与陌生人私聊！");
		return;
	}

	DWORD dwTimeNow = (DWORD)time(NULL);

	// 判断上次接收(不是接受)到私聊邀请的时间是不是已经超过15秒
	if( dwTimeNow < pAcceptor->GetLastAcceptChatTime() + 15 )
	{
		SendChatMsg("对方正忙，请稍后再试！");
		return;
	}
	
	// 如果对方正在私聊中，直接返回
	if( pAcceptor->GetFlag(VoiceUserStateFlag_Chat) )
	{
		SendChatMsg("对方正在私聊中，请稍后重试！");
		return;
	}

	// 通过了，就可以创建私聊对象，发送请求到对方等待对方接收私聊
	CChat * pChat = CChatList::getInstance().Add();
	if( NULL == pChat )
	{
		SendChatMsg( "发送私聊邀请失败，请稍后重试！" );
		return;
	}

	// 初始化
	pChat->Create();

	// 设置接收私聊邀请时间
	pAcceptor->SetLastAcceptChatTime();

	if( pChat->Invite( *this, *pAcceptor ) )
	{
		SendChatMsg("已发送私聊邀请，请等待对方回应。");
	}
}

// 私聊邀请回应 dwInviter 邀请者客户端ID
void CClientUser::ChatInviteResponse( DWORD dwInviter, bool bAccept )
{
	CClientUser * pInviter = CClientList::getInstance().GetUser(dwInviter);
	if( NULL == pInviter )
	{
		SendChatMsg( "私聊邀请人已下线！" );
		return;
	}

	CChat * pChat = pInviter->GetChatInstance();
	if( NULL == pChat )
	{
		SendChatMsg( "私聊邀请人已取消私聊请求！" );
		return;
	}

	// 如果本次私聊里的邀请者和被邀请者ID不吻合，说明已不是本次私聊
	if( pChat->GetChatUser(VoiceChatUserType_Acceptor) != m_dwClientID || pChat->GetChatUser(VoiceChatUserType_Inviter) != pInviter->GetClientID() )
	{
		SendChatMsg( "此私聊邀请已过期！" );
		return;
	}

	// 如果不是等待状态，直接返回
	if( pChat->GetState() != VoiceChatState_WaitAccepter )
	{
		return;
	}

	if( bAccept )	// 接受此私聊
	{
		if( pChat->Accept( *pInviter, *this) )
		{
			pInviter->SendChatMsg( "对方已接受您的私聊邀请！" );
		}
	}
	else	// 拒绝此私聊
	{
		pChat->OnUserLeave(*this);
	}
}

// 关闭私聊
void CClientUser::LeaveChat(void)
{
	CChat * pChat = GetChatInstance();
	if( NULL == pChat )
	{
		return;
	}

	pChat->OnUserLeave(*this);
}

// 设置上次接收（不是接受）邀请时间
void CClientUser::SetLastAcceptChatTime(void)
{
	m_ChatInfo.dwLastAcceptTime = (DWORD)time(NULL);
}

// 获取上次接收（不是接受）邀请时间
DWORD CClientUser::GetLastAcceptChatTime(void)
{
	return m_ChatInfo.dwLastAcceptTime;
}


// 从DB登录一下,用来记录在线状态,和加载一下用户信息
void CClientUser::DBLogin(void)
{
	// 语音角色用户登录
	DB_Param_VoiceUser_Login dbParam;
	dbParam.dwPDBID			= m_UserInfo.context.dwPDBID;			// 角色的PDBID
	dbParam.dwUDBID			= m_UserInfo.context.dwUDBID;			// 通行证UDBID
	dbParam.dwSvrWorldID	= getThisGameWorldID();			// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
	dbParam.dwFromWorldID	= m_UserInfo.context.dwFromWorldID;		// 角色登陆所属游戏世界ID
	dbParam.nBelongID		= 0;
	sstrcpyn( dbParam.szActorName, m_UserInfo.context.szName, sizeof(dbParam.szActorName) );
	CDBRequester::getInstance().DB_VoiceUser_Login(dbParam,static_cast<IDBRetSink *>(this) );

	// 读取粉丝关系信息
	// 条件参数,找到dwFansUDBID的所有粉丝记录,并返回些用户关注的用户通行证UDBID列表,最多1000个
	DB_Param_VoiceFansUser_Read dbParam1;
	dbParam1.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
	dbParam1.dwFansUDBID		= m_UserInfo.context.dwUDBID;		// 粉丝的通行证UDBID
	CDBRequester::getInstance().DB_VoiceFansUser_Read(dbParam1,static_cast<IDBRetSink *>(this) );
}

// 从DB下线,用来记录在线状态
void CClientUser::DBLogout(void)
{
	// 没从数据库登录就不处理
	if (!m_bLoaded)
	{
		return;
	}
	// 语音角色用户登出
	DB_Param_VoiceUser_Logout dbParam;
	dbParam.dwPDBID			= m_UserInfo.context.dwPDBID;			// 角色的PDBID
	dbParam.dwSvrWorldID	= getThisGameWorldID();			// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
	CDBRequester::getInstance().DB_VoiceUser_Logout(dbParam);
}

// 语音角色用户进入频道,保存DB,暂时不用,有用时可调用一下来记录,这个调用频繁,所有没有必要,不要打开
void CClientUser::DBEnterChannel(void)
{
	// 语音角色用户进入频道
	DB_Param_VoiceUser_Enter dbParam;
	dbParam.dwPDBID			= m_UserInfo.context.dwPDBID;			// 角色的PDBID
	dbParam.dwSvrWorldID	= getThisGameWorldID();			// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
	LONGLONG nChannel = m_nRoomID;
    ((SVoiceChannelID*)&nChannel)->dwRoomID = 0;
    dbParam.nChannelID		= nChannel;					// 所在频道ID
	CDBRequester::getInstance().DB_VoiceUser_Enter(dbParam);
}


// 加入关注指定的玩家,用ClientID为参数
bool CClientUser::AddFansUser(DWORD dwClientID)
{
	CClientUser * pUser = CClientList::getInstance().GetUser(dwClientID);
	if( NULL == pUser )
	{
		SendChatMsg( "玩家不在线！" );
		return false;
	}
	// 获取用户详细信息
	SVoiceUserInfo & userInf = pUser->GetUserInfo();

	DWORD dwUDBID = userInf.dwUDBID; 
	// 是否已经存在了
	TMAP_VFansUser::iterator iter = m_mapFansUser.find(dwUDBID);
	if (iter!=m_mapFansUser.end())
	{
		SendChatMsg( "你已经关注了此玩家！" );
		return false;
	}
	
	m_mapFansUser.insert(TMAP_VFansUser::value_type(dwUDBID, dwUDBID));

	// 关注的人粉丝+1
	pUser->FansCountAdd(true);

	// 保存数据库
	pUser->FansAddToDB( m_UserInfo.context.dwUDBID, true );

	CRoom * pRoom = GetCurRoom();
	if( pRoom != NULL )
	{
		pRoom->UpdateFansInfoToClient(*this);
	}

	return true;
}

// 取消关注指定的玩家,用ClientID为参数
bool CClientUser::DelFansUser(DWORD dwClientID)
{
	CClientUser * pUser = CClientList::getInstance().GetUser(dwClientID);
	if( NULL == pUser )
	{
		SendChatMsg( "玩家不在线！" );
		return false;
	}
	// 获取用户详细信息
	SVoiceUserInfo & userInf = pUser->GetUserInfo();

	DWORD dwUDBID = userInf.dwUDBID;
	// 是否已经存在了
	TMAP_VFansUser::iterator iter = m_mapFansUser.find(dwUDBID);
	if (iter==m_mapFansUser.end())
	{
		SendChatMsg( "你没关注此玩家！" );
		return false;
	}

	m_mapFansUser.erase(iter);

	// 关注的人粉丝-1
	pUser->FansCountAdd(false);

	// 保存数据库
	pUser->FansAddToDB( m_UserInfo.context.dwUDBID, false );

	CRoom * pRoom = GetCurRoom();
	if( pRoom != NULL )
	{
		pRoom->UpdateFansInfoToClient(*this);
	}

	return true;

}


// 是否是指定通行证UDBID的粉丝
bool CClientUser::IsFansUser(DWORD dwUDBID)
{
	// 所有的关注的通行证ID列表
	TMAP_VFansUser::iterator iter = m_mapFansUser.find(dwUDBID);
	if (iter!=m_mapFansUser.end())
	{
		return true;
	}
	return false;
}

// 加减粉丝数,并同步到客户端
bool CClientUser::FansCountAdd(bool bAdd, bool bSync )
{
	if (bAdd)
	{
		m_UserInfo.context.dwFansCount++;
	}
	else if (m_UserInfo.context.dwFansCount>0)
	{
		m_UserInfo.context.dwFansCount--;
	}

	DWORD dwNowTicks = GetTickCount();

	// 同步要冷却1秒
	if (bSync && dwNowTicks> m_dwLastUpdateFansTime+1000)
	{
		m_dwLastUpdateFansTime = dwNowTicks;

		bool bSyncOther = false;
		CRoom * pRoom = GetCurRoom();
		if( pRoom != NULL )
		{
			bSyncOther = pRoom->IsMicUser(m_dwClientID);
		}

		// 同步麦上粉丝数,同步当前房间的所有观察者
        UpdateUserInfoToClient(bSyncOther, 1, VOICE_FIELD_FANSCOUNT);
	}

	return true;

}

// 加减粉丝到数据库
void CClientUser::FansAddToDB( DWORD dwFansUDBID, bool bAdd )
{
	// 保存数据库
	if( bAdd )
	{
		// 新增粉丝关系
		DB_Param_VoiceFansUser_Add dbParam;
		dbParam.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		// 要保存的数据
		dbParam.dwUDBID			= m_UserInfo.context.dwUDBID;		// 关注的用户通行证UDBID
		dbParam.dwFansUDBID		= dwFansUDBID;				// 粉丝的通行证UDBID

		CDBRequester::getInstance().DB_VoiceFansUser_Add( dbParam, static_cast<IDBRetSink*>(this) );
	}
	else
	{
		// 保存数据库

		// 删除粉丝关系
		DB_Param_VoiceFansUser_Del dbParam;
		dbParam.dwSvrWorldID	= getThisGameWorldID();		// 语音服所属游戏世界ID,作来操作来源服ID，现不用理会，扩展用
		// 要保存的数据
		dbParam.dwUDBID			= m_UserInfo.context.dwUDBID;					// 关注的用户通行证UDBID
		dbParam.dwFansUDBID		= dwFansUDBID;		// 粉丝的通行证UDBID

		CDBRequester::getInstance().DB_VoiceFansUser_Del( dbParam, static_cast<IDBRetSink*>(this) );
	}
}

// 获取数据库用户类型
DWORD CClientUser::GetDBUserType( DWORD dwChannelType )
{
	DWORD dwUserType = VoiceUserType_Normal;
	switch(m_UserInfo.context.dwDBUserType)
	{
	case VoiceAccountType_Art:		// 官方艺人
		if( dwChannelType == VoiceChannel_GM || dwChannelType == VoiceChannel_World )
		{
			//dwUserType = VoiceUserType_VIP;
		}
		break;

	case VoiceAccountType_Host:	// 官方主播
		if( dwChannelType == VoiceChannel_GM || dwChannelType == VoiceChannel_World )
		{
			dwUserType = VoiceUserType_Host;
		}
		break;

	case VoiceAccountType_Admin:		// 官方管理员
		dwUserType = VoiceUserType_Admin;
		break;

	case VoiceAccountType_GM:		// GM
		dwUserType = VoiceUserType_GM;
		break;

	default:
		break;
	}

	return dwUserType;
}

// 召集粉丝
DWORD CClientUser::CallFans( const char * szText, bool bLocal ,int nParam1,int nParam2 )
{
	if( szText == NULL || strlen(szText) <= 0 )
	{
		return 0;
	}

	DWORD dwTimeNow = (DWORD)time(NULL);

	// 判断是否已经冷却
	if( dwTimeNow <= m_dwLastCallFansTime + VOICE_CALL_FANS_COOL_TIME )
	{
		// 召集粉丝结果 data1:1 成功，0 失败， data2:剩余冷却时间， data3：召集的粉丝数
		gGlobalServer->SendVoiceDataToClient( m_dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_CallFans, 0, m_dwLastCallFansTime, 0 );
		return 0;
	}

	CRoom * pRoom = GetCurRoom();
	if( pRoom == NULL )
	{
		return 0;
	}
	
	// 如果不是个人频道
	if( pRoom->GetType() != VoiceChannel_Personal && pRoom->GetType() != VoiceChannel_GM )
	{
		SendChatMsg("只能在个人频道或官方频道召集粉丝");
		return 0;
	}

	// 清空粉丝接受和取消关注数据
	m_dwFansAcceptCount = 0;
	m_dwFansCancelCount = 0;

	DWORD * pClientIDArray = ((GlobalVoiceServer *)gGlobalServer)->m_dwClientIDBuff;
	DWORD dwCount = 0;
	CClientList::getInstance().GetFansIDList( pClientIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount, *this, bLocal );
	if( dwCount > 0 )
	{
		m_dwLastCallFansTime = dwTimeNow;

		((GlobalVoiceServer*)gGlobalServer)->AddCallFansInfo( m_UserInfo.context.dwUDBID, dwTimeNow );

		obuf1024 obufData;
		SGameMsgHead header;
		header.SrcEndPoint = MSG_ENDPOINT_VOICE;
		header.DestEndPoint= MSG_ENDPOINT_CLIENT;
		header.byKeyModule  = MSG_MODULEID_VOICE;
		header.byKeyAction  = MSG_VOICE_CALLFANS;			// 召集粉丝
		obufData.push_back( &header, sizeof(SGameMsgHead) );

		SVoiceCallFans_VC userData;
		userData.dwTime = 0;
		userData.dwClientID = m_dwClientID;			// 客户端ID
		userData.nID = pRoom->GetIDEx();				// 召集房间ID
		userData.nCountryID = (BYTE)GetChannelTypeID(VoiceChannel_Country);			// 国家ID
		userData.dwFromWorldID = m_UserInfo.context.dwFromWorldID;			// 世界ID
		userData.dwUDBID = m_UserInfo.context.dwUDBID;				// UDBID
		sstrcpyn( userData.szNickname, m_UserInfo.context.szName, sizeof(userData.szNickname) );		// 昵称
		userData.nMsgLen = strlen(szText);
		userData.nParam1 = nParam1;
		userData.nParam2 = nParam2;

		obufData.push_back( &userData, sizeof(SVoiceCallFans_VC) );
		
		// 文本信息
		obufData.push_back( szText, userData.nMsgLen );

		// 发送
		if( 1 == dwCount )
		{
			gGlobalServer->SendDataToClient( pClientIDArray[0], obufData.data(), obufData.size() );
		}
		else
		{
			gGlobalServer->BroadcastDataToClient( pClientIDArray, dwCount, obufData.data(), obufData.size() );
		}

		// 加入缓存队列
		SVoiceCallFansCacheData cacheData;
		cacheData.dwTime = (DWORD)time(NULL);			// 发送邀请的时间
		cacheData.dwClientID = m_dwClientID;			// 客户端ID
		cacheData.nID = userData.nID;					// 召集房间ID
		cacheData.nCountryID = userData.nCountryID;		// 国家ID
		cacheData.dwFromWorldID = userData.dwFromWorldID;
		cacheData.dwUDBID = m_UserInfo.context.dwUDBID;			// 发送邀请的人
		sstrcpyn( cacheData.szNickname, m_UserInfo.context.szName, sizeof(userData.szNickname) );		// 昵称
		cacheData.dwData1 = nParam1;
		cacheData.dwData2 = nParam2;
		sstrcpyn( cacheData.szText, szText, sizeof(cacheData.szText) );	// 邀请信息
		CVoiceCallList::getInstance().AddCallFansCacheData(cacheData);

	}

	// 召集粉丝结果 data1:1 成功，0 失败， data2:剩余冷却时间， data3：召集的粉丝数
	gGlobalServer->SendVoiceDataToClient( m_dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_CallFans, 1, m_dwLastCallFansTime, dwCount );

	
	return dwCount;
}

// 发送在线粉丝数到客户端
DWORD CClientUser::SendFansOnlineCount(void)
{
	DWORD * pClientIDArray = ((GlobalVoiceServer *)gGlobalServer)->m_dwClientIDBuff;
	DWORD dwCount = 0;
	CClientList::getInstance().GetFansIDList( pClientIDArray, VOICE_MAX_ROOM_USERCOUNT, dwCount, *this, false );

	gGlobalServer->SendVoiceDataToClient( m_dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_RequestFansOnlineCount, dwCount  );

	return dwCount;
}

// 增加接受邀请数
void CClientUser::AddFansAcceptCount(void)
{
	m_dwFansAcceptCount ++;

	OnFansCallInfoChange();
}

// 增加取消关注数
void CClientUser::AddFansCancelCount(void)
{
	m_dwFansCancelCount ++;

	OnFansCallInfoChange();
}

// 邀请粉丝后，有数据变化
void CClientUser::OnFansCallInfoChange(void)
{
	((GlobalVoiceServer*)gGlobalServer)->UpdateCallFansInfo( m_UserInfo.context.dwUDBID, m_dwFansAcceptCount, m_dwFansCancelCount );
}

// 发送粉丝召集结果到客户端
void CClientUser::SendFansCallResult(void)
{
	gGlobalServer->SendVoiceDataToClient( m_dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_RequestFansCallResult, m_dwFansAcceptCount, m_dwFansCancelCount );
}

// 设置上次邀请粉丝信息
void CClientUser::SetLastCallFansInfo( DWORD dwTime, DWORD dwFansAcceptCount, DWORD dwFansCancelCount )
{
	m_dwLastCallFansTime = dwTime;
	m_dwFansAcceptCount = dwFansAcceptCount;
	m_dwFansCancelCount = dwFansCancelCount;
}

// 更新昨日粉丝总数到客户端
void CClientUser::SetFansCountYesterday( DWORD dwFansCountYesterday )
{
	if( m_dwFansCountYesterday != dwFansCountYesterday )
	{
		m_dwFansCountYesterday = dwFansCountYesterday;
	}
}

// 进入对方所在的频道
bool CClientUser::EnterRoomByUser( CClientUser & user )
{
	CRoom * pRoom = user.GetCurRoom();
	if( pRoom == NULL )
	{
		SendChatMsg( "该玩家未进入任何语音频道" );
		return false;
	}

	LONGLONG nDesRoomID = pRoom->GetIDEx();
	// 如果对方所在的频道是官方或个人频道，直接进入即可
	if( pRoom->GetType() == VoiceChannel_GM || pRoom->GetType() == VoiceChannel_Personal )
	{
		EnterRoom( nDesRoomID, NULL, false );
		return true;
	}

	// 如果是其他频道，就要判断是否有权限可进
	const SVoiceChannelID & IDInfo = pRoom->GetChannel().GetIDInfo();
	
	// 获取我的所在的房间
	CRoom * pMyRoom = GetCurRoom();
	if( pMyRoom != NULL )
	{
		// 如果所在房间不为空
		LONGLONG nCurRoomID = GetCurRoomID();
		if( nDesRoomID == nCurRoomID )
		{
			SendChatMsg("你已在对方所在的频道！");
			return true;	
		}
	}

	LONGLONG nMyChannelID = GetChannelID(IDInfo.dwType);
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&nMyChannelID);
	if( IDInfo.dwType == pIDInfo->dwType && IDInfo.dwID == pIDInfo->dwID && IDInfo.dwWorldID == pIDInfo->dwWorldID )
	{
		EnterRoom( nDesRoomID, NULL, false );
		return true;
	}
	else
	{
		SendChatMsg( "对方所在的频道不是个人频道或官方频道，也不是与你的国家等相关的频道，无法进入！" );
		return false;
	}
}

// 设置某个频道的发送语音召集令的时间
void CClientUser::SetLastADTime( DWORD dwChannelType, DWORD dwTime )
{
	if( dwChannelType >= VoiceChannel_Max )
	{
		return;
	}

	m_dwLastADTime[dwChannelType] = dwTime;
}


//////////////////////////////////////////////////////////////////////////
// 粉丝关系读取完成
void CClientUser::OnLoadDBFansList(void)
{
	CVoiceCallList::getInstance().CheckCache(*this);
}