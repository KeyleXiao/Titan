/*******************************************************************
** 文件名:	SocialGlobalService_Proxy.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#pragma once;

#include "ISocialGlobalService.h"
#include "IServiceContainer.h"


class SocialGlobalService_Proxy : public ISocialGlobalService
{
public:
	//////////////////////////////////////////////////////////////////////////
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

	virtual IChatManager*				getChatManager();

	virtual IShareReceiver*				getShareReceiver();
	virtual IShareSceneSvrSource*		getShareSceneSvrSource() ;

	virtual IOrderManager*              getOrderManager();

	virtual IClanManager*				getClanManager() ;

	virtual IKinManager*                getKinManager();

	virtual ITeamManager*               getTeamManager();

    virtual ILegendCupManager*          getLegendCupManager();

	virtual IMentorshipManager*         getMentorshipManager();

	virtual ISNSManager*				getSNSManager();
	
    virtual ILuaManager*				getLuaManager();
	
    // 取资源管理对象
    virtual IResourceManager*			getResourceManager();

    // 取杂项管理对象
    virtual IMiscManager*				getMiscManager();

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
	virtual void                        handleServerInfoUpdated(DWORD ServerID,BYTE nState,void* pInfo, size_t nInfolen);

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
	virtual void						handleGateWayClientDisconnect(ClientID client,DWORD dwReason) ;

    ///////ManagerConnector///////////////////////////////////////////////////////////////////
    // 收到设置游戏ID
    virtual void                        handleManagerSetGameWorldID();

    // 收到管理器下发的资源数据
    virtual void                        handleManagerResourceData(LPVOID pData,DWORD dwDataLen);

    // 收到管理器下发的资源数据结果
    virtual void                        handleManagerResourceResult(LPVOID pData,DWORD dwDataLen);

	SocialGlobalService_Proxy(){}

	SERVICE_PTR    m_pContainer;
};