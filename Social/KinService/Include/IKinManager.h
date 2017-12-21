/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IKinMananger.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	15:45
** ��  ��:	1.0
** ��  ��:	ս�ӹ���ӿ�
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "KinDef.h"

struct IKinManager
{
	// ������������
	virtual SERVICE_PTR GetKinService() = 0;

	// ���� 
	virtual bool	Load() = 0;
	// �ͷ� 
	virtual void	Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(KINSERVICE_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(KinService))
extern "C" IKinManager * CreateKinManager();
#	define	CreateKinManagerProc	CreateKinManager
#else													/// ��̬��汾
typedef IKinManager * (RKT_CDECL * procCreateKinManager)();
#	define	CreateKinManagerProc	DllApi<procCreateKinManager>::load(MAKE_DLL_NAME(KinService), "CreateKinManager")
#endif