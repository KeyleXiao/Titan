/*******************************************************************
** 文件名:	GameScene_Dota.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	
            游戏场景对象
********************************************************************/

#include "stdafx.h"
#include "GameScene_Dota.h"
#include "IServerGlobal.h"
#include "Vector3.h"
#include "ISchemeCenter.h"
#include "IEntity.h"
#include "IFramework.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "ITimerService.h"
#include "SceneManager.h"
#include "EventDef.h"
#include "IServiceMgr.h"
#include "ISceneLogicService.h"
#include "HeightFieldMgr.h"

 GameScene_Dota::GameScene_Dota(int nCampCount) : m_AOI_manager(nCampCount)
 	,m_nSceneID(0)
 	,m_nMapID(0)
     ,m_sidLogicService(INVALID_SCENE_ID)
{
 	m_player_count = 0;
 	m_monster_count= 0;
}

void GameScene_Dota::release()
{
    m_playerSet.clear();
    m_monsterSet.clear();
    int nSceneID = m_nSceneID;
	delete this;
    WarningLn("scene id=" << nSceneID << "~GameScene_Dota()");
}

 bool GameScene_Dota::on_stop()
 {
     //gServerGlobal->getTimerService()->KillTimer(TIMER_FLUSH_SIGHT, this);
     gServerGlobal->getTimerService()->KillTimer(TIMER_OUTPUT_INFO, this);

     close();

     return true;
 }

// 关闭场景
void GameScene_Dota::close()
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

int GameScene_Dota::getSceneID()
{
	return m_nSceneID;
}

int GameScene_Dota::getMapID()
{
	return m_nMapID;
}

bool GameScene_Dota::onCreate( int mapID,int sceneID, bool isCopy, int nWarID )
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

	m_AOI_manager.initialize( origin,grid_num,((SceneManager*)gServerGlobal->getSceneManager())->getGlobalForest(mapID) );

	// 创建NPC服务
	SERVICE_SCHEME * pServiceScheme = gServerGlobal->getFramework()->get_service_scheme("SceneLogicService");
	if ( pServiceScheme==0 )
	{
		ErrorLn("can not found the SceneLogicService scheme!mapid="<< mapID );
		return true;
	}

	obuf256 context;
	context  << mapID << sceneID << isCopy << nWarID;

	// 创建场景逻辑服务
	SERVICE_PTR pContainer = gServerGlobal->getFramework()->create_service( pServiceScheme,0,context.data(),context.size());
	if ( pContainer==0 )
	{
		ErrorLn("create SceneLogicService failed!mapid="<< mapID );
		return true;
	}
    m_sidLogicService = pContainer->get_id();

	TraceLn("Create MOBA Scene successful! mapID="<<mapID<<", sceneID="<< sceneID);

	// 启动定时器
	//gServerGlobal->getTimerService()->SetTimer(TIMER_FLUSH_SIGHT,5000,this,INFINITY_CALL,__FILE__);
	gServerGlobal->getTimerService()->SetTimer(TIMER_OUTPUT_INFO,5*60000,this,INFINITY_CALL,__FILE__);

	return true;
}

// 让浮点坐标系标准化，乘系数转成整型
void GameScene_Dota::normalizeLoc( Vector3 fLoc,int iLoc[AOI_GRID_DIM] )
{
	// 注意在三维空间中, Y轴表示高度
	iLoc[0] = fLoc.x;
	iLoc[1] = fLoc.z;
}

bool GameScene_Dota::insertEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag )
{
	UID uid = proxy->m_uid;
	bool isPlayer = UID_2_TYPE(uid)==TYPE_PLAYER_ROLE;

	if (isPlayer ) 
	{
		ENTITY_SET::iterator it = m_playerSet.insert(proxy).first;
		proxy->setSharePtr( &(*it) );
		++m_player_count;
	}else
	{
		ENTITY_SET::iterator it = m_monsterSet.insert(proxy).first;
		proxy->setSharePtr( &(*it) );
		++m_monster_count;
	}

    int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc, myLoc);

	if( m_AOI_manager.insertEntity( proxy,myLoc,isPlayer ) )
    {
        proxy->m_Loc = loc;
        return true;
    }

    ErrorLn("m_AOI_manager insertEntity failed, mapid=" << m_nMapID << "loc(" << loc.x << "," << loc.y << "," << loc.z << ")");

    return false;
}

bool GameScene_Dota::removeEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag )
{
	int loopcount = 0;

    DWORD dwStartTick = getTickCountEx();
    DWORD dwCostTick = getTickCountEx() - dwStartTick;
    if (dwCostTick > 10)
    {
        WarningLn(__FUNCTION__": wait for k_nearest cost " << dwCostTick << " ms");
    }

	UID uid = proxy->m_uid;
	bool isPlayer = UID_2_TYPE(uid)==TYPE_PLAYER_ROLE;
	if (isPlayer ) 
	{
		//m_playerSet.erase(proxy);
		proxy->setSharePtr(0);
		--m_player_count;
	}else
	{
		//m_monsterSet.erase(proxy);
		proxy->setSharePtr(0);
		--m_monster_count;
	}

    int myLoc[AOI_GRID_DIM];
    normalizeLoc(proxy->m_Loc, myLoc);
	bool ret = m_AOI_manager.removeEntity( proxy,myLoc,isPlayer );
	return ret;
}

bool GameScene_Dota::moveEntity( AOI_PROXY_PTR proxy,Vector3 newLoc  )
{
    int myOldLoc[AOI_GRID_DIM];
    normalizeLoc(proxy->m_Loc, myOldLoc);

	int myNewLoc[AOI_GRID_DIM];
	normalizeLoc(newLoc,myNewLoc);

	if ( m_AOI_manager.moveEntity( proxy,myOldLoc,myNewLoc ) )
	{
		// 更新一下坐标
		proxy->m_Loc = newLoc;
		return true;
	}

    ErrorLn("m_AOI_manager moveEntity failed, uid="<< proxy->m_uid <<", mapid=" << m_nMapID 
        << ",oldLoc(" << proxy->m_Loc.x << "," << proxy->m_Loc.y << "," << proxy->m_Loc.z << ")"
        << ",newLoc(" << newLoc.x << "," << newLoc.y << "," << newLoc.z);

	return false;
}

bool GameScene_Dota::setObserver( AOI_PROXY_PTR proxy,bool bObserver )
{
    UID uid = proxy->m_uid;
    bool isPlayer = UID_2_TYPE(uid)==TYPE_PLAYER_ROLE;

    int myLoc[AOI_GRID_DIM];
    normalizeLoc(proxy->m_Loc, myLoc);

    return m_AOI_manager.setObserver( proxy,myLoc,isPlayer,bObserver );
}

void GameScene_Dota::updateObjVisible( AOI_PROXY_PTR proxy )
{
    int myLoc[AOI_GRID_DIM];
    normalizeLoc(proxy->m_Loc, myLoc);

    m_AOI_manager.UpdateObjVisible(myLoc, proxy);
}

void GameScene_Dota::onUpdateObjAntiStealth( AOI_PROXY_PTR proxy )
{
    m_AOI_manager.onUpdateObjAntiStealth(proxy);
}

void GameScene_Dota::setSight( AOI_PROXY_PTR proxy,int nSight )
{
    m_AOI_manager.SetObjSight(proxy, nSight);
}

void GameScene_Dota::updateObjsInSight(AOI_PROXY_PTR proxy)
{
    m_AOI_manager.updateObjsInSight(proxy);
}

int GameScene_Dota::k_nearest( Vector3 loc,float fDist,AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask, bool bCheckDist)
{
	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	// 找的时候多加1个范围,不然找小于1的范围转int为0了
	if ( fDist<1.0f ) fDist= 1.0f;

    DWORD dwStartTick = getTickCountEx();

    DWORD dwCostTick = getTickCountEx() - dwStartTick;
    if (dwCostTick > 1)
    {
        WarningLn(__FUNCTION__": wait for removing cost "<< dwCostTick <<" ms");
    }

	AOI_Proxy * pProxyArray[MAX_INTEREST_OBJECT];
	int count = m_AOI_manager.k_nearest( myLoc,fDist,pProxyArray,MAX_INTEREST_OBJECT,campMask );
	
	int nTrueCount = 0;
	for ( int i=0;i<count && nTrueCount<nArraySize;++i )
	{
		if (pProxyArray[i]->m_pSharePtr)
        {
            if (bCheckDist && !pProxyArray[i]->m_Loc.checkDistance(loc, fDist))
            {
                continue;
            }
			pReturnArray[nTrueCount] = pProxyArray[i]->getSharePtr();
			if ( pReturnArray[nTrueCount]!=0 ) ++nTrueCount;
        }
	}

    DWORD dwCostTick2 = getTickCountEx() - dwStartTick;
    if (dwCostTick2 > 1)
    {
        WarningLn(__FUNCTION__" cost " << dwCostTick2 << " ms");
    }

	return nTrueCount;
}

int GameScene_Dota::k_nearestInSpell( Vector3 loc,float fDist,float fHeight,AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask )
{
#ifdef OPEN_BVTTEST
	PP_BY_NAME( "GameScene_Dota::k_nearestInSpell" );
#endif

	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	// 找的时候多加1个范围,不然找小于1的范围转int为0了
	/*if ( fDist<1.0f ) fDist= 1.0f;*/

	AOI_Proxy * pProxyArray[MAX_INTEREST_OBJECT];
    // loc normalizeLoc 之后  会有 1 x 1的偏差  所以多找 1米范围
	int count = m_AOI_manager.k_nearest( myLoc,fDist+1,pProxyArray,MAX_INTEREST_OBJECT,campMask);
	
	if ( count<=0 )
		return count;

	multimap<float, AOI_Proxy *> mapUID;
	for ( int i=0;i<count;++i )
	{
        if(abs(loc.y - pProxyArray[i]->m_Loc.y) > fHeight)
            continue;

        // 算出距离
        float fLength = (pProxyArray[i]->m_Loc- loc).getLength();
        if(fLength > fDist)
            continue;

        mapUID.insert(multimap<float, AOI_Proxy *>::value_type(fLength, pProxyArray[i]));
	}

	int nTrueCount = 0;
	multimap<float, AOI_Proxy *>::iterator Iter = mapUID.begin();
	while (Iter != mapUID.end() && nTrueCount<nArraySize)
	{
		if ( Iter->second->m_pSharePtr )
		{
			pReturnArray[nTrueCount] = Iter->second->getSharePtr();
			if ( pReturnArray[nTrueCount]!=0 ) ++nTrueCount;
		}
		++Iter;
	}

	return nTrueCount;
}

int GameScene_Dota::k_nearLineest( Vector3 loc,Vector3 dir, float fDist,float fWith, float fHeight, AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask )
{
#ifdef OPEN_BVTTEST
	PP_BY_NAME( "GameScene_Dota::k_nearLineest" );
#endif

	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	// 找的时候多加1个范围,不然找小于1的范围转int为0了
	if ( fDist<1.0f ) fDist= 1.0f;

    // 搜圆的时候搜大一些 保证矩形被圆全都覆盖过来(内接) 另外myLoc也不精确
	AOI_Proxy * pProxyArray[MAX_INTEREST_OBJECT];
    float fRaddi = fDist+fWith+3; 
	int count = m_AOI_manager.k_nearest( myLoc,fRaddi,pProxyArray,MAX_INTEREST_OBJECT,campMask);
	
	if ( count<=0 )
		return count;

	multimap<float, AOI_Proxy *> mapUID;
	for ( int i=0;i<count;++i )
	{
		if ( loc.checkLineDistance(pProxyArray[i]->m_Loc,dir,fDist,fWith,fHeight) )
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
		{
			pReturnArray[nTrueCount] = Iter->second->getSharePtr();
			if ( pReturnArray[nTrueCount]!=0 ) ++nTrueCount;
		}
		++Iter;
	}

	return nTrueCount;
}

int GameScene_Dota::k_nearSectorest( Vector3 loc,Vector3 dir, float fr, float squaredR, float cosTheta, float fHeight, AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask )
{
#ifdef OPEN_BVTTEST
	PP_BY_NAME( "GameScene_Dota::k_nearSectorest" );
#endif

	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	// 找的时候多加1个范围,不然找小于1的范围转int为0了
	if ( fr<1.0f ) fr= 1.0f;

	AOI_Proxy * pProxyArray[MAX_INTEREST_OBJECT];
	int count = m_AOI_manager.k_nearest( myLoc,fr,pProxyArray,MAX_INTEREST_OBJECT,campMask);
	
	if ( count<=0 )
		return count;

	multimap<float, AOI_Proxy *> mapUID;
	for ( int i=0;i<count;++i )
	{
		if ( loc.checkPointInSector(pProxyArray[i]->m_Loc, dir, squaredR, cosTheta, fHeight) )
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
		{
			pReturnArray[nTrueCount] = Iter->second->getSharePtr();
			if ( pReturnArray[nTrueCount]!=0 ) ++nTrueCount;
		}
		++Iter;
	}

	return nTrueCount;
}

int GameScene_Dota::broadcast(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy, bool ignoreSelf)
{
	int myLoc[AOI_GRID_DIM];
	normalizeLoc(loc,myLoc);

	PACKAGE_PTR package = PACKAGE_PTR(new std::string(msg));
	m_AOI_manager.broadcast(id,myLoc,proxy,package,ignoreSelf);
	return 1;
}

int GameScene_Dota::broadcastMultiple(char *pData,int nCount,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy)
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

int GameScene_Dota::broadcast_all( BROADCAST_ID id,std::string & msg,int campMask )
{
	PACKAGE_PTR package = PACKAGE_PTR(new std::string(msg));

	m_AOI_manager.broadcast_all( id,AOI_PROXY_PTR(),package,campMask);

	return 1;
}

int GameScene_Dota::broadcast_nearLoc(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy, bool ignoreSelf,int campMask/*=CAMP_MASK_ALL*/)
{
    vector<Vector3> vLoc;
    vLoc.push_back(loc);

    int myLoc[AOI_GRID_DIM];
    normalizeLoc(loc,myLoc);

    PACKAGE_PTR package = PACKAGE_PTR(new std::string(msg));
    m_AOI_manager.broadcast_nearLoc(id,myLoc,vLoc,proxy,package,ignoreSelf,campMask);
    return 1;
}

int GameScene_Dota::broadcastMultiple_nearLoc(char *pData,int nCount,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy,int campMask/*=CAMP_MASK_ALL*/)
{
    if (nCount <= 0)
    {
        return 1;
    }
    if (nCount > AOI_LOC_COUNT)
    {
        ErrorLn("broadcastMultiple_nearLoc count>AOI_LOC_COUNT");
        return 0;
    }

    vector<Vector3> vLoc;
    int myLoc[AOI_LOC_COUNT][AOI_GRID_DIM];
    memset(myLoc, 0, sizeof(myLoc));
    for (int i=0; i<nCount; ++i)
    {
        Vector3 *pLoc = (Vector3*)pData+i;
        vLoc.push_back(*pLoc);
        normalizeLoc(*pLoc,myLoc[i]);
    }

    PACKAGE_PTR package = PACKAGE_PTR(new std::string(msg));
    m_AOI_manager.broadcastMultiple_nearLoc(id,myLoc,nCount,vLoc,proxy,package,campMask);
    return 1;
}

int GameScene_Dota::getPlayerCount()
{
	return m_player_count;
}

int GameScene_Dota::getMonsterCount()
{
    return m_monster_count;
}

// 获取场景更新共享数据
SceneUpdateShareInfo GameScene_Dota::getUpdateShareInfo()
{
    SceneUpdateShareInfo info;
    info.nMapID = m_nMapID;
    info.nSceneID = m_nSceneID;
    info.nPlayerQty = m_player_count; /*m_playerSet.size();*/
    info.nMonsterQty = m_monster_count; /*m_monsterSet.size();*/

    return info;
}

int GameScene_Dota::getAllPlayers( AOI_PROXY_PTR* pReturnArray,int nArraySize )
{
	int count = 0;
	ENTITY_SET::iterator it = m_playerSet.begin();
	for ( ;it!=m_playerSet.end() && count<nArraySize;++it )
	{
        if((*it)->m_pSharePtr)
		    pReturnArray[count++] = *it;
	}

	return count;
}

void GameScene_Dota::OnTimer( unsigned long dwTimerID )
{
	switch (dwTimerID)
	{
	//case TIMER_FLUSH_SIGHT:
	//	{
	//		ENTITY_SET::iterator it = m_playerSet.begin();
	//		for ( ;it!=m_playerSet.end();++it )
	//		{
	//			int myLoc[AOI_GRID_DIM];
	//			AOI_PROXY_PTR proxy = *it;
	//			normalizeLoc(proxy->m_Loc,myLoc);
	//			m_AOI_manager.testInSight(myLoc,proxy);
 //               // 通常只搜周围可以看见的实体 因为用到这个快照的地方都是些怪物搜敌、攻击之类的
 //               m_AOI_manager.snapshot_nearest(proxy, true);
	//		}

	//		it = m_monsterSet.begin();
	//		for ( ;it!=m_monsterSet.end();++it )
	//		{
	//			int myLoc[AOI_GRID_DIM];
	//			AOI_PROXY_PTR proxy = *it;
	//			normalizeLoc(proxy->m_Loc,myLoc);
	//			m_AOI_manager.testInSight(myLoc,proxy);
 //               // 通常只搜周围可以看见的实体 因为用到这个快照的地方都是些怪物搜敌、攻击之类的
 //               bool bInsightOnly = true;
 //               if(UID_2_TYPE(proxy->m_uid) == TYPE_MONSTER && proxy->m_nVocation == War::EWME_Monster)
 //               {
 //                   // 野怪因为需要支持在草丛就可以打野，为防止野怪回归，野怪需要可以看到隐形单位
 //                   bInsightOnly = false;
 //               }
 //               m_AOI_manager.snapshot_nearest(proxy, bInsightOnly);
	//		}
	//	}
	//	break;

	case TIMER_OUTPUT_INFO:		// 输出场景信息
		{
			OnTimerOutputInfo();
		}
		break;

	default:
		{	
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 定时输出场景信息
void GameScene_Dota::OnTimerOutputInfo(void)
{
	static BYTE byAlert = 0;

	DWORD dwPlayerCount = m_player_count; /*m_playerSet.size();*/
	DWORD dwMonsterCount = m_monster_count; /*m_monsterSet.size();*/

	bool bCanOuptut = false;
	if(dwPlayerCount == 0 && dwMonsterCount == 0)
	{
		++byAlert;

		// 每小时输出一下
		if(byAlert == 60)
		{
			byAlert = 0;
			bCanOuptut = true;
		}
	}
	else
	{
		bCanOuptut = true;
	}

	if(bCanOuptut)
	{
		EmphasisLn("Scene Timer Statistics:SceneID="<<m_nSceneID<<",player_count="<<dwPlayerCount<<",monster_count="<<dwMonsterCount);
	}
}