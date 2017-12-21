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
#include "TeamDef.h"

struct ITeamManager
{
	// ������������
	virtual SERVICE_PTR GetTeamService() = 0;

	// ���� 
	virtual bool	Load() = 0;
	// �ͷ� 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(TEAMSERVICE_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(TeamService))
extern "C" ITeamManager * CreateTeamManager();
#	define	CreateTeamManagerProc	CreateTeamManager
#else													/// ��̬��汾
typedef ITeamManager * (RKT_CDECL * procCreateTeamManager)();
#	define	CreateTeamManagerProc	DllApi<procCreateTeamManager>::load(MAKE_DLL_NAME(TeamService), "CreateTeamManager")
#endif