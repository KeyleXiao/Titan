/*******************************************************************
** 文件名:	GatewayList.h 
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

#ifndef __GATEWAY_LIST_H__
#define __GATEWAY_LIST_H__

#include "GatewayUser.h"
#include "Singleton.h"

#include <map>
using namespace std;


/**
@name : 语音网关用户列表
@brief:
*/
class GatewayList : public Singleton<GatewayList>
{
public:

	typedef std::map<DWORD,GatewayUser*> GATEWAY_LIST;

	GATEWAY_LIST         m_list;


	bool AddUser(GatewayUser * server);

	bool DelUser(GatewayUser * server);

	int  Count() { return m_list.size(); }

	DWORD  GetMaxServerID() { return m_dwMaxServerID; }

	void Clear(void);

	GatewayUser * GetUser(const DWORD & id );

	GatewayUser * GetUserByGatewayID( const DWORD & dwGatewayID );

	bool AddUserByGatewayID(const DWORD & dwGatewayID, GatewayUser * server );

	// 构造函数
	GatewayList(void);

	// 析构函数
	virtual ~GatewayList(void);

private:
	GatewayUser *		m_GatewayList[VOICE_MAX_GATEWAYID+1];
	DWORD				m_dwMaxServerID;
};

#endif//__GATEWAY_LIST_H__
