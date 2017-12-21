/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ClientList.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2012-11-21
** 版  本:	1.0
** 描  述:	语音客户端用户列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "ClientList.h"
#include <strstream>
#include "GlobalVoiceServer.h"

CClientList::CClientList()
{
	memset( m_ClientArray, 0, sizeof(m_ClientArray) );
}

CClientList::~CClientList()
{

}

/**
@name        : 添加一个客户端用户
@param client: 用户指针
@return      : 返回添加后的指针
*/
CClientUser * CClientList::AddUser(DWORD dwClientID)
{
	VClientIDInfo * pIDInfo = (VClientIDInfo *)&dwClientID;
	if (pIDInfo->dwGatewayID<=0 || pIDInfo->dwGatewayID>=VOICE_MAX_GATEWAYID || pIDInfo->dwConnIndex >= VOICE_MAX_CLIENT_COUNT)
	{
		return NULL;
	}

	// 获取对应网关客户端用户列表
	CLIENT_LIST &mlist = m_list[pIDInfo->dwGatewayID];

	// 查找记录
	CLIENT_LIST::iterator iter = mlist.find(dwClientID);

	CClientUser * pUser = NULL;

	// 用户已经存在
	if (iter != mlist.end())
	{
		pUser = iter->second;	// 返回原来的用户
		
		// 数组索引加快查找速度
		m_ClientArray[pIDInfo->dwGatewayID][pIDInfo->dwConnIndex] = pUser;
	}
	else
	{
		// 如果不存在，添加新的
		pUser = new CClientUser();
		if( NULL == pUser )
		{
			return NULL;
		}

		pUser->SetClientID(dwClientID);

		mlist.insert( CLIENT_LIST::value_type( dwClientID, pUser) );

		// 数组索引加快查找速度
		m_ClientArray[pIDInfo->dwGatewayID][pIDInfo->dwConnIndex] = pUser;
	}
	
	return pUser;
}

/**
@name        : 从用户列表中删除一个客户端用户
@param client: 用户指针
@return      : 如果找不到该用户则返回false
*/
bool CClientList::DelUser( DWORD dwClientID, DWORD dwPDBID, DWORD dwUDBID )
{
	VClientIDInfo * pIDInfo = (VClientIDInfo *)&dwClientID;

	// 网关ID不在合法范围内
	if (pIDInfo->dwGatewayID<=0 || pIDInfo->dwGatewayID>=VOICE_MAX_GATEWAYID || pIDInfo->dwConnIndex >= VOICE_MAX_CLIENT_COUNT)
	{
		return false;
	}

	// 获取对应网关客户端用户列表
	CLIENT_LIST &mlist = m_list[pIDInfo->dwGatewayID];

	// 移除
	mlist.erase(dwClientID);

	// 清除相关索引等信息，如果此处修改了，还必须同步修改CClientList::OnVoiceGatewayUpdateUserList中对应代码
	m_ClientArray[pIDInfo->dwGatewayID][pIDInfo->dwConnIndex] = NULL;

	// 清除PDBID索引
	DelPDBIDInfo( dwPDBID, dwClientID );

	// 清除UDBID索引
	DelUDBIDInfo( dwUDBID, dwClientID );

	return true;
}

/**
@name        : 根据ID索引一个用户,更快
@return      : 如果找不到该用户则返回0
*/
CClientUser * CClientList::GetUser(const DWORD dwClientID)
{
	VClientIDInfo * pIDInfo = (VClientIDInfo *)&dwClientID;
	if (pIDInfo->dwGatewayID<=0 || pIDInfo->dwGatewayID>=VOICE_MAX_GATEWAYID || pIDInfo->dwConnIndex >= VOICE_MAX_CLIENT_COUNT)
	{
		return NULL;
	}

	return m_ClientArray[pIDInfo->dwGatewayID][pIDInfo->dwConnIndex];
}

/**
@name        : 根据ID查找一个用户,用Map找查
@return      : 如果找不到该用户则返回0
*/
CClientUser * CClientList::FindUser(const DWORD dwClientID)
{
	VClientIDInfo * pIDInfo = (VClientIDInfo *)&dwClientID;
	if (pIDInfo->dwGatewayID<=0 || pIDInfo->dwGatewayID>=VOICE_MAX_GATEWAYID || pIDInfo->dwConnIndex >= VOICE_MAX_CLIENT_COUNT)
	{
		return NULL;
	}

	CLIENT_LIST &mlist = m_list[pIDInfo->dwGatewayID];
	// 找到记录
	CLIENT_LIST::iterator iter = mlist.find(dwClientID);

	// 不存在返回空
	if (iter==mlist.end())
	{
		return NULL;
	}

	return iter->second;
}

// 获得用户列表，如果网关ID为VOICE_INVALID_ID，dwServerID = VOICE_INVALID_ID则返回所有的用户
bool CClientList::GetUserList( CClientUser ** pUserList, DWORD dwCount, DWORD & dwGetCount, DWORD dwGatewayID, DWORD dwServerID )
{
	dwGetCount = 0;

	if( pUserList == NULL || dwCount == 0 )
	{
		return false;
	}

	// 取所有用户
	if( VOICE_INVALID_ID == dwGatewayID )
	{
		for( DWORD i=1; i<VOICE_MAX_GATEWAYID; i++ )
		{
			CLIENT_LIST &mlist = m_list[i];
			for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end();  ++it )
			{
				// 如果dwServerID为0，或者dwServerID和用户的id相等的都输出
				if( dwServerID == VOICE_INVALID_ID || dwServerID == it->second->GetServerID() )
				{
					*pUserList = it->second;
					pUserList ++;

					dwGetCount ++;
					if( dwGetCount >= dwCount )
					{
						return true;
					}

				}
			}
		}
	}
	else
	{
		if (dwGatewayID<=0 || dwGatewayID>=VOICE_MAX_GATEWAYID )
		{
			return false;
		}

		CLIENT_LIST & mlist = m_list[dwGatewayID];

		for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end(); ++it )
		{
			if( dwServerID == VOICE_INVALID_ID || dwServerID == it->second->GetServerID() )
			{
				*pUserList = it->second;
				pUserList ++;

				dwGetCount ++;
				if( dwGetCount >= dwCount )
				{
					return true;
				}
			}
		}
	}

	return true;
}

// 获得用户ID列表，如果网关ID为VOICE_INVALID_ID，dwServerID = VOICE_INVALID_ID则返回所有的用户
DWORD CClientList::GetClientIDListByServer( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, DWORD dwGatewayID, DWORD dwServerID, bool bSameServer )
{
	// 如果为空，直接返回
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;

	// 取所有网关
	if( VOICE_INVALID_ID == dwGatewayID )
	{
		// 如果是取全部用户
		if( dwServerID == VOICE_INVALID_ID )
		{
			for( DWORD i=1; i<VOICE_MAX_GATEWAYID; i++ )
			{
				CLIENT_LIST &mlist = m_list[i];
				for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end();  ++it )
				{
					if (dwFindCounts >= dwIDArrayCount )
					{
						break;
					}

					(*pClientID) = it->first;
					pClientID ++;
					dwFindCounts++;
				}
			}	
		}
		else
		{
			// 如果是取同一个服务器的上的用户
			if( bSameServer )
			{
				for( DWORD i=1; i<VOICE_MAX_GATEWAYID; i++ )
				{
					CLIENT_LIST &mlist = m_list[i];
					for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end();  ++it )
					{
						if( dwServerID == it->second->GetServerID() )
						{
							if (dwFindCounts >= dwIDArrayCount )
							{
								break;
							}

							(*pClientID) = it->first;
							pClientID ++;
							dwFindCounts++;
						}
					}
				}
			}
			else	// 如果不是取同一个服务器的上的用户
			{
				for( DWORD i=1; i<VOICE_MAX_GATEWAYID; i++ )
				{
					CLIENT_LIST &mlist = m_list[i];
					for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end();  ++it )
					{
						if( dwServerID != it->second->GetServerID() )
						{
							if (dwFindCounts >= dwIDArrayCount )
							{
								break;
							}

							(*pClientID) = it->first;
							pClientID ++;
							dwFindCounts++;
						}
					}
				}
			}
		}
	}
	else
	{
		if (dwGatewayID<=0 || dwGatewayID>=VOICE_MAX_GATEWAYID )
		{
			return false;
		}

		CLIENT_LIST & mlist = m_list[dwGatewayID];

		for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end(); ++it )
		{
			if( dwServerID == VOICE_INVALID_ID || dwServerID == it->second->GetServerID() )
			{
				if (dwFindCounts >= dwIDArrayCount )
				{
					break;
				}

				(*pClientID) = it->first;
				pClientID ++;
				dwFindCounts++;
			}
		}
	}

	dwGetCounts = dwFindCounts;

	return dwFindCounts;
}

// 根据类型和类型ID值，获取用户ID列表
DWORD CClientList::GetClientIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, DWORD dwChannelType, LONGLONG nID )
{
	// 如果为空，直接返回
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;

#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("CClientList::GetClientIDList dwChannelType=" << dwChannelType << ", nID=" << nID );
#endif

	// 如果是官方，则取所有人
	if( dwChannelType == VoiceChannel_GM )
	{
		for( DWORD i=1; i<VOICE_MAX_GATEWAYID; i++ )
		{
			CLIENT_LIST &mlist = m_list[i];
			for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end(); ++it )
			{
				if (dwFindCounts >= dwIDArrayCount )
				{
					break;
				}

				(*pClientID) = it->first;
				pClientID ++;
				dwFindCounts++;
			}
		}
	}
	else
	{
		for( DWORD i=1; i<VOICE_MAX_GATEWAYID; i++ )
		{
			CLIENT_LIST &mlist = m_list[i];
			for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end(); ++it )
			{
				CClientUser * pUser = it->second;
				if( pUser == NULL )
				{
					continue;
				}

#ifdef VOICE_PRINT_DEBUG_INF
				ErrorLn("CClientList::GetClientIDList dwTypeID=" << pUser->GetChannelID(dwChannelType) << ", User's ID=" << (pUser->GetChannelID(dwChannelType) & 0x00FFFFFFFFFFFFFF00) << ", nID=" << nID );
#endif

				// 把房间号置0 如果ID不相等，继续找下一个
				if( (pUser->GetChannelID(dwChannelType) & 0xFFFFFFFFFFFFFF00) != nID )
				{
					continue;
				}

				if (dwFindCounts >= dwIDArrayCount )
				{
					break;
				}

				(*pClientID) = it->first;
				pClientID ++;
				dwFindCounts++;
			}
		}
	}

	dwGetCounts = dwFindCounts;

	return dwFindCounts;
}

// 获取某个玩家的所有粉丝ID列表
DWORD CClientList::GetFansIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, CClientUser & user, bool bLocal )
{
	// 如果为空，直接返回
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;

	DWORD dwUserFromWorldID = user.GetUserInfo().dwFromWorldID;
	DWORD dwUDBID = user.GetUserInfo().dwUDBID;

	// 如果不是本地的话，不需要判断世界ID，提高查询效率
	if( bLocal )
	{
		for( DWORD i=1; i<VOICE_MAX_GATEWAYID; i++ )
		{
			CLIENT_LIST &mlist = m_list[i];
			for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end(); ++ it )
			{
				CClientUser * pUser = it->second;
				if( pUser == NULL )
				{
					continue;
				}

				// 如果取本地的话，来源世界ID不相等则跳过
				if( pUser->GetUserInfo().dwFromWorldID != dwUserFromWorldID )
				{
					continue;
				}

				// 如果不是粉丝跳过
				if( !pUser->IsFansUser(dwUDBID) )
				{
					continue;
				}

				if (dwFindCounts >= dwIDArrayCount )
				{
					break;
				}

				(*pClientID) = it->first;
				pClientID ++;
				dwFindCounts++;
			}
		}
	}
	else
	{
		for( DWORD i=1; i<VOICE_MAX_GATEWAYID; i++ )
		{
			CLIENT_LIST &mlist = m_list[i];
			for( CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end(); ++ it )
			{
				CClientUser * pUser = it->second;
				if( pUser == NULL )
				{
					continue;
				}

				// 如果不是粉丝跳过
				if( !pUser->IsFansUser(dwUDBID) )
				{
					continue;
				}

				if (dwFindCounts >= dwIDArrayCount )
				{
					break;
				}

				(*pClientID) = it->first;
				pClientID ++;
				dwFindCounts++;
			}
		}
	}

	dwGetCounts = dwFindCounts;

	return dwFindCounts;
}


// 清除指定网关上的用户,默认全清
void CClientList::Clear(DWORD dwGatewayID)
{
	if ( VOICE_INVALID_ID == dwGatewayID )
	{
		for (DWORD i=0;i<VOICE_MAX_GATEWAYID;i++)
		{
			list<CClientUser *> deletelist;

			CClientUser * pClientUser = NULL;
			for(CLIENT_LIST::iterator it = m_list[i].begin(); it != m_list[i].end(); ++it)
			{
				pClientUser = it->second;
				deletelist.push_back(pClientUser);
			}

			for (list<CClientUser *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); ++it0)
			{
				SafeRelease((*it0));
			}

			m_list[i].clear();
		}

		memset( m_ClientArray, 0, sizeof(m_ClientArray) );
	}
	else if(dwGatewayID>0 && dwGatewayID<VOICE_MAX_GATEWAYID )
	{
		list<CClientUser *> deletelist;

		CClientUser * pClientUser = NULL;
		for(CLIENT_LIST::iterator it = m_list[dwGatewayID].begin(); it != m_list[dwGatewayID].end(); ++it)
		{
			pClientUser = it->second;
			deletelist.push_back(pClientUser);
		}

		for (list<CClientUser *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); ++it0)
		{
			SafeRelease((*it0));
		}

		m_list[dwGatewayID].clear();

		memset( m_ClientArray[dwGatewayID], 0, sizeof(m_ClientArray[dwGatewayID]) );
	}
}

/**
@name        : 统计当前所有用户数
@return      : 返回用户数
*/
DWORD CClientList::CountUser()
{
	DWORD dwCounts = 0;
	for (DWORD i=1;i<VOICE_MAX_GATEWAYID;i++)
	{
		dwCounts += m_list[i].size();
	}

	return dwCounts;
}


// 保存用户信息列表到csv文件
void CClientList::SaveUserListToCSV()
{	
	char report_path[MAX_PATH];
	sprintf( report_path,"%s\\Report\\",getWorkDir());
	createDirectoryRecursive(report_path);
	char file_name[MAX_PATH];
	sprintf( file_name,"%s\\Report\\VoiceServer_UserInfoList.csv",getWorkDir());

	DWORD dwNowTicks = GetTickCount();
	DWORD dwNowTimes = (DWORD)time(NULL);

	DWORD dwUserCounts = 0;

	ostrstream buffer;

	buffer << "网关ID,客户端ID,世界ID,服务器ID,帐号,昵称,PDBID,等级,职业,性别,连入时间,IP,验证,在线,状态,用户类型,频道类型,说话,广播,麦序,官方频道,世界,国家,帮会,家族,团,队伍,个人," << std::endl;

	for( DWORD i = 1; i < VOICE_MAX_GATEWAYID; i ++ )
	{
		for(CLIENT_LIST::iterator iter = m_list[i].begin();iter!=m_list[i].end();++iter)
		{
			CClientUser *pClient = iter->second;
			if (NULL==pClient)
			{
				continue;
			}

			dwUserCounts++;

			SVoiceUserInfo &userinfo = pClient->GetUserInfo();

			buffer << pClient->GetGatewayID() << "," 
				<<  pClient->GetClientID() << "," 
				<< pClient->GetWorldID() << "," 
				<< pClient->GetServerID() << "," 
				<< userinfo.szName << "," 
				<< userinfo.dwPDBID << "," 
				<< GetTimeString(pClient->GetConnectTime()) << "," 
				<< pClient->GetClientIP() << "," 
				<< (DWORD)(pClient->GetLogin()) << "," 
				<< (DWORD)(pClient->IsOnline())  << ","
				<< (DWORD)(pClient->GetFlagValue())  << "," 
				<< (DWORD)(pClient->GetType())  << "," 
				<< pClient->GetCurChannelType()  << "," 
				<< (DWORD)(pClient->CanSpeak())  << "," 
				<< (DWORD)(pClient->NeedBroadcast())  << "," 
				<< pClient->GetMicIndex()  << "," 
				<< pClient->GetChannelTypeID(VoiceChannel_GM) <<":"<< (DWORD)(pClient->GetType(VoiceChannel_GM)) << "," 
				<< pClient->GetChannelTypeID(VoiceChannel_World) <<":"<< (DWORD)(pClient->GetType(VoiceChannel_World)) << "," 
				<< pClient->GetChannelTypeID(VoiceChannel_Country) <<":"<< (DWORD)(pClient->GetType(VoiceChannel_Country)) << "," 
				<< pClient->GetChannelTypeID(VoiceChannel_Clan)<<":"<< (DWORD)(pClient->GetType(VoiceChannel_Clan))  << "," 
				<< pClient->GetChannelTypeID(VoiceChannel_Kin) <<":"<< (DWORD)(pClient->GetType(VoiceChannel_Kin)) << "," 
				<< pClient->GetChannelTypeID(VoiceChannel_Group) <<":"<< (DWORD)(pClient->GetType(VoiceChannel_Group)) << "," 
				<< pClient->GetChannelTypeID(VoiceChannel_Team) <<":"<< (DWORD)(pClient->GetType(VoiceChannel_Team)) << "," 
				<< pClient->GetChannelTypeID(VoiceChannel_Personal) <<":"<< (DWORD)(pClient->GetType(VoiceChannel_Personal)) << "," 

				<< std::endl;
		}
	}

	std::string strSave = buffer.str();

	//WriteData2File((BYTE*)strSave.c_str(),strSave.size(),file_name);

	ErrorLn("保存用户("<<dwUserCounts<<"个)信息列表到csv文件成功:"<<file_name);

}

/** 写入数据到文件
@param  BYTE* pData:要保存的数据
@param  int nLens: 数据大小
@param  char * szFileName:要保存的文件名
@return  
*/	
void CClientList::WriteData2File(BYTE* pData,int nLens,const _TCHAR * szFileName)
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


/// 设置客户列表数组
void CClientList::SetClientArray( DWORD dwClientID, CClientUser * pUser )
{
	VClientIDInfo * pIDInfo = (VClientIDInfo *)&dwClientID;

	// 网关ID不在合法范围内
	if (pIDInfo->dwGatewayID<=0 || pIDInfo->dwGatewayID>=VOICE_MAX_GATEWAYID || pIDInfo->dwConnIndex >= VOICE_MAX_CLIENT_COUNT)
	{
		return ;
	}

	m_ClientArray[pIDInfo->dwGatewayID][pIDInfo->dwConnIndex] = pUser;
}

// 添加一个PDBID索引
DWORD CClientList::AddPDBIDInfo( DWORD dwPDBID, DWORD dwClientID )
{
	DWORD dwOldClientID = 0;

	TMAP_IDInfo::iterator itr = m_mapIDInfo.find(dwPDBID);
	if( itr == m_mapIDInfo.end() )
	{
		m_mapIDInfo.insert(TMAP_IDInfo::value_type(dwPDBID, dwClientID));
	}
	else
	{
		dwOldClientID = itr->second;

		// 修改为最新的客户端ID，老用户踢出时，就不会删除这个索引
		itr->second = dwClientID;
	}

	return dwOldClientID;
}

// 删除一个PDBID索引
void CClientList::DelPDBIDInfo( DWORD dwPDBID, DWORD dwClientID )
{	
	TMAP_IDInfo::iterator itr = m_mapIDInfo.find(dwPDBID);
	if( itr != m_mapIDInfo.end() && dwClientID == itr->second )
	{
		// 如果要删除的与列表中一致，则删除，如果不一致，说明已有重复登录，不需要删除
		m_mapIDInfo.erase(itr);
	}
}

// 添加一个UDBID索引 返回旧的clientID
DWORD CClientList::AddUDBIDInfo( DWORD dwUDBID, DWORD dwClientID )
{
	DWORD dwOldClientID = 0;

	TMAP_IDInfo::iterator itr = m_mapUDBIDInfo.find(dwUDBID);
	if( itr == m_mapUDBIDInfo.end() )
	{
		m_mapUDBIDInfo.insert(TMAP_IDInfo::value_type(dwUDBID, dwClientID));
	}
	else
	{
		dwOldClientID = itr->second;

		// 修改为最新的客户端ID，老用户踢出时，就不会删除这个索引
		itr->second = dwClientID;
	}

	return dwOldClientID;
}

// 删除一个UDBID索引
void CClientList::DelUDBIDInfo( DWORD dwUDBID, DWORD dwClientID )
{
	TMAP_IDInfo::iterator itr = m_mapUDBIDInfo.find(dwUDBID);
	if( itr != m_mapUDBIDInfo.end() && dwClientID == itr->second )
	{
		// 如果要删除的与列表中一致，则删除，如果不一致，说明已有重复登录，不需要删除
		m_mapUDBIDInfo.erase(itr);
	}
}


// 通过PDBID查找clientID
DWORD CClientList::GetClientIDByPDBID( DWORD dwPDBID )
{
	TMAP_IDInfo::iterator itr = m_mapIDInfo.find(dwPDBID);
	if( itr != m_mapIDInfo.end() )
	{
		return itr->second;
	}

	return 0;

}

// 通过UDBID查找clientID
DWORD CClientList::GetClientIDByUDBID( DWORD dwUDBID )
{
	TMAP_IDInfo::iterator itr = m_mapUDBIDInfo.find(dwUDBID);
	if( itr != m_mapUDBIDInfo.end() )
	{
		return itr->second;
	}

	return 0;

}

// 通过PDBID查找client对象
CClientUser *CClientList::GetClientByPDBID( DWORD dwPDBID )
{
	TMAP_IDInfo::iterator itr = m_mapIDInfo.find(dwPDBID);
	if( itr != m_mapIDInfo.end() )
	{
		return GetUser(itr->second);
	}

	return NULL;

}

// 通过UDBID查找client对象
CClientUser * CClientList::GetClientByUDBID( DWORD dwUDBID )
{
	TMAP_IDInfo::iterator itr = m_mapUDBIDInfo.find(dwUDBID);
	if( itr != m_mapUDBIDInfo.end() )
	{
		return GetUser(itr->second);
	}

	return NULL;
}

// 网关用户离开，需要把所有用户标志为离开
void CClientList::OnGatewayLeave( DWORD dwGatewayID )
{
	// 无效网关ID
	if (dwGatewayID<=0 || dwGatewayID>=VOICE_MAX_GATEWAYID )
	{
		return ;
	}

	CLIENT_LIST & mList = m_list[dwGatewayID];

	for( CLIENT_LIST::iterator it = mList.begin(); it != mList.end(); ++it )
	{
		CClientUser *pClientUser = it->second;
		pClientUser->SetOnline(false);
		pClientUser->SetState(VoiceUserState_Offline);
	}
}


// 服务器连入时，要更新该游戏世界下所有用户的社会服务器ID
bool CClientList::UpdateUserServerInfo( DWORD dwServerID, DWORD dwWorldID )
{
	if( dwWorldID == 0 || dwServerID == 0 )
	{
		return false;
	}

#ifdef VOICE_PRINT_DEBUG_INF
	DWORD dwCount = 0;
	TraceLn("CClientList::UpdateUserServerInfo dwServerID=" << dwServerID << ", worldID=" << dwWorldID);
#endif

	for( DWORD i = 1; i < VOICE_MAX_GATEWAYID; i ++ )
	{
		CLIENT_LIST & mList = m_list[i];

		for( CLIENT_LIST::iterator it = mList.begin(); it != mList.end(); ++it )
		{
			CClientUser *pClientUser = it->second;
			if( dwWorldID == pClientUser->GetWorldID() )
			{
#ifdef VOICE_PRINT_DEBUG_INF
				dwCount ++;
#endif
				pClientUser->SetServerInfo( dwServerID, dwWorldID );
			}
		}
	}

#ifdef VOICE_PRINT_DEBUG_INF
	TraceLn( "一共更新了" << dwCount << "个用户" );
#endif

	return true;
}


// 更新用户列表，网关同步用户到语音服务器用
void CClientList::OnVoiceGatewayUpdateUserList( DWORD dwGatewayID, SMsgVoiceGatewayClientNode * pClientNode, DWORD dwCount )
{
	if( pClientNode == NULL )
	{
		return ;
	}
	
	if( dwGatewayID <=0 || dwGatewayID >=VOICE_MAX_GATEWAYID )
	{
		return ;
	}

	CClientList::CLIENT_LIST &mlist = m_list[dwGatewayID];
	for( CClientList::CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end(); ++it )
	{
		it->second->m_nUpdateFlag = 3;		// 3 删除
	}

	for (DWORD i=0; i<dwCount; i++ )
	{
		CClientUser * pClientUser = FindUser(pClientNode->dwClientID);
		if( NULL == pClientUser )	// 如果不存在，添加
		{
			CClientUser * pClientUser = AddUser(pClientNode->dwClientID);
			if (pClientUser!=NULL)
			{
				pClientUser->m_nUpdateFlag = 1;	// 1 新增
				pClientUser->SetClientIP(pClientNode->dwRemoteIP);
				 gGlobalServer->OnClientUserEnter(pClientUser);
			}
		}
		else	// 如果存在，更新
		{
			pClientUser->m_nUpdateFlag = 2;	// 2 修改
			pClientUser->SetClientIP(pClientNode->dwRemoteIP);
		}

		pClientNode++;
	}

	// 清除不存在的用户
	for( CClientList::CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end(); )
	{
		CClientUser *pClientUser = it->second;
		if( pClientUser->m_nUpdateFlag == 3 )
		{
			// 告诉游戏服务器该连接断开了
			gGlobalServer->OnClientUserLeave(pClientUser);

			DWORD dwClientID = pClientUser->GetClientID();
			DWORD dwPDBID = pClientUser->GetUserInfo().dwPDBID;
			DWORD dwUDBID = pClientUser->GetUserInfo().dwUDBID;
			VClientIDInfo * pIDInfo = (VClientIDInfo *)&dwClientID;

			// 释放对象
			delete pClientUser;
			pClientUser = NULL;

			// 移除迭代器
			mlist.erase(it ++);
			m_mapIDInfo.erase(dwPDBID);
			m_mapUDBIDInfo.erase(dwUDBID);
			
			// 网关ID不在合法范围内
			if (pIDInfo->dwGatewayID<=0 || pIDInfo->dwGatewayID>=VOICE_MAX_GATEWAYID || pIDInfo->dwConnIndex >= VOICE_MAX_CLIENT_COUNT)
			{
				continue;
			}

			// 清除索引等信息
			m_ClientArray[pIDInfo->dwGatewayID][pIDInfo->dwConnIndex] = NULL;
		}
		else
		{
			it ++;
		}
	}
}


// 同步指定数量的正在语音用户列表给GM
void CClientList::UpdateVoiceListToClient( CClientUser & user, DWORD dwCount, DWORD dwSecond )
{	
	if ( dwCount > 300 )
	{
		//user.SendChatMsg( "最大数不能超过300！" );
		dwCount = 300;
	}

	dwSecond *= 1000;

	DWORD dwNow = ::GetTickCount();
	bool bExit = false;

	// 保证一个频道只有一条记录
	typedef map<LONGLONG, SMsgVoiceVoiceNode_VC> MapVoiceList;
	typedef map<DWORD,string> MapNameList;
	MapVoiceList tmpVoiceList;
	MapNameList tmpNameList;
	string strUserName;
	DWORD dwAllSize = 0;

	for( DWORD i = 1; i < VOICE_MAX_GATEWAYID; i ++ )
	{
		CLIENT_LIST & mlist = m_list[i];
		for( CClientList::CLIENT_LIST::iterator it = mlist.begin(); it != mlist.end();++it )
		{
			CClientUser *pClientUser = it->second;
			if (NULL==pClientUser)
			{
				continue;
			}

			// 取30秒内说话人
			if( dwNow > pClientUser->m_dwLastAudioTime + dwSecond )
			{
				continue;
			}

			// 私聊的用户跳过
			if( pClientUser->GetFlag(VoiceUserStateFlag_Chat) )
			{
				continue;
			}

			CRoom * pRoom = pClientUser->GetCurRoom();
			if( NULL == pRoom )
			{
				continue ;
			}

			LONGLONG nRoomID = pRoom->GetIDEx();

			// 查找是否存在，如果存在跳过
			MapVoiceList::iterator itRes = tmpVoiceList.find(nRoomID);
			if( itRes != tmpVoiceList.end() )
			{
				continue;
			}
			
			SVoiceUserInfo & userInfo = pClientUser->GetUserInfo();

			SVoiceChannelID * pIDInfo = (SVoiceChannelID *)(&nRoomID);
			SMsgVoiceVoiceNode_VC nodeData;
			nodeData.dwPDBID		= userInfo.dwPDBID;			// 说话人的PDBID
			nodeData.nChannelType	= (BYTE)(pIDInfo->dwType);		// 频道类型

			if( pIDInfo->dwWorldID > 0 )
			{
				nodeData.dwWorldID		= pIDInfo->dwWorldID;			// 世界ID
			}
			else
			{
				nodeData.dwWorldID		= userInfo.dwFromWorldID;			// 取人物身上的世界ID
			}

			nodeData.nRoomID		= (BYTE)(pIDInfo->dwRoomID);	// 子房间ID
			nodeData.dwID			= pIDInfo->dwID;
			nodeData.dwCount		= pRoom->Count();				// 人数
			nodeData.nTime			= WORD((dwNow - pClientUser->m_dwLastAudioTime)/1000);
			nodeData.nNameLen		= strlen(userInfo.szName);

			strUserName = userInfo.szName;
			tmpNameList.insert( MapNameList::value_type( userInfo.dwPDBID, strUserName ) );
			tmpVoiceList.insert( MapVoiceList::value_type(nRoomID, nodeData) );

			dwAllSize += sizeof(SMsgVoiceVoiceNode_VC) + nodeData.nNameLen;

			// 包满了不再添加
			if( (dwAllSize + sizeof(SMsgVoiceVoiceNode_VC)*5) >= MAX_NET_PACKAGE_SIZE )
			{
				bExit = true;
				break;
			}
			else if( tmpVoiceList.size() >= dwCount )		// 到数量了不在添加
			{
				bExit = true;
				break;
			}
		}

		if(bExit)
		{
			break;
		}
	}

	GlobalVoiceServer* pVoiceServer = (GlobalVoiceServer*)gGlobalServer;
	ofixbuf obufData(pVoiceServer->m_szDataBuf, sizeof(pVoiceServer->m_szDataBuf));
	ofixbuf obufName(pVoiceServer->m_szNameBuf, sizeof(pVoiceServer->m_szNameBuf));

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_VOICELIST;

	SMsgVoiceVoiceList_VC listData;
	listData.dwCount = tmpVoiceList.size();

	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&listData, sizeof(SMsgVoiceVoiceList_VC));

	for( MapVoiceList::iterator it2 = tmpVoiceList.begin(); it2 != tmpVoiceList.end(); ++it2  )
	{
		SMsgVoiceVoiceNode_VC & nodeData = it2->second;
		
		// 把名字加进去，如果找不到，要置0
		MapNameList::iterator itName = tmpNameList.find(nodeData.dwPDBID);
		if( itName == tmpNameList.end() )
		{
			nodeData.nNameLen = 0;
		}
		else
		{
			string & strName = itName->second;
			nodeData.nNameLen = strName.size();
			obufName.push_back( strName.c_str(), nodeData.nNameLen );
		}

		obufData.push_back( &nodeData, sizeof(SMsgVoiceVoiceNode_VC) );
	}

	// 最后把名字添加进去
	obufData.push_back( obufName.data(), obufName.size() );
	
	gGlobalServer->SendDataToClient( user.GetClientID(), obufData.data(), obufData.size() );
}




/** 取得时间字串
@param   DWORD dwTime:时间
@param   
@return  LPCSTR
*/
LPCSTR CClientList::GetTimeString(DWORD dwTime)
{
	static char szTimebuf[10][32] = {0};
	static BYTE nBufIndex = 0;

	nBufIndex++;
	if (nBufIndex>=10)
	{
		nBufIndex =0;
	}

	memset(szTimebuf[nBufIndex],0,sizeof(szTimebuf[nBufIndex]));
	if (dwTime==0)
	{
		return szTimebuf[nBufIndex];
	}
	time_t t = (time_t) dwTime;
	tm local;
	memset(&local,0,sizeof(local));
	localtime_s(&local,&t);

	sprintf_s(szTimebuf[nBufIndex], sizeof(szTimebuf[nBufIndex]), "%d-%.2d-%.2d %.2d:%.2d:%.2d",
		1900+local.tm_year,local.tm_mon+1,local.tm_mday,local.tm_hour,local.tm_min,local.tm_sec);

	return szTimebuf[nBufIndex];
}
