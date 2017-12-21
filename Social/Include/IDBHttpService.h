/*******************************************************************
** 文件名:	IDBHttpService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:	HTTPSERVICE接口

********************************************************************/

#pragma once

#ifndef __I_DB_HTTPSERVICE_H__
#define __I_DB_HTTPSERVICE_H__


// HTTPSERVICE接口
struct IDBHttpService
{
public:
    virtual void            Release(void) = 0;

    virtual bool            Create(void) = 0;

    /** 执行Service的on_stop
    @return
    */
    virtual void            onStop(void) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SOCIALGLOBAL_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(SocialGlobal))
extern "C" IDBHttpService * CreateDBHttpService();
#	define	CreateDBHttpServiceProc	CreateDBHttpService
#else													/// 动态库版本
typedef IDBHttpService * (RKT_CDECL * procCreateDBHttpService)();
#	define	CreateDBHttpServiceProc	DllApi<procCreateDBHttpService>::load(MAKE_DLL_NAME(SocialGlobal), "CreateDBHttpService")
#endif


#endif // __I_DB_HTTPSERVICE_H__
