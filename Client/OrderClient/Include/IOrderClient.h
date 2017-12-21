/*******************************************************************
** �ļ���:	IOrderClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	2015/10/19  17:21
** ��  ��:	1.0
** ��  ��:	���а�ϵͳ�ͻ���
** Ӧ  ��:  		
	
*******************************************************************/

#pragma once
#include "OrderMsgDef.h"

struct IOrderClient
{
	/** �ͷ�
	@param   
	@param   
	@param   
	@return  
	@note     �ͷ�������Դ���������ٴ˶���
	@warning 
	@retval buffer 
	*/
	virtual void			release(void) = NULL;

};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// ��̬����
#	pragma comment(lib, MAKE_LIB_NAME(OrderClient))
extern "C" IOrderClient * CreateOrderClient(void);
#	define	CreateOrderClientProc	CreateOrderClient
#	define	CloseOrderClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // ��̬����
#	pragma comment(lib, MAKE_DLL_LIB_NAME(OrderClient))
API_IMPORT IOrderClient * CreateOrderClient(void);
#	define	CreateOrderClientProc	CreateOrderClient
#	define	CloseOrderClientProc()
#else													// ��̬����
typedef IOrderClient * (RKT_CDECL *procCreateOrderClient)(void);
#	define	CreateOrderClientProc	DllApi<procCreateOrderClient>::load(MAKE_DLL_NAME(OrderClient), "CreateOrderClient")
#	define	CloseOrderClientProc	DllApi<procCreateOrderClient>::freelib
#endif