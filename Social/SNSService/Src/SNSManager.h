/*******************************************************************
** �ļ���:	SNSManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:  6/12/2016
** ��  ��:	1.0
** ��  ��:	
			SNS����
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "ISNSManager.h"
#include <map>

class SNSManager:public ISNSManager
{
public:
	////////////////////////////////ISNSManager//////////////////////////////////////////
	virtual SERVICE_PTR getSNSService(){return m_SNSServices;};
	virtual bool		load();
	virtual void		release();

private:
	SERVICE_PTR m_SNSServices;

private:
	bool CreateSNSService();
};

