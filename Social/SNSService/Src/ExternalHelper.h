/*******************************************************************
** �ļ���:	ExternalHelper.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:  2017/10/15
** ��  ��:	1.0
** ��  ��:	������
			
********************************************************************/

#pragma once

#include <string>

class CExternalHelper
{
public:
	CExternalHelper();
	~CExternalHelper();

public:
	// ��ȡ��������
	std::string getClanName(DWORD dwClanID);

	// ��ȡ��Ϸ��������
	std::string getWorldName();
};

extern CExternalHelper g_EHelper;