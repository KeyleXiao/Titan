/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:  2017/10/15
** 版  本:	1.0
** 描  述:	帮助类
			
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
