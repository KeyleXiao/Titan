/*******************************************************************
** 文件名:	LuaService.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	Lua服务实现
********************************************************************/

#pragma once

#include "stdafx.h"
#include "LuaService.h"
#include "API_Wrapper.h"
#include "ISocialGlobal.h"
#include "IResourceManager.h"

using namespace rkt;
using namespace std;

//////////////////////////////////////////////////////////////////////////

// 将lua目录加进搜索列表
inline DFP_RESULT LuaDirAddPathProc(bool isFile, const char* absolutePath, const char* relativePath, const char* name, uint userdata)
{
    std::string searchPath = absolutePath;
    //StringHelper::replace( searchPath,'\\','/');

    std::string & lua_path = *((std::string *)userdata);
    //lua_path += searchPath;
    //lua_path += "\\?;";

    lua_path += searchPath;
    lua_path += "\\?.lua;";

    return DFP_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////


/////////////////////////////////// ILuaService ///////////////////////////////////
/** 执行一个lua脚本
@param   szLuaFileName ： lua文件名
@return  成功：返回true,失败：false
*/
void LuaService::LoadLuaFile(const char* szLuaFileName)
{
	m_pLuaEngine->LoadLuaFile(szLuaFileName);
}

/** 执行一段内存里lua
@param   pLuaData : 载入的lua 数据
@param   nDataLen ：数据长度
@return  成功：返回true,失败：false
*/
void LuaService::RunMemoryLua(const char* pLuaData, int nDataLen)
{
	m_pLuaEngine->RunMemoryLua(pLuaData, nDataLen);
}

/** 执行一个lua函数
@param   szFuncName : lua函数名
@param   pInParam : 输入参数数组指针
@param   nInNum : 输入参数个数
@param   pOutParam : 输出参数数组指针
@param   nOutNum : 输出参数个数
@return  成功：返回true,失败：false
*/
void LuaService::RunLuaFunction(const char* szFunName, CLuaParam * pInParam, int nInNum/*, CLuaParam * pOutParam =0, int nOutNum =0*/)
{
	m_pLuaEngine->RunLuaFunction(szFunName, pInParam, nInNum);
}

/** 是否存在某个函数
*/
bool LuaService::IsFunction(const char * szFunName)
{
	return m_pLuaEngine->IsFunction(szFunName);
}

/** 刷新lua脚本
@param   szLuaFileName ： lua文件名
@return  成功：返回true,失败：false
*/
void LuaService::UpdateLuaFile(const char* szLuaFileName)
{
    // 没有初始化将不允许刷新LUA
    if(!m_isInit)
        return;

    if(szLuaFileName == NULL )
        return;

    string strLuaFileName = szLuaFileName;

    // 自动调unload函数
    std::string::size_type pos = strLuaFileName.find_last_of("\\");
    if(pos == std::string::npos)
    {
        return;
    }
    std::string::size_type num = strLuaFileName.size() - pos - strlen(".lua");
    string stringFileName(strLuaFileName, pos + 1, num - 1);
    //CharLowerBuffA((LPSTR)stringFileName.c_str(), stringFileName.size());
    std::transform(stringFileName.begin(), stringFileName.end(), stringFileName.begin(), ::tolower);
    stringFileName += "_unload";
    if(m_pLuaEngine->IsFunction(szLuaFileName))
    {
        m_pLuaEngine->RunLuaFunction(szLuaFileName);
    }

    char szBuf[512] = {0};
    bool bResult = m_pLuaEngine->LoadLuaFile(szLuaFileName);
    if(bResult)
    {
        sprintf_s(szBuf, sizeof(szBuf), "Update Lua Success!file=%s", szLuaFileName);
    }
    else
    {
        sprintf_s(szBuf, sizeof(szBuf), "Update Lua Fail!file=%s", szLuaFileName);
    }

    // 输出到管理器
    AlertEmphasis(MG_AlertSrcType_Lua, szBuf);
}

// 释放
void LuaService::release() 
{
    on_stop();
	delete this;
}

/////////////////////////////////// LuaService ///////////////////////////////////
LuaService::LuaService() : m_pLuaEngine(0),m_isInit(false),m_bFromPackLoadLua(false)
{
}

LuaService::~LuaService()
{
}

bool LuaService::on_start()
{
    IResourceManager * pResourceManger = gSocialGlobal->getResourceManager();
    m_bFromPackLoadLua = pResourceManger ? pResourceManger->isLoadLuaFromPack(): false;

    // 调用lua加载静态怪物和NPC
    m_pLuaEngine = CreateLuaEngine(m_bFromPackLoadLua);
    if ( m_pLuaEngine==0 )
    {
        ErrorLn("LuaEngine create failed!");
        return false;
    }

    // 构造API
    BindLuaAPIProc(m_pLuaEngine);
    
    if(m_bFromPackLoadLua)
    {
        string strMain = "LuaMain.lua";
        if (m_pLuaEngine->LoadLuaFile(strMain.c_str()))
        {
            // 载入主函数
            if(!m_pLuaEngine->RunLuaFunction("lua_main"))
            {
                return false;
            }
        }
        else
        {
            ErrorLn("LuaService Load error strMain="<<strMain.c_str());
            return false;
        }
    }
    else
    {
        string path = getWorkDir();

        // 加载默认版本LUA搜索目录路径
        string strLuaEntry = path;
        strLuaEntry += "\\Lua\\";

        std::string strLuaPath = "";
        browseDirectoryEx( strLuaEntry.c_str(), "", LuaDirAddPathProc, (uint)&strLuaPath, bdm_dir);
        // 追加本项目的LUA目录到PACKAG中
        m_pLuaEngine->AppendLuaPath( strLuaPath.c_str() );


        string strMain = strLuaEntry + "LuaMain.lua";
        if (m_pLuaEngine->LoadLuaFile(strMain.c_str()))
        {
            // 载入主函数
            if(!m_pLuaEngine->RunLuaFunction("lua_main"))
            {
                return false;
            }
        }
        else
        {
            ErrorLn("LuaService Load error strMain="<<strMain.c_str());
            return false;
        }
    }

    m_isInit = true;

    return true;
}

bool LuaService::on_stop()
{
    if ( m_pLuaEngine )
    {
        m_pLuaEngine->Release();
        m_pLuaEngine = 0;
    }

    return true;
}