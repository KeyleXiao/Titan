/*******************************************************************
** 文件名:	AntiAddictServer.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/8/2016
** 版  本:	1.0
** 描  述:	防沉迷服务

********************************************************************/

#include "stdafx.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "IServiceContainer.h"
#include "ITimerService.h"
#include "ICenterConnectorService.h"
#include "IActorService.h"
#include "EventDef.h"
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include "AntiAddictServer.h"
#include "Pupil.h"

#ifndef MAKE_ANTI_MESSAGE_KEY
#define MAKE_ANTI_MESSAGE_KEY(point, action)    ( ((point) << 16) | (action) )
#endif

CAntiAddictServer* g_pAntiAddictServer = NULL;


/////////////////////////////////////// CAntiAddictServer ///////////////////////////////////
/** 构造函数
*/
CAntiAddictServer::CAntiAddictServer(void)
    : m_nReduceLevel(0)//(100) // 玩家防沉迷等级
{

}

/** 析构函数
*/
CAntiAddictServer::~CAntiAddictServer(void)
{
}

/** 创建
*/
bool CAntiAddictServer::create(void)
{
    IMessageDispatch* pMsgDispatch = gServerGlobal->getMessageDispatch();
    IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
    if(pMsgDispatch == NULL || pEventEngine == NULL)
    {
        return false;
    }

    // 注册 MSG_MODULEID_ANTIADDICT 服务器转发模块消息
    pMsgDispatch->registerTransmitHandler(MSG_MODULEID_ANTIADDICT, (ITransmitHandler *)this);
    // 注册 MSG_MODULEID_ANTIADDICT 客户端发送过来的消息
    pMsgDispatch->registerMessageHandler(MSG_MODULEID_ANTIADDICT, (IMessageHandler*)this);


    // EVENT_GAME_ACTOR_LOGIN
    pEventEngine->Subscibe(CAST_TYPE(IEventExecuteSink* ,this), EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0, __FUNCTION__);
    // EVENT_GAME_ACTOR_LOGOUT
    //pEventEngine->Subscibe(CAST_TYPE(IEventExecuteSink* ,this), EVENT_GAME_ACTOR_LOGOUT, SOURCE_TYPE_PERSON, 0, __FUNCTION__);

    // 数据库接口注册
    gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

    return true;
}

/** 释放服务
*/
void CAntiAddictServer::onstop(void)
{
	IMessageDispatch* pMsgDispatch = gServerGlobal->getMessageDispatch();
	if(pMsgDispatch)
	{
		// 注册 MSG_MODULEID_ANTIADDICT 服务器转发模块消息
		pMsgDispatch->unregisterTransmitHandler(MSG_MODULEID_ANTIADDICT);
		// 注册 MSG_MODULEID_ANTIADDICT 客户端发送过来的消息
		pMsgDispatch->unregisterMessageHandler(MSG_MODULEID_ANTIADDICT);
	}

	IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
	if(pEventEngine)
	{
		// EVENT_PERSON_LOGIN
		pEventEngine->UnSubscibe(CAST_TYPE(IEventExecuteSink* ,this), EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0);
		// EVENT_PERSON_LOGOUT
		//pEventEngine->UnSubscibe(CAST_TYPE(IEventExecuteSink* ,this), EVENT_GAME_ACTOR_LOGOUT, SOURCE_TYPE_PERSON, 0);
	}

	// 数据库接口反注册
	gServerGlobal->getDBEngineService()->unregisterHandler(this);
}

/** 释放对象
*/
void CAntiAddictServer::release(void)
{
    for (speed::hashmap<IDCARD, CPupil*>::iterator iter = m_mapPupilIdCard.begin(); iter != m_mapPupilIdCard.end(); ++iter)
    {
         safeRelease(iter->second);
    }
    m_mapPupilIdCard.clear();

    delete this;
}


/** 取得玩家的衰减比例
@param uid: 角色uid
*/
int CAntiAddictServer::getReduceRate(UID uid, int nPlayLevel/*=0*/)
{
    if (uid == INVALID_UID)
    {
        return 100;
    }

    ActorHelper helper(uid, ACTOR_UID);
    IActorService * pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        return 100;
    }
    // 可控制要不要让玩家沉迷
    int nLevel = nPlayLevel;
    if(nLevel == 0)
    {
        nLevel = pActorService->getPlayerLevel();
        if (nLevel < m_nReduceLevel)
        {
            return 100;
        }
    }

    // 成年人不衰减，并清除沉迷列表中玩家信息
    bool isAdult = pActorService->isAdult();
    if(isAdult)
    {
        return 100;
    }

    DWORD dwIdCard = pActorService->getIdCard();
    CPupil* pPupil = getPupil(dwIdCard);
    return ( pPupil ? pPupil->getReduceRate()  : 100);
}

/** 设置防沉迷等级
*/
void CAntiAddictServer::setReduceLevel(int nReduceLevel)
{
    m_nReduceLevel = nReduceLevel;
}

/** 查看防沉迷等级
*/
int CAntiAddictServer::getReduceLevel()
{
    return m_nReduceLevel;
}

/////////////////////////////// ITransmitHandler ////////////////////////////////////
/** 消息处理
@param server	源服务器ID
@param uMsgID	消息ID
@param head		消息头
@param data		消息
@param len		消息长度
*/
void CAntiAddictServer::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    switch ( MAKE_ANTI_MESSAGE_KEY(head->bySrcEndPoint, head->byKeyAction) )
    {
    case MAKE_ANTI_MESSAGE_KEY(MSG_ENDPOINT_WORLD, MSG_ADDICT_REFRESH):     // 世界服返回防沉迷信息给场景服
        {
            onTransmitUpdateAddictInfo(data, len);
        }
        break;
	case MAKE_ANTI_MESSAGE_KEY(MSG_ENDPOINT_WORLD, MSG_ADDICT_BATCH_REFRESH):// 世界服批量返回防沉迷信息给场景服
		{
			onTransmitBatchUpdateAddictInfo(data, len);
		}
		break;

    case MAKE_ANTI_MESSAGE_KEY(MSG_ENDPOINT_WORLD, MSG_REMOVEADDICT_W2Z):   // 世界服务器通知其他场景服务器，解除防沉迷
        {
            onTransmitRemovePupil(data, len);
        }
        break;
	case MAKE_ANTI_MESSAGE_KEY(MSG_ENDPOINT_WORLD, MSG_UPDATE_IDCARD_W2Z):	// 世界服通知场景服，更新身份证
		{
			onTransmitUpdateIdCard(data, len);
		}
		break;
	case MAKE_ANTI_MESSAGE_KEY(MSG_ENDPOINT_WORLD, MSG_UPDATE_REALNAME_W2Z): // 世界服通知场景服，更新实名认证信息
		{
			onTransmitUpdateRealNameAuther(data, len);
		}
		break;

    default:
        break;
    }
}


/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void CAntiAddictServer::onServerListUpdated()
{

}

/** 通知服务器信息更新
@param nState		: SERVER_STATE
@param pServerData	: ServerData
@brief        : 例如服务器人数变化等等
*/
void CAntiAddictServer::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

/** 刷新玩家反沉迷信息
*/
void CAntiAddictServer::onTransmitUpdateAddictInfo(void * data, size_t len)
{
    if (data==nullptr || len != sizeof(SMsgAddictRefresh_WS))
    {
        ErrorLn(__FUNCTION__": message length is invalid! len="<< sizeof(SMsgAddictRefresh_WS) <<", nRealLen="<< len);
        return;
    }
    SMsgAddictRefresh_WS* pRefreshInfo = (SMsgAddictRefresh_WS*)data;

    SUserAddictInfo* pAddictInfo = &pRefreshInfo->info;
    if (pAddictInfo == nullptr || pAddictInfo->dwIdCard == 0)
    {
        return;
    }

	// 上线
	if(pRefreshInfo->bLoginInfo)
	{
		// 增加玩家未成年信息
		addPupilCardInfo(pAddictInfo->dwIdCard, pAddictInfo->nPDBID, INVALID_UID);

		ADDICT_TRACELN(__FUNCTION__":userid=" << pAddictInfo->dwUserID << ", pdbid="<< pAddictInfo->nPDBID << ", dwIdCard=" << pAddictInfo->dwIdCard << " is pupil.");

		CPupil * pPupil = getPupil(pAddictInfo->dwIdCard);
		if(pPupil == nullptr)
		{
			return;
		}

		// 更新沉迷数据
		pPupil->update(pAddictInfo);

		// 当前角色要被踢
		if(pRefreshInfo->bKickout)
		{
			pPupil->addKickActor(pAddictInfo->nPDBID);
		}
	}
	// 下线
	else
	{
		CPupil * pPupil = getPupil(pAddictInfo->dwIdCard);
		if(pPupil == nullptr)
		{
			return;
		}

		// 下线时从踢人队列中删除当前用户
		pPupil->removeKickActor(pAddictInfo->nPDBID);

		pPupil->removeActor(pAddictInfo->nPDBID, INVALID_UID);
		if(pPupil->empty())
		{
			SAFE_RELEASE(pPupil);
			removeContainerIDCard(pAddictInfo->dwIdCard);      
		}

		ADDICT_TRACELN(__FUNCTION__":userid=" << pAddictInfo->dwUserID << ", pdbid=" << pAddictInfo->nPDBID <<", dwIdCard=" << pAddictInfo->dwIdCard);
	}
}

/** 批量刷新玩家反沉迷信息(新加入场景服被同步全部的防沉迷信息)
*/
void CAntiAddictServer::onTransmitBatchUpdateAddictInfo(void * data, size_t len)
{
	if (data == nullptr || len < sizeof(int))
	{
		return;
	}

	int *pCount = (int *)data;
	int nCount = *pCount;

	for (int i = 0; i < nCount; ++i)
	{
		int nOffset = sizeof(int) + i * sizeof(SMsgAddictRefresh_WS);

		SMsgAddictRefresh_WS* pRefreshInfo = (SMsgAddictRefresh_WS*)((char*)data + nOffset);
		SUserAddictInfo* pAddictInfo = &pRefreshInfo->info;
		if (pAddictInfo == nullptr || pAddictInfo->dwIdCard == 0 || !pRefreshInfo->bLoginInfo)
		{
			return;
		}

		// 增加玩家未成年信息
		addPupilCardInfo(pAddictInfo->dwIdCard, pAddictInfo->nPDBID, INVALID_UID);

		ADDICT_TRACELN(__FUNCTION__":userid=" << pAddictInfo->dwUserID << ", pdbid="<< pAddictInfo->nPDBID << ", dwIdCard=" << pAddictInfo->dwIdCard << " is pupil.");

		CPupil * pPupil = getPupil(pAddictInfo->dwIdCard);
		if(pPupil == nullptr)
		{
			return;
		}

		// 更新沉迷数据
		pPupil->update(pAddictInfo);

		// 当前角色要被踢
		if(pRefreshInfo->bKickout)
		{
			pPupil->addKickActor(pAddictInfo->nPDBID);
		}
	}
}

/** 移除玩家反沉迷信息
*/
void CAntiAddictServer::onTransmitRemovePupil(void * data, size_t len)
{
    if (data==nullptr || len != sizeof(SMsgRemoveAddict_W2Z))
    {
        ErrorLn(__FUNCTION__": message length is invalid! len="<< sizeof(SMsgRemoveAddict_W2Z) <<", nRealLen="<< len);
        return;
    }
    SMsgRemoveAddict_W2Z* pMsg = (SMsgRemoveAddict_W2Z*)data;

    DWORD dwIdCard = pMsg->dwIdCard;
    removePupilCardInfo(dwIdCard);
}

/** 更新玩家身份证信息
*/
void CAntiAddictServer::onTransmitUpdateIdCard(void * data, size_t len)
{
	if (data == nullptr || len != sizeof(SMsgUpdateIdCard_W2Z))
	{
		ErrorLn(__FUNCTION__": receive message is invalid, cur len = " << len << " correct len = " << sizeof(SMsgUpdateIdCard_W2Z));
		return;
	}

	SMsgUpdateIdCard_W2Z * pMsg = (SMsgUpdateIdCard_W2Z *)data;
	updatePupilCardInfo(pMsg->dwOldIdCard, pMsg->dwNewIdCard);
}

/** 更新玩家实名认证状态
*/
void CAntiAddictServer::onTransmitUpdateRealNameAuther(void * data, size_t len)
{
	if (data == nullptr || len != sizeof(SMsgUpdateRealNameAuther_W2Z))
	{
		ErrorLn(__FUNCTION__": receive message is invalid, cur len = " << len << " correct len = " << sizeof(SMsgUpdateRealNameAuther_W2Z));
		return;
	}

	SMsgUpdateRealNameAuther_W2Z * pMsg = (SMsgUpdateRealNameAuther_W2Z *)data;

	CPupil * pPupil = getPupil(pMsg->dwIdCard);
	if (pPupil == nullptr)
	{
		return;
	}

	// 更新在线账号下的实名认证信息
	pPupil->setRealNameAuther();
}

/////////////////////////////// IMessageHandler ////////////////////////////////////
/** 消息处理
@param msg 网络消息
*/
void CAntiAddictServer::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{
    switch ( MAKE_ANTI_MESSAGE_KEY(head->bySrcEndPoint, head->byKeyAction) )
    {
    case MAKE_ANTI_MESSAGE_KEY(MSG_ENDPOINT_CLIENT, MSG_ADDICT_AUTH):     // 游戏世界实名认证
        {
            onMessageAuthPupil(clientID, head, data, len);
        }        
        break;

    default:
        break;
    }
}

/** 游戏中玩家身份证认证处理
*/
void CAntiAddictServer::onMessageAuthPupil(ClientID clientID, SNetMsgHead* head, void* data, size_t len)
{
    if (data == NULL || len != sizeof(SMsgAddictAuth_CS))
    {
        return;
    }
    SMsgAddictAuth_CS* pMsg = (SMsgAddictAuth_CS*)data;

    IActorMgr* pActorMgr = gServerGlobal->getActorMgr();
    //assert(pActorMgr != nullptr);
	if (pActorMgr == nullptr)
		return;

    SERVICE_PTR pContainer = pActorMgr->getActorByClientID(clientID);
    if ( pContainer==NULL )
        return;

    IActorService * pActor = (IActorService*)pContainer->query_interface();
    if ( pActor==NULL )
        return;   
    
    IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return;
    }

    // TODO 取得老的IdCard
    //DWORD dwOldIdCard = pActor->getIdCard();
	
    DBREQ_PARAM_WRITE_AUTHENT refer;
    refer.nUserID = pActor->getUserID();
    sstrcpyn(refer.szRealName, pMsg->entityname, sizeof(refer.szRealName));
    sstrcpyn(refer.szIDCardNo, pMsg->card, sizeof(refer.szIDCardNo));

    refer.nCreateIP = ntohl(inet_addr(getIpAddress(clientID).c_str()));

    if (pDBEngine->executeSP(GAMEDB_REQUEST_WRITE_AUTHENT, clientID, (LPCSTR)&refer, sizeof(refer), this, 0) == false)
    {
        ErrorLn(__FUNCTION__": execute GAMEDB_REQUEST_WRITE_AUTHENT failed! userID="<<refer.nUserID << ",card="<< refer.szIDCardNo <<",name="<< refer.szRealName <<",ip="<<refer.nCreateIP);
    }
}

/// purpose: 取得某个人的ID
string CAntiAddictServer::getIpAddress(ClientID clientID)
{
    IGatewayAcceptorService *pGatewayAcceptorService = gServerGlobal->getGatewayAcceptorService();
    if (pGatewayAcceptorService == NULL)
    {
        return "";
    }
    return pGatewayAcceptorService->getIpAddress(clientID);
}
/////////////////////////////// IEventExecuteSink ////////////////////////////////////
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
void CAntiAddictServer::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch( MAKE_EVENT_KEY(bSrcType, wEventID) )
    {
    case MAKE_EVENT_KEY(SOURCE_TYPE_PERSON, EVENT_GAME_ACTOR_LOGIN) :       // 角色登进场景服 
        {
            onEventLogin(dwSrcID, pszContext, nLen);
        }
        break;

    default:
        break;
    }
}

/** 玩家登陆事件处理
*/
void CAntiAddictServer::onEventLogin(DWORD dwSrcID, LPCSTR pData, int nLen)
{
    if(pData == NULL || nLen != sizeof(event_actor_login))
    {
        return;
    }

    event_actor_login * pLoginEvent = CAST_TYPE(event_actor_login *, pData);

    UID & uid         = pLoginEvent->uid;
    DWORD & dwPDBID   = pLoginEvent->dwDBID;

	// 角色登陆控制
	if (pLoginEvent->eMode != elogin_online)
	{
		return;
	}

    ActorHelper helper(uid, ACTOR_UID);
    IActorService * pActorService = helper.m_ptr;
    if (pActorService == NULL)
    {
        return;
    }
    bool isAdult = pActorService->isAdult();
    // 成人就不再往下执行
    if (isAdult)
    {
        return;
    }

    // 未成年处理
    SMsgAddictLogin_SW login;
    login.dwPDBID	= dwPDBID;                      // 角色ID
    login.dwUserID	= pActorService->getUserID();   // 账号ID
    login.dwIdCard	= pActorService->getIdCard();   // 身份证ID

    // 往世界服发送玩家上线消息
    sendToWorldSvr(MSG_ADDICT_LOGIN, (char*)&login, sizeof(login));

    //// 增加玩家未成年信息
    //addPupilCardInfo(login.dwIdCard, dwPDBID, uid);

    //ADDICT_TRACELN(__FUNCTION__":userid="<<login.dwUserID<<", pdbid="<< login.nPDBID <<", dwIdCard="<<login.dwIdCard<<" is pupil.");
}


/** 玩家下线事件处理
*/
void CAntiAddictServer::onActorLogout(PDBID pdbid, UID uid, DWORD dwUserID, bool bAudlt, DWORD nIdCard)
{
    // 成人就不再往下执行
    if(bAudlt) {
        return;
    }

    // 未成年处理
    IDCARD dwIdCard	= nIdCard;		// 身份证
    DWORD dwPDBID	= pdbid;		// 玩家PDBID
    CPupil * pPupil = getPupil(dwIdCard);
    // 没有未成年对象
    if(pPupil == nullptr)
    {
        return;
    }
    
    SMsgAddictLogout_SW logout;
	logout.dwPDBID	= dwPDBID;
    logout.dwUserID = dwUserID;
    logout.dwIdCard = dwIdCard;

    // 往世界服发送玩家下线消息
    sendToWorldSvr(MSG_ADDICT_LOGOUT, (char*)&logout, sizeof(logout));

 //   // 下线时从踢人队列中删除当前用户
 //   pPupil->removeKickActor(dwPDBID);

	//pPupil->removeActor(dwPDBID, uid);
	//if(pPupil->empty())
	//{
	//	SAFE_RELEASE(pPupil);
 //       removeContainerIDCard(dwIdCard);      
	//}
 //   
 //   ADDICT_TRACELN(__FUNCTION__":userid="<<logout.dwUserID<<", pdbid="<< dwPDBID <<", dwIdCard="<<dwIdCard);
}


/////////////////////////////// IDBRetSink ////////////////////////////////////
/** 数据库请求返回回调方法
@param   pDBRetSink		：结果返回接口	
@param   nCmdID ：命令ＩＤ
@param   nDBRetCode：数据库请求返回值，参考上面定义
@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
@param   nQueueIndex：队列定义
@param   pOutData：输出数据
@param   nOutLen：输出数据长度
@return  
@note     
@warning 此对像千万不能在数据库返回前释放，否则会非法！
@retval buffer 
*/	
void CAntiAddictServer::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    switch(nCmdID)
    {
    case GAMEDB_REQUEST_WRITE_AUTHENT:
        {
            onReturnActorAuth(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;

    default:
        break;
    }
}

/** 玩家身份认证数据库回调
*/
void  CAntiAddictServer::onReturnActorAuth(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    DWORD clientID = nQueueIndex;

	// 未验证成功
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		sendToClientAuthResult(clientID, nDBRetCode, pszDBRetDesc);
		return;
	}

	if (pOutData == NULL || nOutLen != sizeof(DBREQ_RESULT_WRITE_AUTHENT))
	{
		ErrorLn(__FUNCTION__": DB return message is invalid, length = " << nOutLen);
		return;
	}
	DBREQ_RESULT_WRITE_AUTHENT * pResult = (DBREQ_RESULT_WRITE_AUTHENT * )pOutData;

	IActorMgr* pActorMgr = gServerGlobal->getActorMgr();
	if(pActorMgr == nullptr)
		return;
	SERVICE_PTR pContainer = pActorMgr->getActorByClientID(clientID);
	if ( pContainer==NULL )
		return;
	IActorService * pActor = (IActorService*)pContainer->query_interface();
	if ( pActor==NULL )
		return;
	DWORD dwOldIdCard = pActor->getIdCard();

	// 认证成功、更新实名认证标识
	reqUpdateRealNameAuther(dwOldIdCard);

	// 更新成年信息
	if (!pResult->bIsAdult)
	{
		char desc[128];
		sstrcpyn(desc, a2utf8("实名认证信息填写成功！验证为未成年用户，已纳入防沉迷范围！"), sizeof(desc));
		sendToClientAuthResult(clientID, nDBRetCode, desc);

		// 未成年,更新IdCard
		reqUpdateIdCardInfo(dwOldIdCard, pResult->dwNewIdCard);
		return;
	}
	else
	{
		sendToClientAuthResult(clientID, nDBRetCode, pszDBRetDesc);

		// 成年，删除沉迷信息
		removeAddictInfo(dwOldIdCard);
	}
}
//////////////////////////////////////////////////////////////////////////
/** 获取未成年对象
*/
CPupil*  CAntiAddictServer::getPupil(IDCARD dwIdCard)
{
	CPupil* pPupil = nullptr;

	if( m_mapPupilIdCard.find(dwIdCard, pPupil) )
		return pPupil;

	return nullptr;
}

// 将用户插入到队列中，并返回其身份证标识
bool CAntiAddictServer::addPupilCardInfo(IDCARD dwIdCard, DWORD dwPDBID, UID uid)
{
    if(dwIdCard == 0 || dwPDBID == INVALID_PDBID)
    {
        return false;
    }

    // 是否已经创建未成年对象
    CPupil * pPupil = getPupil(dwIdCard);
    if(pPupil == nullptr)
    {
        pPupil = new CPupil();

		// 先添加一个未数据验证的初始化结构，防止这个异步过程中（世界服同步数据未下来），通知客户端出错
		SUserAddictInfo addictInfo;
		addictInfo.bDBRead          = false;
		addictInfo.dwUserID         = 0;
		addictInfo.dwIdCard         = dwIdCard;
		addictInfo.nPDBID           = dwPDBID;
		addictInfo.nLoginTime       = (int)time(NULL);
		addictInfo.nLogoutTime      = 0;
		addictInfo.nOnlineCount     = 1;
		addictInfo.nPreOnlineTime   = 0;
		addictInfo.nPreOffLineTime	= 0;

        if(pPupil == nullptr || !pPupil->create(&addictInfo))
        {
            ErrorLn(__FUNCTION__": create pupil object failed! idCard="<<dwIdCard <<", uid="<< uid);
            return false;
        }

        m_mapPupilIdCard.insert(dwIdCard, pPupil);
    }

    // 将角色数据加入到未成年对象
    return pPupil->addActor(dwPDBID, uid);
}

// 将用户从队列中移除，并返回其身份证标识
void CAntiAddictServer::removePupilCardInfo(IDCARD dwIdCard)
{
    if(dwIdCard == 0)
    {
        return;
    }

    CPupil* pPupil = getPupil(dwIdCard);
    if(pPupil == nullptr)
    {
        return;
    }
    // 更新在线账号下的角色为成年人状态
    pPupil->setAdult();

    // 删除未成年对象
    SAFE_RELEASE(pPupil);

    // 容器中移除脏数据
    removeContainerIDCard(dwIdCard);
}

/** 更新用户身份证信息
*/
void CAntiAddictServer::updatePupilCardInfo(IDCARD dwOldIdCard, IDCARD dwNewIdCard)
{
	if (dwOldIdCard == 0 || dwNewIdCard == 0)
	{
		return;
	}

	CPupil * pPupil = getPupil(dwOldIdCard);
	if (pPupil == nullptr)
	{
		return;
	}

	// 更新在线账号下的身份证信息
	pPupil->updateIdCard(dwNewIdCard);

	// 更新容器身份证信息
	m_mapPupilIdCard.erase(dwOldIdCard);
	m_mapPupilIdCard.insert(dwNewIdCard, pPupil);
}


/** 删除未成年沉迷信息
*/
bool  CAntiAddictServer::removeAddictInfo(IDCARD dwIdCard)
{
    if(dwIdCard == 0)
    {
        return false;
    }

    // 未成年队列查找玩家
    CPupil * pPupil = getPupil(dwIdCard);
    if(pPupil == nullptr)
    {
        return false;
    }

    // 发送给世界服解除身份证下其他帐号的沉迷信息
    SMsgAddictAllGame_Z2W data;
    data.dwIdCard = dwIdCard;
    // 往世界服发送玩家消息
    sendToWorldSvr(MSG_ADDICT_ALLZONE, (char*)&data, sizeof(data));

    return true;
}

/** 更新未成年身份证信息
*/
bool CAntiAddictServer::reqUpdateIdCardInfo(IDCARD dwOldIdCard, DWORD dwNewIdCard)
{
	if (dwOldIdCard == 0 || dwNewIdCard == 0)
	{
		return false;
	}

	// 发送世界服更新身份证信息
	SMsgUpdateIdCard_Z2W data;
	data.dwOldIdCard = dwOldIdCard;
	data.dwNewIdCard = dwNewIdCard;
	sendToWorldSvr(MSG_UPDATE_IDCARD_Z2W, (char *)&data, sizeof(data));

	return true;
}

/** 更新实名认证信息
*/
bool CAntiAddictServer::reqUpdateRealNameAuther(IDCARD dwIdCard)
{
	if (dwIdCard == 0)
	{
		return false;
	}

	SMsgUpdateRealNameAuther_Z2W data;
	data.dwIdCard = dwIdCard;

	// 发送世界服更新实名认证信息
	sendToWorldSvr(MSG_UPDATE_REALNAME_Z2W, (char *)&data, sizeof(data));

	return true;
}

/** 从容器中删除IDCARD
*/
void CAntiAddictServer::removeContainerIDCard(IDCARD card)
{
    m_mapPupilIdCard.erase(card);

    // 没有数据时还回内存
    if (m_mapPupilIdCard.size() == 0)
    {
        m_mapPupilIdCard.clear();
    }
}

/** 发送消息到世界服
*/
void CAntiAddictServer::sendToWorldSvr(BYTE byAction, char* pData, size_t nSize)
{
    // 往世界服发送玩家上线消息
    obuf256 ob;
    SNetMsgHead header;
    header.byDestEndPoint   = MSG_ENDPOINT_WORLD;
    header.bySrcEndPoint    = MSG_ENDPOINT_SCENE;
    header.byKeyModule      = MSG_MODULEID_ANTIADDICT;
    header.byKeyAction      = byAction;

    ob.push_back(&header, sizeof(header));
    ob.push_back(pData, nSize);

    // 往世界服发送消息
    ICenterConnectorService * pCenterConnectorService = gServerGlobal->getCenterConnectorService();
    if (pCenterConnectorService)
    {
        pCenterConnectorService->sendDataToWorldServer(ob.data(), ob.size());
    }
}

/** 发送客户端认证结果
*/
void CAntiAddictServer::sendToClientAuthResult(ClientID clientID, int nDBRetCode, char * pszDBRetDesc)
{
	obuf ob;

	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule    = MSG_MODULEID_ANTIADDICT;
	head.byKeyAction    = MSG_ADDICT_GAME_Z2C;
	ob << head;

	SMsgAddictGame_Z2C data;
	data.ret = nDBRetCode;
	sstrcpyn(data.szRetDes, pszDBRetDesc, sizeof(data.szRetDes));
	ob.push_back(&data, sizeof(data));

	UID uid = gServerGlobal->getActorMgr()->ClientID2UID(clientID);
	send2Client(uid, PACKAGE_PTR(new std::string(ob.data(), ob.size())));
}

//////////////////////////////////////////////////////////////////////////
// 输出函数
API_EXPORT IAntiAddictServer* CreateAntiAddictServer(void)
{
    // 如果创建了，则直接返回
    if(g_pAntiAddictServer != NULL)
    {
        return g_pAntiAddictServer;
    }

    // 实例化系统模块
    CAntiAddictServer * pAntiAddictServer = new CAntiAddictServer();
    if(pAntiAddictServer == NULL || !pAntiAddictServer->create())
    {
        SAFE_RELEASE(pAntiAddictServer);
        return NULL;
    }

    g_pAntiAddictServer = pAntiAddictServer;

    return g_pAntiAddictServer;
}
