/*******************************************************************
** 文件名:	CsvReadHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	公共帮助函数
********************************************************************/

#include "stdafx.h"
#include "lua.h"
#include "CommonHelper.h"
#include "CsvReadHelper.h"
#include "ISocialGlobal.h"

/**
读取csv文件
@param 读取是否成功
*/
MY_LUA_API int readCsvFile(lua_State *pLuaState)
{
    if (pLuaState == NULL)
    {
        return 0;
    }

    ISchemeEngine *pSchemeEngine = gSocialGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return 0;
    }

    const char* szFile =  ((const char*)lua_tolstring(pLuaState,1,0));
    std::string strFileName = szFile;
    StringHelper::toLower(strFileName);
    if ( strFileName.find("scp\\")==std::string::npos )
    {
        strFileName = "Scp\\" + strFileName;
        szFile = strFileName.c_str();
    }

    lua_newtable(pLuaState); // -1是要返回的表
    CsvReadHelper reader(pLuaState);
    bool bLoaded = pSchemeEngine->LoadScheme(szFile, &reader,false);
    char fileName[256] = {0};
    sstrcpyn(fileName, szFile, sizeof(fileName));
    strlwr(fileName);
    pSchemeEngine->UnregisterSink(fileName, &reader);

    if (!bLoaded)
    {
        return 0;
    }

    return 1;
}

/**
输出信息
@param 输出字符串
*/
MY_LUA_API void trace(const char *pStr)
{
    if (pStr == NULL)
    {
        return;
    }


    Trace(pStr);
}

/**
输出警告
@param 输出字符串
*/
MY_LUA_API void warning(const char *pStr)
{
    if (pStr == NULL)
    {
        return;
    }

    Warning(pStr);
}

/**
输出错误
@param 输出字符串
*/
MY_LUA_API void error(const char *pStr)
{
    if (pStr == NULL)
    {
        return;
    }

    Error(pStr);
}

/**
输出严重错误
@param 输出字符串
*/
MY_LUA_API void emphasis(const char *pStr)
{
    if (pStr == NULL)
    {
        return;
    }

    Emphasis(pStr);
}

/**
输出信息,并换行
@param 输出字符串
*/
MY_LUA_API void traceLn(const char *pStr)
{
    if (pStr == NULL)
    {
        return;
    }

    TraceLn(pStr);
}

/**
输出警告,并换行
@param 输出字符串
*/
MY_LUA_API void warningLn(const char *pStr)
{
    if (pStr == NULL)
    {
        return;
    }

    WarningLn(pStr);
}

/**
输出错误,并换行
@param 输出字符串
*/
MY_LUA_API void errorLn(const char *pStr)
{
    if (pStr == NULL)
    {
        return;
    }

    ErrorLn(pStr);
}

/**
输出严重错误,并换行
@param 输出字符串
*/
MY_LUA_API void emphasisLn(const char *pStr)
{
    if (pStr == NULL)
    {
        return;
    }

    EmphasisLn(pStr);
}