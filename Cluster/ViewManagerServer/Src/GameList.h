/*******************************************************************
** �ļ���:	GameList.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/08
** ��  ��:	1.0
** ��  ��:	��Ϸ�������б�
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "UserList.h"

class GameUser;


class GameList : public UserList<GameUser, GameID>, public Singleton<GameList>
{
public:
	GameList();
	virtual ~GameList();

	//// ȡ������׼���ö��⿪�ŵ�View����
	//int		GetReadyCount();
	//// �����������ֵ�Gate��Ϣ���͸�����View
	//void	BroadcastGatesInfoToView();

protected:
	virtual void	OnDelUser(ISessionUser<GameID>* pUser) {};

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void	RegisterHandlers();

	virtual GameID	GetMaxID();

};
extern GameList& gGameList;