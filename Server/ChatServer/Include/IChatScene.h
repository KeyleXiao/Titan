/*******************************************************************
** �ļ���:	E:\speed\Server\ChatServer\Include\IChatScene.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/16/2015	14:41
** ��  ��:	1.0
** ��  ��:	����ϵͳ������			
  
********************************************************************/

#pragma once
#include "ChatDef.h"

struct IChatScene
{
	// ���� 
	virtual bool			load() = 0;
    // ִ���ͷ�Service
    virtual void            onStop() = 0;
	// �ͷ� 
	virtual void			release() = 0;

	// ��ȡ���쳡��������
	virtual SERVICE_PTR		getChatSceneService() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ChatSceneServer_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ChatSceneServer))
extern "C" IChatScene * CreateChatScene();
#	define	CreateChatSceneProc	CreateChatScene
#else													/// ��̬��汾
typedef IChatScene * (RKT_CDECL * procCreateChatScene)();
#	define	CreateChatSceneProc	DllApi<procCreateChatScene>::load(MAKE_DLL_NAME(ChatSceneServer), "CreateChatScene")
#endif


