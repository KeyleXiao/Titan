#include "stdafx.h"
#include "LuaService_Proxy.h"
#include "StubMacroDef.h"

////// ILuaService ////////////////////////////////////////////////////////////////////
/** 执行一个lua脚本
@param   szLuaFileName ： lua文件名
@return  成功：返回true,失败：false
*/
void LuaService_Proxy::LoadLuaFile(const char* szLuaFileName) 
{
	if (m_pContainer == 0)
	{
		return;
	}
	obuf256 t_data;
	t_data << szLuaFileName;

	BUILD_MSG_BUFFER( ILuaService::LoadLuaFile,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};

//////////////////////////////////////////////////////////////////////////////////
/** 执行一段内存里lua
@param   pLuaData : 载入的lua 数据
@param   nDataLen ：数据长度
@return  成功：返回true,失败：false
*/
void LuaService_Proxy::RunMemoryLua(const char* pLuaData, int nDataLen)
{
	if (m_pContainer == 0)
	{
		return;
	}
	obuf256 t_data;
	t_data << pLuaData << nDataLen;

	BUILD_MSG_BUFFER( ILuaService::RunMemoryLua,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};

//////////////////////////////////////////////////////////////////////////////////
/** 执行一个lua函数
@param   szFuncName : lua函数名
@param   pInParam : 输入参数数组指针
@param   nInNum : 输入参数个数
@param   pOutParam : 输出参数数组指针
@param   nOutNum : 输出参数个数
@return  成功：返回true,失败：false
*/
void LuaService_Proxy::RunLuaFunction(const char* szFunName, CLuaParam * pInParam, int nInNum/*, CLuaParam * pOutParam, int nOutNum*/)
{
	if (m_pContainer == 0)
	{
		return;
	}
	obuf256 t_data;
	t_data << szFunName;	// LUA函数
	t_data << nInNum;	// 输入参数数量
	if (nInNum > 0)
	{
		for (int i=0; i<nInNum; ++i)
		{
			t_data << pInParam[i];
		}
	}

	BUILD_MSG_BUFFER( ILuaService::RunLuaFunction,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};

/** 是否存在某个函数
*/
bool LuaService_Proxy::IsFunction(const char * szFunName)
{
	if (m_pContainer == 0)
	{
		return false;
	}
    obuf256 t_data;
	t_data << szFunName;

	BUILD_MSG_BUFFER( ILuaService::IsFunction,t_data);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf,MSG_FLAG_READ_ONLY);
	RETURN_FROM_MSG_BUF(bool);
	return false;
};


/** 刷新lua脚本
@param   szLuaFileName ： lua文件名
@return  成功：返回true,失败：false
*/
void LuaService_Proxy::UpdateLuaFile(const char* szLuaFileName)
{
	if (m_pContainer == 0)
	{
		return;
	}
    obuf256 t_data;
    t_data << szLuaFileName;

    BUILD_MSG_BUFFER( ILuaService::UpdateLuaFile,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}


// 释放
void LuaService_Proxy::release()
{
	if (m_pContainer == 0)
	{
		return;
	}
	obuf256 t_data;

	BUILD_MSG_BUFFER( ILuaService::release,t_data);

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
};