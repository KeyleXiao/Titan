/*******************************************************************
** 文件名:	ILuaService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	本文件为根据ILuaService接口定义，自动生成的LuaService接口
            代理文件，该文件是为符合Framework对接口进行的包装
********************************************************************/

#pragma once

#include "ILuaEngine.h"

// ILuaService接口
struct ILuaService
{
	/** 执行一个lua脚本
	@param   szLuaFileName ： lua文件名
	@return  成功：返回true,失败：false
	*/
	virtual void			LoadLuaFile(const char* szLuaFileName) = 0;

	/** 执行一段内存里lua
	@param   pLuaData : 载入的lua 数据
	@param   nDataLen ：数据长度
	@return  成功：返回true,失败：false
	*/
	virtual void			RunMemoryLua(const char* pLuaData, int nDataLen)  = 0;

	/** 执行一个lua函数
	@param   szFuncName : lua函数名
	@param   pInParam : 输入参数数组指针
	@param   nInNum : 输入参数个数
	@param   pOutParam : 输出参数数组指针
	@param   nOutNum : 输出参数个数
	@return  成功：返回true,失败：false
	*/
	virtual void			RunLuaFunction(const char* szFunName, CLuaParam * pInParam =0, int nInNum =0/*, CLuaParam * pOutParam =0, int nOutNum =0*/)  = 0;

	/** 是否存在某个函数
	*/
	virtual bool			IsFunction(const char * szFunName)  = 0;

	/** 刷新lua脚本
	@param   szLuaFileName ： lua文件名
	@return  成功：返回true,失败：false
	*/
	virtual void			UpdateLuaFile(const char* szLuaFileName) = 0;

    // 释放
	virtual void			release() = 0;
};