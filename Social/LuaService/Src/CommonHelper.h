/*******************************************************************
** 文件名:	CommonHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	公共帮助函数
********************************************************************/

#include "stdafx.h"
#include "SocialLuaAPI.h"

/**
读取csv文件
@param 读取是否成功
*/
MY_LUA_API int readCsvFile(lua_State *L);

/**
输出信息
@param 输出字符串
*/
MY_LUA_API void trace(const char *pStr);

/**
输出警告
@param 输出字符串
*/
MY_LUA_API void warning(const char *pStr);

/**
输出错误
@param 输出字符串
*/
MY_LUA_API void error(const char *pStr);

/**
输出严重错误
@param 输出字符串
*/
MY_LUA_API void emphasis(const char *pStr);

/**
输出信息,并换行
@param 输出字符串
*/
MY_LUA_API void traceLn(const char *pStr);

/**
输出警告,并换行
@param 输出字符串
*/
MY_LUA_API void warningLn(const char *pStr);

/**
输出错误,并换行
@param 输出字符串
*/
MY_LUA_API void errorLn(const char *pStr);

/**
输出严重错误,并换行
@param 输出字符串
*/
MY_LUA_API void emphasisLn(const char *pStr);