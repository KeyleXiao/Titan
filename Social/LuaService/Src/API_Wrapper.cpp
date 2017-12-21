/*******************************************************************
** 文件名:	API_Wrapper.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	社会服API封装
********************************************************************/


#include "stdafx.h"
 
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge/LuaBridge.h"
#include "ILuaEngine.h"

#include "CommonHelper.h"
#include "SocialLuaAPI.h"
#include "CupHelper.h"

using namespace  luabridge;

extern "C" __declspec(dllexport) bool BindLuaAPI(ILuaEngine * pLua)
{
	lua_State * L = pLua->GetLuaState();

    getGlobalNamespace (L)
        .beginNamespace ("commonHepler")
        .addCFunction ("readCsvFile", &readCsvFile)
        .addFunction ("trace", &trace)
        .addFunction ("warning", &warning)
        .addFunction ("error", &error)
        .addFunction ("emphasis", &emphasis)
        .addFunction ("traceLn", &traceLn)
        .addFunction ("warningLn", &warningLn)
        .addFunction ("errorLn", &errorLn)
        .addFunction ("emphasisLn", &emphasisLn)
        .addFunction ("getTickCountEx", &getTickCountEx)
    .endNamespace ();

    getGlobalNamespace (L)
        .beginNamespace ("LegendCupHelper")
        .addFunction ("addCupRegister", &addCupRegister)
        .addFunction ("removeCupRegister", &removeCupRegister)
    .endNamespace ();

	return true;
}
