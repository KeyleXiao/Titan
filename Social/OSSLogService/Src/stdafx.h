// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "Base.h"
#pragma comment(lib, MAKE_DLL_LIB_NAME_BY_ARCH(Base))
using namespace rkt;

#include "GameDef.h"
#include "StubMacroDef.h"
#include "NetMessageDef.h"
#include "EventDef.h"
#include "TaskStorageDef.h"
#include "IGameDatabaseDef.h"

#include "IDBEngine.h"
#include "ITimerService.h"
#include "IDBEngineService.h"
 
#include "ISocialGlobal.h"

#ifndef SAFE_RELEASE
#	define SAFE_RELEASE(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(p)			{ if((p) != NULL) { delete (p);     (p)=NULL; } }
#endif

#if !defined(SAFE_DELETEARRAY)
#define SAFE_DELETEARRAY(p)		{ if((p) != NULL) { delete [](p);	(p) = NULL;} }
#endif