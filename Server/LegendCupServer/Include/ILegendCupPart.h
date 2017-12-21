/*******************************************************************
** �ļ���:	__ILegendCupPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�	

********************************************************************/

#pragma once
#include "IEntity.h"

struct __ILegendCupPart:public __IEntityPart
{

};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __ILegendCupPart * CreateLegendCupPart();
#	define	CreateLegendCupPartProc	CreateLegendCupPart
#else													/// ��̬��汾
typedef __ILegendCupPart * (RKT_CDECL * procCreateLegendCupPart)();
#	define	CreateLegendCupPartProc	DllApi<procCreateLegendCupPart>::load(MAKE_DLL_NAME(LegendCupSceneServer), "CreateLegendCupPart")
#endif
