/*******************************************************************
** 文件名:	ServerList.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/14/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ServerList.h"

//////////////////////////////////////////////////////////////////////////
#define  MAX_TCP_LIENT_COUNT    8192



ServerList::ServerList()
{
    m_list.clear();

    // 初始化客户端连接ID缓冲池
    for ( DWORD i=0;i<MAX_TCP_LIENT_COUNT;++i )
    {
        RestoreID(i);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
bool ServerList::GetID(DWORD & id)
{
    if ( m_list.empty() )
    {
        return false;
    }

    id = m_list.front();
    m_list.pop_front();

    return true;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
void ServerList::RestoreID(const DWORD & id)
{
    m_list.push_back(id);
}


bool ServerList::AddUser(ServerUser * server)
{
	DWORD id = 0;
    if( !GetID(id) )
    {
        ErrorLn("Add user fail for not id!!"<< server->ToString());
    }

	server->SetID(id);

	m_userList[id] = server;
	return true;
}

bool ServerList::DelUser(ServerUser * server)
{
    m_userList.erase(server->GetID());
	return true;
}

ServerUser * ServerList::GetUser(const DWORD & id )
{
    USER_LIST::iterator it = m_userList.find(id);
	if ( it==m_userList.end())
		return 0;
	else 
		return it->second;
}