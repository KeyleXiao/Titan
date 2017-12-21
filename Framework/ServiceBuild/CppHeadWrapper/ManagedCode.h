/*******************************************************************
** 文件名:	ManagedCode.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	陈涛
** 日  期:	2015/1/10
** 版  本:	1.0
** 描  述:	托管与非托管代码定义
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:	
********************************************************************/

#pragma once

#ifdef _MANAGED
	using namespace System;
	using namespace System::Runtime::InteropServices;
#endif

// 定义枚举结构
#ifdef _MANAGED
#define   ENUM  public enum class
#else
#define   ENUM  enum
#endif

// 定义结构
#ifdef _MANAGED
#define   STRUCT  public value struct 
#else
#define   STRUCT  struct
#endif

// 定义类
#ifdef _MANAGED
#define   CLASS  public ref class 
#else
#define   CLASS  class
#endif

// 定义纯虚函数
#ifdef _MANAGED
#define   PURE_VIRTUAL   abstract 
#else
#define   PURE_VIRTUAL  =0
#endif

// 定义委托(即C++中的函数指针)
#ifdef _MANAGED
#define     REF ^
#define DELEGATE(TYPE,NAME) public delegate TYPE NAME
#	else
#define DELEGATE(TYPE,NAME) typedef TYPE (__stdcall *NAME)
#define     REF     
#endif

// 指针定义
#ifdef _MANAGED
#define   POINTER  ^
#else
#define   POINTER  *
#endif

// 默认值定义
#ifdef _MANAGED
#define   DEFAULT(X)
#else
#define   DEFAULT(X)  =X
#endif

// 基础数据类型定义
#ifdef _MANAGED 
#	define BYTE						Byte
#	define sp_string				String ^
#	define sp_const_string			String ^
#	define sp_pointer				IntPtr
#define sp_char_array(name, size) \
	[MarshalAs(UnmanagedType::ByValTStr, SizeConst=size)]\
	String^ name; 
#else
#	define sp_string				char *
#	define sp_const_string			const char *
#	define sp_pointer				void *
#define sp_char_array(name, size) \
	char name[size];
#endif

// 定义注释(注释会自动导出为特性)
#	ifdef _MANAGED
#		define     DESC(str) [Desc(str)]

	// 实现描述信息
	[AttributeUsage(AttributeTargets::All)]
	public ref struct DescAttribute : public Attribute {
	public:
		DescAttribute(String ^ Description_in)
		{
			description = Description_in;
		}

		String ^ description;
	};


#	else
#		define     DESC(str) 
#	endif

	// 定义函数只读属性(会自动导出为特性)
#	ifdef _MANAGED
#		define     READONLY(b) [ReadOnly(b)]

	// 实现描述信息
	[AttributeUsage(AttributeTargets::All)]
	public ref struct ReadOnlyAttribute : public Attribute {
	public:
		ReadOnlyAttribute(bool readonly_attr_in)
		{
			readonly_attr = readonly_attr_in;
		}

		bool  readonly_attr;
	};


#	else
#		define     READONLY(b) 
#	endif

// 通用BIN文件头结构
STRUCT BIN_FILE_HEADER
{
	sp_char_array(szMagicFlag,4);  // 魔数，恒为:"BIN" (如果不为这个数，则表示不是这个类型)
	sp_char_array(szTypeName,64);  // 类型名
	int             nTypeSize;     // 类型大小
	int             nItemTotal;    // 节点数目     
};