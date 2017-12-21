/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IMatch.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/15/2015	15:45
** ��  ��:	1.0
** ��  ��:	
			���а����
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "LegendCupDef.h"

struct ILegendCupManager
{
	// ������������
	virtual SERVICE_PTR getLegendCupService() = 0;

	// ���� 
	virtual bool	load() = 0;
	// �ͷ� 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(LEDENDCUPSERVICE_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(LegendCupService))
extern "C" ILegendCupManager * CreateLegendCupManager();
#	define	CreateLegendCupManagerProc	CreateLegendCupManager
#else													/// ��̬��汾
typedef ILegendCupManager * (RKT_CDECL * procCreateLegendCupManager)();
#	define	CreateLegendCupManagerProc	DllApi<procCreateLegendCupManager>::load(MAKE_DLL_NAME(LegendCupService), "CreateLegendCupManager")
#endif