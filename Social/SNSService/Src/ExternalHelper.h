/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:  2017/10/15
** 版  本:	1.0
** 描  述:	帮助类
			
********************************************************************/

#pragma once

#include <string>

class CExternalHelper
{
public:
	CExternalHelper();
	~CExternalHelper();

public:
	// 获取联盟名字
	std::string getClanName(DWORD dwClanID);

	// 获取游戏世界名字
	std::string getWorldName();
};

extern CExternalHelper g_EHelper;