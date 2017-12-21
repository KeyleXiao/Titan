/*******************************************************************
** �ļ���:	WebUrlManager.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	12/5/2017
** ��  ��:	1.0
** ��  ��:  WEB���ӹ���

********************************************************************/

#include "stdafx.h"
#include "WebUrlManager.h"
#include "EncryptUtil.h"
#include "IServerGlobal.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"


// Web�Զ���¼KEY
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

        // ���Զ���¼��
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

        // ��ȡ���е�����
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

// ��ȡ�Զ���¼Webҳ��
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

        // ������װ������
        return strWebPrev + strAttachParams + strWebFollow;

    } while (0);
    
    return string();
}

// Webҳ��ǰ�õ�ַ
string CWebUrlManager::parseAutoLoginPrexUrl(WORD wAutoLoginType)
{
    switch (wAutoLoginType)
    {

    default:  return "";
    }
}

// Webҳ���������ַ
string CWebUrlManager::parseAutoLoginFollowUrl(WORD wAutoLoginType)
{
    return string();
}

// ת����MD5�ַ���
string CWebUrlManager::transformMd5String(const char * szText)
{
    static string s_nullString;
    if (szText == NULL)
    {
        return s_nullString;
    }

    /*
    �������ַ���ת����md5������ַ���
    ���������pIn - ������ַ���
    ���������strOut - ��pInת����md5������ַ���������
    ����ֵ��ת���ɹ��򷵻�true��ʧ���򷵻�false
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

// sig:MD5("�˺�ID"+"ʱ���"+"����ID"+"��ɫID"+"�̶�Key"+"�˺�����")��MD5��Сд��ֵ
string CWebUrlManager::buildAutoLoginString(DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword)
{
    char szBuf[256] = { 0 };
    DWORD dwNowTime = static_cast<DWORD>(time(NULL));

    // sig:MD5("�˺�ID"+"ʱ���"+"����ID"+"��ɫID"+"�̶�Key"+"�˺�����")��MD5��Сд��ֵ
    ssprintf_s(szBuf, sizeof(szBuf), "%d%d%d%d%s%s", dwUserID, dwNowTime, dwWorldID, dwActorID, WEB_AUTOLOGIN_KEY, szPassword);

    // MD5����
    string str = transformMd5String(szBuf);

    // ?uid=�˺�ID&t=ʱ���&sn=����ID&n=��ɫID&sig=ǩ��
    char szWebBuf[512] = { 0 };
    ssprintf_s(szWebBuf, sizeof(szWebBuf), "uid=%d&t=%d&sn=%d&n=%d&sig=%s", dwUserID, dwNowTime, dwWorldID, dwActorID, str.c_str() );

    //TraceLn("------Web-----");
    //TraceLn("str="<< szBuf);
    //TraceLn("Md5str="<< str.c_str());
    return szWebBuf;
}
