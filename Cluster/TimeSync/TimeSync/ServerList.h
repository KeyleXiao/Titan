/*******************************************************************
** 文件名:	ServerList.h 
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

#ifndef __SERVER_LIST_H__
#define __SERVER_LIST_H__

#include "ServerUser.h"
#include "Singleton.h"

#include <map>
using namespace std;


/**
@name : 服务器用户列表
@brief:
*/
class ServerList : public Singleton<ServerList>
{
public:
	bool AddUser(ServerUser * server);

	bool DelUser(ServerUser * server);

	int  Count() { return m_userList.size(); }

	ServerUser * GetUser(const DWORD & id );


	ServerList();

protected:

    bool    GetID(DWORD & id);
    void    RestoreID(const DWORD & id);

protected:
	std::list<DWORD>		        m_list;

    typedef std::map<DWORD,ServerUser*> USER_LIST; 
    USER_LIST                       m_userList;
};

#endif//__SERVER_LIST_H__
