/*******************************************************************
** 文件名:	ResourceManager.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2016-12-17
** 版  本:	1.0
** 描  述:	资源管理类
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "ControllerDef.h"
#include "MemoryModule.h"
#include "DManagerModule.h"
#include "IServerGlobal.h"
#include "IMiscManager.h"

#include "ResourceManager.h"

// 内部调试SCRIPT功能
//#define RESOURCE_SCRIPT_DEBUG
// 可读写文件
#ifdef RESOURCE_SCRIPT_DEBUG
#include <fstream>
#endif

/** 
@param   
@param   
@return  
*/
CResourceManager::CResourceManager(void)
    : m_hModule(NULL)
    , m_bFromPackLoadLua(false) /* 是否从MPK中读取LUA */
{
}

/** 
@param   
@param   
@return  
*/
CResourceManager::~CResourceManager(void)
{

}


/** 加载
@return  
*/
bool CResourceManager::load()
{
    return true;
}


/** 执行Service的on_stop
@return  
*/
void CResourceManager::onStop(void)
{
     releaseData(0);
     EmphasisLn(__FUNCTION__": Free Resource...");
}

/** 释放
@note     释放所有资源，并且销毁此对像
@warning 
*/
void  CResourceManager::release(void)
{
    delete this;
}


/** 执行收到的消息
*/
void CResourceManager::onRecvMessage(const char * pData, size_t nLen)
{
    if(pData==NULL|| nLen < sizeof(SMsgManagerSendScriptData_RM))
    {
        ErrorLn( _GT("收到服务器启动脚本文件数据出错,数据有误！"));
        return;   
    }

    SMsgManagerSendScriptData_RM * pMsg = (SMsgManagerSendScriptData_RM *)(pData);

    TraceLn("Recv Start Resource Data, WorldID="<<pMsg->dwWorldID<<",ServerType="<<pMsg->nServerType<<",GroupID="<<pMsg->dwScriptGroupID<<",MaxSchemeIndex="<<pMsg->nMaxSchemeIndex<<",SchemeIndex="<<pMsg->nSchemeIndex<<",DataLen="<<pMsg->dwDataLens);

    if (pMsg->nServerType!= MSG_ENDPOINT_SCENE)
    {
        ErrorLn("Recv Start Resource Data, ServerType["<<pMsg->nServerType<<"] Is not the same for this ServerType["<<MSG_ENDPOINT_SOCIAL<<"]!");
        return;
    }
    if(nLen != sizeof(SMsgManagerSendScriptData_RM) + pMsg->dwDataLens )
    {
        ErrorLn( _GT("收到服务器启动脚本文件数据出错,数据长度有误！RealLen=")<< nLen <<",Len="<<(sizeof(SMsgManagerSendScriptData_RM) + pMsg->dwDataLens) );
        return;
    }
    
    // 资源类型
    int nResourceType = pMsg->nResourceType;

    // 文件数据,本配置号数据长度 pMsg->dwDataLens
    LPSTR pDataBuffer = (LPSTR)(pData+sizeof(SMsgManagerSendScriptData_RM));
    TMAP_RESOURCE::iterator iter = m_mapResources.find(nResourceType);
    if(iter == m_mapResources.end())
    {
        TMAP_RESOURCEDATA resource;
        std::pair< TMAP_RESOURCE::iterator , bool> ret = m_mapResources.insert(make_pair(nResourceType, resource));
        if(!ret.second)
            return;

        iter = ret.first;
    }
    TMAP_RESOURCEDATA & resource = iter->second;

    // 将资源加入到容器
    ServerResourceData getData;
    pair<TMAP_RESOURCEDATA::iterator,bool> retPair = resource.insert(TMAP_RESOURCEDATA::value_type(pMsg->nSchemeIndex,getData));
    if(!retPair.second)
        return;
    ServerResourceData * pIndexScheme = &(retPair.first->second);
    pIndexScheme->nEncry = pMsg->nEncry;
    pIndexScheme->nResourceType = pMsg->nResourceType;
    // new内存
    pIndexScheme->getDataMemory(pMsg->dwDataLens);
    pIndexScheme->nBufferLen = pMsg->dwDataLens;
    // copy内存
    memcpy(pIndexScheme->pData,pDataBuffer,pMsg->dwDataLens);


    // 处理资源
    switch(nResourceType)
    {
    case RESOURCE_TYPE_DBSERVER:        // DBServer资源
        {
            EmphasisLn(__FUNCTION__": Load RESOURCE_TYPE_DBSERVER");
            // 启动DBService
            gServerGlobal->getMiscManager()->create_service("DBEngineService");
        }
        break;

    case RESOURCE_TYPE_DLL:     // DLL资源
        {
            EmphasisLn(__FUNCTION__": Load RESOURCE_TYPE_DLL");
            if(!m_hModule )
            {
                // 内存来加载DLL
                rkt::HMEMORYMODULE hModule = MemoryLoadLibrary(pDataBuffer);
                if(  hModule == NULL)
                {
                    ErrorLn(__FUNCTION__":load memory data failed!");
                    return;
                }

                m_hModule = hModule;
            }

            //typedef const char * (*FnGet)();
            //FnGet pfnUserName = (FnGet)MemoryGetProcAddress(hModule, "UserName");
            //if(pfnUserName == NULL)
            //{
            //    return;
            //}
            //FnGet pfnPassword = (FnGet)MemoryGetProcAddress(hModule, "Password");
            //if(pfnPassword == NULL)
            //{
            //    return;
            //}
            //string strUserName( pfnUserName() );
            //string strPassWord( pfnPassword() );
            //EmphasisLn(__FUNCTION__": UserName="<< strUserName.c_str() << ", Password="<< strPassWord.c_str());
        }
        break;

    case RESOURCE_TYPE_SCRIPT:      // MPK资源
        {
#ifdef RESOURCE_SCRIPT_DEBUG
            // 写下本地文件
            char* pContent = reinterpret_cast<char*>( pIndexScheme->pData);
            ofstream out;  
            out.open("pack.mpk", ios::in|ios::out|ios::binary | ios::trunc);
            out.write(pContent,pIndexScheme->nBufferLen);
            out.close();
#endif
            // 加入文件系统中
            IFileSystem * pFileSystem = getFileSystem()->addMemoryPkg(pIndexScheme->pData,pIndexScheme->nBufferLen);
            if(pFileSystem == NULL)
            {
                ErrorLn("创建服务器数据失败!");
            }
            EmphasisLn(__FUNCTION__": Load RESOURCE_TYPE_SCRIPT");
        }
        break;

    default:
        break;
    }
}


/** 获取指定类型的资源
*/
int CResourceManager::getData(int nResourceType, void **ppOutBuff)
{
    TMAP_RESOURCE::iterator iter = m_mapResources.find(nResourceType);
    if(iter == m_mapResources.end())
    {
        return 0;
    }

    TMAP_RESOURCE_BUFFER::iterator iterBuff = m_mapResourceBuffer.find(nResourceType);
    if(iterBuff != m_mapResourceBuffer.end())
    {
        RES_BUFF_NODE & node = iterBuff->second;
        if(node.pDataBuff != NULL)
        {
            *ppOutBuff = iterBuff->second.pDataBuff;
            return node.nSize;
        }
        else
        {
            iterBuff->second.pDataBuff = 0;
            iterBuff->second.nSize = 0;
        }
    }
    else
    {
        std::pair< TMAP_RESOURCE_BUFFER::iterator, bool > ret = m_mapResourceBuffer.insert( make_pair(nResourceType, RES_BUFF_NODE() ));
        if( !ret.second )
        {
            return 0;
        }
        iterBuff = ret.first;
    }

    // 统计出相关的容量
    int nBuffSize = 0;
    TMAP_RESOURCEDATA & resources = iter->second;
    for (TMAP_RESOURCEDATA::iterator it = resources.begin(); it != resources.end(); ++it)
    {
        ServerResourceData& data = it->second;
        nBuffSize += (int)data.dwSize;
    }

    char * p = (char*)malloc(nBuffSize);
    if(p)
    {
        iterBuff->second.pDataBuff = p;
        iterBuff->second.nSize = nBuffSize;
        for (TMAP_RESOURCEDATA::iterator itor = resources.begin(); itor != resources.end(); ++itor)
        {
            ServerResourceData& data = itor->second;
            memcpy(p, data.pData, data.dwSize);
            p += data.dwSize;
        }
    }    

    *ppOutBuff = iterBuff->second.pDataBuff;

    return nBuffSize;
}

/* 校验账号有效性*/
bool CResourceManager::checkAccount(const char * pszUserName, const char * pszPassword)
{
    if(m_hModule == NULL)
        return false;

    rkt::HMEMORYMODULE hMoudle = static_cast<rkt::HMEMORYMODULE>( m_hModule );
    if(hMoudle == NULL)
        return false;

    /* 校验账号有效性*/
    typedef bool  (*pFnCheckAccount)(int nSvrType, const char * pszUserName, const char * pszPassword);
    pFnCheckAccount pfnCheckAccount = (pFnCheckAccount)MemoryGetProcAddress(hMoudle, "checkAccount");
    if(pfnCheckAccount == NULL)
    {
        return false;
    }
    return pfnCheckAccount(MSG_ENDPOINT_SCENE, pszUserName, pszPassword);
}

/** 释放指定资源
*/
void CResourceManager::releaseData(int nResourceType)
{
    if(nResourceType == 0)
    {
        for(TMAP_RESOURCE::iterator iter = m_mapResources.begin(); iter != m_mapResources.end(); ++iter)
        {
            TMAP_RESOURCEDATA & resources = iter->second;
            for (TMAP_RESOURCEDATA::iterator it = resources.begin(); it != resources.end(); ++it)
            {
                ServerResourceData & data = it->second;
                data.freeDataMemory();
            }
        }
        m_mapResources.clear();

        for (TMAP_RESOURCE_BUFFER::iterator iterBuff = m_mapResourceBuffer.begin(); iterBuff != m_mapResourceBuffer.end(); ++iterBuff)
        {
            RES_BUFF_NODE & node = iterBuff->second;
            if(node.pDataBuff)
            {
                free(node.pDataBuff);
            }
        }
        m_mapResourceBuffer.clear();
    }
    else
    {
        TMAP_RESOURCE::iterator iter = m_mapResources.find(nResourceType);
        if(iter == m_mapResources.end())
        {
            return;
        }

        TMAP_RESOURCEDATA & resources = iter->second;
        for (TMAP_RESOURCEDATA::iterator it = resources.begin(); it != resources.end(); ++it)
        {
            ServerResourceData & data = it->second;
            data.freeDataMemory();
        }

        m_mapResources.erase(iter);


        TMAP_RESOURCE_BUFFER::iterator iterBuff = m_mapResourceBuffer.find(nResourceType);
        if(iterBuff != m_mapResourceBuffer.end())
        {
            RES_BUFF_NODE & node = iterBuff->second;
            if(node.pDataBuff)
            {
                free(node.pDataBuff);
            }

            m_mapResourceBuffer.erase(nResourceType);
        }

    }  
}

