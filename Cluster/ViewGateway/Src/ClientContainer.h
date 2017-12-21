/*******************************************************************
** �ļ���:	ClientContainer.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/06
** ��  ��:	1.0
** ��  ��:	��ս�ͻ�������
** Ӧ  ��:  ������������ÿͻ��˹�ս������
			������Ժ�����������ͻ��˹�ս�����󣬿����ٿ�һ��Container����
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
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

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void RegisterHandlers();

	virtual DWORD GetMaxID();

};
extern ClientContainer& gClientContainer;