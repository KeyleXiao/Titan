/*******************************************************************
** �ļ���:	E:\speed\Server\LegendCupServer\Include\ILegendCupScene.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�ϵͳ������			
  
********************************************************************/

#pragma once
#include "LegendCupDef.h"

struct ILegendCupScene
{
	// ���� 
	virtual bool			load() = 0;
    // ִ���ͷ�Service
    virtual void            onStop() = 0;
	// �ͷ� 
	virtual void			release() = 0;

	// ��ȡ������������������
	virtual SERVICE_PTR		getLegendCupSceneService() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(LegendCupSceneServer_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(LegendCupSceneServer))
extern "C" ILegendCupScene * CreateLegendCupScene();
#	define	CreateLegendCupSceneProc	CreateLegendCupScene
#else													/// ��̬��汾
typedef ILegendCupScene * (RKT_CDECL * procCreateLegendCupScene)();
#	define	CreateLegendCupSceneProc	DllApi<procCreateLegendCupScene>::load(MAKE_DLL_NAME(LegendCupServer), "CreateLegendCupScene")
#endif


