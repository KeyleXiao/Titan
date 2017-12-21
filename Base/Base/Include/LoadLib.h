/**
* Created by PeakGao 2008.01.03
*/

#ifndef RKT_LOADLIB_H
#define RKT_LOADLIB_H

#include "Common.h"

#ifdef SUPPORT_LOADLIB

#ifdef RKT_WIN32
#	include <windows.h>
#	include <stdio.h>
#elif defined (RKT_LINUX)
#	include <dlfcn.h>
#endif

namespace rkt {

	/** 加载一个动态链接库
	@param filename 动态链接库的路径名
	@retval 成功返回指定动态链接库的句柄，否则返回NULL
	@see freelib
	*/
	RKT_API void* loadlib(const char* filename);

	/** 释放一个动态链接库
	@param moduleHandle 要释放的动态链接库的句柄
	@return 成功返回true，否则返回false
	@see loadlib
	*/
	RKT_API bool freelib(void* moduleHandle);


	/// api函数类型的定义，同一种调用方式下函数原型可以重新定义，不影响返回的结果
	typedef int (RKT_CDECL *Proc)(void);


	/** 获取动态链接库中对应API的函数地址
	@param moduleHandle 要释放的动态链接库的句柄
	@param procName 函数名称（请保证大小写没有错误）
	@retval 返回对应函数的地址
	*/
	RKT_API Proc getProcAddress(void* moduleHandle, const char* procName);


	/// DLL对象创建辅助类
	template<class _Proc>
	class DllApi
	{
	protected:
		static void* handle;	/// dll句柄
		static int ref;			/// 引用计数

	public:
		static void freelib()
		{
#ifdef RKT_WIN32
			if (handle && --ref == 0)
			{
				::FreeLibrary((HMODULE)handle);
				handle = 0;
			}
#else
			if (handle && --ref == 0)
			{
				dlclose(handle);
				handle = 0;
			}
#endif
		}

		static _Proc load(const char* filename, const char* proc)
		{
#ifdef RKT_WIN32
			if (!handle)
			{
				handle = ::LoadLibraryA(filename);
				if (!handle)
				{
					ulong errId = ::GetLastError();
					ErrorLn("LoadLibrary(\""<<filename<<"\") failed, error code: "<<errId);
					return NULL;
				}
				
				ref = 1;
			}
			else ref++;

			_Proc func = (_Proc)GetProcAddress((HMODULE)handle, proc);
			if (!func)
			{
				ulong errId = ::GetLastError();
				ErrorLn("GetProcAddress(\""<<proc<<"\") failed, error code: "<<errId);
				return NULL;
			}
			return func;
#else
			if (!handle)
			{
				handle = dlopen(filename, RTLD_NOW);
				if (!handle) return NULL;
				ref = 1;
			}
			else ref++;

			return (_Proc)dlsym(handle, proc);
#endif
		}
	};

	template<class _Proc> void* DllApi<_Proc>::handle = 0;
	template<class _Proc> int DllApi<_Proc>::ref = 0;

/* 例子
	// IFoo.h
	#pragma once

	#include "LoadLib.h"

	namespace rkt {

		struct IFoo
		{
			virtual int foo(int a) = 0;
			virtual void release() = 0;
		};


		#if defined(_LIB) || defined(FOO_STATIC_LIB)/// 静态库版本
		#	pragma comment(lib, MAKE_LIB_NAME(Foo))
		extern "C" IFoo* CreateFoo(int dummy);
		#	define	CreateFooProc	CreateFoo
		#else /// 动态库版本
		typedef IFoo* (RKT_CDECL *procCreateFoo)(int dummy);
		#	define	CreateFooProc	DllApi<procCreateFoo>::load(MAKE_DLL_NAME(Foo), "CreateFoo")
		#endif

	}

	// Foo.cpp
	#include "stdafx.h"
	#include "Foo.h"

	namespace rkt {

		API_EXPORT IFoo* CreateFoo(int dummy)
		{
			return static_cast<IFoo*>(new Foo());
		}

	}

	// main.cpp
	#include "stdafx.h"

	#define FOO_STATIC_LIB
	#include "IFoo.h"

	int _tmain(int argc, _TCHAR* argv[])
	{
		using namespace rkt;
		IFoo* i = CreateFooProc(25);
		int ret = i->foo(10);
		i->release();

		return 0;
	}
*/
}

#endif // #ifdef SUPPORT_LOADLIB

#endif // RKT_LOADLIB_H