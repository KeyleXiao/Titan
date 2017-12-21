/*******************************************************************
** �ļ���:	IScreenCaptureClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���Ĳ�
** ��  ��:	24/7/2017
** ��  ��:	1.0
** ��  ��:	¼���ͻ��˽ӿ�
********************************************************************/

#ifndef __ISCREENCAPTURECLIENT_H__
#define __ISCREENCAPTURECLIENT_H__

struct IScreenCaptureClient
{
	//�ͷ�
	virtual void release(void) = 0;
};

#if defined(_LIB) || defined(SCREENCAPTURE_STATIC_LIB)	// ��̬����
#	pragma comment(lib, MAKE_LIB_NAME(ScreenCaptureClient))
extern "C" IScreenCaptureClient * CreateScreenCaptureClient(void);
#	define	CreateScreenCaptureClinetProc	CreateScreenCaptureClient
#	define	CloseScreenCaptureClientProc()
#elif !defined(SCREENCAPTURECLIENT_EXPORTS) && defined(DYN_LIB) // ��̬����
#	pragma comment(lib, MAKE_DLL_LIB_NAME(ScreenCaptureClient))
API_IMPORT IScreenCaptureClient * CreateScreenCaptureClient(void);
#	define	CreateScreenCaptureClientProc	CreateScreenCaptureClient
#	define	CloseScreenCaptureClientProc()
#else													// ��̬����
typedef IScreenCaptureClient * (RKT_CDECL *procCreateScreenCaptureClient)(void);
#	define	CreateScreenCaptureClientProc	DllApi<procCreateScreenCaptureClient>::load(MAKE_DLL_NAME(ScreenCaptureClient), "CreateScreenCaptureClient")
#	define	CloseScreenCaptureClientProc	DllApi<procCreateScreenCaptureClient>::freelib
#endif

#endif // __ISCREENCAPTURECLIENT_H__