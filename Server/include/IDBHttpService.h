/*******************************************************************
** 文件名:	IDBHttpService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:	DBHTTPService接口

********************************************************************/

#pragma once

#ifndef __I_DB_HTTPSERVICE_H__
#define __I_DB_HTTPSERVICE_H__

struct IUrlResponse;

// DBHTTPService接口
struct IDBHttpService
{

    /**
    @name    : 释放
    */
    virtual void release(void) = 0;

    /**
    @name    :
    */
    virtual bool create(void) = 0;

    /** 执行Service的on_stop
    @return
    */
    virtual void onStop(void) = 0;

    /**
    @name    : 请求url调用（外部接口调用）
    @param const char* pUrl: 请求地址
    @param WORD wLen : 地址长度
    @param IUrlResponse* pUrlResponse : 回调者
    */
    virtual void requestUrl(const char* pUrl, unsigned short nLen, IUrlResponse* pUrlResponse) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SERVERGLOBAL_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ServerGlobal))
extern "C" IDBHttpService * CreateDBHttpService();
#	define	CreateDBHttpServiceProc	CreateDBHttpService
#else													/// 动态库版本
typedef IDBHttpService * (RKT_CDECL * procCreateDBHttpService)();
#	define	CreateDBHttpServiceProc	DllApi<procCreateDBHttpService>::load(MAKE_DLL_NAME(ServerGlobal), "CreateDBHttpService")
#endif
#endif // __I_DB_HTTPSERVICE_H__
