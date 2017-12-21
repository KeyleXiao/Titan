/*******************************************************************
** 文件名:	ServerGlobalService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#pragma once;

#include "IServerGlobalService.h"
#include "IFramework.h"
#include "IServiceContainer.h"
#include "ITimerService.h"
#include "IEventEngine.h"
#include "ISchemeEngine.h"
#include "IServiceMgr.h"
#include "IGatewayAcceptorService.h"
#include "ICenterConnectorService.h"
#include "ISceneManager.h"
#include "ISchemeCenter.h"
#include "ICondition.h"
#include "IEffect.h"
#include "ISpell.h"
#include "IFreezeServer.h"
#include "IBuffServer.h"
#include "IDamageServer.h"
#include "ITankServer.h"
#include "IEnmityServer.h"
#include "ITestServer.h"
#include "IPathFindManager.h"
#include "IMatchScene.h"
#include "IWarManager.h"
#include "IGameResService.h"
#include "IShareServer.h"
#include "IChatScene.h"
#include "IOrderScene.h"
#include "IClanScene.h"
#include "IKinScene.h"
#include "IAIFactory.h"
#include "ITaskServer.h"
#include "ITeamScene.h"
#include "IOSSLogServer.h"
#include "IAntiAddictServer.h"
#include "ILegendCupScene.h"
#include "IMentorshipScene.h"
#include "IStaticWarScene.h"
#include "IMailScene.h"
#include "IDBHttpService.h"

#include "MessageDispatch.h"
#include "ServerMessageHandler.h"
#include "HeightFieldMgr.h"

#define OPTIMIZATION_SERVER

class ServerGlobalService : public IServerGlobalService, public IEventExecuteSink, public TimerHandler
{
    enum
    {
        ETIMER_CHECK_BASESERVICE,			    // 检测服务器启动时要加载的服务
        ETIMER_MANAGER_RETRYSTART,              // 重请求管理器启动服务器
    };

    enum
    {
        CHECK_BASESERVICE_INTERVAL  = 500,       // 检测服务器启动时要加载的服务定时器间隔
        CHECK_RETRYSTART_INTERVAL   = 1000,       // 重请求管理器启动服务器定时器间隔
    };

    enum EMapUpdatePlayer
    {
        MAP_UPDATEPLAYER_NONE,
        MAP_UPDATEPLAYER_ADD,		// 增加人数
        MAP_UPDATEPLAYER_REMOVE,	// 移出人数
        MAP_UPDATEPLAYER_PREENTER_UPDATE,		// 预进入地图的人数更新
    };
public:
	////////////////// IServerGlobalService ////////////////////////////////////////////////////////
	// 创建
	virtual bool						create();

	// 释放
	virtual void						release();

    // 消息分发器
    virtual IMessageDispatch*           getMessageDispatch();

	// 获取框架接口
	virtual IFramework *				getFramework();

	// 获取配置中心接口
	virtual ISchemeCenter *				getSchemeCenter();

	// 取得场景管理器
	virtual ISceneManager *				getSceneManager();

	// 取得配置服务
	virtual ISchemeEngine *				getSchemeEngine();

	// 取得条件工厂
	virtual IConditionFactory *			getConditionFactory();

	// 取得效果工厂
	virtual IEffectFactory *			getEffectFactory();

	// 取得技能工厂
	virtual ISpellFactory *				getSpellFactory();

	// 取得冷却服务器
	virtual IFreezeServer *				getFreezeServer();

	// 取得Buff服务
	virtual IBuffServer *				getBuffServer();

	// 取得伤害服务器
	virtual IDamageServer *				getDamageServer();

	// 取得tank服务器
	virtual ITankServer *				getTankServer();

	// 取得仇恨服务器
	virtual IEnmityServer *				getEnmityServer();

    // 取得测试服务器
    virtual ITestServer *               getTestServer();

	// 取得寻路管理器
	virtual IPathFindManager *			getPathFindManager();

	// 比赛大厅场景服
	virtual IMatchScene*				getMatchScene();

	// 聊天系统场景服
	virtual IChatScene*					getChatScene();

	virtual IWarManager*				getWarManager();

	virtual IShareReceiver*				getShareReceiver();
	virtual IShareSceneSvrSource*		getShareSceneSvrSource();

    virtual IOrderScene*                getOrderScene();

    virtual IActorMgr*                  getActorMgr();

    virtual IClanScene*                 getClanScene();

    virtual IKinScene*                  getKinScene();

    virtual IAIFactory*                 getAIFactory();

    virtual ITeamScene*				    getTeamScene();

	virtual IAntiAddictServer*			getAntiAddictServer();
    
    // 取资源管理对象
    virtual IResourceManager*			getResourceManager();

    // 取杂项管理对象
    virtual IMiscManager*				getMiscManager();
	
    virtual ILegendCupScene*            getLegendCupScene();

	// 取得师徒场景服
	virtual IMentorshipScene*           getMentorshipScene();

	virtual IStaticWarScene*            getStaticWarScene();

	// 取得邮件场景服
	virtual IMailScene*					getMailScene();

    virtual HeightFieldMgr*             getHeightFieldMgr();
    
    //////////////////////////////////////////////////////////////////////////
    // 请求跨Service处理服务控制，有些数据需要在Service上才能处理
    virtual void                        requestHandleServerControl(DWORD dwState);

    // 请求管理器启动
    virtual bool                        requestManagerStart(DWORD dwStartFlag, DWORD dwUserData);
    
	///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void                        handleServerListUpdated();

	/**
	@name         : 通知服务器信息更新
	@brief        : 例如服务器人数变化等等
	*/
	virtual void                        handleServerInfoUpdated(DWORD ServerID, BYTE nState, void* pInfo, size_t nInfolen);

    /**
	@name         : 处理其他服务器通过中心服务器转发的消息
	@brief        : 具体内容不清楚
	@param server : 该消息是哪个服务器发过来的
	@param pData  : 消息内容
	@param wDataLen:消息长度
	*/
	virtual void                        handlePostMessage(DWORD server,const char * pData,WORD wDataLen);
	///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
    // 网关服务器通知游戏服务器有一个新的客户端连进来
	virtual void                        handleClientIncoming(ClientID client);
	// 网关服务器通知游戏服务器有新的数据包到达
	virtual void						handleGateWayReceivedData(ClientID client, void* data, size_t len) ;
	// 网关服务器通知游戏服务器有连接断开
	virtual void						handleGateWayClientDisconnect(ClientID client,DWORD dwReason);

    ///////ManagerConnector///////////////////////////////////////////////////////////////////
    // 收到设置游戏ID
    virtual void                        handleManagerSetGameWorldID();

    // 收到管理器下发的资源数据
    virtual void                        handleManagerResourceData(LPVOID pData,DWORD dwDataLen);

    // 收到管理器下发的资源数据结果
    virtual void                        handleManagerResourceResult(LPVOID pData,DWORD dwDataLen);

	////////////////// IEventExecuteSink ////////////////////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void						OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////// TimerHandler ////////////////////////////////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void                        OnTimer( unsigned long dwTimerID );

	////////////////// ServerGlobalService ////////////////////////////////////////////////////////

	// 构造函数
	ServerGlobalService();

	// 析构函数
	virtual ~ServerGlobalService();

	// 启动
	// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
	bool on_start(void * data,int len );

	// 停止
	bool on_stop();

private:
	// 后续启动
	void startEntries(void);

    // 停掉本身启动的服务
    void stopServices();
	// 创建服务
	bool createService(char * pszServiceSchemeName);

    // 启动DB服务
    void startDBService();
    // 启动迁移服务
    void startMigrateService();
    // 启动中心服务
    void startCenterService();
    // 启动网关服务
    void startGateWayService();
    // 启动桥服务
    void startBridgeService();
    // 启动GUARD服务
    void startGuardService();

	// 创建共享
	bool createShareService(void);

    // 场景服信息更新到中心服
    void updateZoneServerInfo2Center(int nSceneID = 0, EMapUpdatePlayer eUpdatePlayer = MAP_UPDATEPLAYER_NONE);

    void makeServerInfo();

    // 定时检测基础服务是否已启好
    void onTimerCheckBaseService(void);

    // 执行服务器信息认证
    void startAuthControl();

private:
    CMessageDispatch			m_MessageDispatch;
    CServerMessageHandler       m_serverMessageHanlder;

	/// 获得事件中心接口
	IFramework *				m_pFramework;
	unsigned short				m_nServerID;
	ICenterConnectorService *	m_pCenterConnector;
	IGatewayAcceptorService *	m_pGatewayAcceptor;
	ISceneManager *				m_pSceneManager;
	ISchemeCenter *				m_pSchemeCenter;
	ISchemeEngine*				m_pSchemeEngine;	// 配置服务
	IConditionFactory *         m_pConditionFactory;
	IEffectFactory *            m_pEffectFactory;
	ISpellFactory *             m_pSpellFactory;
	IFreezeServer *             m_pFreezeServer;
	IBuffServer *				m_pBuffServer;
	IDamageServer *				m_pDamageServer;
	ITankServer*				m_pTankServer;
	IEnmityServer*				m_pEnmityServer;
    ITestServer *               m_pTestServer;
	IPathFindManager*			m_pPathFindManager;
	IMatchScene*				m_pMatchScene;
	IWarManager*				m_pWarManager;
	IGameResService*			m_pGameResService;
	IShareReceiver *			m_pShareReceiver;
	IShareSceneSvrSource *		m_pShareSceneSvrSource;
	IChatScene*					m_pChatScene;
	IOrderScene*				m_pOrderScene;
    IActorMgr*                  m_pActorMgr;
    IClanScene*                 m_pClanScene;
    IKinScene*                  m_pKinScene;     
    IAIFactoryWrapper*          m_pAIFactoryWrapper;
	ITaskServer*				m_pTaskServer;		// 目前这个TaskServer只在这声明了，以后再看外界是否需要
    ITeamScene*                 m_pTeamScene;
    IOSSLogServer*              m_pOssLogServer;
	IAntiAddictServer*			m_pAntiAddictServer;
    IResourceManager*           m_pResourceManager;
    IMiscManager*               m_pMiscManager;
	ILegendCupScene*			m_pLegendCupScene;
	IMentorshipScene*			m_pMentorshipScene;
    IStaticWarScene*			m_pStaticWarScene;
	IMailScene*					m_pMailScene;
	IPlayerActionRecord*		m_pActionLib;
    HeightFieldMgr*             m_pHeightFieldMgr;
    IDBHttpService *            m_pDBHttpService;

    int                         m_nCurPlayerNum;    // 本进程当前玩家数量
    DWORD                       m_dwRetryStartFlag;
    DWORD                       m_dwRetryUserData;

    void*                       m_pServerInfo;    
};