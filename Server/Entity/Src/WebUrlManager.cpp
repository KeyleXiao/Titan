/*******************************************************************
** 文件名:	WebUrlManager.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/5/2017
** 版  本:	1.0
** 描  述:  WEB链接管理

********************************************************************/

#include "stdafx.h"
#include "WebUrlManager.h"
#include "EncryptUtil.h"
#include "IServerGlobal.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"


// Web自动登录KEY
#define WEB_AUTOLOGIN_KEY "8A2D1c0a0D6B9A1c0a1b2D8"

CWebUrlManager  g_WebUrlManager;

///////////////////////////////////////////////////////////////

CWebUrlManager::CWebUrlManager()
{

}

string CWebUrlManager::getWebUrl(DWORD nWebUrlID, DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword)
{
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    do
    {
        if (NULL == pSchemeCenter)
            break;

        ISchemeWebUrl * pISchemeWebUrl = pSchemeCenter->getSchemeWebUrl();
        if (NULL == pISchemeWebUrl)
            break;

        SSchemeWebUrl* pSchemeWebUrl = pISchemeWebUrl->getByID(nWebUrlID);
        if (NULL == pSchemeWebUrl)
            break;

        // 非自动登录的
        if (!pSchemeWebUrl->isAutoLogin)
        {
            return getSingleWebUrl(pSchemeWebUrl);
        }
        else
        {
            return getAutoWebUrl(pSchemeWebUrl, dwWorldID, dwUserID, dwActorID, szPassword);
        }

    } while (0);

    return string();

}

int CWebUrlManager::getNavGroup(obuf1024 & buf)
{
    int nCount = 0;
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    do
    {
        if (NULL == pSchemeCenter)
            break;

        ISchemeWebUrl * pISchemeWebUrl = pSchemeCenter->getSchemeWebUrl();
        if (NULL == pISchemeWebUrl)
            break;

        // 获取所有的配置
        std::map<DWORD, SSchemeWebUrl*>* pMapScheme = pISchemeWebUrl->getScheme();
        if (nullptr == pMapScheme)
            break;

        for (auto iter = pMapScheme->begin(); iter != pMapScheme->end(); ++iter) {
            SSchemeWebUrl* pNode = iter->second;
            if (pNode && pNode->isClientNav) {
                SGameNavItem item;
                item.nID = pNode->nID;
                item.nStrLen = strlen(pNode->szName) + 1;

                buf.push_back(&item, sizeof(item));
                buf.push_back(pNode->szName, item.nStrLen);

                ++nCount;
            }
        }

    } while (0);

    return nCount;
}

string CWebUrlManager::getSingleWebUrl(SSchemeWebUrl * pSchemeWebUrl)
{
    do
    {
        if (pSchemeWebUrl->isAutoLogin)
            break;

        return string(pSchemeWebUrl->szPrevUrl);
    } while (0);

    return string();
}

// 获取自动登录Web页面
string CWebUrlManager::getAutoWebUrl(SSchemeWebUrl* pSchemeWebUrl, DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword)
{
    do
    {
        if (!pSchemeWebUrl->isAutoLogin)
            break;

        string strWebPrev = pSchemeWebUrl->szPrevUrl;
        if (strWebPrev.empty())
            break;

        string strAttachParams = buildAutoLoginString(dwWorldID, dwUserID, dwActorID, szPassword);

        string strWebFollow = pSchemeWebUrl->szFollowUrl;

        // 返回组装的连接
        return strWebPrev + strAttachParams + strWebFollow;

    } while (0);
    
    return string();
}

// Web页面前置地址
string CWebUrlManager::parseAutoLoginPrexUrl(WORD wAutoLoginType)
{
    switch (wAutoLoginType)
    {

    default:  return "";
    }
}

// Web页面后面跟随地址
string CWebUrlManager::parseAutoLoginFollowUrl(WORD wAutoLoginType)
{
    return string();
}

// 转换成MD5字符串
string CWebUrlManager::transformMd5String(const char * szText)
{
    static string s_nullString;
    if (szText == NULL)
    {
        return s_nullString;
    }

    /*
    将传入字符串转换成md5编码的字符串
    输入参数：pIn - 传入的字符串
    输出参数：strOut - 将pIn转换成md5编码的字符串的引用
    返回值：转换成功则返回true，失败则返回false
    bool GetMD5(const char *pIn,string &strOut );
    */
    std::string str;
    bool bRc = EncryptUtil::GetMD5(szText, str);
    if (!bRc)
    {
        return s_nullString;
    }

    return str;
}

// sig:MD5("账号ID"+"时间戳"+"世界ID"+"角色ID"+"固定Key"+"账号密码")，MD5是小写的值
string CWebUrlManager::buildAutoLoginString(DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword)
{
    char szBuf[256] = { 0 };
    DWORD dwNowTime = static_cast<DWORD>(time(NULL));

    // sig:MD5("账号ID"+"时间戳"+"世界ID"+"角色ID"+"固定Key"+"账号密码")，MD5是小写的值
    ssprintf_s(szBuf, sizeof(szBuf), "%d%d%d%d%s%s", dwUserID, dwNowTime, dwWorldID, dwActorID, WEB_AUTOLOGIN_KEY, szPassword);

    // MD5处理
    string str = transformMd5String(szBuf);

    // ?uid=账号ID&t=时间戳&sn=世界ID&n=角色ID&sig=签名
    char szWebBuf[512] = { 0 };
    ssprintf_s(szWebBuf, sizeof(szWebBuf), "uid=%d&t=%d&sn=%d&n=%d&sig=%s", dwUserID, dwNowTime, dwWorldID, dwActorID, str.c_str() );

    //TraceLn("------Web-----");
    //TraceLn("str="<< szBuf);
    //TraceLn("Md5str="<< str.c_str());
    return szWebBuf;
}
