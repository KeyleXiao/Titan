/*******************************************************************
** 文件名:	IScreenCaptureClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	昔文博
** 日  期:	24/7/2017
** 版  本:	1.0
** 描  述:	录屏客户端接口
********************************************************************/

#ifndef __ISCREENCAPTURECLIENT_H__
#define __ISCREENCAPTURECLIENT_H__

struct IScreenCaptureClient
{
	//释放
	virtual void release(void) = 0;
};

#if defined(_LIB) || defined(SCREENCAPTURE_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(ScreenCaptureClient))
extern "C" IScreenCaptureClient * CreateScreenCaptureClient(void);
#	define	CreateScreenCaptureClinetProc	CreateScreenCaptureClient
#	define	CloseScreenCaptureClientProc()
#elif !defined(SCREENCAPTURECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(ScreenCaptureClient))
API_IMPORT IScreenCaptureClient * CreateScreenCaptureClient(void);
#	define	CreateScreenCaptureClientProc	CreateScreenCaptureClient
#	define	CloseScreenCaptureClientProc()
#else													// 动态加载
typedef IScreenCaptureClient * (RKT_CDECL *procCreateScreenCaptureClient)(void);
#	define	CreateScreenCaptureClientProc	DllApi<procCreateScreenCaptureClient>::load(MAKE_DLL_NAME(ScreenCaptureClient), "CreateScreenCaptureClient")
#	define	CloseScreenCaptureClientProc	DllApi<procCreateScreenCaptureClient>::freelib
#endif

#endif // __ISCREENCAPTURECLIENT_H__