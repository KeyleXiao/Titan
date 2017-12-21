/*******************************************************************
** �ļ���:	E:\speed\Server\KinServer\Include\IKinScene.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�ϵͳ������			
  
********************************************************************/

#pragma once
#include "KinDef.h"
#include "IServiceContainer.h"

// ս�Ӷ���ӿ�
struct IKin
{
    /// purpose: �ͷ�
    virtual void release() = 0;

    /// purpose: ȡ��ս�ӵ���������
    virtual int getNumProp(DWORD dwPropID) = 0;

    /// purpose: ȡ��ս������
    virtual LPCSTR getName() = 0;

};


struct IKinScene
{
	/// purpose:  ���� 
	virtual bool			load() = 0;
    // ִ���ͷ�Service
    virtual void            onStop() = 0;
	/// purpose:  �ͷ� 
	virtual void			release() = 0;

	/// purpose:  ��ȡս�ӷ���
	virtual SERVICE_PTR		getKinSceneService() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(KinSceneServer_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(KinSceneServer))
extern "C" IKinScene * CreateKinScene();
#	define	CreateKinSceneProc	CreateKinScene
#else													/// ��̬��汾
typedef IKinScene * (RKT_CDECL * procCreateKinScene)();
#	define	CreateKinSceneProc	DllApi<procCreateKinScene>::load(MAKE_DLL_NAME(KinServer), "CreateKinScene")
#endif


