/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\MatchServer\Include\IMatch.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/15/2015	15:45
** ��  ��:	1.0
** ��  ��:	
			�������
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "ChatDef.h"

struct IChatManager
{
	// ������������
	virtual SERVICE_PTR getChatService() = 0;
	// ���� 
	virtual bool	load() = 0;
	// �ͷ� 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(CHATSERVICE_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ChatService))
extern "C" IChatManager * CreateChatManager();
#	define	CreateChatManagerProc	CreateChatManager
#else													/// ��̬��汾
typedef IChatManager * (RKT_CDECL * procCreateChatManager)();
#	define	CreateChatManagerProc	DllApi<procCreateChatManager>::load(MAKE_DLL_NAME(ChatService), "CreateChatManager")
#endif