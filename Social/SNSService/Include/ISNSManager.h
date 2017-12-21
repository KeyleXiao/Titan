/*******************************************************************
** �ļ���:	ISNSManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:	12/6/2016
** ��  ��:	1.0
** ��  ��:	SNS����
********************************************************************/

#pragma once
#include "IServiceContainer.h"

struct ISNSManager
{
	// ���SNS������
	virtual SERVICE_PTR getSNSService() = 0;
	// ���� 
	virtual bool	load() = 0;
	// �ͷ� 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SNSSERVICE_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(SNSService))
extern "C" ISNSManager * CreateSNSManager();
#	define	CreateSNSManagerProc	CreateSNSManager
#else													/// ��̬��汾
typedef ISNSManager * (RKT_CDECL * procCreateSNSManager)();
#	define	CreateSNSManagerProc	DllApi<procCreateSNSManager>::load(MAKE_DLL_NAME(SNSService), "CreateSNSManager")
#endif