/*******************************************************************
** 文件名:	IMentorshipClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	12/2/2016
** 版  本:	1.0
** 描  述:	师徒客户端接口
********************************************************************/

#ifndef __IMENTORSHIPCLIENT_H__
#define __IMENTORSHIPCLIENT_H__

struct IMentorshipClient
{
	// 释放
	virtual void release(void) = 0;
};


#if defined(_LIB) || defined(MENTORSHIPCLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(MentorshipClient))
extern "C" IMentorshipClient * CreateMentorshipClient(void);
#	define	CreateMentorshipClientProc	CreateMentorshipClient
#	define	CloseMentorshipClientProc()
#elif !defined(MENTORSHIPCLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(MentorshipClient))
API_IMPORT IMentorshipClient * CreateMentorshipClient(void);
#	define	CreateMentorshipClientProc	CreateMentorshipClient
#	define	CloseMentorshipClientProc()
#else													// 动态加载
typedef IMentorshipClient * (RKT_CDECL *procCreateMentorshipClient)(void);
#	define	CreateMentorshipClientProc	DllApi<procCreateMentorshipClient>::load(MAKE_DLL_NAME(MentorshipClient), "CreateMentorshipClient")
#	define	CloseMentorshipClientProc	DllApi<procCreateMentorshipClient>::freelib
#endif

#endif // __IMENTORSHIPCLIENT_H__