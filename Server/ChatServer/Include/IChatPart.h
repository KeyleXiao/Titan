/*******************************************************************
** 文件名:	__IChatPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once
#include "IEntity.h"

struct __IChatPart:public __IEntityPart
{

};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IChatPart * CreateChatPart();
#	define	CreateChatPartProc	CreateChatPart
#else													/// 动态库版本
typedef __IChatPart * (RKT_CDECL * procCreateChatPart)();
#	define	CreateChatPartProc	DllApi<procCreateChatPart>::load(MAKE_DLL_NAME(ChatSceneServer), "CreateChatPart")
#endif
