/*******************************************************************
** 文件名:	SocialGlobalService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#pragma once

#include "ISocialGlobalService.h"
#include "IFramework.h"
#include "IServiceContainer.h"
#include "ITimerService.h"
#include "IEventEngine.h"
#include "ISchemeEngine.h"
#include "IServiceMgr.h"
#include "IGatewayAcceptorService.h"
#include "ICenterConnectorService.h"
#include "ISchemeCenter.h"
#include "IShareServer.h"
#include "IOrderManager.h"
#include "IClanManager.h"
#include "IKinManager.h"
#include "ILegendCupManager.h"
#include "IMentorshipManager.h"
#include "ISNSManager.h"
#include "IMailManager.h"

#include "MessageDispatch.h"
#include "MonitorOnline.h"
#include "UnLockHandler.h"
#include "IOSSLogServer.h"
#include "IDBHttpService.h"


class SocialGlobalService : public ISocialGlobalService, public IEventExecuteSink, public TimerHandler
{
    enum
    {
        ETIMER_CHECK_BASESERVICE,			    // 检测服务器启动时要加载的服务
        ETIMER_CHECK_NEWDAY,                    // 检测新一天
        ETIMER_MANAGER_RETRYSTART,              // 重请求管理器启动服务器
    };

    enum
    {
        CHECK_BASESERVICE_INTERVAL  = 500,       // 检测服务器启动时要加载的服务定时器间隔
        CHECK_NEWDAY_INTERVAL       = 20000,     // 检测新一天定时器间隔
        CHECK_RETRYSTART_INTERVAL   = 1000,       // 重请求管理器启动服务器定时器间隔
    };
public:
	////////////////// ISocialGlobalService ////////////////////////////////////////////////////////
    // 创建
    virtual bool						create();

    // 释放
    virtual void						release();

    // 消息分发器
    virtual IMessageDispatch*           getMessageDispatch();

    // 获取配置中心接口
    virtual ISchemeCenter *				getSchemeCenter();

    // 取得配置服务
    virtual ISchemeEngine *				getSchemeEngine();

	virtual IMatchManager*				getMatchManager();

	virtual IShareReceiver*				getShareReceiver();
	virtual IShareSceneSvrSource*		getShareSceneSvrSource();

	virtual IChatManager*				getChatManager();

    virtual IOrderManager*              getOrderManager();

    virtual IClanManager*               getClanManager();

    virtual IKinManager*                getKinManager();

    virtual ITeamManager*               getTeamManager();

    virtual ILegendCupManager*			getLegendCupManager();


	virtual IMentorshipManager*         getMentorshipManager();

    // 取资源管理对象
    virtual IResourceManager*			getResourceManager();

    // 取杂项管理对象
    virtual IMiscManager*				getMiscManager();
    
	virtual ISNSManager*				getSNSManager();

    virtual ILuaManager*                getLuaManager();

	// 取OSS日志管理对象
	virtual IOSSLogServer*				getOSSLogServer();

    // 取任务数据操作管理对象
    virtual ITaskManager*				getTaskManager();

	// 获取邮件管理对象
	virtual IMailManager*				getMailManager();

    //////////////////////////////////////////////////////////////////////////
    // 请求跨Service处理服务控制，有些数据需要在Service上才能处理
    virtual void                        requestHandleServerControl(DWORD dwState);

    // 请求管理器启动
    virtual bool                        requestManagerStart(DWORD dwStartFlag, DWORD dwUserData);

    // 请求执行解锁操作
    virtual void                        requstExecuteUnlock(void);

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

    /**
    @name           : 处理其他服务器掉线时共享对象回调
    @param wSrvType : 服务器类型
    @param pData    : 消息内容
    @param nDataSize:消息长度
    */
    virtual void                        handleServerDisconnectShareCallback(WORD wSrvType, char* pData,  size_t nDataSize);    

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

	////////////////// SocialGlobalService ////////////////////////////////////////////////////////

	// 构造函数
	SocialGlobalService();

	// 析构函数
	virtual ~SocialGlobalService();

	// 启动
	// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
	bool on_start(void * data,int len );

	// 停止
	bool on_stop();

private:
    // 获取当前天
    int getDayOfMonth(void);

    //// 启动中心服务
    //void startCenterService();
    // 启动网关服务
    void startGateWayService();
    // 启动桥服务
    void startBridgeService();
	// 启动SNS连接器服务
	void startSNSService();


	// 后续启动
	void startEntries(void);

	// 创建共享
	bool createShareService(void);


    // 定时检测基础服务是否已启好
    void onTimerCheckBaseService(void);

    // 定时检测是否新一天
    void onTimerCheckNewDay(void);

private:
    CMessageDispatch			m_MessageDispatch;
    CMonitorOnline              m_MonitorOnline;
    CUnLockHandler              m_UnLockHandler;

	ISchemeCenter *				m_pSchemeCenter;
	ISchemeEngine*				m_pSchemeEngine;	    // 配置服务
	IMatchManager*				m_pMatchManager;
	IShareReceiver *			m_pShareReceiver;
	IShareSceneSvrSource *		m_pShareSceneSvrSource;
	IChatManager*				m_pChatManager;
    IOrderManager*              m_pOrderManager;
    IClanManager*               m_pClanManager;
    IKinManager*                m_pKinManager;
    ITeamManager*               m_pTeamManager;
    ILegendCupManager*          m_pLegendCupManager;
	IMentorshipManager*         m_pMentorshipManager;
    ISNSManager*				m_pSNSManager;
    ILuaManager*				m_pLuaManager;
    IResourceManager*           m_pResourceManager;
    IMiscManager*               m_pMiscManager;
	IOSSLogServer*				m_pOSSLogServer;
    ITaskManager*               m_pTaskManager;
	IMailManager*				m_pMailManager;
    IDBHttpService*             m_DBHttpService;

    int							m_nCheckDay;        // 上次定时自动处理的时间日期数
    DWORD                       m_dwRetryStartFlag;
    DWORD                       m_dwRetryUserData;
};