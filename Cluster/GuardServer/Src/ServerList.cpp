/*******************************************************************
** 文件名:	ServerList.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器用户列表
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

bool ServerList::AddUser(ServerUser * server)
{
	m_dwMaxServerID++;

	DWORD id = m_dwMaxServerID;
	server->SetServerID(id);

	m_list[id] = server;

	return true;
}

bool ServerList::DelUser(ServerUser * server)
{
    m_list.erase(server->GetServerID());
	return true;
}

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
		SAFE_RELEASE((*it0));
	}

	m_dwMaxServerID = 0;
	m_list.clear();

}

ServerUser * ServerList::GetUser(const DWORD & dwID )
{
	SERVER_LIST::iterator it = m_list.find(dwID);
	if ( it==m_list.end())
		return 0;
	else 
		return it->second;
}