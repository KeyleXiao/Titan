/*******************************************************************
** �ļ���:	ILegendCupClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	2015/10/19  17:21
** ��  ��:	1.0
** ��  ��:	����ϵͳ�ͻ���
** Ӧ  ��:  		
	
*******************************************************************/

#pragma once


struct ILegendCupClient
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
	virtual void			Release(void) = NULL;

};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// ��̬����
#	pragma comment(lib, MAKE_LIB_NAME(LegendCupClient))
extern "C" ILegendCupClient * CreateLegendCupClient(void);
#	define	CreateLegendCupClientProc	CreateLegendCupClient
#	define	CloseLegendCupClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // ��̬����
#	pragma comment(lib, MAKE_DLL_LIB_NAME(LegendCupClient))
API_IMPORT ILegendCupClient * CreateLegendCupClient(void);
#	define	CreateLegendCupClientProc	CreateLegendCupClient
#	define	CloseLegendCupClientProc()
#else													// ��̬����
typedef ILegendCupClient * (RKT_CDECL *procCreateLegendCupClient)(void);
#	define	CreateLegendCupClientProc	DllApi<procCreateLegendCupClient>::load(MAKE_DLL_NAME(LegendCupClient), "CreateLegendCupClient")
#	define	CloseLegendCupClientProc	DllApi<procCreateLegendCupClient>::freelib
#endif