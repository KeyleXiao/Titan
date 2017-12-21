/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\AudioList.cpp
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
#include "AudioList.h"
#include <strstream>

/// purpose: 添加一个客户端用户
bool CAudioList::AddUser(CAudioUser * pClient)
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

	if (dwID >= VOICE_MAX_CLIENT_COUNT)
	{
		ErrorLn(_GT("CAudioList::AddUser 语音通道ID已经全部用完！！"));
		return false;
	}

	pClient->SetID(dwID);
	m_list[dwID] = pClient;
	m_dwCounts++;

	return true;
}

/// purpose: 删除用户
bool CAudioList::DelUser(CAudioUser * pClient)
{
	if (NULL==pClient)
	{
		return false;
	}

	DWORD dwID = pClient->GetID();

	if (dwID >= VOICE_MAX_CLIENT_COUNT)
	{
		return false;
	}

	if (NULL == m_list[dwID])
	{
		return false;
	}

	RestoreID(dwID);
	m_list[dwID] = NULL;
	if (m_dwCounts > 0)
	{
		m_dwCounts--;
	}

	return true;
}

/// purpose: 通过客户端ID查找用户
CAudioUser * CAudioList::FindUser(const DWORD dwID)
{
	if (dwID >= VOICE_MAX_CLIENT_COUNT )
	{
		return NULL;
	}

	return m_list[dwID];
}

/// purpose: 通过IP和端口查找用户
CAudioUser * CAudioList::FindUser( const char * szIP, const WORD nPort )
{
	for (int i=0; i < VOICE_MAX_CLIENT_COUNT; i++)
	{
		CAudioUser * pClient = m_list[i];
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
DWORD CAudioList::CountUser()
{
	return m_dwCounts;
}

/// purpose: 清除所有用户
void CAudioList::Clear(void)
{
	for( int i = 0; i < VOICE_MAX_CLIENT_COUNT; i ++ )
	{
		CAudioUser *pUser = m_list[i];
		if( NULL != pUser )
		{
			SafeRelease(pUser);
		}
	}
}


/// purpose: 设置网关ID
void  CAudioList::SetGatewayID(DWORD dwID) 
{
	// 重起了
	m_IDStock.clear();

	m_dwGatewayID = dwID; 

	// 初始化客户端连接ID缓冲池
	for ( DWORD i=0;i<VOICE_MAX_CLIENT_COUNT;++i )
	{
		RestoreID(i);
	}
}

/// purpose: 获得网关ID
DWORD CAudioList::GetGatewayID()
{ 
	return m_dwGatewayID; 
}

/// purpose: 生成ID
bool CAudioList::GenerateID(DWORD & dwID)
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
void CAudioList::RestoreID(const DWORD & dwID)
{
	m_IDStock.push_back(dwID);
}


/** 取得时间字串
@param   DWORD dwTime:时间
@param   
@return  LPCSTR
*/
LPCSTR CAudioList::GetTimeString(DWORD dwTime)
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
