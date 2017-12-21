/*******************************************************************
** �ļ���:	IClanManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/31/2016	16:57
** ��  ��:	1.0
** ��  ��:	������
********************************************************************/


#pragma once
#include "IServiceContainer.h"

struct IClanManager
{
	// ������������
	virtual SERVICE_PTR getClanService() = 0;

    // ���� 
	virtual bool	load() = 0;

	// �ͷ� 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(CLANSERVICE_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ClanService))
extern "C" IClanManager * CreateClanManager();
#	define	CreateClanManagerProc	CreateClanManager
#else													/// ��̬��汾
typedef IClanManager * (RKT_CDECL * procCreateClanManager)();
#	define	CreateClanManagerProc	DllApi<procCreateClanManager>::load(MAKE_DLL_NAME(ClanService), "CreateClanManager")
#endif