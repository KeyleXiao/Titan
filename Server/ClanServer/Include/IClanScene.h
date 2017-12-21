/*******************************************************************
** �ļ���:	IClanScene.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/6/2016
** ��  ��:	1.0
** ��  ��:	��᳡���ӿ�
********************************************************************/

#pragma once
#include "ClanDef.h"
#include "IServiceContainer.h"
#include "GatewayServiceDef.h"
using Gateway::ClientID;

// ��᳡���ӿ�
struct IClanScene
{
	// ���� 
	virtual bool			load() = 0;
    // ִ���ͷ�Service
    virtual void            onStop() = 0;
	// �ͷ� 
	virtual void			release() = 0;

	// ��ȡ������
	virtual SERVICE_PTR		getClanSceneService() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(CLANSCENESERVER_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ClanSceneServer))
extern "C" IClanScene * CreateClanScene();
#	define	CreateClanSceneProc	CreateClanScene
#else													/// ��̬��汾
typedef IClanScene * (RKT_CDECL * procCreateClanScene)();
#	define	CreateClanSceneProc	DllApi<procCreateClanScene>::load(MAKE_DLL_NAME(ClanServer), "CreateClanScene")
#endif


