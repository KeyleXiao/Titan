/*******************************************************************
** 文件名:	ServerList.h 
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

#ifndef __SERVER_LIST_H__
#define __SERVER_LIST_H__

#include "ServerUser.h"
#include "Singleton.h"

#include <map>
using namespace std;



/**
@name : 语音服务器用户列表
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
