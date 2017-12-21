/*******************************************************************
** 文件名:	LanguageVer.h  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-03-02
** 版  本:	1.0
** 描  述:	游戏语言版本信息

********************************************************************/
#ifndef __COMMON_LANGUAGEVER_H__ 
#define __COMMON_LANGUAGEVER_H__

#include "Singleton.h"
#include "LanguageDef.h"

// 支持语言定义，后面只可以最加
static string g_language_dir[LANGUAGE_MAX] =
{
    "zh_cn",
    "en_us",
};

// 多语言版本类
class CLanguageVer : public Singleton<CLanguageVer>
{
public:
    // 获取目录名称
    const char * getDirName()
    {
#ifdef CHECK_LANGUAGE_OPEN
        return g_language_dir[m_nlangType].c_str();
#else
        return "";
#endif
    }

    CLanguageVer()
        : m_nlangType(LANGUAGE_CHINESE)
    {
        m_nlangType = LANGUAGE_CHINESE;
        rkt::Ini ini("config.ini");

        char szBuf[128] = {0};
        if( ini.getString("App", "language", szBuf, sizeof(szBuf))) 
        {
            string str = szBuf;
            rkt::StringHelper::toLower(str);
            for (int i=0; i< LANGUAGE_MAX; ++i)
            {
                if(g_language_dir[i] == str)
                {
                    m_nlangType = i;
                    return;
                }
            }
        }        
    }

private:
    BYTE m_nlangType;
};

#endif  // __COMMON_LANGUAGEVER_H__