/*******************************************************************
** 文件名:	GameScene_Dota.h
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
#include "AOI_Forest.h"
#include "AOI_Manager_Dota.h"
#include "IServiceContainer.h"
#include "TimerHandler.h"
#include "Simple_Atom_Lock.h"
#include <set>


class GameScene_Dota : public IGameSceneService,public TimerHandler
{
	enum
	{
		TIMER_FLUSH_SIGHT = 0,		// 刷新视野
		TIMER_OUTPUT_INFO,			// 输出场景信息
	};

public:
	//////////////IGameSceneService////////////////////////////////////////////////////////////
	virtual bool onCreate( int mapID,int sceneID, bool isCopy, int nWarID=INVALID_WAR_ID );

	virtual int getSceneID();

	virtual int getMapID();

	virtual bool insertEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag );

	virtual bool removeEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag );

	virtual bool moveEntity( AOI_PROXY_PTR proxy,Vector3 newLoc );

    virtual bool setObserver( AOI_PROXY_PTR proxy,bool bObserver );

    virtual void updateObjVisible( AOI_PROXY_PTR proxy );

    virtual void onUpdateObjAntiStealth( AOI_PROXY_PTR proxy );

    virtual void setSight( AOI_PROXY_PTR proxy,int nSight );

    virtual void updateObjsInSight(AOI_PROXY_PTR proxy);

	virtual int k_nearest( Vector3 loc,float fDist,AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL, bool bCheckDist = false);

	virtual int k_nearestInSpell( Vector3 loc,float fDist,float fHeight,AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL );

	virtual int k_nearLineest( Vector3 loc,Vector3 dir, float fDist, float fWith, float fHeight, AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL);

	virtual int k_nearSectorest( Vector3 loc,Vector3 dir, float fr, float squaredR, float cosTheta, float fHeight,AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL );

	virtual int broadcast(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0),bool ignoreSelf=false);

	virtual int broadcastMultiple(char *pData,int nCount,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0));

	virtual int broadcast_all( BROADCAST_ID id,std::string & msg,int campMask=CAMP_MASK_ALL );

    virtual int broadcast_nearLoc(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0),bool ignoreSelf=false,int campMask=CAMP_MASK_ALL );

    virtual int broadcastMultiple_nearLoc(char *pData,int nCount,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0),int campMask=CAMP_MASK_ALL );

	virtual int getPlayerCount();

    virtual int getMonsterCount();

    // 获取场景更新共享数据
    virtual SceneUpdateShareInfo getUpdateShareInfo();

	virtual int getAllPlayers( AOI_PROXY_PTR * pReturnArray,int nArraySize );

	virtual UINT_PTR getLogicServiceID()
	{ 
        return m_sidLogicService;
	}

    // 取得给ob分配的阵营ID
    virtual int getObCampID(){return m_AOI_manager.getObCamp();}

    bool on_stop();

	// 关闭场景
	virtual void close();

	virtual void release();

	//////////////TimerHandler////////////////////////////////////////////////////////////

	virtual void OnTimer( unsigned long dwTimerID );

public:
	GameScene_Dota(int nCampCount);

private:

	// 让浮点坐标系标准化，乘系数转成整型
	void normalizeLoc( Vector3 fLoc,int iLoc[AOI_GRID_DIM] );

	// 定时输出场景信息
	void OnTimerOutputInfo(void);

protected:
	int					m_nSceneID;
	int					m_nMapID;

	typedef std::set<AOI_PROXY_PTR>	ENTITY_SET;
	ENTITY_SET			m_playerSet;				// 这里要记录一份，因为AOI_MANAGER里没有用智能指针
	ENTITY_SET			m_monsterSet;				// 这里要记录一份，因为AOI_MANAGER里没有用智能指针
	int                 m_player_count;
	int                 m_monster_count;

	typedef AOI_Manager_Dota< AOI_Proxy *,          // 智能指针不支持拷贝复制
		                      int,
							  AOI_GRID_DIM,
							  AOI_GRID_SIZE,
							  AOI_DEFAULT_SIGHT,
							  AOI_Forest>  AOI_MANAGER_PROXY;

	AOI_MANAGER_PROXY	m_AOI_manager;

    UINT_PTR            m_sidLogicService;          // 场景逻辑service id
};