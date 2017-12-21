/*******************************************************************
** 文件名:	http.h 
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

#ifndef __HTTP_H__
#define __HTTP_H__

#include "Config.h"

#ifdef SUPPORT_NET

#include "Handle.h"
#include <string>
#include <map>

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

	/// 协议版本号
	enum HTTP_VERSION
	{
		HTTP_VER_1_0    = 0,  // 1.0HTTP协议,一个连接发送一个请求
		HTTP_VER_1_1       ,  // 1.1HTTP协议,一个连接可以发送多个请求
	};

	/// HTTP响应头
	typedef std::map<std::string,std::string>   HTTP_Response_Header;

	/**
	@name : 用来处理HTTP的响应
	@brief:
	*/
	struct HTTP_Response_Handler
	{
		/**
		@name         : 服务器返回响应
		@brief        : 
		@param pContent:返回的内容实体,已经去掉了头信息
		@param dwLen  : 返回的内容长度
		@param header : HTTP响应头,响应头包含一系列字符串键值对,例如Content-Type:text/html
		@param url    : 之前调用Request方法传进来的url,以便让你知道是哪一个请求返回了
		@return       :
		*/
		virtual void OnResponse( const char * pContent,DWORD dwLen,const HTTP_Response_Header & header,const char * url) = 0;

		/**
		@name         : HTTP请求错误
		@brief        : 例如找不到页面等
		@param dwError: 错误号
		@param url    : 之前调用Request方法传进来的url,以便让你知道是哪一个请求返回了
		@note         : HTTP错误含义:
		1xx : 信息响应类，表示接收到请求并且继续处理
		2xx : 处理成功响应类，表示动作被成功接收、理解和接受
		3xx : 重定向响应类，为了完成指定的动作，必须接受进一步处理
		4xx : 客户端错误，客户请求包含语法错误或者是不能正确执行
		5xx : 服务端错误，服务器不能正确执行一个正确的请求
		@return       :
		*/
		virtual void OnError(DWORD dwError,const char * url) = 0;

		/**
		@name         : 服务器返回页面跳转
		@brief        : 自动访问跳转后的页面
		@param new_url: 新页面
		@param url    : 旧页面，即之前调用Request方法传进来的url,以便让你知道是哪一个请求返回了
		@return       :
		*/
		virtual void OnLocation(const char * new_url,const char * url) = 0;

		/**
		@name         : 整个文件还没下载完,收到部分数据流
        @brief        : 下载时可以通过这个函数准确的统计到下载速度
	    @return       : 返回false就表示不要下载了,取消
		*/
		virtual bool OnDataStream( const char * pData,unsigned __int64 dwPos,DWORD dwLen,const HTTP_Response_Header & header,const char * url ) { return true;}
	};

	/**
	@name         : 发送HTTP请求
	@brief        : 使用GET方式
	@param   url  : 要请求的url,带参数，例如http://www.carfax.com/register.php?username=no1
	@param httpver: 要使用的http协议,目前有1.0和1.1
	@param data_rng:要请求的数据段,如果为空则不指定
	@param handler: 处理http响应的回调接口,为了安全起见请使用句柄,该句柄指向HTTP_Response_Handler的指针
	@return       :
	*/
	NET_API bool HTTP_Request( const char * url,HTTP_VERSION httpver,__int64 range_begin,__int64 range_end,handle handler);

	/**
	@name         : 发送HTTP请求，参考w3c实现的winnet请求方式，兼容性更好 by 刘晓锋
	@brief        : 使用GET方式
	@param   url  : 要请求的url,带参数，例如http://www.carfax.com/register.php?username=no1
	@param httpver: 
	@param data_rng:要请求的数据段,如果为空则不指定
	@param handler: 处理http响应的回调接口,为了安全起见请使用句柄,该句柄指向HTTP_Response_Handler的指针
	@return       :
	*/
	NET_API bool HTTP_W3C_Request( const char * url,HTTP_VERSION httpver,__int64 range_begin,__int64 range_end,handle handler);

	enum
	{
		HTTP_METHOD_SOCKET = 0,   // 使用SOCKET方式实现HTTP请求  (这个测试充分一些，但是兼容性还是有点问题)
		HTTP_METHOD_WININET= 1,   // 使用WININET API实现HTTP请求 (这个兼容性可能要好一点，但是还没经过大规模的测试)
	};

	/** 
	@name         : 取得当前的HTTP请求发送方式
	@desc         : 底层实现了两种方式发送HTTP请求,默认是使用SOCKET方式直接发送,但是兼容性还是有点问题
	@desc         : 这两种方式对用户来讲是透明的，接口一样，得到的结果也一样，只是如果有时候发现异常，可以试着切换到另外一种方式看看有没有问题
	*/
	NET_API int  GetCurrentHttpMethod();

	/** 
	@name         : 切换HTTP请求发送方式
	@desc         : 底层实现了两种方式发送HTTP请求,默认是使用SOCKET方式直接发送,但是兼容性还是有点问题
	@desc         : 这两种方式对用户来讲是透明的，接口一样，得到的结果也一样，只是如果有时候发现异常，可以试着切换到另外一种方式看看有没有问题
	@return       : 返回切换后的HTTP发送方式
	*/
	NET_API int  SwitchHttpReuqestMethod();
};

#endif // #ifdef SUPPORT_NET

#endif//__HTTP_H__