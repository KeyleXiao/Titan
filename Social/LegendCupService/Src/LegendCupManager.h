/*******************************************************************
** �ļ���:	E:\speed\Social\LegendCupServer\Src\LegendCupManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/18/2015	14:47
** ��  ��:	1.0
** ��  ��:	�������������
			
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "ILegendCupManager.h"
#include <map>

class LegendCupManager:public ILegendCupManager
{


public:
	////////////////////////////////ILegendCupManager//////////////////////////////////////////
	virtual SERVICE_PTR getLegendCupService(){return m_LegendCupServices;};
	virtual bool		load();
	virtual void		release();

	

private:
	SERVICE_PTR m_LegendCupServices;

private:
	bool CreateLegendCupService();
};