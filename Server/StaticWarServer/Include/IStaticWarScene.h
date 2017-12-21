/*******************************************************************
** �ļ���:	E:\speed\Server\OrderServer\Include\IStaticWarScene.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	����ս�����ݷ���			
  
********************************************************************/

#pragma once

struct IStaticWarScene
{
	// ���� 
	virtual bool			load() = 0;
    // ִ���ͷ�Service
    virtual void            onStop() = 0;
	// �ͷ� 
	virtual void			release() = 0;

	// ��ȡ������������������
	virtual SERVICE_PTR		getStaticWarSceneService() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(StaticWarSceneServer_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(StaticWarSceneServer))
extern "C" IStaticWarScene * CreateStaticWarScene();
#	define	CreateStaticWarSceneProc	CreateStaticWarScene
#else													/// ��̬��汾
typedef IStaticWarScene * (RKT_CDECL * procCreateStaticWarScene)();
#	define	CreateStaticWarSceneProc	DllApi<procCreateStaticWarScene>::load(MAKE_DLL_NAME(StaticWarServer), "CreateStaticWarScene")
#endif


