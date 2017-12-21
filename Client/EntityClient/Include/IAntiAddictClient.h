/*******************************************************************
** �ļ���:	IAntiAddictClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	11/15/2016
** ��  ��:	1.0
** ��  ��:	

********************************************************************/

#pragma once

// �����Է���
struct IAntiAddictClient
{
	/**
	@name   : ���������Է���
	*/
	virtual bool Create() = 0;

	/**
	@name   : �ͷŷ����Է���
	*/
	virtual bool Release() = 0;
};


////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(EntityClient_STATIC_LIB)       ///��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(EntityClient))
extern "C" IAntiAddictClient *CreateAntiAddictClient(void);
#	define CreateAntiAddictClientProc CreateAntiAddictClient
#else															///��̬��汾
typedef IAntiAddictClient *(RKT_CDECL *procCreateAntiAddictClient)(void);
#	define CreateAntiAddictClientProc DllApi<procCreateAntiAddictClient>::load(MAKE_DLL_NAME(EntityClient), "CreateAntiAddictClient")
#endif     


