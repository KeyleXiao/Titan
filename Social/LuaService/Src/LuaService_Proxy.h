/*******************************************************************
** 文件名:	LuaService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	Lua服务代理
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "ILuaService.h"

using namespace rkt;
using namespace std;

class LuaService_Proxy : public ILuaService
{
public:
	////////////////// LuaService_Proxy ////////////////////////////////////////////////////////

	LuaService_Proxy() :m_pContainer(0) {};
	~LuaService_Proxy() {};
public:
	///////////////////////////////////ILuaService////////////////////////////
    /** 执行一个lua脚本
	@param   szLuaFileName ： lua文件名
	@return  成功：返回true,失败：false
	*/
	virtual void			LoadLuaFile(const char* szLuaFileName);

	/** 执行一段内存里lua
	@param   pLuaData : 载入的lua 数据
	@param   nDataLen ：数据长度
	@return  成功：返回true,失败：false
	*/
	virtual void			RunMemoryLua(const char* pLuaData, int nDataLen);

	/** 执行一个lua函数
	@param   szFuncName : lua函数名
	@param   pInParam : 输入参数数组指针
	@param   nInNum : 输入参数个数
	@param   pOutParam : 输出参数数组指针
	@param   nOutNum : 输出参数个数
	@return  成功：返回true,失败：false
	*/
	virtual void			RunLuaFunction(const char* szFunName, CLuaParam * pInParam =0, int nInNum =0/*, CLuaParam * pOutParam =0, int nOutNum =0*/);

	/** 是否存在某个函数
	*/
	virtual bool			IsFunction(const char * szFunName);

	/** 刷新lua脚本
	@param   szLuaFileName ： lua文件名
	@return  成功：返回true,失败：false
	*/
	virtual void			UpdateLuaFile(const char* szLuaFileName);

    // 释放
	virtual void			release();

public:
	SERVICE_PTR m_pContainer;
};