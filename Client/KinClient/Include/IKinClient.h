/*******************************************************************
** �ļ���:	IKinClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������
** ��  ��:	2015/10/19  17:21
** ��  ��:	1.0
** ��  ��:	ս��ϵͳ�ͻ���
** Ӧ  ��:  		
	
*******************************************************************/

#pragma once
#include "KinMsgDef.h"


struct IKin
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

struct IKinClient
{
	/** �ͷ�
	@param   
	@warning 
	@retval buffer 
	*/
	virtual void			release(void) = NULL;

    /** ����ս��
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallCreateKin(const char* szName) = NULL;

    /** �������ս��
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallJoinKin(DWORD dwKinID, string strLeaveWord) = NULL;

     /** �˳�ս��
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallQuitKin() = NULL;

    /** ��ɢս��
	@param   
	@retval buffer 
	*/
	virtual void		dragonBallDissmissKin() = NULL;

};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// ��̬����
#	pragma comment(lib, MAKE_LIB_NAME(KinClient))
extern "C" IKinClient * CreateKinClient(void);
#	define	CreateKinClientProc	CreateKinClient
#	define	CloseKinClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // ��̬����
#	pragma comment(lib, MAKE_DLL_LIB_NAME(KinClient))
API_IMPORT IKinClient * CreateKinClient(void);
#	define	CreateKinClientProc	CreateKinClient
#	define	CloseKinClientProc()
#else													// ��̬����
typedef IKinClient * (RKT_CDECL *procCreateKinClient)(void);
#	define	CreateKinClientProc	DllApi<procCreateKinClient>::load(MAKE_DLL_NAME(KinClient), "CreateKinClient")
#	define	CloseKinClientProc	DllApi<procCreateKinClient>::freelib
#endif