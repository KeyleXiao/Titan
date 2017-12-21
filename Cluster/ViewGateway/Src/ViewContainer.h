/*******************************************************************
** �ļ���:	ViewList.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/04
** ��  ��:	1.0
** ��  ��:	Gate�ϵ�ViewServer����List
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "UserContainer.h"


class ViewSession;

class ViewContainer : public UserContainer<ViewSession>, public Singleton<ViewContainer>
{
public:
	ViewContainer();
	virtual ~ViewContainer();

protected:
	virtual void	onDelUser(ISessionUser* pUser) {};

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void	RegisterHandlers();

	virtual DWORD	GetMaxID() { return MAX_VIEW_SERVER_COUNT; }

};
extern ViewContainer& gViewContainer;