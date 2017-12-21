/*******************************************************************
** 文件名:	e:\CoGame\LuaEngine\build\ILuaEngine.h
** 版  权:	(C) 深圳未名网络技术有限公司(www.weimingtech.com)
** 创建人:	隆寒辉(Forlion Lon)
** 日  期:	2007/8/24  19:22
** 版  本:	1.0
** 描  述:	Lua引擎
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "Config.h"

#ifdef SUPPORT_LUAENGINE

#include "buffer.h"
#include "Api.h"

typedef struct lua_State lua_State;

namespace rkt {

/*! \addtogroup LuaEngine
*  lua引擎
*  @{
*/

enum
{
	PARAM_DATATYPE_NONE = -1,			/// LUA_TNONE
	PARAM_DATATYPE_NIL = 0,				/// LUA_TNIL
	PARAM_DATATYPE_BOOLEAN = 1,			/// LUA_TBOOLEAN
	PARAM_DATATYPE_LIGHTUSERDATA = 2,	/// LUA_TLIGHTUSERDATA
	PARAM_DATATYPE_NUMBER = 3,			/// LUA_TNUMBER
	PARAM_DATATYPE_STRING =	4,			/// LUA_TSTRING
	PARAM_DATATYPE_TABLE = 5,			/// LUA_TTABLE
	PARAM_DATATYPE_FUNCTION = 6,		/// LUA_TFUNCTION
	PARAM_DATATYPE_USERDATA	= 7,		/// LUA_TUSERDATA
	PARAM_DATATYPE_THREAD = 8,			/// LUA_TTHREAD
};

#pragma pack(push)
#pragma pack(4)

/// Lua参数，这个类只定位成一个简单类，没有考虑多次赋值带来的泄露隐患以及弱类型处理，强转类型可能出错
class CLuaParam
{
	enum { _BUF_SIZE = 256 };
	int				m_nDataType:8;	/// 数据类型
	int				m_strLen:24;	/// 当为字符串时，表达字符串的长度

	union{
		char		m_Buf[_BUF_SIZE];
		char*		m_Ptr;
		void*		m_userData;
		__int64		m_Value;
	};

public:
	CLuaParam(void)
	{
        memset(m_Buf, 0, sizeof(m_Buf));
        m_Ptr = NULL;
        m_userData = NULL;
		m_nDataType = PARAM_DATATYPE_NUMBER;
		m_strLen = 0;
		m_Value = 0;
	}

	~CLuaParam(void)
	{
		if (m_nDataType == PARAM_DATATYPE_STRING && m_strLen >= _BUF_SIZE)
			delete[] m_Ptr;
		m_nDataType = PARAM_DATATYPE_NUMBER;
		m_Value = 0;
	}

	void operator = (__int64 nValue)
	{
		m_nDataType = PARAM_DATATYPE_NUMBER;
		m_Value = nValue;
	}

	void operator = (const char* pszValue)
	{
		m_nDataType = PARAM_DATATYPE_STRING;
		if (pszValue)
		{
			size_t len = strlen(pszValue);
			if (len >= _BUF_SIZE)
			{
                if (m_Ptr != NULL)
                {
                    delete[] m_Ptr;
                }
				m_Ptr = new char[len+1];
				sstrcpyn(m_Ptr, pszValue, len+1);
			}
			else
			{
				sstrcpyn(m_Buf, pszValue, _BUF_SIZE);
			}
			m_strLen = len;
		}
	}

	void operator = (void * pValue)
	{
		m_nDataType = PARAM_DATATYPE_LIGHTUSERDATA;		
		m_userData = pValue;
	}

	operator const char*(void)
	{
		return m_strLen >= _BUF_SIZE ? m_Ptr : m_Buf;
	}

	operator int(void)
	{
		return (int)m_Value;
	}

	operator __int64(void)
	{
		return m_Value;
	}

	operator void *(void)
	{
		return m_userData;
	}

	int		GetDataType(void)
	{
		return m_nDataType;
	}
};

#pragma pack(pop)

/// lua错误回调接口
struct ILuaErrorCallback
{
	/**
	@param err_desc	错误的描述
	@param context	上下文数据，如文件名，函数名，内存字符串等
	*/
	virtual void onLuaError(const char* err_desc, const char* context) = 0;
};


/// lua服务器
struct RKT_EXPORT ILuaEngine
{
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			Release(void) = 0;

	/** 取得lua_State *
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual lua_State *		GetLuaState(void) = 0;	

	/** 执行一个lua脚本
	@param   szLuaFileName ： lua文件名
	@param   
	@param   
	@return  成功：返回true,失败：false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			LoadLuaFile(const char* szLuaFileName) = 0;

	/** 执行一段内存里lua
	@param   pLuaData : 载入的lua 数据
	@param   nDataLen ：数据长度
	@param   
	@return  成功：返回true,失败：false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			RunMemoryLua(const char* pLuaData, int nDataLen) = 0;

	/** 执行一个lua函数
	@param   szFuncName : lua函数名
	@param   pInParam : 输入参数数组指针
	@param   nInNum : 输入参数个数
	@param   pOutParam : 输出参数数组指针
	@param   nOutNum : 输出参数个数
	@return  成功：返回true,失败：false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			RunLuaFunction(const char* szFunName, CLuaParam * pInParam = 0, int nInNum = 0, CLuaParam * pOutParam = 0, int nOutNum = 0) = 0;

	/** 取得最近发送的lua错误
	@param
	@param
	@param 
	@return  错误描述
	@note     
	@warning 
	@retval buffer 
	*/
	virtual const char *	GetLastError(void) = 0;

	/** 是否存在某个函数
	@param   
	@param   
	@return  
	*/
	virtual bool			IsFunction(const char * szFunName) = 0;

   /** 追加Lua搜索路径（package.path）
	@param  szPath: 路径 
	@param   
	@return  
	*/
    virtual bool            AppendLuaPath( const char* szPath ) = 0;
};

RKT_API ILuaEngine * CreateLuaEngine(bool bFromPackLoadLua);

///////////////////////////////////////////////////////////////////////////////////////////////////////
#define	CreateLuaEngineProc	CreateLuaEngine


} //*namespace rkt


#endif // #ifdef SUPPORT_LUAENGINE