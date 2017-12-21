/*******************************************************************
** �ļ���:	ILuaManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	12/27/2016
** ��  ��:	1.0
** ��  ��:	Lua����
********************************************************************/


#pragma once
#include "IServiceContainer.h"

struct ILuaManager
{
	// ���Lus����
	virtual SERVICE_PTR getLuaService() = 0;

    // ���� 
	virtual bool	load() = 0;

	// �ͷ� 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(LUASERVICE_STATIC_LIB)		/// ��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(LuaService))
extern "C" ILuaManager * CreateLuaManager();
#	define	CreateLuaManagerProc	CreateLuaManager
#else													/// ��̬��汾
typedef ILuaManager * (RKT_CDECL * procCreateLuaManager)();
#	define	CreateLuaManagerProc	DllApi<procCreateLuaManager>::load(MAKE_DLL_NAME(LuaService), "CreateLuaManager")
#endif