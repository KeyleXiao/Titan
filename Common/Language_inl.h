/*******************************************************************
** 文件名:	Language_inl.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-03-02
** 版  本:	1.0
** 描  述:	游戏语言内部处理

-------------------------------------------------------------------
目的：为了应对不同语言版本中对发往客户端字符串能自动变化
方案：用LanguageText.csv配置文件将源代码中发往客户端显示文字配置起来，
      而源代码中将用数字标识符的方式来代替，以后要修改只需
      修改LanguageText.csv文件即可

为了使标识符更直接将表达体现，不使用魔数而用枚举定义
单个模块可以设定属于自己的区间范围

LanguageParse.LoadString(SECEN_WAR_NOTPURCHASE_MAXNUM);
********************************************************************/

#ifndef __COMMON_LANGUAGE_INL_H__
#define __COMMON_LANGUAGE_INL_H__

#include "StringHelper.h"
#include "LanguageVer.h"
//////////////////////////////////////////////////////////////////////////

inline string replaceScpDirName(const char* szPath)
{
    string strPath = szPath;
    rkt::StringHelper::toLower(strPath);
    char szBuf[128] = {0};    
    ssprintf_s(szBuf, sizeof(szBuf), "scp\\%s\\", CLanguageVer::getInstance().getDirName());
    rkt::StringHelper::replace(strPath,"scp\\", szBuf);
    return strPath;
}


//#define CHECK_LANGUAGE_OPEN

#ifdef CHECK_LANGUAGE_OPEN
#define SCP_PATH_FORMAT(path)   replaceScpDirName(path)
#else
#define SCP_PATH_FORMAT(path)  (path)
#endif





#endif	// __COMMON_LANGUAGE_INL_H__