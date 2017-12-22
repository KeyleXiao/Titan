#pragma once
#include "UserList.h"

class ViewUser;

class ViewList : public UserList<ViewUser>, public Singleton<ViewList>
{
public:
	ViewList();
	virtual ~ViewList();

	// 取所有已准备好对外开放的View数量
	int		GetReadyCount();
	// 将所有已握手的Gate信息推送给所有View
	void	BroadcastGatesInfoToViews();

protected:
	virtual void onDelUser(ISessionUser* pUser) {};

	//注册这里要处理的所有消息，到m_Processer中
	virtual void RegisterHandlers();

	virtual DWORD GetMaxID() { return MAX_VIEW_SERVER_COUNT; }

public:
};
extern ViewList& gViewList;