/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ClientUser.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2012-12-10
** 版  本:	1.0
** 描  述:	频道列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ChannelList.h"
#include <strstream>
#include "GlobalVoiceServer.h"

// 人数多的的排在前面
static bool TopListCompare(const SVoicePersonalTopData &elem1, const SVoicePersonalTopData &elem2)
{
	if( elem1.dwOwnerType > elem2.dwOwnerType )	// 如果帐号类型大的，排前面
	{
		return true;
	}
	else if( elem1.dwOwnerType == elem2.dwOwnerType )	// 如果帐号类型相等，就按人数排
	{
		if( elem1.dwUserCount > elem2.dwUserCount )
		{
			return true;
		}
		else if( elem1.dwUserCount == elem2.dwUserCount )	// 如果在线人数相等，则比较最高在线
		{
			if( elem1.dwMaxMemberCount > elem2.dwMaxMemberCount )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}


// 人数多的的排在前面
static bool SortListCompare(const SVoiceChannelSortInfo &elem1, const SVoiceChannelSortInfo &elem2)
{
	if( elem1.dwUserCount > elem2.dwUserCount )
	{
		return true;
	}

	return false;
}


// 构造函数
CChannelList::CChannelList(void)
	: m_dwTopListUpdateTime(0)
{
	
}

// 析构函数
CChannelList::~CChannelList(void)
{
	
}

bool CChannelList::Create(void)
{
	gGlobalServer->SetTimer( ETimerEventID_PersonalTopList, 60000, this, "CChannelList::CChannelList" );

	return true;
}

void CChannelList::Release(void)
{
	gGlobalServer->KillTimer( ETimerEventID_PersonalTopList, this );
}

// 定时器
void CChannelList::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_PersonalTopList:
		OnTimerUpdatePersonalTopList();
		break;

	default:
		break;
	}
}

// 定时器计算个人频道排行
void CChannelList::OnTimerUpdatePersonalTopList(void)
{
#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn(_GT("CChannelList::OnTimerUpdatePersonalTopList 准备计算新的个人频道排行"));
#endif

// 个人排行个数
#	define VOICE_MAX_PERSONAL_TOP_COUNT	50

	DWORD dwTimeNow = ::GetTickCount();
	TOP_LIST tmpList;

	for(CHANNEL_LIST::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		CChannel * pChannel = it->second;

		if( NULL != pChannel )
		{
			// 如果是个人频道 频道公开 人数大于6个人 有名称 就可以纳入排行榜了
			if( pChannel->CanInsertPersonalTopList() )
			{
				SVoicePersonalTopData data;
				data.dwUDBID	= pChannel->GetIDInfo().dwID;			// 通行证ID
				sstrcpyn( data.szOwnerName, pChannel->GetOwnerName(), sizeof(data.szOwnerName) );	// 名称
				data.dwUserCount = pChannel->Count();					// 在线人数
				data.dwMaxMemberCount = pChannel->GetMaxMemberCount();
				pChannel->GetOwnerWorldInfo( data.dwWorldID, data.dwCountryID );
				data.dwOwnerType = pChannel->GetBaseInfo().dwOwnerType;

				// 30秒内有说过话，说明有语音
				if( dwTimeNow <= pChannel->m_dwLastAudioTime + 60000 )
				{
					data.nState |= VoicePersonalTopState_Voice;
				}

				// 判断是否有人在视频直播
				for( int i = VoiceRoomType_Main; i < VoiceRoomType_Max; i ++ )
				{
					if( pChannel->GetRoom(i).IsVideoShow() )
					{
						data.nState |= VoicePersonalTopState_Video;
						break;
					}
				}

#ifdef VOICE_PRINT_DEBUG_INF
				char szBuf[512]={0};
				sprintf_s(szBuf, _countof(szBuf),"CVoiceClient::OnTimerUpdatePersonalTopList UDBID=%d, 名字=%s, 人数=%d, 历史最高在线=%d", data.dwUDBID, data.szOwnerName, data.dwUserCount, data.dwMaxMemberCount );
				TraceLn(szBuf);
#endif

				tmpList.push_back(data);
			}
		}
	}

	DWORD dwCount = tmpList.size();
	if( dwCount > 0 )
	{
		if( dwCount >= VOICE_MAX_PERSONAL_TOP_COUNT )
		{
			dwCount = VOICE_MAX_PERSONAL_TOP_COUNT;
		}

		partial_sort(tmpList.begin(), tmpList.begin()+dwCount, tmpList.end(), TopListCompare );
	}

	m_topList.clear();
	for( TOP_LIST::iterator it2 = tmpList.begin(); it2 != tmpList.end(); ++ it2 )
	{
		m_topList.push_back(*it2);
		if( m_topList.size() >= VOICE_MAX_PERSONAL_TOP_COUNT )
		{
			break;
		}
	}
	
	m_dwTopListUpdateTime = ::GetTickCount();

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn("CChannelList::OnTimerUpdatePersonalTopList 计算新的个人频道排行 结束 排行榜生成时间=" << m_dwTopListUpdateTime );
#endif
}


// 计算得到自动流量控制中的频道列表 dwMinOnline:最小人数要求,dwNeedMaxCount:最多取多少个频道
void CChannelList::GetAutoNetSpeedList(DWORD dwMinOnline,DWORD dwNeedMaxCount)
{

	CHANNELSORT_LIST tmpList;

	for(CHANNEL_LIST::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		CChannel * pChannel = it->second;

		if( NULL != pChannel )
		{
			// 人数大于
			DWORD dwOnlines = pChannel->CountEx(true);
			if( dwOnlines>=dwMinOnline )
			{
				SVoiceChannelSortInfo data;
				data.nChannelID = it->first;
				data.dwUserCount = dwOnlines;
				//data.dwUserData = pChannel->GetBaseInfo().dwQuality;

				tmpList.push_back(data);
			}
		}
	}

	DWORD dwCount = tmpList.size();
	if( dwCount > 0 )
	{
		if( dwCount >= dwNeedMaxCount )
		{
			dwCount = dwNeedMaxCount;
		}

		partial_sort(tmpList.begin(), tmpList.begin()+dwCount, tmpList.end(), SortListCompare );
	}

	m_autoNetSpeed.clear();
	for( CHANNELSORT_LIST::iterator it2 = tmpList.begin(); it2 != tmpList.end(); it2 ++ )
	{
		m_autoNetSpeed.push_back(*it2);
		if( m_autoNetSpeed.size() >= dwNeedMaxCount )
		{
			break;
		}
	}

}



// 增加频道
CChannel * CChannelList::Add( DWORD dwType, DWORD dwWorldID, DWORD dwID, bool & bNew )
{
	// 如果频道未开放，返回
	if( !GetChannelConfig(dwType).bOpen )
	{
		return NULL;
	}

	// 如果值为0，无频道，无需创建频道
	if( dwType != VoiceChannel_GM && dwID == 0 )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CChannelList::Add 创建频道失败 值为0，无频道，无需创建频道，dwID=" << dwID );
#endif
		return NULL;
	}

	SVoiceChannelID idInfo;
	idInfo.dwType		= dwType;		// 频道类型
	idInfo.dwWorldID	= dwWorldID;	// 世界ID
	idInfo.dwID			= dwID;			// 频道ID
	idInfo.dwRoomID		= 0;			// 房间ID
	
	LONGLONG *pID = (LONGLONG *)(&idInfo);

	CChannel * pChannel = NULL;

	CHANNEL_LIST::iterator it = m_list.find(*pID);
	if( it == m_list.end() )
	{
		pChannel = new CChannel(*pID);

		pChannel->Create();

		m_list.insert( CHANNEL_LIST::value_type(*pID, pChannel) );

		bNew = true;

		// 通知逻辑层房间创建
		gGlobalServer->OnChannelCreate(pChannel);

#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn(">>>CChannelList::Add 创建新频道 dwType=" << dwType << ", dwWorldID=" << dwWorldID << ", dwID=" << dwID );
#endif
	}
	else
	{
		pChannel = it->second;
		bNew = false;
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn(">>>CChannelList::Add 返回原来的频道，频道ID=" << pChannel->GetID() );
#endif
	}

	return pChannel;
}

// 删除频道
bool CChannelList::Del( LONGLONG nChannelID )
{
	CHANNEL_LIST::iterator it = m_list.find(nChannelID);
	if( it == m_list.end() )
	{
		return false;
	}

	// 释放由Channel对象自身操作，此处只删除索引
	//CChannel * pChannel = it->second;

	//pChannel->Release();

	m_list.erase(it);

	return true;
}

// 查找频道
CChannel * CChannelList::Get( LONGLONG nChannelID )
{
	CHANNEL_LIST::iterator it = m_list.find(nChannelID);
	if( it == m_list.end() )
	{
		return NULL;
	}

	return it->second;
}

// 查找频道
CChannel * CChannelList::Get( DWORD dwType, DWORD dwWorldID, DWORD dwID )
{
	SVoiceChannelID idInfo;
	idInfo.dwType		= dwType;		// 频道类型
	idInfo.dwWorldID	= dwWorldID;	// 世界ID
	idInfo.dwID			= dwID;			// 频道ID
	idInfo.dwRoomID		= 0;			// 房间ID

	LONGLONG *pID = (LONGLONG *)(&idInfo);

	CHANNEL_LIST::iterator it = m_list.find(*pID);
	if( it == m_list.end() )
	{
		return NULL;
	}

	return it->second;
}

// 查找房间
CRoom * CChannelList::GetRoom( LONGLONG nRoomID )
{
	if( nRoomID == 0 )
	{
		return NULL;
	}

	// 先取出房间ID，然后原频道ID里的房间ID置0
	SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&nRoomID);
	DWORD dwRoomID = pIDInfo->dwRoomID;
	pIDInfo->dwRoomID = 0;

	CHANNEL_LIST::iterator it = m_list.find(nRoomID);
	if( it == m_list.end() )
	{
#ifdef VOICE_PRINT_DEBUG_INF
		TraceLn("CChannelList::GetRoom 没找到频道");
#endif
		return NULL;
	}

	return it->second->GetRoomPtr(dwRoomID);
}


// 获取频道数量
DWORD CChannelList::Count(void)
{
	return m_list.size();
}

// 清除所有频道
void CChannelList::Clear(void)
{
	list<CChannel *> deletelist;

	CChannel * pCChannel = NULL;
	for(CHANNEL_LIST::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		pCChannel = it->second;
		deletelist.push_back(pCChannel);
	}

	for (list<CChannel *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); ++it0)
	{
		SafeRelease((*it0));
	}

	m_list.clear();
}

// 同步个人频道排行榜到客户端
void CChannelList::UpdatePersonalTopListToClient( DWORD dwClientID )
{
	if( dwClientID <= 0 )
	{
		return;
	}

	// 使用全局缓冲内存
	GlobalVoiceServer* pVoiceServer = (GlobalVoiceServer*)gGlobalServer;
	ofixbuf obufData(pVoiceServer->m_szDataBuf, sizeof(pVoiceServer->m_szDataBuf));
	ofixbuf obufName(pVoiceServer->m_szNameBuf, sizeof(pVoiceServer->m_szNameBuf));

	// 组个头
	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_PERSONALTOPLIST;		// 个人排行榜
	obufData.push_back( &header, sizeof(SGameMsgHead) );

	SVoicePersonalTopList_VC listData;
	listData.dwUpdateTime = m_dwTopListUpdateTime;
	listData.nCount = (BYTE)m_topList.size();
	obufData.push_back( &listData, sizeof(SVoicePersonalTopList_VC) );

	for( TOP_LIST::iterator it = m_topList.begin(); it != m_topList.end(); ++it  )
	{
		SVoicePersonalTopData & topData = *it;

		SVoicePersonalTopData_VC nodeData;
		nodeData.dwUDBID = topData.dwUDBID;			// 通行证ID
		nodeData.dwUserCount = topData.dwUserCount;		// 在线人数
		nodeData.nNameLen = strlen(topData.szOwnerName);			// 名称长度
		nodeData.dwMaxMemberCount = topData.dwMaxMemberCount;
		nodeData.dwWorldID = topData.dwWorldID;
		nodeData.nState = topData.nState;
		nodeData.nCountryID = (BYTE)topData.dwCountryID;
		nodeData.nOwnerType = (BYTE)topData.dwOwnerType;

		obufData.push_back( &nodeData, sizeof(SVoicePersonalTopData_VC) );

		if( nodeData.nNameLen > 0 )	// 名称
		{
			obufName.push_back( topData.szOwnerName, nodeData.nNameLen );
		}
	}

	obufData.push_back( obufName.data(), obufName.size() );

	// 发送
	gGlobalServer->SendDataToClient( dwClientID, obufData.data(), obufData.size() );
}

// 获取个人频道排行榜更新时间
DWORD CChannelList::GetPersonalTopListUpdateTime(void)
{
	return m_dwTopListUpdateTime;
}




// 保存用户信息列表到csv文件
void CChannelList::SaveUserListToCSV()
{	
	char report_path[MAX_PATH];
	sprintf( report_path,"%s\\Report\\",getWorkDir());
	createDirectoryRecursive(report_path);
	char file_name[MAX_PATH];
	sprintf( file_name,"%s\\Report\\VoiceServer_ChannelList.csv",getWorkDir());

	DWORD dwChannelCounts = 0;

	ostrstream buffer;

	buffer << "频道UID,类型,世界ID,频道ID值,大厅状态标记,大厅麦模式,大厅人数,大厅观察者数,大厅麦序数,统战状态标记,统战麦模式,统战人数,统战观察者数,统战麦序数,娱乐状态标记,娱乐麦模式,娱乐人数,娱乐观察者数,娱乐麦序数,聊天状态标记,聊天麦模式,聊天人数,聊天观察者数,聊天麦序数,挂机状态标记,挂机麦模式,挂机人数,挂机观察者数,挂机麦序数,会议状态标记,会议麦模式,会议人数,会议观察者数,会议麦序数,办公状态标记,办公麦模式,办公人数,办公观察者数,办公麦序数,密室状态标记,密室麦模式,密室人数,密室观察者数,密室麦序数," << std::endl;

	for( CHANNEL_LIST::iterator iter = m_list.begin(); iter!= m_list.end(); ++iter)
	{
		CChannel * pChannel = iter->second;
		if( NULL == pChannel )
		{
			continue;
		}

		dwChannelCounts ++;

		buffer << pChannel->GetID() << "," 
			<<  pChannel->GetType() << "," 
			<<  pChannel->GetIDInfo().dwWorldID << "," 
			<<  pChannel->GetIDInfo().dwID << ","; 

		// 大厅状态标记,大厅麦模式,大厅人数,大厅观察者数,大厅麦序数,
		for( DWORD i = 0; i < VoiceRoomType_Max; i ++ )
		{
			CRoom & room = pChannel->GetRoom(i);
			buffer << room.GetFlagValue() << "," 
				   << room.GetMicState() << "," 
				   << room.Count() << "," 
				   << pChannel->GetObserversCount(i) << "," 
				   << room.GetMicOrderCount() << ","; 
		}

		buffer << std::endl;
			
	}

	std::string strSave = buffer.str();

	//WriteData2File((BYTE*)strSave.c_str(),strSave.size(),file_name);

	ErrorLn("保存频道("<<dwChannelCounts<<"个)信息列表到csv文件成功:"<<file_name);

}


/** 写入数据到文件
@param  BYTE* pData:要保存的数据
@param  int nLens: 数据大小
@param  char * szFileName:要保存的文件名
@return  
*/	
void CChannelList::WriteData2File(BYTE* pData,int nLens,const _TCHAR * szFileName)
{
	try
	{
		BOOL   bSuccess ;
		DWORD  dwBytesWritten ;
		HANDLE hFile= CreateFile (szFileName, GENERIC_WRITE, 0, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

		if (hFile == INVALID_HANDLE_VALUE)
		{
			return ;
		}

		bSuccess = WriteFile (hFile, pData, nLens, &dwBytesWritten, NULL) ;

		CloseHandle (hFile) ;
	}
	catch(...)
	{

	}
}

// 取得频道列表引用，特殊情况下只读遍历使用
CChannelList::CHANNEL_LIST & CChannelList::GetList(void)
{
	return m_list;
}