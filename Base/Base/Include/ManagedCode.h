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
    using namespace System::Reflection;
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

// 定义委托(即C++中的函数指针)
#ifdef _MANAGED
#define     REF ^
#define DELEGATE(TYPE,NAME) public delegate TYPE NAME
#	else
#define DELEGATE(TYPE,NAME) typedef TYPE (__stdcall *NAME)
#define     REF     
#endif

// 基础数据类型定义
#ifdef _MANAGED 
// for c# defined
#	define BYTE						Byte
#	define sp_string				String ^
#	define sp_const_string			String ^
#	define sp_pointer				IntPtr

#define sp_char_array(name, size) \
	[MarshalAs(UnmanagedType::ByValTStr, SizeConst=size)]\
    String^ name; 

#define sp_int_array(name, size) \
    [MarshalAs(UnmanagedType::ByValArray, SizeConst=size)]\
    array<int>^ name;

#define sp_longlong_array(name, size) \
	[MarshalAs(UnmanagedType::ByValArray, SizeConst=size)]\
	array<long long>^ name;

#define sp_float_array(name, size) \
    [MarshalAs(UnmanagedType::ByValArray, SizeConst=size)]\
    array<float>^ name;

#define sp_struct_array(struct_name, array_name, size) \
    [MarshalAs(UnmanagedType::ByValArray, SizeConst=size)]\
    array<struct_name>^ array_name;

#else
// for c++ defined
#	define sp_string				char *
#	define sp_const_string			const char *
#	define sp_pointer				void *

#define sp_char_array(name, size) \
	char name[size];

#define sp_int_array(name, size) \
    int name[size];

#define sp_longlong_array(name, size) \
	long long name[size];

#define sp_float_array(name, size) \
    float name[size];

#define sp_struct_array(struct_name, array_name, size) \
    struct_name array_name[size];

#endif


// 结构体初始化
#ifdef _MANAGED
#	define STRUCT_INIT(STRUCT_NAME)
#else
#	define STRUCT_INIT(STRUCT_NAME)	\
		STRUCT_NAME() { memset(this,0,sizeof(*this));};
#endif


// 定义注释(注释会自动导出为特性)
#	ifdef _MANAGED
#		ifdef SPELL_EDITOR
#			define  DESC(str)       [Desc(str)]
#           define  BITMASK(str)    [BitMaskFlag(str)]
#		else
#			define	DESC(str)
#           define  BITMASK(str)
#		endif

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

// 位映射对应属性
public ref struct BitMaskFlagAttribute : public Attribute
{
public:
    BitMaskFlagAttribute(String^ strEnum)
    {
        _strMaskEnum = strEnum;
    }

    String^ getValue()
    {
        return _strMaskEnum;
    }
private:
    String ^    _strMaskEnum;
};

#	else
#		define  DESC(str)
#       define  BITMASK(str)
#	endif

// 通用BIN文件头结构
STRUCT BIN_FILE_HEADER
{
	sp_char_array(szMagicFlag,4);  // 魔数，恒为:"BIN" (如果不为这个数，则表示不是这个类型)
	sp_char_array(szTypeName,64);  // 类型名
	int             nTypeSize;     // 类型大小
	int             nItemTotal;    // 节点数目     
};