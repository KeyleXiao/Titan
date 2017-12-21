/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\ClientList.cpp
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

/// purpose: 添加一个客户端用户
bool CClientList::AddUser(CClientUser * pClient)
{
	if (NULL == pClient)
	{
		return false;
	}

	DWORD dwID = 0;
	if ( !GenerateID(dwID) )
	{
		return false;
	}

	DWORD dwIndex = ID2Index(dwID);

	if (dwIndex >= VOICE_MAX_CLIENT_COUNT)
	{
		return false;
	}

	pClient->SetID(dwID);
	m_list[dwIndex] = pClient;
	m_dwCounts++;

	return true;
}

/// purpose: 删除用户
bool CClientList::DelUser(CClientUser * pClient)
{
	if (NULL==pClient)
	{
		return false;
	}

	DWORD dwID = pClient->GetID();
	DWORD dwIndex = ID2Index(dwID);

	if (dwIndex >= VOICE_MAX_CLIENT_COUNT)
	{
		return false;
	}

	if (NULL == m_list[dwIndex])
	{
		return false;
	}

	RestoreID(dwID);
	m_list[dwIndex] = NULL;
	if (m_dwCounts > 0)
	{
		m_dwCounts--;
	}

	return true;
}

/// purpose: 通过客户端ID查找用户
CClientUser * CClientList::FindUser(const DWORD dwID)
{
	DWORD dwIndex = ((VClientIDInfo *)&dwID)->dwConnIndex;
	if (dwIndex >= VOICE_MAX_CLIENT_COUNT )
	{
		return NULL;
	}

	return m_list[dwIndex];
}

/// purpose: 通过IP和端口查找用户
CClientUser * CClientList::FindUser( const char * szIP, const WORD nPort )
{
	for (int i=0; i < VOICE_MAX_CLIENT_COUNT; i++)
	{
		CClientUser * pClient = m_list[i];
		if (NULL==pClient)
		{
			continue;
		}

		if ( pClient->GetPort()== nPort && strcmp( pClient->GetClientIP(),szIP )==0 )
		{
			return pClient;
		}
	}

	return NULL;
}

/// purpose: 返回用户数量
DWORD CClientList::CountUser()
{
	return m_dwCounts;
}

/// purpose: 清除所有用户
void CClientList::Clear(void)
{
	for( int i = 0; i < VOICE_MAX_CLIENT_COUNT; i ++ )
	{
		CClientUser *pUser = m_list[i];
		if( NULL != pUser )
		{
			SafeRelease(pUser);
		}
	}
}

/// 用索引查询用户
CClientUser * CClientList::GetUserByIndex(DWORD dwIndex)
{
	if (dwIndex >= VOICE_MAX_CLIENT_COUNT )
	{
		return NULL;
	}
	return m_list[dwIndex];
}

/// purpose: 设置网关ID
void  CClientList::SetGatewayID(DWORD dwID) 
{
	// 重起了
	m_IDStock.clear();

	m_dwGatewayID = dwID; 

	// 初始化客户端连接ID缓冲池
	for ( DWORD i=0;i<VOICE_MAX_CLIENT_COUNT;++i )
	{
		RestoreID(Index2ID(i));
	}
}

/// purpose: 获得网关ID
DWORD CClientList::GetGatewayID()
{ 
	return m_dwGatewayID; 
}

// 保存用户信息列表到csv文件
void CClientList::SaveUserListToCSV()
{	

	char report_path[MAX_PATH];
	sprintf( report_path,"%s\\Report\\",getWorkDir());
	createDirectoryRecursive(report_path);
	char file_name[MAX_PATH];
	sprintf( file_name,"%s\\Report\\VoiceGateway_UserInfoList.csv",getWorkDir());

	DWORD dwNowTicks = GetTickCount();
	DWORD dwNowTimes = (DWORD)time(NULL);

	ostrstream buffer;

	buffer << "网关ID,客户端ID,IP,端口,网卡地址,帐号,昵称,PDBID,连入时间" << std::endl;

	for (DWORD i=0;i<VOICE_MAX_CLIENT_COUNT;i++)
	{
		CClientUser * pClient = m_list[i];
		if (NULL==pClient)
		{
			continue;
		}

		buffer << m_dwGatewayID << "," 
			<< pClient->GetID() << "," 
			<< pClient->GetClientIP() << "," 
			<< pClient->GetPort() << "," 
			<< pClient->GetMacAddress() << "," 
			<< pClient->m_szUsername.c_str() << "," 
			<< pClient->m_szNickname.c_str() << "," 
			<< pClient->m_dwPDBID << ","
			<< GetTimeString(pClient->GetConnectTime())
			<< std::endl;
	}

	std::string strSave = buffer.str();

	WriteData2File((BYTE*)strSave.c_str(),strSave.size(),file_name);

	ErrorLn(_GT("保存用户信息列表到csv文件成功:")<<file_name);

}

/** 写入数据到文件
@param  BYTE* pData:要保存的数据
@param  int nLens: 数据大小
@param  char * szFileName:要保存的文件名
@return  
*/	
void CClientList::WriteData2File(BYTE* pData,int nLens,const char * szFileName)
{
	try
	{
		BOOL   bSuccess ;
		DWORD  dwBytesWritten ;
		HANDLE hFile= CreateFileA (szFileName, GENERIC_WRITE, 0, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

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

/// purpose: 生成ID
bool CClientList::GenerateID(DWORD & dwID)
{
	if ( m_IDStock.empty() )
	{
		return false;
	}

	dwID = m_IDStock.front();
	m_IDStock.pop_front();

	return true;
};

/// purpose: 归还一个客户端ID
void CClientList::RestoreID(const DWORD & dwID)
{
	m_IDStock.push_back(dwID);
}

/// purpose: 从ID转成索引
DWORD CClientList::ID2Index(const DWORD & dwID)
{
	return ((VClientIDInfo *)&dwID)->dwConnIndex;
}

/// 从索引转成ID
DWORD CClientList::Index2ID(DWORD dwIndex)
{
	VClientIDInfo data;
	data.dwGatewayID = m_dwGatewayID;
	data.dwConnIndex = dwIndex;

	return *(DWORD*)&data;
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
