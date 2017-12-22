#pragma once
#include "UserList.h"

class ViewUser;

class ViewList : public UserList<ViewUser>, public Singleton<ViewList>
{
public:
	ViewList();
	virtual ~ViewList();

	// ȡ������׼���ö��⿪�ŵ�View����
	int		GetReadyCount();
	// �����������ֵ�Gate��Ϣ���͸�����View
	void	BroadcastGatesInfoToViews();

protected:
	virtual void onDelUser(ISessionUser* pUser) {};

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void RegisterHandlers();

	virtual DWORD GetMaxID() { return MAX_VIEW_SERVER_COUNT; }

public:
};
extern ViewList& gViewList;