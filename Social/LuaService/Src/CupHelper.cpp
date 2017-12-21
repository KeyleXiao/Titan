
#include "stdafx.h"
#include "lua.h"
#include "CupHelper.h"
#include "ISocialGlobal.h"
#include "LegendCupHelper.h"

MY_LUA_API void addCupRegister(DWORD dwLengedCupID, const char *pStr)
{
    // 添加杯赛请求
    if (pStr == NULL)
    {
        return;
    }

    ErrorLn("addCupRegister : dwLengedCupID="<<dwLengedCupID);
}

MY_LUA_API void removeCupRegister(DWORD dwLengedCupID, const char *pStr)
{
    // 移除杯赛请求
    if (pStr == NULL)
    {
        return;
    }
    ErrorLn("addCupRegister : dwLengedCupID="<<dwLengedCupID);
}
