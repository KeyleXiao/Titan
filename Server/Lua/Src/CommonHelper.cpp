/*******************************************************************
** 文件名:	CsvReadHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/24/2016
** 版  本:	1.0
** 描  述:	公共帮助函数
********************************************************************/

#include "stdafx.h"
#include "CommonHelper.h"
#include "CsvReadHelper.h"
#include "lua.h"
#include "IServerGlobal.h"
#include "ITimeSyncService.h"

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

    ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
    if (pSchemeEngine == NULL)
    {
        return 0;
    }

    const char* szFile =  ((const char*)lua_tolstring(pLuaState, 1, 0));
    std::string strFileName = szFile;
    StringHelper::toLower(strFileName);
    if ( strFileName.find("scp\\")==std::string::npos )
    {
        strFileName = "Scp\\" + strFileName;
        szFile = strFileName.c_str();
    }

    lua_newtable(pLuaState); // -1是要返回的表
    CsvReadHelper reader;
    bool bLoaded = reader.OnSchemeLoad(pLuaState, szFile);
    if (!bLoaded)
    {
        return 0;
    }

    return 1;
}

MY_LUA_API int getSyncTick()
{
    if (gServerGlobal == 0)
    {
        ErrorLn(__FUNCTION__ << ", gClientGlobal == 0");
        return 0;
    }

    if (gServerGlobal->getTimeSyncService() == 0)
    {
        ErrorLn(__FUNCTION__ << ", gClientGlobal->getTimeSyncService() == 0");
        return 0;
    }

    return gServerGlobal->getTimeSyncService()->GetTick();
}