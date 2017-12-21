/*******************************************************************
** �ļ���:	ExternalHelper.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:  2017/10/15
** ��  ��:	1.0
** ��  ��:	������
			
********************************************************************/
#include "stdafx.h"
#include "ExternalHelper.h"
#include "ClanHelper.h"
#include "IShareServer.h"
#include "DManagerModule.h"
#include "ISocialGlobal.h"
#include "IManagerConnectorService.h"

CExternalHelper g_EHelper;

CExternalHelper::CExternalHelper()
{
}

CExternalHelper::~CExternalHelper()
{
}

std::string CExternalHelper::getClanName(DWORD dwClanID)
{
	ClanHelper clanHelper;
	if (clanHelper.m_ptr == NULL)
		return "";

	string clanName = clanHelper.m_ptr->getClanName(dwClanID);

	return clanName;
}

std::string CExternalHelper::getWorldName()
{
	IManagerConnectorService* pManagerConnectorService = gSocialGlobal->getManagerConnectorService();
	if (pManagerConnectorService == NULL)
		return "";

	SGameWorldConnectorInfo info = pManagerConnectorService->getGameWorldInfo();

	return info.szWorldName;
}
