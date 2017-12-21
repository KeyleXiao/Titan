/*******************************************************************
** 文件名:	ServerList.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音服务器用户列表
** 应  用:  		

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ServerList.h"



// 构造函数
ServerList::ServerList(void)
{
	m_dwMaxServerID = 0;
}

// 析构函数
ServerList::~ServerList(void)
{
	Clear();
}

/// purpose: 添加服务器用户
bool ServerList::AddUser(ServerUser * server)
{
	m_dwMaxServerID++;

	DWORD id = m_dwMaxServerID;
	server->SetServerID(id);

	m_list[id] = server;

	return true;
}

/// purpose: 删除服务器用户
bool ServerList::DelUser(ServerUser * server)
{
    m_list.erase(server->GetServerID());
	return true;
}

/// purpose: 清除所有用户
void ServerList::Clear(void)
{
	list<ServerUser *> deletelist;

	ServerUser * pServerUser = NULL;
	for(SERVER_LIST::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		pServerUser = it->second;
		deletelist.push_back(pServerUser);
	}

	for (list<ServerUser *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); ++it0)
	{
		SafeRelease((*it0));
	}

	m_dwMaxServerID = 0;

	m_list.clear();
}

/// purpose: 获得用户
ServerUser * ServerList::GetUser(const DWORD & dwID )
{
	SERVER_LIST::iterator it = m_list.find(dwID);
	if ( it==m_list.end())
		return 0;
	else 
		return it->second;
}