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
#include "OrderDef.h"

struct IOrderManager
{
	// ������������
	virtual SERVICE_PTR getOrderService() = 0;

	// ���� 
	virtual bool	load() = 0;
	// �ͷ� 
	virtual void	Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ORDERSERVICE_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(OrderService))
extern "C" IOrderManager * CreateOrderManager();
#	define	CreateOrderManagerProc	CreateOrderManager
#else													/// ��̬��汾
typedef IOrderManager * (RKT_CDECL * procCreateOrderManager)();
#	define	CreateOrderManagerProc	DllApi<procCreateOrderManager>::load(MAKE_DLL_NAME(OrderService), "CreateOrderManager")
#endif