/*******************************************************************
** 文件名:	IClanClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/7/2016
** 版  本:	1.0
** 描  述:	帮会客户端接口
********************************************************************/

#ifndef __ICLANCLIENT_H__
#define __ICLANCLIENT_H__

struct IClanClient
{
	// 释放
	virtual void release(void) = 0;

    // 能否发布联盟杯赛
    virtual bool canPublicClanLegenCup() = 0;
};


#if defined(_LIB) || defined(CLANCLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(ClanClient))
extern "C" IClanClient * CreateClanClient(void);
#	define	CreateClanClientProc	CreateClanClient
#	define	CloseClanClientProc()
#elif !defined(CLANCLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(ClanClient))
API_IMPORT IClanClient * CreateClanClient(void);
#	define	CreateClanClientProc	CreateClanClient
#	define	CloseClanClientProc()
#else													// 动态加载
typedef IClanClient * (RKT_CDECL *procCreateClanClient)(void);
#	define	CreateClanClientProc	DllApi<procCreateClanClient>::load(MAKE_DLL_NAME(ClanClient), "CreateClanClient")
#	define	CloseClanClientProc	DllApi<procCreateClanClient>::freelib
#endif

#endif // __ICLANCLIENT_H__