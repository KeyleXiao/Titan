/*******************************************************************
** 文件名:	SceneLogicService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-10
** 版  本:	1.0
** 描  述:	

		单个场景逻辑服务，管理所有非玩家对象，包括怪物、NPC和载具等(一个协程)
********************************************************************/

#pragma once

#include "ISchemeCenter.h"
#include "IEventEngine.h"
#include "ISceneLogicService.h"
#include "SchemeDef.h"
#include "TimerAxis.h"
#include "ITimerService.h"
#include "IPathFindService.h"
#include "IGameResService.h"
#include "IWarService.h"
#include "IWarManager.h"
#include "ActorMgr.h"


#include <map>
using namespace rkt;

/**
	职责: 
	1.管理某个场景的所有怪物，NPC的创建销毁
	2.管理某个场景的所有怪物AI
	3.管理某个场景的所有NPC对话
	4.管理某个场景的所有载具创建销毁
	5.某个场景中LUA的逻辑处理
*/

class SceneLogicService : public ISceneLogicService, public IEventExecuteSink
{
	int						m_nSceneID;			// 场景ID
	int						m_nMapID;			// 地图ID
	INPCService*			m_pNPCService;		// NPCService对象
	ITankService*			m_pTankService;		// TankService对象
	ILuaService*			m_pLuaService;		// LuaService对象
	IEventEngine*   		m_pEventService;	// 事件对象
	IPathFindService*		m_PathFindService;	// 寻路

    bool                    m_bIsDynamicScene; // 是否是副本

	typedef std::map<UID, DWORD> MAP_DEAD_MONSTER;
	MAP_DEAD_MONSTER		m_mapDeadMonster;

    // AI service挂在这个服务下面
    SERVICE_PTR             m_pAIService;

public:
	/////// ISceneLogicService //////////////////////////////////////////////////
	// 取得NPCService对象
	virtual INPCService * getNPCService(void)
	{
		return m_pNPCService;
	}

	// 取得TankService对象
	virtual ITankService * getTankService(void)
	{
		return m_pTankService;
	}

	// 取得LuaService对象
	virtual ILuaService * getLuaService(void)
	{
		return m_pLuaService;
	}

	virtual IEventEngine * getEventService(void)
	{
		return m_pEventService;
	}

	virtual IPathFindService * getPathFindService(void)
	{
		return m_PathFindService;
    }

    virtual SERVICE_PTR getAIService(void)
    {
        return m_pAIService;
    }

    virtual void destroyActor(UID uid, int nReason)
    {
        // 先通知lua角色下线
        CLuaParam luaParam[2];
        luaParam[0] = gServerGlobal->getActorMgr()->UID2PDBID(uid); 
        luaParam[1] = nReason;
        m_pLuaService->RunLuaFunction("ActorLogoutHandler", luaParam, 2);

        ((CActorMgr*)(gServerGlobal->getActorMgr()))->destroyActor(uid, nReason);
    }

	// 释放
	virtual void release()
	{
		delete this;
	}


	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		// 系统的事件
		switch ( MAKE_EVENT_KEY (bSrcType, wEventID) )
		{
		case MAKE_EVENT_KEY (SOURCE_TYPE_SYSTEM, EVENT_TASK_UPDATE_RANK_TASK):
			{
				m_pLuaService->RunLuaFunction("RequestClearRankTask");
			}
			break;

		default: 
			break;
		}
	}
	
	/////// SceneLogicService //////////////////////////////////////////////////

	SceneLogicService()
		: m_nSceneID(0)
		, m_nMapID(0)
		, m_pNPCService(0)
		, m_pTankService(0)
		, m_pLuaService(0)
		, m_pEventService(0)
		, m_PathFindService(0)
        , m_bIsDynamicScene(false)
        , m_pAIService(0)
	{

	}

	~SceneLogicService()
	{

	}

	bool on_start(int nMapID, int scene, bool isSceneCopy, int nWarID )
	{
		m_nMapID = nMapID;
		m_nSceneID = scene;
        m_bIsDynamicScene = isSceneCopy;
		
		// 获得事件中心接口
		IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
		if (pEventEngine)
		{
			event_scene_created eventdata;
			eventdata.nMapID = nMapID;
			eventdata.nSceneID = scene;

			pEventEngine->FireExecute(EVENT_SCENE_CREATED, SOURCE_TYPE_SCENE, (DWORD)scene, (LPCSTR)&eventdata, sizeof(eventdata));

			// 订阅更新排位任务事件
			pEventEngine->Subscibe(this, EVENT_TASK_UPDATE_RANK_TASK, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
		}

		// 战场时要被通知
        int nWarTypeID = -1;
		if(nWarID != INVALID_WAR_ID)
		{
			SERVICE_PTR pWarContainer = gServerGlobal->getWarManager()->GetWar(nWarID);
			if(pWarContainer != 0)
			{
				IWarService* pWarService = (IWarService*)pWarContainer->query_interface();
				if(pWarService != NULL)
				{
					pWarService->onSceneCreated(nWarID);

                    nWarTypeID = pWarService->getWarTypeId();
				}
			}
		}

		EmphasisLn("SceneLogicService() mapID="<< nMapID <<", m_nSceneID="<< m_nSceneID << ", WarID="<< nWarID );

        // 开启AI Service
        SERVICE_SCHEME* pAIServiceScheme = gServerGlobal->getFramework()->get_service_scheme("AIService");
        if(pAIServiceScheme != NULL)
        {
            obuf data;
            data << m_nSceneID << nWarTypeID << m_nMapID;
            m_pAIService = gServerGlobal->getFramework()->create_service(pAIServiceScheme, NULL, data.data(), data.size());
        }

		return true;
	}

	bool on_stop()
	{
		if ( m_pEventService )
		{
			m_pEventService->Release();
			m_pEventService = 0;
		}

		// 取消订阅更新排位任务事件
		IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
		if (pEventEngine)
		{
			pEventEngine->UnSubscibe(this, EVENT_TASK_UPDATE_RANK_TASK, SOURCE_TYPE_SYSTEM, 0);
		}

		return true;
	}

	bool create(INPCService* pNPCService, ITankService* pTankService, ILuaService* pLuaService, IPathFindService *pPathFindService)
	{
		// NPCService对象
		m_pNPCService = pNPCService;
		// TankService对象
		m_pTankService = pTankService;
		// LuaService对象
		m_pLuaService = pLuaService;
		// EventService对象
		m_pEventService = gServerGlobal->getFramework()->create_event_engine();
		// 寻路对象
		m_PathFindService = pPathFindService;

		return true;
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
