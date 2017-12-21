/*******************************************************************
** �ļ���:	E:\speed\Server\TeamServer\Include\ITeamScene.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�ϵͳ������			
  
********************************************************************/

#pragma once
#include "TeamDef.h"

struct ITeamScene
{
	// ���� 
	virtual bool			load() = 0;
    // ִ���ͷ�Service
    virtual void            onStop() = 0;
	// �ͷ� 
	virtual void			release() = 0;

	// ��ȡ������������������
	virtual SERVICE_PTR		getTeamSceneService() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(TeamSceneServer_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(TeamSceneServer))
extern "C" ITeamScene * CreateTeamScene();
#	define	CreateTeamSceneProc	CreateTeamScene
#else													/// ��̬��汾
typedef ITeamScene * (RKT_CDECL * procCreateTeamScene)();
#	define	CreateTeamSceneProc	DllApi<procCreateTeamScene>::load(MAKE_DLL_NAME(TeamServer), "CreateTeamScene")
#endif


