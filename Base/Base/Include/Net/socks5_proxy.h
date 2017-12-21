/*******************************************************************
** 文件名:	sockts5_proxy.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/06/2009
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __SOCKS_5_PROXY_H__
#define __SOCKS_5_PROXY_H__

#include "Config.h"
#include "net.h"

#ifdef SUPPORT_NET

//////////////////////////////////////////////////////////////////////////
// 导出宏定义
#ifndef NET_API
#define NET_API RKT_API
//#	ifdef _LIB
//#		define NET_API		extern "C"
//#	elif NET_EXPORTS
//#		define NET_API		extern "C" __declspec(dllexport)
//#	else
//#		define NET_API		extern "C" __declspec(dllimport)
//#	endif
#endif//NET_API

#include <Windows.h>

namespace rkt
{
	/**
	@name             : 设置Socks5代理服务器
	@brief            : 设置了代理服务器之后,发起的所有连接默认都走代理
	@param szProxyHost: 代理服务器地址
	@param nProxyPort : 代理服务器端口
	@param szUsername : 如果代理服务器需要账号认证则传入用户名,否则传0
	@param szPassword : 如果代理服务器需要账号认证则传入密码,否则传0
	@return           :
	*/
	NET_API bool SetSocks5Proxy( const char * szProxyHost,unsigned short nProxyPort,const char * szUsername=0,const char * szPassword=0 );

	/**
	@name             : 取得当前Socks5代理服务器设置
	@param szProxyHost: 返回代理服务器地址
	@param nProxyPort : 返回代理服务器端口
	@param szUsername : 返回如果代理服务器需要账号认证则传入用户名,否则传0
	@param szPassword : 返回如果代理服务器需要账号认证则传入密码,否则传0
	@return           :
	*/
	NET_API bool GetSocks5Proxy( const char ** szProxyHost,unsigned short & nProxyPort,const char ** szUsername,const char ** szPassword );


    enum SOCK5_STATE				// SOCKS5代理服务器连接状态
    {
        STATE_IDLE           ,      // 空闲状态
        CONNECT_SOCKS5_SERVER,      // 正在连接代理服务器
        METHOD_REQUEST,             // 向SOCKS5代理服务器请求认证方式
        AUTHENT_REQUEST,            // 向SOCKS5代理服务器请求身份认证
        CONNECT_DEST_SERVER,        // 向SOCKS5代理服务器请求连接目标服务器
		CONNECT_OK,                 // 通过代理服务器连接目标服务器成功
		CONNECT_FAILED,             // 通过代理服务器连接目标服务器失败
    };    

	/**
	@name             : 取得当前Socks5代理服务器连接状态
	@brief            : 
	@param pConn      : 连接对象
	@param ppConnInfo : 连接信息,提示给用户
	@return           : 返回本次连接代理服务器状态(连上还是没连上)
	*/
	NET_API SOCK5_STATE GetSocks5ProxyConnectState( IConnection * pConn,const char ** ppConnInfo );
};

#endif // #ifdef SUPPORT_NET


#endif//__SOCKS_5_PROXY_H__