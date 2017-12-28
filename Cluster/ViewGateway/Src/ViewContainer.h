/*******************************************************************
** 文件名:	ViewList.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/04
** 版  本:	1.0
** 描  述:	Gate上的ViewServer连接List
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "UserContainer.h"


class ViewSession;

class ViewContainer : public UserContainer<ViewSession, ViewID>, public Singleton<ViewContainer>
{
public:
	ViewContainer();
	virtual ~ViewContainer();

protected:
	virtual void	OnDelUser(ISessionUser<ViewID>* pUser) {};

	//注册这里要处理的所有消息，到m_Processer中
	virtual void	RegisterHandlers();

	virtual ViewID	GetMaxID() { return MAX_VIEW_SERVER_COUNT; }

};
extern ViewContainer& gViewContainer;