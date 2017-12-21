/*******************************************************************
** �ļ���:	ClanManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/31/2016	17:19
** ��  ��:	1.0
** ��  ��:	������
********************************************************************/

#ifndef __CLANMANAGER_H__
#define __CLANMANAGER_H__

#include "IServiceContainer.h"
#include "IClanManager.h"

class ClanManager:public IClanManager
{
public:
	////////////////////////////////IClanManager//////////////////////////////////////////
	virtual SERVICE_PTR getClanService(){return m_ClanServices;};
	virtual bool		load();
	virtual void		release();

private:
	SERVICE_PTR m_ClanServices;

private:
	bool createClanService();
};

#endif // __CLANMANAGER_H__