/*******************************************************************
** �ļ���:	ILogoutPart.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	
** ��  ��:	
** ��  ��:	1.0
** ��  ��:	ʵ�����ߡ����� ����

********************************************************************/

#pragma once

#include "IEntity.h"

// ʵ�����ݲֿⲿ���ӿ�
struct __ILogoutPart : public __IEntityPart
{
    virtual int getLogoutReason() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __ILogoutPart * CreateLogoutPart();
#	define	CreateLogoutPartProc	CreateLogoutPart
#else													/// ��̬��汾
typedef __ILogoutPart * (RKT_CDECL * procCreateLogoutPart)();
#	define	CreateLogoutPartProc	DllApi<procCreateLogoutPart>::load(MAKE_DLL_NAME(ENTITY), "CreateLogoutPart")
#endif
