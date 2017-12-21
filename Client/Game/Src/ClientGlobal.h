/*******************************************************************
** 文件名:	ClientGlobal.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/25/2014
** 版  本:	1.0
** 描  述:	
			
********************************************************************/

#pragma once

#include "Singleton.h"
#include "IClientGlobal.h"
#include "MessageDispatch.h"
#include "Stage_Manager.h"
//#include "EntityFactory.h"
#include "TimeSyncClient.h"
#include "TimerAxis.h"
#include "net/net.h"
#include "IEventEngine.h"
#include "IEntity.h"
#include "IEntityClient.h"
#include "Trace.h"
#include "IRenderView.h"
#include "gatewayclient.h"
#include "SchemeCenter.h"
#include "ICondition.h"
#include "IEffect.h"
#include "ISpell.h"
#include "IFreezeClient.h"
#include "IBuffClient.h"
#include "ITankClient.h"
#include "IWarClient.h"
#include "IPathFindManager.h"
#include "IHandleCommand.h"
#include "HandleCommandManager.h"
#include "IChatClient.h"
#include "LogicTask.h"
#include "IOrderClient.h"
#include "IClanClient.h"
#include "IKinClient.h"
#include "ITeamClient.h"
#include "IMatchClient.h"
#include "IAntiAddictClient.h"
#include "ITaskClient.h"
#include "ILegendCupClient.h"
#include "IMentorshipClient.h"
#include "IMailClient.h"
#include "IKeyWordClient.h"
#include "IDistrictListClient.h"
#include "ISNSClient.h"
#include "IMiscManager.h"
#include "IScreenCaptureClient.h"
#include "ClientConnection.h"

using namespace rkt;

class SchemeCenter;

extern HWND  g_MainWnd;


/**
	全局类，通过该类访问各种全局资源
	用来管理各种类的索引和初始化释放顺序
*/
class CClientGlobal : public IClientGlobal,public Singleton<CClientGlobal>
{
public:
	/////////////////////IClientGlobal////////////////////////////////////////////////////////
	virtual IEventEngine *  getEventEngine() { return m_pEventEngine; }


    /// 获取网络连接
    virtual IClientConnection*  getNetConnection();

    /// 获取队列里未处理的数据包个数
    virtual unsigned long getPendingCount(bool bIsSend);

    /// 获取网络延迟
    virtual unsigned long getNetLatency();

	virtual IMessageDispatch * getMessageDispatch() const { return m_pMessageDispatch; }

	virtual Stage_Manager * getStageManager() const { return m_pStageManager; }

	virtual IEntityFactory * getEntityFactory() const { return m_pEntityClient->GetEntityFactory(); }

	virtual IClientEntity * getHero() const { return m_pHero; }
	
	virtual void setHero(IClientEntity * pHero) { m_pHero = pHero;}

	virtual TimeSyncClient * getTimeSyncer() { return &m_TimeSyncer; }

	virtual TimerAxis * getTimerAxis() { return CLogicTask::getInstance().getTimerAxis(); }

	virtual ISchemeEngine * getSchemeEngine() {return m_pSchemeEngine;}

	virtual IEntityClient * getEntityClient() {return m_pEntityClient;}

	virtual IRenderViewProxy * getRenderView();

	/// 取得游戏脚本配置中心
	virtual ISchemeCenter*	getSchemeCenter()	{ return m_pSchemeCenter; }

	/// 取得条件工厂
	virtual IConditionFactory* getConditionFactory() { return m_pConditionFactory;}

	/// 取得效果工厂
	virtual IEffectFactory* getEffectFactory() { return m_pEffectFactory;}

	/// 取得技能工厂
	virtual ISpellFactory* getSpellFactory() { return m_pSpellFactory;}

	/// 取得冷却客户端
	virtual IFreezeClient* getFreezeClient() { return m_pFreezeClient;}

	/// 取得Buff客户端
	virtual IBuffClient* getBuffClient() { return m_pBuffClient;}

	/// 取得tank客户端
	virtual ITankClient* getTankClient() { return m_pTankClient;}

	/// 取得War客户端
	virtual IWarClient* getWarClient() { return m_pWarClient;}

	// 获得聊天系统客户端
	virtual IChatClient* getChatClient() {return m_pChatClient;}

    // 获得排行榜系统客户端
    virtual IOrderClient* getOrderClient() {return m_pOrderClient;}

    virtual IPathFindManager*   getPathFindManager()  { return m_pPathFindManager; }

	/// 取得操作命令管理接口
	virtual IHandleCommandManager*  getHandleCommandManager() { return m_pHandleCommandManager; };

	// 获得SNS客户端
	virtual ISNSClient*	            getSNSClient() { return m_pSNSClient; }

    // 获得战队系统客户端
    virtual IClanClient*            getClanClient() { return m_pClanClient; }

    // 获得战队系统客户端
    virtual IKinClient*             getKinClient() { return m_pKinClient; }

    // 取得组队系统客户端
    virtual ITeamClient*            getTeamClient() { return m_pTeamClient; }

    // 获得MatchClient
    virtual IMatchClient*           getMatchClient() { return m_pMatchClient; };

	// 获得师徒系统客户端
	virtual IMentorshipClient*      getMentorshipClient() { return m_pMentorshipClient; }

    virtual bool                    gotoState( BYTE nState,void * context=NULL,int len=0 );

    virtual BYTE                    getCurState();

	// 取得防沉迷系统客户端
	virtual IAntiAddictClient*      getAntiAddictClient()   { return m_pAntiAddictClient; }

    // 取得任务客户端
    virtual ITaskClient*            getTaskClient() { return m_pTaskClient; }

    virtual ILegendCupClient*       getLegendCupClient() { return m_pLegendCupClient; }

	// 获得LOG文件路径
	virtual string				    getLogFileName() { return szLogFileName; }

	// 获取Mail客户端
	virtual IMailClient*		    getMailClient() { return m_pMailClient; }
	
	// 获取关键字客户端
	virtual IKeyWordClient*		    getKeyWordClient() { return m_pKeyWordClient; }

	virtual IDistrictListClient*    getDistrictListClient();
	
    // 获得杂项管理对象
    virtual IMiscManager*           getMiscManager()  { return m_pMiscManager; } 

	virtual IScreenCaptureClient*   getScreenCaptureClient() {return m_pScreenCaptureClient;}

    virtual HeightFieldMgr* getHeightFieldMgr() {return m_pHeightFieldMgr;}
	
	/////////////////////CClientGlobal////////////////////////////////////////////////////////
    void openLog();

	bool create();
	bool destroy();
	CClientGlobal();

private:
    void outputMachineDetail();

private:
	ClientConnection	m_Connection;		// 当前网络连接对象
	IMessageDispatch*	m_pMessageDispatch; // 当前消息派发对象
	Stage_Manager*		m_pStageManager;    // 当前游戏阶段状态管理器
	TimeSyncClient      m_TimeSyncer;       // 时间同步器
	IEventEngine*		m_pEventEngine;     // 事件引擎
	IClientEntity*		m_pHero;            // 客户端主角
	IEntityClient*      m_pEntityClient;	// 客户端实体模块
	TraceListener*		m_traceListener;    // 调试信息输出对象
	ISchemeEngine*		m_pSchemeEngine;	// 配置服务
	ISchemeCenter*		m_pSchemeCenter;	// 游戏脚本配置中心
	IConditionFactory*	m_pConditionFactory;// 条件工厂
	IEffectFactory*		m_pEffectFactory;	// 效果工厂
	ISpellFactory*		m_pSpellFactory;	// 技能工厂
	IFreezeClient*		m_pFreezeClient;	// 冷却客户端
	IBuffClient*		m_pBuffClient;		// Buff客户端
	ITankClient*		m_pTankClient;		// tank客户端
	IWarClient*			m_pWarClient;		// 战场客户端
	IChatClient*		m_pChatClient;		// 聊天系统客户端
    IMatchClient*		m_pMatchClient;     // 房间比赛相关客户端
    IOrderClient*       m_pOrderClient;     // 排行榜系统客户端
    IClanClient*        m_pClanClient;      // 帮会
    IKinClient*         m_pKinClient;       // 战队  
    ITeamClient*        m_pTeamClient;      // 组队
	IMentorshipClient*         m_pMentorshipClient;// 师徒  
	IAntiAddictClient*	m_pAntiAddictClient;// 防沉迷
	IPathFindManager*   m_pPathFindManager; // 寻路
    ITaskClient*	    m_pTaskClient;      // 任务
    ILegendCupClient*	m_pLegendCupClient;      // 杯赛
	IHandleCommandManager*	m_pHandleCommandManager;	// 操作命令管理接口
    IRenderViewProxy*   m_pRenderView;
	IStackWalk*			pStackWalk;
	ISNSClient*			m_pSNSClient;		// SNS系统客户端
	IMailClient*		m_pMailClient;		// mail客户端
	IKeyWordClient*		m_pKeyWordClient;	// 关键字客户端
    IMiscManager*       m_pMiscManager;     // 杂项管理对象
	IScreenCaptureClient* m_pScreenCaptureClient;  //录屏客户端
    HeightFieldMgr*     m_pHeightFieldMgr;

	bool				m_bCreated;			// 是否已创建
	char				szLogFileName[GAME_TEXTSTR_MAXSIZE]; //LOG文件名
};
