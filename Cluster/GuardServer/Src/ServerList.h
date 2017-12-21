/*******************************************************************
** 文件名:	ServerList.h 
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

#ifndef __SERVER_LIST_H__
#define __SERVER_LIST_H__

#include "ServerUser.h"
#include "Singleton.h"

#include <map>
using namespace std;



/**
@name : 数据监护服务器用户列表
@brief:
*/
class ServerList : public Singleton<ServerList>
{
public:

	typedef std::map<DWORD,ServerUser*> SERVER_LIST;

	SERVER_LIST         m_list;


	bool AddUser(ServerUser * server);

	bool DelUser(ServerUser * server);

	int  Count() { return m_list.size(); }
	DWORD  GetMaxServerID() { return m_dwMaxServerID; }

	void Clear(void);

	ServerUser * GetUser(const DWORD & id );

	// 构造函数
	ServerList(void);

	// 析构函数
	virtual ~ServerList(void);


protected:

	DWORD				m_dwMaxServerID;
};

#endif//__SERVER_LIST_H__
