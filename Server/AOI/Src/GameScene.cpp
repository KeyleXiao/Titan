/*******************************************************************
** 文件名:	GameScene.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	
            游戏场景对象
********************************************************************/

#include "stdafx.h"
#include "GameScene.h"
#include "IServerGlobal.h"
#include "Vector3.h"
#include "ISchemeCenter.h"
#include "IEntity.h"
#include "IFramework.h"
#include "INPCService.h"
#include "ITankService.h"
#include "EntityDef.h"
#include "ITimerService.h"
#include "AOI_Manager.h"
#include "SchemeDef.h"
#include "EventDef.h"
#include "ISceneLogicService.h"
#include "HeightFieldMgr.h"

bool GameScene::onCreate( int mapID,int sceneID, bool isCopy, int nWarID )
{
	m_nSceneID = sceneID;
	m_nMapID   = mapID;

    float bmin[3];
    float bmax[3];
    if(!gServerGlobal->getHeightFieldMgr()->getMapBounds(mapID, bmin, bmax))
    {
        ErrorLn(__FUNCTION__ << ", get heightfield failed, mapID=" << mapID);
        return false;
    }

    int origin[AOI_GRID_DIM] = { bmin[0],bmin[2] };
    int grid_num[AOI_GRID_DIM] = { (bmax[0] - bmin[0]) / AOI_GRID_SIZE + 1, (bmax[2] - bmin[2]) / AOI_GRID_SIZE + 1 };

	m_AOI_manager.initialize(origin, grid_num);
	
	// 创建场景逻辑服务
	SERVICE_SCHEME * pServiceScheme = gServerGlobal->getFramework()->get_service_scheme("SceneLogicService");
	if ( pServiceScheme==0 )
	{
		ErrorLn("can not found the SceneLogicService scheme!mapid="<< mapID );
		return true;
	}

	obuf256 context;
	context  << mapID<< sceneID << isCopy << nWarID;

	// 创建场景逻辑服务
	SERVICE_PTR pContainer = gServerGlobal->getFramework()->create_service( pServiceScheme,0,context.data(),context.size());
	if ( pContainer==0 )
	{
		ErrorLn("create SceneLogicService failed!mapid="<< mapID );
		return true;
	}

	m_pLogicService = pContainer;
	
	TraceLn("Create MMORPG Scene successful! mapID="<<mapID<<", sceneID="<< sceneID);

	return true;
}

int GameScene::getSceneID()
{
	return m_nSceneID;
}

int GameScene::getMapID()
{
	return m_nMapID;
}

// 让浮点坐标系标准化，乘系数转成整型
void GameScene::normalizeLoc( Vector3 fLoc,int iLoc[AOI_GRID_DIM] )
{
	// 注意在三维空间中, Y轴表示高度
	iLoc[0] = fLoc.x;
	iLoc[1] = fLoc.z;
}

bool GameScene::insertEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag/*,int layer*/ )
{
	UID uid = proxy->m_uid;
	if ( UID_2_TYPE(uid)==TYPE_PLAYER_ROLE )
	{
		PLAYER_SET::iterator it = m_playerSet.insert(proxy).first;
		proxy->setSharePtr( &(*it) );
		++m_player_count;
	}

    // modify 
    int layer = LAYER_WATCHER;
    if(proxy->m_nSight <= 0)
    {
        layer = LAYER_BLIND;
    }

    int myLoc[AOI_GRID_DIM];
    normalizeLoc(loc, myLoc);
    if( m_AOI_manager.insertEntity( proxy,myLoc,layer ) )
    {
        // 更新一下坐标
        proxy->m_Loc = loc;
        return true;
    }

	return false;
}

bool GameScene::removeEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag/*,int layer*/ )
{
    if ( proxy==NULL )
        return false;

	UID uid = proxy->m_uid;
	if ( UID_2_TYPE(uid)==TYPE_PLAYER_ROLE )
	{
		m_playerSet.erase(proxy);
		proxy->setSharePtr(0);
		--m_player_count;
	}

	int myLoc[AOI_GRID_DIM];
	normalizeLoc(proxy->m_Loc,myLoc);

    // modify 
    int layer = LAYER_WATCHER;
    if(proxy->m_nSight <= 0)
    {
        layer = LAYER_BLIND; 
    }
	bool ret= m_AOI_manager.removeEntity( proxy,myLoc,layer );
	return ret;
}

bool GameScene::moveEntity( AOI_PROXY_PTR proxy,Vector3 newLoc)
{
	int myOldLoc[AOI_GRID_DIM];
	normalizeLoc(proxy->m_Loc,myOldLoc);

	int myNewLoc[AOI_GRID_DIM];
	normalizeLoc(newLoc,myNewLoc);

    // modify 
    int layer = LAYER_WATCHER;
    if(proxy->m_nSight <= 0)
    {
        layer = LAYER_BLIND; 
    }

	if ( m_AOI_manager.moveEntity( proxy,myOldLoc,myNewLoc,layer ) )
	{
		// 更新一下坐标
		proxy->m_Loc = newLoc;
		return true;
	}

	return false;
}

void GameScene::updateObjVisible( AOI_PROXY_PTR proxy )
{
    m_AOI_manager.onUpdateObjVisible( proxy );
}

void GameScene::onUpdateObjAntiStealth( AOI_PROXY_PTR proxy )
{

}

void GameScene::setSight( AOI_PROXY_PTR proxy,int nSight )
{

}

int GameScene::k_nearest( Vector3 loc,float fDist,AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask, bool bCheckDist)
{
	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	// 找的时候多加1个范围,不然找小于1的范围转int为0了
	if ( fDist<1.0f ) fDist= 1.0f;

	AOI_Proxy * pProxyArray[MAX_INTEREST_OBJECT];
	int count = m_AOI_manager.k_nearest( myLoc,fDist,pProxyArray,MAX_INTEREST_OBJECT,layerMask);

	if ( count > nArraySize ) count = nArraySize;

	int nTrueCount = 0;
	for ( int i=0;i<count && nTrueCount<nArraySize;++i )
	{
		if ( pProxyArray[i]->m_pSharePtr) 
			pReturnArray[nTrueCount++] = pProxyArray[i]->getSharePtr();
	}

	return nTrueCount;
}

int GameScene::k_nearLineest( Vector3 loc,Vector3 dir, float fDist,float fWith, float fHeight, AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask )
{
	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	AOI_Proxy * pProxyArray[MAX_INTEREST_OBJECT];

	// 找的时候多加1个范围,不然找小于1的范围转int为0了
	if ( fDist<1.0f ) fDist= 1.0f;

    // 直接查找要加上线段宽带fWith，不然正好在切面上线和与圆弧线间的实体找不到
	int count = m_AOI_manager.k_nearest( myLoc,fDist+fWith,pProxyArray,MAX_INTEREST_OBJECT,layerMask);
	if ( count<0 )
		return count;

	int nTrueCount = 0;
	for ( int i=0;i<count;++i )
	{
		if ( nTrueCount<nArraySize && loc.checkLineDistance(pProxyArray[i]->m_Loc,dir,fDist,fWith,fHeight) )
		{
			if ( pProxyArray[i]->m_pSharePtr )
				pReturnArray[nTrueCount++] = pProxyArray[i]->getSharePtr();
		}
	}

	return nTrueCount;
}

int GameScene::k_nearestInSpell( Vector3 loc,float fDist,float fHeight,AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask )
{
#ifdef OPEN_BVTTEST
    PP_BY_NAME( "GameScene_Dota::k_nearestInSpell" );
#endif

    int myLoc[AOI_GRID_DIM];
    normalizeLoc(loc,myLoc);

	// 找的时候多加1个范围,不然找小于1的范围转int为0了
	if ( fDist<1.0f ) fDist= 1.0f;

	AOI_Proxy * pProxyArray[MAX_INTEREST_OBJECT];
    int count = m_AOI_manager.k_nearest( myLoc,fDist,pProxyArray,MAX_INTEREST_OBJECT,layerMask);
    if ( count<0 )
        return count;

    multimap<float, AOI_Proxy *> mapUID;
    for ( int i=0;i<count;++i )
    {
        if ( loc.checkDistance(pProxyArray[i]->m_Loc,fDist, fHeight) )
        {
            // 算出距离
            float fLength = (pProxyArray[i]->m_Loc- loc).getLength();
            mapUID.insert(multimap<float, AOI_Proxy *>::value_type(fLength, pProxyArray[i]));
        }
    }

    int nTrueCount = 0;
    multimap<float, AOI_Proxy *>::iterator Iter = mapUID.begin();
    while (Iter != mapUID.end() && nTrueCount<nArraySize)
    {
		if ( Iter->second->m_pSharePtr )
			pReturnArray[nTrueCount++] = Iter->second->getSharePtr();
        ++Iter;
    }

    return nTrueCount;
}

int GameScene::k_nearSectorest( Vector3 loc,Vector3 dir, float fr, float squaredR, float cosTheta, float fHeight, AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask)
{
	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	AOI_Proxy * pProxyArray[MAX_INTEREST_OBJECT];

	// 找的时候多加1个范围,不然找小于1的范围转int为0了
	if ( fr<1.0f ) fr= 1.0f;

	int count = m_AOI_manager.k_nearest( myLoc,fr,pProxyArray,MAX_INTEREST_OBJECT,layerMask);
	if ( count<0 )
		return count;

	int nTrueCount = 0;
	for ( int i=0;i<count;++i )
	{
		if ( nTrueCount<nArraySize && loc.checkPointInSector(pProxyArray[i]->m_Loc, dir, squaredR, cosTheta, fHeight) )
		{
			if ( pProxyArray[i]->m_pSharePtr )
				pReturnArray[nTrueCount++] = pProxyArray[i]->getSharePtr();
		}
	}

	return nTrueCount;
}

int GameScene::broadcast(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,/*int layerMask*/AOI_PROXY_PTR proxy,bool ignoreSelf)
{
	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	PACKAGE_PTR package = PACKAGE_PTR(new std::string(msg));
	m_AOI_manager.broadcast(myLoc,dist,id,proxy,package,LAYER_WATCHER,ignoreSelf);
	return 1;
}

int GameScene::broadcastMultiple(char *pData,int nCount,BROADCAST_ID id,std::string & msg,/*int layerMask*/AOI_PROXY_PTR proxy)
{
    if (nCount <= 0)
    {
        return 1;
    }
    if (nCount > AOI_LOC_COUNT)
    {
        ErrorLn("broadcastMultiple count>AOI_LOC_COUNT");
        return 0;
    }

    int myLoc[AOI_LOC_COUNT][AOI_GRID_DIM];
    memset(myLoc, 0, sizeof(myLoc));
    for (int i=0; i<nCount; ++i)
    {
        Vector3 *pLoc = (Vector3*)pData+i;
        normalizeLoc(*pLoc,myLoc[i]);
    }

    PACKAGE_PTR package = PACKAGE_PTR(new std::string(msg));
    m_AOI_manager.broadcastMultiple(id,myLoc,nCount,proxy,package);
    
    return 1;
}

int GameScene::broadcast_all( BROADCAST_ID id,std::string & msg, int layerMask )
{
	PACKAGE_PTR package = PACKAGE_PTR(new std::string(msg));

	PLAYER_SET::iterator it = m_playerSet.begin();
	for ( ;it!=m_playerSet.end();++it )
	{
		(*it)->onBroadcast( id,0,package );
	}

	return 1;
}

int GameScene::broadcast_nearLoc(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,/*int layerMask*/AOI_PROXY_PTR proxy,bool ignoreSelf,int campMask/*=CAMP_MASK_ALL*/)
{
    int myLoc[AOI_GRID_DIM];
    normalizeLoc(loc,myLoc);

    PACKAGE_PTR package = PACKAGE_PTR(new std::string(msg));
    m_AOI_manager.broadcast(myLoc,dist,id,proxy,package,LAYER_WATCHER,ignoreSelf);
    return 1;
}

int GameScene::broadcastMultiple_nearLoc(char *pData,int nCount,BROADCAST_ID id,std::string & msg,/*int layerMask*/AOI_PROXY_PTR proxy,int campMask/*=CAMP_MASK_ALL*/)
{
    return 1;
}

int GameScene::getPlayerCount()
{
	return m_player_count;
}

int GameScene::getMonsterCount()
{
	return 0;
}

// 获取场景更新共享数据
SceneUpdateShareInfo GameScene::getUpdateShareInfo()
{
    SceneUpdateShareInfo info;
    info.nMapID = m_nMapID;
    info.nSceneID = m_nSceneID;
    info.nPlayerQty = m_playerSet.size();
    info.nMonsterQty = 0;

    return info;
}

int GameScene::getAllPlayers( AOI_PROXY_PTR* pReturnArray,int nArraySize )
{
	int count = 0;
	PLAYER_SET::iterator it = m_playerSet.begin();
	for ( ;it!=m_playerSet.end() && count<nArraySize;++it )
	{
		pReturnArray[count++] = *it;
	}

	return count;
}

// 关闭场景
void GameScene::close()
{
	// 获得事件中心接口
	IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
	if (pEventEngine==NULL)
	{
		return;
	}
	// 发送场景销毁事件
	event_scene_destroy eventdata;
	eventdata.nMapID = m_nMapID;
	eventdata.nSceneID = m_nSceneID;

	pEventEngine->FireExecute(EVENT_SCENE_DESTROY, SOURCE_TYPE_SCENE, (DWORD)m_nSceneID, (LPCSTR)&eventdata, sizeof(eventdata));
}

bool GameScene::on_stop()
{
    close();
    return true;
}

void GameScene::release()
{
	delete this;
}
