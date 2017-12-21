/*******************************************************************
** 文件名:	SceneManager.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	
            场景管理器
********************************************************************/

#include "stdafx.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "GameScene_Dota.h"
#include "IServerGlobal.h"
#include "IFramework.h"
#include "GameSceneService_Stub.h"
#include "TimeSyncService_Stub.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include "EntityDef.h"
#include "IServiceMgr.h"
#include "ISceneLogicService.h"
#include "Simple_Atom_Lock.h"
#include "GameDef.h"
#include "ICenterConnectorService.h"
#include "EntityHelper.h"



SceneManager::SceneManager()
	:m_nMinMirrorMapID(INVALID_MAPID)
    ,m_nMaxMirrorMapID(INVALID_MAPID)
    ,m_mutex(0)
    ,m_nCurSerialNo(0)
    ,m_nSceneCount(0)
{
    
}

SceneManager::~SceneManager()
{

}

void SceneManager::onStop()
{
    m_nMinMirrorMapID = INVALID_MAPID;
    m_nMaxMirrorMapID = INVALID_MAPID;

    for ( T_FORESTS::iterator it=m_forestMap.begin();it!=m_forestMap.end();++it )
    {
        if( it->second )
        {
            delete it->second;
        }
    }

    for(int i=0; i<MAX_SCENE_COUNT; ++i)
    {
        if(m_scenesList[i].pService)
        {
            closeScene(i);
        }
        m_scenesList[i].clear();
    }    
}

void SceneManager::release()
{


	delete this;
}

bool SceneManager::load()
{
	// 加载默认开启的场景
    int aMapID[256] = {0};
    int nLen = sizeof(aMapID) / sizeof(int); 
    gServerGlobal->getSchemeCenter()->getSchemeMapInfo()->getMapIDList(aMapID, nLen);

    for(int i = 0; i < nLen; ++i)
    {
        int & nMapID = aMapID[i];
        SMapSchemeInfo* pMapScheme = gServerGlobal->getSchemeCenter()->getSchemeMapInfo()->GetMapSchemeInfo(nMapID);
        if(pMapScheme == NULL)
        {
            continue;
        }

        // 是否开启
        if(pMapScheme->lZoneServerIDForJustLaunch == gServerGlobal->getServerID() && pMapScheme->lStartLoadForJustLaunch == 1/*载入*/ )
        {
            SCreateSceneContext sCreateSceneContext;
            sCreateSceneContext.mapID = nMapID;
            sCreateSceneContext.isDynamic = false;
            sCreateSceneContext.gameMode = EGM_RPG;
            createScene(sCreateSceneContext);
        }

        // 过滤出本场景服中的分流地图ID
        if( IsMainMap(nMapID) )
        {
            if(m_nMinMirrorMapID == INVALID_MAPID)
            {
                m_nMaxMirrorMapID = m_nMinMirrorMapID = nMapID;
            }
            else
            {
                m_nMinMirrorMapID = min(m_nMinMirrorMapID, nMapID);
                m_nMaxMirrorMapID = max(m_nMaxMirrorMapID, nMapID);
            }
        }
    }

	return true;
}

int SceneManager::createScene(SCreateSceneContext context, void* pUserData, int len)
{
	// 创建场景服务
	SERVICE_SCHEME * pServiceScheme = gServerGlobal->getFramework()->get_service_scheme("SceneService");
	if ( pServiceScheme==0 )
	{
		ErrorLn("can not found the SceneService scheme!mapid="<< context.mapID );
		return INVALID_SCENE_ID;
	}

    int sceneID = INVALID_SCENE_ID;
    if(context.isDynamic)
    {
        if(!allocSceneID(sceneID))
        {
            return INVALID_SCENE_ID;
        }
    }
    else
    {
        if(context.mapID >= (MAP_MAXCOUNT + 1))
        {
            ErrorLn("create static scene failed, mapID=" << context.mapID << ",mapid max=" << (MAP_MAXCOUNT + 1));
            return INVALID_SCENE_ID;
        }

        if(sceneID >= MAX_SCENE_COUNT)
        {
            return INVALID_SCENE_ID;
        }
        SceneInfo & info = m_scenesList[context.mapID];
        if(info.nMapID != INVALID_MAPID)
        {
            return INVALID_SCENE_ID;
        }

        sceneID = context.mapID;
    }

	obuf256 obcontext;
	obcontext <<context<< sceneID<<len;
    if(len > 0)
    {
        obcontext.push_back(pUserData, len);
    }

	// 在当前线程创建一个场景服务
	SERVICE_PTR pContainer = gServerGlobal->getFramework()->create_service( pServiceScheme,0,obcontext.data(),obcontext.size());
	if ( pContainer==0 )
    {
        //revertSceneID( sceneID );
		return INVALID_SCENE_ID;
    }

	{
        InterlockedIncrement(&m_nSceneCount);
        SceneInfo & refData = m_scenesList[sceneID];

		refData.nSerialNo = InterlockedIncrement(&m_nCurSerialNo);
		refData.pService = pContainer;
		refData.nMapID = context.mapID;
	}

	TraceLn(__FUNCTION__ ": mapid="<<context.mapID<<", sceneid="<< sceneID);

	return sceneID;
}

bool SceneManager::closeScene( int sceneID )
{
    SERVICE_PTR pSceneService = getScene( sceneID );
    if(pSceneService == 0)
    {
        ErrorLn(__FUNCTION__ << ", scene not found, sceneID=" << sceneID);
        return false;
    }

    IGameSceneService* pScene = (IGameSceneService*)pSceneService->query_interface();
    if(pScene == NULL)
    {
        ErrorLn(__FUNCTION__ << ", pScene == NULL, sceneID=" << sceneID);
        return false;
    }
    UINT_PTR nLogicSID = pScene->getLogicServiceID();
    if (nLogicSID == INVALID_SCENE_ID)
    {
        ErrorLn(__FUNCTION__ << ", LogicSID == INVALID_SCENE_ID, sceneID=" << sceneID);
        return false;
    }
    SERVICE_PTR pLogicService = gServerGlobal->getFramework()->get_service_manager()->get_service( SERVICE_ID(nLogicSID) );
    if(pLogicService == 0)
    {
        ErrorLn(__FUNCTION__ << ", logic service not found, sceneID=" << sceneID);
        return false;
    }
    ISceneLogicService* pLogic = (ISceneLogicService*)pLogicService->query_interface();
    if(pLogicService == NULL)
    {
        ErrorLn(__FUNCTION__ << ", pLogicService == NULL, sceneID=" << sceneID);
        return false;
    }

    SERVICE_PTR pAIService = pLogic->getAIService();

    // 停止AI(stop已改为同步操作)
    if(pAIService != 0)
    {
        pAIService->stop();
    }

    // 停止场景逻辑(stop已改为同步操作)
    pLogicService->stop();

    // 停止场景(stop已改为同步操作)
    pSceneService->stop();

	return true;
}

// 场景service真正退出 回调
void SceneManager::onSceneClosed( int sceneID )
{
    if(sceneID < MAX_SCENE_COUNT)
    {
        InterlockedDecrement(&m_nSceneCount);
        m_scenesList[sceneID].clear();
    }

}

SERVICE_PTR SceneManager::getScene( int sceneID )
{
    if(sceneID >= 0 && sceneID < MAX_SCENE_COUNT)
    {
        return m_scenesList[sceneID].pService;
    }


	return SERVICE_PTR(0);
}

int SceneManager::getSceneCount()
{
    return m_nSceneCount;

}

int SceneManager::getAllScenes( SERVICE_PTR* pReturnArray,int nArraySize )
{
	int count = 0;
    if (nArraySize > m_nSceneCount)
        nArraySize = m_nSceneCount;

    for(int i=0; i<MAX_SCENE_COUNT && count < nArraySize; ++i)
    {
        if(m_scenesList[i].nMapID != INVALID_MAPID)
        {
            pReturnArray[count++] = m_scenesList[i].pService;
        }
    }

	return count;
}

int SceneManager::getAllSceneID( int* pReturnArray, int nArraySize )
{
    int count = 0;

    if (nArraySize > m_nSceneCount)
        nArraySize = m_nSceneCount;

    for(int i=0; i<MAX_SCENE_COUNT && count < nArraySize; ++i)
    {
        if(m_scenesList[i].nMapID != INVALID_MAPID)
        {
            pReturnArray[count++] = i;
        }
    }

    return count;
}

void SceneManager::clearAllScenes()
{
    for(int i=0; i<MAX_SCENE_COUNT; ++i)
    {
        m_scenesList[i].clear();
    }

}

int SceneManager::sceneIDToMapID( int sceneID )
{
    if(sceneID < MAX_SCENE_COUNT)
    {
        return m_scenesList[sceneID].nMapID;
    }

    return 0;
}

int SceneManager::sceneIDToSerialNo( int sceneID )
{
    if(sceneID < MAX_SCENE_COUNT)
    {
        return m_scenesList[sceneID].nSerialNo;
    }

    return 0;
}

void SceneManager::getMirrorMapID(BYTE& nMin, BYTE &nMax)
{
    nMin = m_nMinMirrorMapID;
    nMax = m_nMaxMirrorMapID;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// 分配一个新的场景ID(只用于动态场景)，需要保证不同场景间的唯一性
bool SceneManager::allocSceneID( int& sceneID )
{
    if(m_idAllocator.size() >= MAX_SCENE_COUNT - (MAP_MAXCOUNT + 1)) // 动态场景数量超过上限
    {
        ErrorLn( "SceneManager::allocSceneID size exceed "<<MAX_SCENE_COUNT<<", please check the scene counts!" );
        return false;
    }

    int nIndex = m_idAllocator.allotID();
    if(nIndex >= MAX_SCENE_COUNT - (MAP_MAXCOUNT + 1))
    {
        m_idAllocator.revertID(nIndex);
        return false;
    }

    sceneID = nIndex + (MAP_MAXCOUNT + 1); 

    return true;
}

void SceneManager::revertSceneID( int sceneID )
{
    // 静态场景的id=mapid 不是这个分配器分配的 不用还
    if(sceneID < (MAP_MAXCOUNT + 1))
    {
        return;
    }

    m_idAllocator.revertID(sceneID - (MAP_MAXCOUNT + 1));
}

AOI_Forest * SceneManager::getGlobalForest( int mapID )
{
#ifdef DEBUG_AOI
    return NULL;
#endif
	{
		Simple_Atom_Lock lock(&m_mutex);

		T_FORESTS::iterator itr = m_forestMap.find( mapID );
		if ( itr!=m_forestMap.end() )
		{
			return itr->second;
		}
	}

    AOI_Forest * pForest = new AOI_Forest;
    if ( pForest==NULL )
        return NULL;

    if ( !pForest->Load(mapID) )
    {
        delete pForest;
        return NULL;
    }

	{
		Simple_Atom_Lock lock(&m_mutex);
		m_forestMap.insert( make_pair(mapID,pForest) );
	}
    return pForest;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) ISceneManager * CreateSceneManager()
{
	return new SceneManager;
}