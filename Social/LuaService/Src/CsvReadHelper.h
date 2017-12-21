/*******************************************************************
** 文件名:	CsvReadHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	CSV读取帮助函数
********************************************************************/

#include "stdafx.h"
#include "lua.h"

/** 
@brief: 读取csv脚本,返回到一个lua_table中
@param: 堆栈中放了一个参数，脚本的文件名
*/
//////////////////////////////////////////////////////////////////////////////
class CsvReadHelper : public ISchemeUpdateSink
{
public:
    virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName) 
    {
        ICSVReader *pCSVReader = reader.pCSVReader;
        if (pCSVReader == NULL)
        {
            return false;
        }

        for ( DWORD i=0;i<pCSVReader->GetRecordCount();++i )
        {
            lua_pushnumber(m_pLuaState,i+1);
            lua_newtable(m_pLuaState); // 新建一条记录
            for ( DWORD j=0;j<pCSVReader->GetFieldCount();++j )
            {
                int type = pCSVReader->GetFieldType(j);
                switch(type)
                {
                case emCSVBool:
                    lua_pushnumber(m_pLuaState,j+1);
                    lua_pushboolean(m_pLuaState,pCSVReader->GetBool(i,j));
                    lua_rawset(m_pLuaState,-3);
                    break;
                case emCSVInt:
                    lua_pushnumber(m_pLuaState,j+1);
                    lua_pushnumber(m_pLuaState,pCSVReader->GetInt(i,j));
                    lua_rawset(m_pLuaState,-3);
                    break;
                case emCSVFloat:
                    lua_pushnumber(m_pLuaState,j+1);
                    lua_pushnumber(m_pLuaState,pCSVReader->GetFloat(i,j));
                    lua_rawset(m_pLuaState,-3);
                    break;
                case emCSVString:
                    {
                        char buff[1024] = {0};
                        int  len = 1024;
                        pCSVReader->GetString(i,j,buff,len);
                        lua_pushnumber(m_pLuaState,j+1);
                        lua_pushstring(m_pLuaState,buff);
                        lua_rawset(m_pLuaState,-3);
                        break;
                    }
                default:
                    ErrorLn("ReadCsvFile error type,i=" << i << ",j=" << j << ",szFileName=" <<szFileName);
                    break;
                }
            }

            // 添加一行
            lua_rawset(m_pLuaState,-3);
        }
        return true;
    }

    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
    {
        return OnSchemeLoad(reader, szFileName);
    }

    CsvReadHelper(lua_State *pLuaState) : m_pLuaState(pLuaState)
    {
    }

    lua_State *m_pLuaState;
};