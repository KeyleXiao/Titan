/*******************************************************************
** �ļ���:	E:\speed\Server\OrderServer\Include\IOrderScene.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�ϵͳ������			
  
********************************************************************/

#pragma once
#include "OrderDef.h"

struct IOrderScene
{
	// ���� 
	virtual bool			load() = 0;
    // ִ���ͷ�Service
    virtual void            onStop() = 0;
	// �ͷ� 
	virtual void			release() = 0;

	// ��ȡ������������������
	virtual SERVICE_PTR		getOrderSceneService() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(OrderSceneServer_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(OrderSceneServer))
extern "C" IOrderScene * CreateOrderScene();
#	define	CreateOrderSceneProc	CreateOrderScene
#else													/// ��̬��汾
typedef IOrderScene * (RKT_CDECL * procCreateOrderScene)();
#	define	CreateOrderSceneProc	DllApi<procCreateOrderScene>::load(MAKE_DLL_NAME(OrderServer), "CreateOrderScene")
#endif


