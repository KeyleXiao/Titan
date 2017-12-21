/*******************************************************************
** 文件名:	WebUrlManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/5/2017
** 版  本:	1.0
** 描  述:  WEB链接管理

http://网站域名/applogin?uid=账号ID&t=时间戳&sn=世界ID&n=角色ID&sig=签名&r=页面地址
参数：
uid:账号ID
t:时间，UINX格式。秒数
sn:世界ID
n:角色ID
sig:MD5("账号ID"+"时间戳"+"世界ID"+"角色ID"+"固定Key"+"账号密码")，MD5是小写的值
r:访问的url地址

********************************************************************/

#ifndef __WEB_URL_MANAGER_H__
#define __WEB_URL_MANAGER_H__

#include <string>
using namespace std;

struct SSchemeWebUrl;

// WEB链接管理类
class CWebUrlManager
{
public:
    // 构造函数
    CWebUrlManager();

    // 获取Web页面
    string getWebUrl(DWORD nWebUrlID, DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword);

    // 获取导航
    int getNavGroup(obuf1024 & buf);
private:
    // 获取自动登录Web页面
    string getSingleWebUrl(SSchemeWebUrl* pSchemeWebUrl);
    // 获取自动登录Web页面
    string getAutoWebUrl(SSchemeWebUrl* pSchemeWebUrl, DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword);
    // Web页面前置地址
    string parseAutoLoginPrexUrl(WORD wAutoLoginType);
    // Web页面后面跟随地址
    string parseAutoLoginFollowUrl(WORD wAutoLoginType);
    // 转换成MD5字符串
    string transformMd5String(const char* szText);
    // 构建AutoLogin字符串
    string buildAutoLoginString(DWORD dwWorldID, DWORD dwUserID, DWORD dwActorID, const char* szPassword);
};


extern CWebUrlManager  g_WebUrlManager;
#endif	// __WEB_URL_MANAGER_H__