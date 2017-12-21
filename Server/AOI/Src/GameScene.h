/*******************************************************************
** 文件名:	GameScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	
            游戏场景对象
********************************************************************/

#pragma once

#include "ISceneManager.h"
#include "AOI_Define.h"
#include "AOI_Manager.h"
#include "IServiceContainer.h"
#include <set>

class GameScene : public IGameSceneService
{
public:
	virtual bool onCreate( int mapID,int sceneID, bool isCopy, int nWarID=INVALID_WAR_ID );

	virtual int getSceneID();

	virtual int getMapID();

	virtual bool insertEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag/*,int layer*/ );

	virtual bool removeEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag/*,int layer*/ );

	virtual bool moveEntity( AOI_PROXY_PTR proxy,Vector3 newLoc/*,int layer*/  );

    virtual bool setObserver( AOI_PROXY_PTR proxy,bool bObserver ){return true;}

    virtual void updateObjVisible( AOI_PROXY_PTR proxy );

    virtual void onUpdateObjAntiStealth( AOI_PROXY_PTR proxy );

    virtual void setSight( AOI_PROXY_PTR proxy,int nSight );

    virtual void updateObjsInSight(AOI_PROXY_PTR proxy){}

	virtual int k_nearest( Vector3 loc,float fDist,AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask, bool bCheckDist);

	virtual int k_nearLineest( Vector3 loc,Vector3 dir, float fDist, float fWith, float fHeight, AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask );

    virtual int k_nearestInSpell( Vector3 loc,float fDist,float fHeight,AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask=LAYER_ALL );

	virtual int k_nearSectorest( Vector3 loc,Vector3 dir, float fr, float squaredR, float cosTheta, float fHeight, AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask );

    // 为了兼容基类接口,加了一个无用参数proxy 去掉了layerMask标志 默认只在watcher层广播，
	virtual int broadcast(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg, AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0),bool ignoreSelf = false);

	virtual int broadcastMultiple(char *pData,int nCount,BROADCAST_ID id,std::string & msg,/*int layerMask*/ AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0));

    // 为了兼容基类接口,加了一个无用参数layerMask
	virtual int broadcast_all( BROADCAST_ID id,std::string & msg, int layerMask=LAYER_ALL );

    // 向看得见的人广播(在点附近才广播,用于怪物使用技能等广播)
    virtual int broadcast_nearLoc(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,/*int layerMask*/ AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0),bool ignoreSelf = false, int campMask=CAMP_MASK_ALL);

    // 向看得见的多个点广播(在点附近才广播,用于怪物使用技能等广播)
    virtual int broadcastMultiple_nearLoc(char *pData,int nCount,BROADCAST_ID id,std::string & msg,/*int layerMask*/ AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0), int campMask=CAMP_MASK_ALL);

	virtual int getPlayerCount();

    virtual int getMonsterCount();
    
    // 获取场景更新共享数据
    virtual SceneUpdateShareInfo getUpdateShareInfo();

	virtual int getAllPlayers( AOI_PROXY_PTR * pReturnArray,int nArraySize );

	virtual UINT_PTR getLogicServiceID()
	{
		if ( m_pLogicService==0 ) 
			return INVALID_SERVICE_ID; 
		else
			return m_pLogicService->get_id(); 
	}

    // 取得给ob分配的阵营ID
    virtual int getObCampID(){return -1;}

	// 关闭场景
	virtual void close();

    bool on_stop();

	virtual void release();
	
	// 让浮点坐标系标准化，乘系数转成整型
	void normalizeLoc( Vector3 fLoc,int iLoc[AOI_GRID_DIM] );

	GameScene() : m_nSceneID(0), m_nMapID(0), m_pLogicService(0),m_player_count(0) {}

protected:
	int					m_nSceneID;
	int					m_nMapID;

	typedef std::set<AOI_PROXY_PTR>	PLAYER_SET;
	PLAYER_SET			m_playerSet;
	int                 m_player_count;

	typedef AOI_Manager< AOI_Proxy *,        // 智能指针不支持拷贝复制
		                 int,
						 AOI_GRID_DIM,
						 AOI_GRID_SIZE,LAYER_COUNT>						 AOI_MANAGER_PROXY;
	AOI_MANAGER_PROXY	m_AOI_manager;

	// 场景逻辑服务
	SERVICE_PTR         m_pLogicService;
};