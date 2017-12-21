/*******************************************************************
** �ļ���:	ITasktClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	7/12/2016
** ��  ��:	1.0
** ��  ��:	����ӿ�
********************************************************************/

#pragma once

struct ITaskClient
{
	/**
	@name   : ����
	*/
	virtual bool Create() = 0;

	/**
	@name   : �ͷ�
	*/
	virtual bool Release() = 0;
};


////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(EntityClient_STATIC_LIB)       ///��̬��汾
#	pragma comment(lib, MAKE_LIB_NAME(EntityClient))
extern "C" ITaskClient *CreateTaskClient(void);
#	define CreateTaskClientProc CreateTaskClient
#else															///��̬��汾
typedef ITaskClient *(RKT_CDECL *procCreateTaskClient)(void);
#	define CreateTaskClientProc DllApi<procCreateTaskClient>::load(MAKE_DLL_NAME(EntityClient), "CreateTaskClient")
#endif     


