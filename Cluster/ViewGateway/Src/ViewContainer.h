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

class ViewContainer : public UserContainer<ViewSession, ViewID>, public Singleton<ViewContainer>
{
public:
	ViewContainer();
	virtual ~ViewContainer();

protected:
	virtual void	OnDelUser(ISessionUser<ViewID>* pUser) {};

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void	RegisterHandlers();

	virtual ViewID	GetMaxID() { return MAX_VIEW_SERVER_COUNT; }

};
extern ViewContainer& gViewContainer;