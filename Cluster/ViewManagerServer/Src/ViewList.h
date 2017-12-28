#pragma once
#include "UserList.h"

class ViewUser;

class ViewList : public UserList<ViewUser, ViewID>, public Singleton<ViewList>
{
public:
	ViewList();
	virtual ~ViewList();

	// ȡ������׼���ö��⿪�ŵ�View����
	int		GetReadyCount();
	// �����������ֵ�Gate��Ϣ���͸�����View
	void	BroadcastGatesInfoToViews();

protected:
	virtual void OnDelUser(ISessionUser<ViewID>* pUser) {};

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void RegisterHandlers();

	virtual ViewID GetMaxID() { return MAX_VIEW_SERVER_COUNT; }

public:
};
extern ViewList& gViewList;