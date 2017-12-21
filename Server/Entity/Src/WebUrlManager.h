/*******************************************************************
** �ļ���:	WebUrlManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	12/5/2017
** ��  ��:	1.0
** ��  ��:  WEB���ӹ���

http://��վ����/applogin?uid=�˺�ID&t=ʱ���&sn=����ID&n=��ɫID&sig=ǩ��&r=ҳ���ַ
������
uid:�˺�ID
t:ʱ�䣬UINX��ʽ������
sn:����ID
n:��ɫID
sig:MD5("�˺�ID"+"ʱ���"+"����ID"+"��ɫID"+"�̶�Key"+"�˺�����")��MD5��Сд��ֵ
r:���ʵ�url��ַ

********************************************************************/

#ifndef __WEB_URL_MANAGER_H__
#define __WEB_URL_MANAGER_H__

#include <string>
using namespace std;

struct SSchemeWebUrl;

// WEB���ӹ�����
class CWebUrlManager
{
public:
    // ���캯��
    CWebUrlManager();

    // ��ȡWebҳ��
    string getWebUrl(DWORD nWebUrlID, DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword);

    // ��ȡ����
    int getNavGroup(obuf1024 & buf);
private:
    // ��ȡ�Զ���¼Webҳ��
    string getSingleWebUrl(SSchemeWebUrl* pSchemeWebUrl);
    // ��ȡ�Զ���¼Webҳ��
    string getAutoWebUrl(SSchemeWebUrl* pSchemeWebUrl, DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword);
    // Webҳ��ǰ�õ�ַ
    string parseAutoLoginPrexUrl(WORD wAutoLoginType);
    // Webҳ���������ַ
    string parseAutoLoginFollowUrl(WORD wAutoLoginType);
    // ת����MD5�ַ���
    string transformMd5String(const char* szText);
    // ����AutoLogin�ַ���
    string buildAutoLoginString(DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword);
};


extern CWebUrlManager  g_WebUrlManager;
#endif	// __WEB_URL_MANAGER_H__