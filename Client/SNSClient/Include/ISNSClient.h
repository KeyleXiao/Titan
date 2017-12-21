/*******************************************************************
** �ļ���:	ISNSClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	���к�
** ��  ��:	12/7/2016
** ��  ��:	1.0
** ��  ��:	SNSϵͳ�ͻ���
** Ӧ  ��:  	
	
*******************************************************************/

#pragma once

#include "SNSDef.h"

struct ISNSClient
{
	// �ͷ�
	virtual void release(void) = 0;
};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// ��̬����
#	pragma comment(lib, MAKE_LIB_NAME(SNSClient))
extern "C" ISNSClient * CreateSNSClient(void);
#	define	CreateSNSClientProc	CreateSNSClient
#	define	CloseSNSClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // ��̬����
#	pragma comment(lib, MAKE_DLL_LIB_NAME(SNSClient))
API_IMPORT ISNSClient * CreateSNSClient(void);
#	define	CreateSNSClientProc	CreateSNSClient
#	define	CloseSNSClientProc()
#else													// ��̬����
typedef ISNSClient * (RKT_CDECL *procCreateSNSClient)(void);
#	define	CreateSNSClientProc	DllApi<procCreateSNSClient>::load(MAKE_DLL_NAME(SNSClient), "CreateSNSClient")
#	define	CloseSNSClientProc	DllApi<procCreateSNSClient>::freelib
#endif