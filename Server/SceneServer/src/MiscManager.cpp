/*******************************************************************
** 文件名:	MiscManager.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-12-9
** 版  本:	1.0
** 描  述:	辅助全局对象的一个杂项管理器

********************************************************************/

#include "stdafx.h"

#include "IServerGlobal.h"
#include "IFramework.h"
#include "MiscManager.h"

//////////////////////////////////////////////////////////////////////////
// 构造函数
CMiscManager::CMiscManager()
{

}

// 析构函数
CMiscManager::~CMiscManager()
{

}

////////////////// IMiscManager ////////////////////////////////////////////////////////

/* 用Service配置名称创建Service
@param pszServiceSchemeName: Service配置名称
*/
SERVICE_PTR CMiscManager::create_service(const char * pszServiceSchemeName)
{
    do 
    {
        if (!pszServiceSchemeName || strlen(pszServiceSchemeName) < 2 )
        {
            ErrorLn(__FUNCTION__": create service failed because input parameter is invalid!");
            break;
        }

        IFramework * pFrameowork = gServerGlobal->getFramework();
        if(!pFrameowork)
        {
            ErrorLn(__FUNCTION__": don't get framework object!");
            break;
        }

        SERVICE_SCHEME* pServiceScheme = pFrameowork->get_service_scheme(pszServiceSchemeName);
        if(pServiceScheme == 0)
        {
            ErrorLn(__FUNCTION__": don't get scheme config info! scheme="<< pszServiceSchemeName);
            break;
        }

        // 多线程模式下需要检测开启场景ID控制
        if(pFrameowork->isMulitThreadMode())
        {
            // 判断当前场景服上是否能创建此服务，如果不配置将默认可创建
            TiXmlElement * pConfig = (TiXmlElement *)pServiceScheme->scheme->pExtend;
            if ( pConfig )
            {
                // 取得当前场景序号
                ushort uServerID = gServerGlobal->getServerID();
                char szBuf[32] = {0};
                ssprintf_s(szBuf, sizeof(szBuf), "%d;", uServerID);
                string strServerID(szBuf);            

                // 禁开场景服序号列表
                const char * disable_index_list = pConfig->Attribute("disable_index_list");
                if(disable_index_list)
                {
                    string strDisableIndexs (disable_index_list);
                    // 在列表中
                    if(strDisableIndexs.find(strServerID) != string::npos)
                    {
                        break;
                    }
                }

                // 可开场景服序号列表，如果不配置将默认可创建
                const char * enable_index_list = pConfig->Attribute("enable_index_list");
                if(enable_index_list)
                {
                    string strEnableIndexs (enable_index_list);
                    // 没在列表中
                    if(strEnableIndexs.find(strServerID) == string::npos)
                    {
                        break;
                    }
                }
            }
        }

        TraceLn(__FUNCTION__": fire create service, schemename="<< pszServiceSchemeName);
        return pFrameowork->create_service(pServiceScheme);

    } while (false);

    return SERVICE_PTR(0);
}


