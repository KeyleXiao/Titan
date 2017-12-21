/*******************************************************************
** �ļ���:	__IOrderPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�	

********************************************************************/

#pragma once
#include "IEntity.h"

struct __IOrderPart:public __IEntityPart
{

};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IOrderPart * CreateOrderPart();
#	define	CreateOrderPartProc	CreateOrderPart
#else													/// ��̬��汾
typedef __IOrderPart * (RKT_CDECL * procCreateOrderPart)();
#	define	CreateOrderPartProc	DllApi<procCreateOrderPart>::load(MAKE_DLL_NAME(OrderSceneServer), "CreateOrderPart")
#endif
