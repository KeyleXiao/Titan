/*******************************************************************
** 文件名:	GatewayList.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音网关用户列表
** 应  用:  		

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "GatewayList.h"



// 构造函数
GatewayList::GatewayList(void)
{
	m_dwMaxServerID = 0;
	memset( m_GatewayList, 0, sizeof(m_GatewayList) );
}

// 析构函数
GatewayList::~GatewayList(void)
{
	Clear();
}

bool GatewayList::AddUser(GatewayUser * server)
{
	m_dwMaxServerID++;

	DWORD id = m_dwMaxServerID;
	server->SetServerID(id);

	m_list[id] = server;

	// 网关ID的在AddUserByGatewayID中处理

	return true;
}

bool GatewayList::DelUser(GatewayUser * server)
{
	m_list.erase(server->GetServerID());

	DWORD dwGatewayID = server->GetGatewayID();
	if( dwGatewayID > 0 && dwGatewayID < VOICE_MAX_GATEWAYID )
	{
		m_GatewayList[dwGatewayID] = NULL;
	}

	return true;
}

void GatewayList::Clear(void)
{
	list<GatewayUser *> deletelist;

	GatewayUser * pGatewayUser = NULL;
	for(GATEWAY_LIST::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		pGatewayUser = it->second;
		deletelist.push_back(pGatewayUser);
	}

	for (list<GatewayUser *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); ++it0)
	{
		SafeRelease((*it0));
	}

	m_dwMaxServerID = 0;
	m_list.clear();
	memset( m_GatewayList, 0, sizeof(m_GatewayList) );

}

GatewayUser * GatewayList::GetUser(const DWORD & dwID )
{
	GATEWAY_LIST::iterator it = m_list.find(dwID);
	if ( it==m_list.end())
		return 0;
	else 
		return it->second;
}

GatewayUser * GatewayList::GetUserByGatewayID( const DWORD & dwGatewayID )
{
	if( dwGatewayID <= 0 || dwGatewayID >= VOICE_MAX_GATEWAYID )
	{
		return NULL;
	}

	return m_GatewayList[dwGatewayID];
	
}

bool GatewayList::AddUserByGatewayID(const DWORD & dwGatewayID, GatewayUser * server )
{
	if( dwGatewayID <= 0 || dwGatewayID >= VOICE_MAX_GATEWAYID )
	{
		return false;
	}

	if( NULL == server )
	{
		return false;
	}

	if( GetUserByGatewayID(dwGatewayID) != NULL )
	{
		return false;
	}
	
	server->SetGatewayID(dwGatewayID);
	m_GatewayList[dwGatewayID] = server;

	return true;
}