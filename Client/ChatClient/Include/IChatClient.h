/*******************************************************************
** �ļ���:	IChatClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	����
** ��  ��:	2015/10/19  17:21
** ��  ��:	1.0
** ��  ��:	����ϵͳ�ͻ���
** Ӧ  ��:  	
	
*******************************************************************/

#pragma once

#include "ChatDef.h"
#include "IChatPart.h"

struct IChatClient
{
	// �ͷ�
	virtual void release(void) = 0;

	// ��ʵ�崴�����첿��������new�Ĳ����ŵ��˷���������ǻ��յģ�����Ҫ
	virtual IChatPart* CreatePart(void) = 0;

	// ��ʾϵͳ��ʾ(�����÷ֺŷָ�)
	// @param tipID: EMChatTipID
	virtual void showSystemMessage(int tipID, LPCSTR szParam = NULL, WORD nReserveData = 0) const = 0;

	// ��ȡϵͳ��ʾ
	virtual std::string getSystemTipText(LPCSTR szParam, int nTipID) const = 0;
};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// ��̬����
#	pragma comment(lib, MAKE_LIB_NAME(ChatClient))
extern "C" IChatClient * CreateChatClient(void);
#	define	CreateChatClientProc	CreateChatClient
#	define	CloseChatClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // ��̬����
#	pragma comment(lib, MAKE_DLL_LIB_NAME(ChatClient))
API_IMPORT IChatClient * CreateChatClient(void);
#	define	CreateChatClientProc	CreateChatClient
#	define	CloseChatClientProc()
#else													// ��̬����
typedef IChatClient * (RKT_CDECL *procCreateChatClient)(void);
#	define	CreateChatClientProc	DllApi<procCreateChatClient>::load(MAKE_DLL_NAME(ChatClient), "CreateChatClient")
#	define	CloseChatClientProc	DllApi<procCreateChatClient>::freelib
#endif