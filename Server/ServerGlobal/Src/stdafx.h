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
#include "Event_ManagedDef.h"
#include "TestModuleDef.h"
#include "ControllerDef.h"

#ifndef SAFE_RELEASE
#	define SAFE_RELEASE(ptr)		if ((ptr)) {(ptr)->Release(); (ptr) = 0;}
#endif
#ifndef Safe_Release
#	define Safe_Release(ptr)		if ((ptr)) {(ptr)->release(); (ptr) = 0;}
#endif
#ifndef SAFE_DELETEARRAY
#   define SAFE_DELETEARRAY(data)   if(data) { delete[] (data); data=NULL;};
#endif