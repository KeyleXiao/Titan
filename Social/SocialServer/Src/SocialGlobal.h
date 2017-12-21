/*******************************************************************
** 文件名:	SocialGlobal.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	
            社会服全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "ISocialGlobal.h"
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
#include "ISNSConnectorService.h"
#include "ISchemeCenter.h"
#include "MessageDispatch.h"
#include "NetMessageHandler.h"
#include "IMatchManager.h"
#include "GatewayServiceDef.h"
#include "IDBEngineService.h"
#include "IShareServer.h"


class SocialGlobal : public ISocialGlobal
{
public:
	////////////////// ISocialGlobal ////////////////////////////////////////////////////////
    /// 获取框架接口
    virtual IFramework*							getFramework();

    /// 获得事件中心接口
    virtual IEventEngine*						getEventEngine();

    /// 获得对时服务接口
    virtual ITimeSyncService*					getTimeSyncService();

    /// 获取时间轴接口
    virtual ITimerService*						getTimerService();

    // 获取日期触发器
    virtual IDateTriggerService*                getDateTriggerService();

    /// 获取配置中心接口
    virtual ISchemeCenter*						getSchemeCenter();

    /// 获取服务器ID
    virtual unsigned short						getServerID();

    /// 获取中心服连接器接口
    virtual ICenterConnectorService*			getCenterConnectorService();

    /// 获取语音服连接器接口
    virtual IVoiceConnectorService*             getVoiceConnectorService();

    /// 获取网关连接器接口
    virtual IGatewayAcceptorService*			getGatewayAcceptorService();

	// 获取桥连接器接口
	virtual IBridgeConnectorService *			getBridgeConnectorService();

	// 获取管理器连接器接口
	virtual IManagerConnectorService *			getManagerConnectorService();

	// 获取SNS连接器接口
	virtual ISNSConnectorService *				getSNSConnectorService();

    /// 取得配置服务
    virtual ISchemeEngine*						getSchemeEngine();

    // 消息分发器
    virtual IMessageDispatch*					getMessageDispatch();

	// 获取网络消息处理接口
	virtual INetMessageHandler*					getNetMessageHandler();

    // 获取资源管理接口
    virtual IResourceManager  *                 getResourceManager();

	virtual IMatchManager*						getMatchManager();

    virtual IClanManager*						getClanManager();

    virtual IKinManager*						getKinManager();
    
	virtual IChatManager*						getChatManager();

    virtual IOrderManager*                      getOrderManager();

	// 取得DBEngine服务
	virtual IDBEngineService*					getDBEngineService(void);

	// 取得数据共享接收器
	virtual IShareReceiver *					getShareReceiver(void);

	// 取得场景服的数据共享源
	virtual IShareSceneSvrSource *				getShareSceneSvrSource(void);

	// 设置共享对象
	virtual void								setShareObject(IShareReceiver * pShareReceiver, IShareSceneSvrSource *pShareSceneSvrSource);

	// 启动共享
	virtual bool								startShareServer(void);
	////////////////////////////////////////////////////////////////////////////
    // 设置SocialGlobal的SERVICE_ID
    virtual void								setSocialGlobalService( SERVICE_ID serviceID );

    // 取得SocialGlobal的SERVICE_ID
    virtual SERVICE_ID							getSocialGlobalService();

    // 取得服务器控制器
    virtual IServerController*                  getServerControllor();
	
	// 取得组队管服务
    virtual ITeamManager *	                    getTeamManager(void);


    // 取得组队管理
    virtual ILegendCupManager*			        getLegendCupManager();

	virtual IMentorshipManager*					getMentorshipManager();

	// 获取SNS服务
	virtual ISNSManager*						getSNSManager();

    // 获取Lua服务
    virtual ILuaManager *				        getLuaManager();
	
    // 取杂项管理对象
    virtual IMiscManager*				        getMiscManager();

	// 获取日志管理
	virtual IOSSLogService *					getOSSLogService();

    // 取任务数据操作管理对象
    virtual ITaskManager*				        getTaskManager();

	// 获取邮件服务
	virtual IMailManager*						getMailManager();

public:
	////////////////// SocialGlobal ////////////////////////////////////////////////////////
	SocialGlobal();

	~SocialGlobal();

	/** 初始化启动环境
	@return  
	*/
	bool								        InitEnvironment(void);

    bool Start( unsigned short nServerID );
    void Stop();

    void OnTimer();

private:
    bool Initialize();
    void UnInitialize();
	// 创建IEventEngine对象
	void createEventEngine(void);

    // 设定Trace相关
    bool setTrace();

private:
    

#ifdef SERVICE_HANDLE_MESSAGE
    IMessageDispatch*           m_pMessageDispatch;
#else
    CMessageDispatch			m_MessageDispatch;
#endif
	CNetMessageHandler			m_NetMessageHandler;

	/// 获得事件中心接口
	IFramework *			    m_pFramework;
	ICenterConnectorService *	m_pCenterConnector;
    IVoiceConnectorService *    m_pVoiceConnector;
	IGatewayAcceptorService *	m_pGatewayAcceptor;
	IBridgeConnectorService *	m_pBridgeConnector;
	IManagerConnectorService *	m_pManagerConnector;
	ISNSConnectorService*		m_pSNSConnector;
	ISceneManager *				m_pSceneManager;
	ISchemeCenter *				m_pSchemeCenter;
	ISchemeEngine*				m_pSchemeEngine;
	rkt::IEventEngine*			m_pEventEngine;
    IResourceManager  *         m_pResourceManager;
    IMiscManager *              m_pMiscManager;

	IMatchManager*				m_pMatchManager;
    IDBEngineService*           m_pDBEngineService;

	IShareReceiver *			m_pShareReceiver;
	IShareSceneSvrSource *		m_pShareSceneSvrSource;
    IClanManager*				m_pClanManager;
	IKinManager*				m_pKinManager;
	IChatManager*				m_pChatManager;
    IOrderManager*				m_pOrderManager;

    unsigned short				m_nServerID;
    SERVICE_ID					m_SocialGlobalServiceID;
    char						m_szWorkDir[MAX_PATH];      // 工作目录
	
	ITeamManager*				m_pTeamManager;   
    ILegendCupManager*			m_pLegendCupManager; 
	IMentorshipManager*			m_pMentorshipManager;
	ISNSManager*				m_pSNSManager;
    ILuaManager*				m_pLuaManager;
	IOSSLogService*				m_pOSSLogService;
    ITaskManager*               m_pTaskManager;
	IMailManager*				m_pMailManager;
};
