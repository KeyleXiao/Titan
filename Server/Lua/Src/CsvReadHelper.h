/*******************************************************************
** 文件名:	CsvReadHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/24/2016
** 版  本:	1.0
** 描  述:	CSV读取帮助函数
********************************************************************/

#include "stdafx.h"
#include "lua.h"
#include "ISchemeCenter.h"
#include "IServerGlobal.h"

/** 
@brief: 读取csv脚本,返回到一个lua_table中
@param: 堆栈中放了一个参数，脚本的文件名
*/
//////////////////////////////////////////////////////////////////////////////
class CsvReadHelper
{
public:
    virtual bool OnSchemeLoad(lua_State *pLuaState, const char* szFileName) 
    {
        // 任务列表
        if (strcmpi(szFileName, TASK_SCHEME_FILENAME) == 0)
        {
            return ReadTaskScheme(pLuaState, szFileName);
        }

	    return false;
    }

    // 读取任务配置
    bool ReadTaskScheme(lua_State *pLuaState, const char* szFileName)
    {
        ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            return false;
        }

        ISchemeTask *pSchemeTask = pSchemeCenter->getSchemeTask();
        if (pSchemeTask == NULL)
        {
            return false;
        }

        // 取得任务配置
        SCHEME_TASK *schemeTask = pSchemeTask->getTaskScheme();
        int nRowCount = (int)(*schemeTask).size();
        for (int i=0; i<nRowCount; ++i)
        {
            lua_pushnumber(pLuaState, i+1);
            lua_newtable(pLuaState); // 新建一条记录

            // 当前列数
            TASK_VECTOR taskVector = (*schemeTask)[i];
            int nColCount = (int)taskVector.size();
            for (int j=0; j<nColCount; ++j)
            {
                STask &data = taskVector[j];
                switch(data.nType)
                {
                case emCSVBool:
                    {
                        lua_pushnumber(pLuaState, j+1);
                        lua_pushboolean(pLuaState, atoi(data.strValue.c_str()));
                        lua_rawset(pLuaState, -3);
                    }
                    break;
                case emCSVInt:
                    {
                        lua_pushnumber(pLuaState, j+1);
                        lua_pushnumber(pLuaState, atoi(data.strValue.c_str()));
                        lua_rawset(pLuaState, -3);
                    }
                    break;
                case emCSVFloat:
                    {
                        lua_pushnumber(pLuaState, j+1);
                        lua_pushnumber(pLuaState, atof(data.strValue.c_str()));
                        lua_rawset(pLuaState, -3);
                    }
                    break;
                case emCSVString:
                    {
                        lua_pushnumber(pLuaState, j+1);
                        lua_pushstring(pLuaState, data.strValue.c_str());
                        lua_rawset(pLuaState, -3);
                        break;
                    }
                default:
                    ErrorLn("ReadCsvFile error type,i=" << i << ",j=" << j << ",szFileName=" <<szFileName);
                    break;
                }
            }

            // 添加一行
            lua_rawset(pLuaState, -3);
        }

        return true;
    }
};