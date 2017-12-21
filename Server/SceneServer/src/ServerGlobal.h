/*******************************************************************
** 文件名:	ServerGlobal.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "IServerGlobal.h"
#include "IFramework.h"
#include "IServiceContainer.h"
#include "ITimerService.h"
#include "IEventEngine.h"
#include "ISchemeEngine.h"
#include "IServiceMgr.h"
#include "IGatewayAcceptorService.h"
#include "ICenterConnectorService.h"
#include "IVoiceConnectorService.h"
#include "IBridgeConnectorService.h"
#include "IManagerConnectorService.h"
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
#include "IPathFindManager.h"
#include "MessageDispatch.h"
#include "IMatchScene.h"
#include "IWarManager.h"
#include "IGameResService.h"
#include "IMigrateService.h"
#include "IDBEngineService.h"
#include "IShareServer.h"
#include "ITestServer.h"
#include "IAIFactory.h"
#include "IGuardConnectorService.h"
#include "SceneServerControl.h"
#include "ITeamScene.h"
#include "IAntiAddictServer.h"
#include "IStaticWarScene.h"

#include "NetMessageHandler.h"


class ServerGlobal : public IServerGlobal
{
	enum
	{
		STATE_STOPPED,
		STATE_STOPPING,
		STATE_START,
		STATE_RUNNING
	};

public:
	////////////////// IServerGlobal ////////////////////////////////////////////////////////
    virtual void stop();

    // 取得场景服的工作目录
    virtual const char* getWorkDir(void);

    // 是否开启自动运行场景服
    virtual bool isAutoStart(void);

    // 是否开启七龙珠功能
    virtual bool isOpenSupperStone(void);

	/// 获取框架接口
	virtual IFramework *   getFramework();

	/// 获得事件中心接口
	virtual IEventEngine *  getEventEngine();

	/// 获得对时服务接口
	virtual ITimeSyncService *  getTimeSyncService();

	/// 获取时间轴接口
	virtual ITimerService * getTimerService();

	/// 获取配置中心接口
	virtual ISchemeCenter * getSchemeCenter();

	// 取得服务器ID
	virtual unsigned short getServerID();

	/// 获取中心服连接器接口
	virtual ICenterConnectorService * getCenterConnectorService(bool ignoreCache=false);

    // 获取语音服连接器
    virtual IVoiceConnectorService* getVoiceConnectorService(bool ignoreCache=false);

    // 获取数据监护连接器接口
    virtual IGuardConnectorService * getGuardConnectorService(bool ignoreCache=false);

	/// 获取网关连接器接口
	virtual IGatewayAcceptorService * getGatewayAcceptorService(bool ignoreCache=false);

	// 获取桥连接器接口
	virtual IBridgeConnectorService * getBridgeConnectorService(bool ignoreCache=false);

	// 获取管理器连接器接口
	virtual IManagerConnectorService * getManagerConnectorService(bool ignoreCache=false);

	/// 取得配置服务
	virtual ISchemeEngine * getSchemeEngine();

	virtual ISceneManager * getSceneManager();
	
	// 取得条件工厂
	virtual IConditionFactory * getConditionFactory();

	// 取得效果工厂
	virtual IEffectFactory * getEffectFactory();

	// 取得技能工厂
	virtual ISpellFactory * getSpellFactory();

	// 取得冷却服务
	virtual IFreezeServer * getFreezeServer();

	// 取得Buff服务
	virtual IBuffServer *getBuffServer();

	// 取得伤害服务器
	virtual IDamageServer *getDamageServer();

	// 取得Tank服务器
	virtual ITankServer *getTankServer();

	// 取得仇恨服务器
	virtual IEnmityServer *getEnmityServer();

    // 取得测试服务器
    virtual ITestServer *getTestServer();

	// 取得GameResService
	virtual IGameResService* getGameResService();

	virtual IPathFindManager *getPathFindManager();

	// 消息分发器
	virtual IMessageDispatch* getMessageDispatch();

	// 获取网络消息处理接口
	virtual INetMessageHandler*	getNetMessageHandler();

	virtual IMatchScene* getMatchScene();

    virtual IMigrateService* getMigrateService();

	// 取得DBEngine服务
	virtual IDBEngineService*				getDBEngineService(void);

	// 取得数据共享接收器
	virtual IShareReceiver *				getShareReceiver(void);

	// 取得场景服的数据共享源
	virtual IShareSceneSvrSource *			getShareSceneSvrSource(void);

    // 启动共享
    virtual bool						    startShareServer(void);

	virtual IWarManager *                   getWarManager();

	// 设置ServerGlobal的SERVICE_ID
	virtual void                            setServerGlobalService(SERVICE_ID serviceID);
	// 取得ServerGlobal的SERVICE_ID
	virtual SERVICE_ID                      getServerGlobalService();

    // 取得角色管理器
    virtual IActorMgr*                      getActorMgr();

    // 取得帮会服务
    virtual IClanScene*                     getClanScene();

    // 取得战队
    virtual IKinScene*                      getKinScene();

	// 取得聊天服务器
	virtual IChatScene*                     getChatScene();

    virtual IAIFactory*                     getAIFactory();

    virtual ITeamScene*                     getTeamScene();

    virtual IOrderScene*                    getOrderScene();
    
    // 取得服务器控制器
    virtual IServerController*              getServerControllor();

    // 取得日记服务
    virtual IOSSLogService *                getOSSLogService();

	// 取防沉迷服务器
	virtual IAntiAddictServer*				getAntiAddictServer();

    // 取杂项管理对象
    virtual IMiscManager*				    getMiscManager();

    // 获取资源管理接口
    virtual IResourceManager  *             getResourceManager();
	
    // 杯赛服务
	virtual ILegendCupScene*                getLegendCupScene();

	// 取得师徒服务器
	virtual IMentorshipScene*               getMentorshipScene();

    // 战绩服务
    virtual IStaticWarScene*                getStaticWarScene();

	// 取得邮件服务
	virtual IMailScene*						getMailScene();

    virtual HeightFieldMgr*                 getHeightFieldMgr();
public:
	////////////////// ServerGlobal ////////////////////////////////////////////////////////

	ServerGlobal();
	~ServerGlobal();

	/** 初始化启动环境
	@return  
	*/
	bool								InitEnvironment(void);

    bool Start(unsigned short nServerID );



	void OnTimer();

private:
	// 内部初始化接口
	bool Initialize(bool isMulitThreadMode);
	void UnInitialize();

    // 设定Trace相关
    bool SetTrace();
private:
	/// 获得事件中心接口
	IFramework *				m_pFramework;
	ICenterConnectorService *	m_pCenterConnector;
    IVoiceConnectorService *    m_pVoiceConnector;
    IGuardConnectorService *	m_pGuardConnector;
	IGatewayAcceptorService *	m_pGatewayAcceptor;
	IBridgeConnectorService *	m_pBridgeConnector;
	IManagerConnectorService *	m_pManagerConnector;

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
    IMigrateService*            m_pMigrateService;
    IDBEngineService*           m_pDBEngineService;
	IShareReceiver *			m_pShareReceiver;
	IShareSceneSvrSource *		m_pShareSceneSvrSource;
    IActorMgr*                  m_pActorMgr;
    IClanScene*                 m_pClanScene;
    IKinScene*                  m_pKinScene;
	IChatScene*					m_pChatScene;
    IAIFactory*                 m_pAIFactory;
    ITeamScene*                 m_pTeamScene;
    IOrderScene*                m_pOrderScene;
    IOSSLogService *            m_pOssLogService;
	IAntiAddictServer*			m_pAntiAddictServer;
	ILegendCupScene*            m_pLegendCupScene;
	IMentorshipScene*			m_pMentorshipScene;
    IStaticWarScene*			m_pStaticWarScene;
	IMailScene*					m_pMailScene;
    HeightFieldMgr*             m_pHeightFieldMgr;
	rkt::IEventEngine*			m_pEventEngine;
#ifdef SERVICE_HANDLE_MESSAGE
    IMessageDispatch*           m_pMessageDispatch;
#else
	MessageDispatch				m_MessageDispatch;
#endif
	CNetMessageHandler			m_NetMessageHandler;
    IResourceManager  *         m_pResourceManager;
    IMiscManager *              m_pMiscManager;

	unsigned short				m_nServerID;
	SERVICE_ID					m_ServerGlobalServiceID;
	int							m_nState;
    char						m_szWorkDir[MAX_PATH];      // 工作目录
    bool                        m_bAutoStart;               // 是否自动启动
    int                         m_bOpenSupperStone;         // 是否开启七龙珠功能
};