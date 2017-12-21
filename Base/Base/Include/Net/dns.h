/*******************************************************************
** 文件名:	dns.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	04/02/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_DNS_H__
#define __ASYN_DNS_H__

#include "Config.h"

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

//////////////////////////////////////////////////////////////////////////

namespace rkt
{
	/**
	@name : DNS查询者 (发起域名查询请求的对象)
	@brief: 当域名查询成功后会调用接口通知域名对应的IP地址
	*/
	struct DNS_Queryer
	{
		/**
		@name         : 查询域名成功
		@brief        : 
		@param domain : 要查询的域名
		@param ipset  : 对应的ip列表,即该域名对应的多个ip
		@return       :
		*/
		virtual void Handle_DNS_Response( const char * szDomain,const char ** ppIPList,unsigned int nIPNum) = 0;

		/**
		@name         : 查询域名失败
		@brief        : 
		@param domain : 要查询的域名
		@param dwError: 出错码,例如域名不存在
		@return       :
		*/
		virtual void Handle_DNS_Error( const char * szDomain,DWORD dwErrorCode ) = 0;
	};


	/**
	@name         : 查询一个域名
	@brief        :
	@param domain : 待查询的域名
	@param queryer: 查询者,用于查询成功后的通知
	@note         : 在查询过程中该指针必须保持有效,如果该指针被释放,需要主动调用Cancel_DNS_Query取消查询
	@return       :
	*/
	NET_API bool DNS_Query( const char * domain,DNS_Queryer * queryer );

	/**
	@name         : 取消一个域名查询请求
	@brief        :
	@param domain : 待查询的域名
	@param queryer: 查询者,用于查询成功后的通知
	@return       :
	*/
	NET_API bool Cancel_DNS_Query( const char * domain,DNS_Queryer * queryer );
};

#endif // #ifdef SUPPORT_NET

#endif//__ASYN_DNS_H__