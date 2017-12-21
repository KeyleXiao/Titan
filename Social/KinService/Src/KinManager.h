/*******************************************************************
** �ļ���:	E:\speed\Social\KinServer\Src\KinManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/18/2015	14:47
** ��  ��:	1.0
** ��  ��:	
********************************************************************/

#pragma once
#include "IMessageHandler.h"
#include "IServiceContainer.h"
#include "IKinManager.h"
#include <map>

class KinManager:public IKinManager
{


public:
	////////////////////////////////IKinManager//////////////////////////////////////////
	virtual SERVICE_PTR GetKinService(){return m_KinServices;};
	
	virtual bool		Load();
	virtual void		Release();

	

private:
	bool CreateKinService();

private:
    SERVICE_PTR m_KinServices;
};

