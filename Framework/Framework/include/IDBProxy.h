/*******************************************************************
** 文件名:	IDBProxy.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-25
** 版  本:	1.0
** 描  述:	DB代理接口			
********************************************************************/

#pragma once

#ifndef __FRAMEWORK_FRAMEWORK_INCLUDE_IDBPROXY_H__
#define __FRAMEWORK_FRAMEWORK_INCLUDE_IDBPROXY_H__


// DB代理接口
struct IDBProxy
{
	/** 初始化数据代理
	@param pSink 数据库请求返回回调接口
	*/
	virtual bool init(IDBRetSink * pSink, const char * szDebugInfo ) = 0;

	/** 关闭数据代理
	@param pSink 数据库请求返回回调接口
	*/
    virtual IDBProxy * closeProxy(IDBRetSink * pSink) = 0;

	/** 获取Sink对象
	*/
    virtual IDBRetSink * getHandler(void) = 0;

	/** 转成Sink对象
	*/
    virtual IDBRetSink * toSink(void) = 0;

	/** 增加引用计数
	@param pSink 数据库请求返回回调接口
	*/
    virtual void addRef() = 0;

	/** 释放
	@param pSink 数据库请求返回回调接口
	*/
    virtual void release() = 0;
};


#endif  // __FRAMEWORK_FRAMEWORK_INCLUDE_IDBPROXY_H__
