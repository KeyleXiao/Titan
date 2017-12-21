/*******************************************************************
** 文件名:	LanguageDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-03-02
** 版  本:	1.0
** 描  述:	游戏语言公共定义

********************************************************************/

#ifndef __COMMON_LANGUAGEDEF_H__
#define __COMMON_LANGUAGEDEF_H__

//////////////////////////////////////////////////////////////////////////

#include "ManagedCode.h"

// 语言类型 
ENUM ENLanguageType
{
    LANGUAGE_CHINESE = 0,               // 中文，默认语言
    LANGUAGE_ENGLISH,                   // 英文

    LANGUAGE_MAX,                       // 最大
};

#endif	// __COMMON_LANGUAGEDEF_H__