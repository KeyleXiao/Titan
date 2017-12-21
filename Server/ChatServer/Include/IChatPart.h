/*******************************************************************
** �ļ���:	__IChatPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	

********************************************************************/

#pragma once
#include "IEntity.h"

struct __IChatPart:public __IEntityPart
{

};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IChatPart * CreateChatPart();
#	define	CreateChatPartProc	CreateChatPart
#else													/// ��̬��汾
typedef __IChatPart * (RKT_CDECL * procCreateChatPart)();
#	define	CreateChatPartProc	DllApi<procCreateChatPart>::load(MAKE_DLL_NAME(ChatSceneServer), "CreateChatPart")
#endif
