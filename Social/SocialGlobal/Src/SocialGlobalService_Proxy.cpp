/*******************************************************************
** 文件名:	SocialGlobalService_Proxy.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	
            游戏全局对象(主要用来方便的索引其他组件的指针)
********************************************************************/

#include "stdafx.h"
#include "SocialGlobalService_Proxy.h"


//////////////////////////////////////////////////////////////////////////
// 创建
bool SocialGlobalService_Proxy::create()
{

	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::create );
	
	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(bool);
	
	return false;
}

void SocialGlobalService_Proxy::release()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::release );
	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 消息分发器
IMessageDispatch* SocialGlobalService_Proxy::getMessageDispatch()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getMessageDispatch );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IMessageDispatch*);
	
	return 0;
}

/// 获取配置中心接口
ISchemeCenter * SocialGlobalService_Proxy::getSchemeCenter()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getSchemeCenter );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(ISchemeCenter*);
	
	return 0;
}

/// 取得配置服务
ISchemeEngine * SocialGlobalService_Proxy::getSchemeEngine()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getSchemeEngine );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(ISchemeEngine*);
	
	return 0;
}

IMatchManager* SocialGlobalService_Proxy::getMatchManager()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getMatchManager );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IMatchManager*);
	
	return 0;
}

IChatManager* SocialGlobalService_Proxy::getChatManager()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getChatManager );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IChatManager*);
	
	return 0;
}

IShareReceiver*	SocialGlobalService_Proxy::getShareReceiver()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getShareReceiver );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IShareReceiver*);
	
	return 0;
}

IShareSceneSvrSource* SocialGlobalService_Proxy::getShareSceneSvrSource()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getShareSceneSvrSource );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IShareSceneSvrSource*);
	
	return 0;
}

IOrderManager* SocialGlobalService_Proxy::getOrderManager()
{
 	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getOrderManager );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IOrderManager*);
	
	return 0;
}

IKinManager* SocialGlobalService_Proxy::getKinManager()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getKinManager );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IKinManager*);

	return 0;
}

IClanManager* SocialGlobalService_Proxy::getClanManager()
{
 	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getClanManager );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IClanManager*);
	
	return 0;
}

ITeamManager* SocialGlobalService_Proxy::getTeamManager()
{
 	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getTeamManager );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(ITeamManager*);
	
	return 0;
}


IMentorshipManager* SocialGlobalService_Proxy::getMentorshipManager()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getMentorshipManager );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(IMentorshipManager*);

	return 0;
}

ISNSManager* SocialGlobalService_Proxy::getSNSManager()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getSNSManager );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(ISNSManager*);

	return 0;
}

ILuaManager* SocialGlobalService_Proxy::getLuaManager()
{
    BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getLuaManager );

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ILuaManager*);

    return 0;
}

// 取资源管理对象
IResourceManager* SocialGlobalService_Proxy::getResourceManager()
{
    BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getResourceManager );

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IResourceManager*);

    return 0;
}

// 取杂项管理对象
IMiscManager* SocialGlobalService_Proxy::getMiscManager()
{
    BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getMiscManager );

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(IMiscManager*);

    return 0;
}

// 取OSS日志管理对象
IOSSLogServer* SocialGlobalService_Proxy::getOSSLogServer()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getOSSLogServer);
	
	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
	RETURN_FROM_MSG_BUF(IOSSLogServer*);

	return 0;
}


// 取任务数据操作管理对象
ITaskManager* SocialGlobalService_Proxy::getTaskManager()
{
    BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getTaskManager);

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
    RETURN_FROM_MSG_BUF(ITaskManager*);

    return 0;
}

// 获取邮件管理对象
IMailManager* SocialGlobalService_Proxy::getMailManager()
{
	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getMailManager);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
	RETURN_FROM_MSG_BUF(IMailManager*);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 请求跨Service处理服务控制，有些数据需要在Service上才能处理
void SocialGlobalService_Proxy::requestHandleServerControl(DWORD dwState)
{
 	BUILD_MSG_CONTEXT_1( ISocialGlobalService::requestHandleServerControl,DWORD,dwState );

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 请求管理器启动
bool SocialGlobalService_Proxy::requestManagerStart(DWORD dwStartFlag, DWORD dwUserData)
{
    BUILD_MSG_CONTEXT_2( ISocialGlobalService::requestManagerStart,DWORD,dwStartFlag, DWORD, dwUserData);
   
    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(bool);

    return false;
}

// 请求执行解锁操作
void  SocialGlobalService_Proxy::requstExecuteUnlock(void)
{
    BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::requstExecuteUnlock );

    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void  SocialGlobalService_Proxy::handleServerListUpdated()
{
 	BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::handleServerListUpdated );

	m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/**
@name         : 通知服务器信息更新
@brief        : 例如服务器人数变化等等
*/
void SocialGlobalService_Proxy::handleServerInfoUpdated(DWORD ServerID, BYTE nState, void* pInfo, size_t nInfolen)
{
    obuf256 t_data;
    t_data <<ServerID<<nState<<nInfolen;
    t_data.push_back(pInfo,nInfolen);
    
	BUILD_MSG_BUFFER( ISocialGlobalService::handleServerInfoUpdated,t_data );

    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

/**
@name         : 处理其他服务器通过中心服务器转发的消息
@brief        : 具体内容不清楚
@param server : 该消息是哪个服务器发过来的
@param pData  : 消息内容
@param wDataLen:消息长度
*/
void SocialGlobalService_Proxy::handlePostMessage(DWORD server,const char * pData,WORD wDataLen)
{
    obuf256 t_data;
    t_data <<server<<wDataLen;
    t_data.push_back(pData,wDataLen);

	BUILD_MSG_BUFFER( ISocialGlobalService::handlePostMessage,t_data );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}


/**
@name           : 处理其他服务器掉线时共享对象回调
@param wSrvType : 服务器类型
@param pData    : 消息内容
@param nDataSize:消息长度
*/
void SocialGlobalService_Proxy::handleServerDisconnectShareCallback(WORD wSrvType, char* pData, size_t nDataSize)
{
    obuf2048 t_data;
    t_data <<wSrvType<<nDataSize;
    t_data.push_back(pData,nDataSize);

    BUILD_MSG_BUFFER( ISocialGlobalService::handleServerDisconnectShareCallback,t_data );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
// 网关服务器通知游戏服务器有一个新的客户端连进来
void SocialGlobalService_Proxy::handleClientIncoming(ClientID client)
{
	BUILD_MSG_CONTEXT_1( ISocialGlobalService::handleClientIncoming,ClientID,client );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 网关服务器通知游戏服务器有新的数据包到达
void SocialGlobalService_Proxy::handleGateWayReceivedData(ClientID client, void* data, size_t len)
{
    obuf256 t_data;
    t_data <<client<<len;
    t_data.push_back(data,len);

	BUILD_MSG_BUFFER( ISocialGlobalService::handleGateWayReceivedData,t_data );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 网关服务器通知游戏服务器有连接断开
void SocialGlobalService_Proxy::handleGateWayClientDisconnect(ClientID client,DWORD dwReason)
{
	BUILD_MSG_CONTEXT_2( ISocialGlobalService::handleGateWayClientDisconnect,ClientID ,client,DWORD ,dwReason );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

///////ManagerConnector///////////////////////////////////////////////////////////////////
// 收到设置游戏ID
void SocialGlobalService_Proxy::handleManagerSetGameWorldID()
{
    BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::handleManagerSetGameWorldID );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

// 收到管理器下发的资源数据
void SocialGlobalService_Proxy::handleManagerResourceData(LPVOID pData,DWORD dwDataLen)
{
    if(dwDataLen == 0)
        return;

    obuf256 t_data;
    t_data <<dwDataLen;
    t_data.push_back(pData,dwDataLen);

    BUILD_MSG_BUFFER( ISocialGlobalService::handleManagerResourceData,t_data );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}


// 收到管理器下发的资源数据结果
void  SocialGlobalService_Proxy::handleManagerResourceResult(LPVOID pData,DWORD dwDataLen)
{
    if(dwDataLen == 0)
        return;

    obuf256 t_data;
    t_data <<dwDataLen;
    t_data.push_back(pData,dwDataLen);

    BUILD_MSG_BUFFER( ISocialGlobalService::handleManagerResourceResult,t_data );
    m_pContainer->post_message(pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

//////////////////////////////////////////////////////////////////////////
ILegendCupManager* SocialGlobalService_Proxy::getLegendCupManager()
{
    BUILD_MSG_CONTEXT_VOID( ISocialGlobalService::getLegendCupManager );

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(ILegendCupManager*);

    return 0;
}


