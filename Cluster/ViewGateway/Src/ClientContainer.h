/*******************************************************************
** 文件名:	ClientContainer.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/06
** 版  本:	1.0
** 描  述:	观战客户端容器
** 应  用:  负责管理所有用客户端观战的连接
			（如果以后有其他脱离客户端观战的需求，可以再开一个Container处理）
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "UserContainer.h"


class ClientSession;

class ClientContainer : public UserContainer<ClientSession>, public Singleton<ClientContainer>
{
public:
	ClientContainer();
	virtual ~ClientContainer();

	ClientSession* GetByPlayerID(PlayerID dwPlayerID);

protected:
	virtual void onDelUser(ISessionUser* pUser) {};

	//注册这里要处理的所有消息，到m_Processer中
	virtual void RegisterHandlers();

	virtual DWORD GetMaxID();

};
extern ClientContainer& gClientContainer;