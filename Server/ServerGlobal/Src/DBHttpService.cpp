/*******************************************************************
** 文件名:	DBHttpService.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2010 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:
** 应  用:

**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#include "stdafx.h"
#include "IServerGlobal.h"
#include "IMessageDispatch.h"
#include "ICenterConnectorService.h"
#include "IGatewayAcceptorService.h"
#include "DBHttpActionway.h"
#include "EntityHelper.h"
#include "ChatHelper.h"
#include "DBHttpService.h"



DBHttpService* g_pDBHttpService = NULL;


//////////////////////////////////////////////////////////////////////////
/*
@brief：构造
*/
DBHttpService::DBHttpService()
    : m_dwSession(0)
{
	memset(m_ActionwayFun, 0, sizeof(m_ActionwayFun));
}

/*
@brief：析构
*/
DBHttpService::~DBHttpService()
{

}

//////////////////////////////////////////////////////////////////////////
void DBHttpService::release(void)
{
    delete this;
}

//////////////////////////////////////////////////////////////////////////
bool DBHttpService::create(void)
{
    IMessageDispatch * pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (pMessageDispatch) {
        pMessageDispatch->registerTransmitHandler(MSG_MODULEID_DBHTTP, static_cast<ITransmitHandler*>(this));
    }

	RegisterActionwayHandler(DBHttpAction_Notity, new DBHttpActionNotity);

	return true;
}

void DBHttpService::onStop(void)
{
    UnRegisterActionwayHandler(DBHttpAction_Notity);

    IMessageDispatch * pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (pMessageDispatch) {
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_DBHTTP);
    }
}

/**
@name    : 请求url调用（外部接口调用）
@param const char* pUrl: 请求地址
@param WORD wLen : 地址长度
@param IUrlResponse* pUrlResponse : 回调者
*/
void DBHttpService::requestUrl(const char* pUrl, unsigned short nLen, IUrlResponse* pUrlResponse)
{
	if (NULL == pUrl || nLen <= 0 || NULL == pUrlResponse)
	{
		return;
	}

    ICenterConnectorService * pCenterConnectorService  = gServerGlobal->getCenterConnectorService();
	if (nullptr == pCenterConnectorService)
	{
		return;
	}

	// 生成会话ID
    if (m_dwSession >= INT_MAX)
    {
        m_dwSession = 0;
    }
    m_dwSession++;

	m_mapUrlResponseNodes[m_dwSession] = pUrlResponse;

	// 组装消息体
	SMsgDBHttp_RequestUrl msg;
    msg.dwServerID = pCenterConnectorService->getLocalServerCSID();
	msg.dwSessionID	= m_dwSession;
	obuf2048 buff;
	buff << msg;
	buff.push_back(pUrl, nLen);
	buff << "\0";
    pCenterConnectorService->sendDataToDBHttpSvr(MSG_MODULEID_DBHTTP, MSG_DBHTTP_REQUESTURL, buff.data(), buff.size());
}

void DBHttpService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void * pServerData)
{
}

//////////////////////////////////////////////////////////////////////////
bool DBHttpService::RegisterActionwayHandler(DWORD actionway, IActionway* handler)
{
	if(actionway < 0 && actionway >= DBHttpAction_NUM) return false;
	if (m_ActionwayFun[actionway])
	{
		return false;
	}

	m_ActionwayFun[actionway] = handler;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void DBHttpService::UnRegisterActionwayHandler(DWORD actionway)
{
	if(actionway < 0 && actionway >= DBHttpAction_NUM) return;
	if (m_ActionwayFun[actionway])
	{
		delete m_ActionwayFun[actionway];
		m_ActionwayFun[actionway] = 0;
	}
}

/*
@Brief: 解析请求消息
*/
bool DBHttpService::ParseParam(ibuffer& buf)
{
	buf >> m_msg;
	if( !buf.good() )
		return false;

	for (int i=0; i<m_msg.num; i++)
	{
		DWORD ID;
		buf >> ID;
		m_PDBIDList.push_back(ID);
		if (!buf.good())
			return false;
	}

	std::string temp;
	buf >> temp;
	std::vector<std::string> thingslist;
	StringHelper::split(thingslist, temp.c_str() ,"|||");
	if (thingslist.size() == 0)
		return false;

	m_value = thingslist[0];
	if (thingslist.size() == 2)
	{
		StringHelper::split(m_valueParams,thingslist[1].c_str(),',');
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void	DBHttpService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
	switch(uMsgID)
	{
	case MSG_DBHTTP2ZONE_REQUEST:
		{
            onTransmitHandleDBHttpRequest(data,len);
		}
		break;
	case MSG_DBHTTP_KICKOUT:
		{
            onTransmitHandleDBHttpKickout(data,len);
		}
		break;
    case MSG_DBHTTP_REQUESTURL:			// DBHttp服务器转发后后台http请求结果到场景服
		{
			HandleDBHttpUrlResponse(data,len);
		}
		break;
	default:
		break;
	}
}

void DBHttpService::onServerListUpdated()
{
}

IActionway * DBHttpService::getActionway(BYTE nAction)
{
    if (nAction <= 0 || nAction >= DBHttpAction_NUM)
        return 0;

    return m_ActionwayFun[nAction];
}

/*
@brief：处理请求，核心函数
*/
void DBHttpService::onTransmitHandleDBHttpRequest(void * data, size_t len)
{
	ibuffer buf(data,len);

	//解析参数
	ClearParam();
    if (!ParseParam(buf)) {
        return;
    }

    IActionway * pActionway = getActionway(m_msg.action);
    if (NULL == pActionway) {
        return;
    }

	if (m_msg.num <= 0)
	{
		m_ActionwayFun[m_msg.action]->OnGodExectue(m_msg, m_value,m_valueParams);
	}
	else
	{	
		//根据module action actionway 进行操作
		for (int n=0; n<m_msg.num; n++)
		{
            DWORD & dwPDBID = m_PDBIDList[n];

			m_ActionwayFun[m_msg.action]->OnExectue(dwPDBID, m_msg, m_value,m_valueParams);
		}
	}
}

/*
@brief: 数据库后台请求下线
*/
void DBHttpService::onTransmitHandleDBHttpKickout(void* data, size_t len)
{
	ibuffer ibuf(data,len);
	SMsgDBHttp_Kickout msg;
	ibuf>>msg;
	if (!ibuf.good())
	{
		WarningLn(__FUNCTION__": Execute Failed because buff is bad!");
		return;
	}

    IGatewayAcceptorService * pGatewayAcceptorService = gServerGlobal->getGatewayAcceptorService();
    if (nullptr == pGatewayAcceptorService) {
        WarningLn(__FUNCTION__": Execute Failed because nullptr == pGatewayAcceptorService, pdbid=" << msg.pdbid);
        return;
    }

    ActorHelper helper(msg.pdbid, ACTOR_DBID);
    if (nullptr == helper.m_ptr) {
		WarningLn(__FUNCTION__": Execute Failed because not find the player, pdbid="<< msg.pdbid);
		return;
	}
    ClientID clientID = helper.m_ptr->getClientID();
    if (INVALID_CLIENT == clientID) {
        WarningLn(__FUNCTION__": Execute Failed because clientID == INVALID_CLIENT, pdbid=" << msg.pdbid);
        return;
    }


    // 通知客户端踢人
    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService)
    {
        pChatSceneService->broadcastSystemMessageToChatBox(CHAT_BROADCASTTYPE_SINGLE,
            msg.pdbid,
            CHAT_TIP_CHATBOX_AND_MIDDLE_TIP,
            CHAT_CHANNEL_WORLD,
            a2utf8(msg.szReason),
            0, 0);
    }

    pGatewayAcceptorService->kickOutClient(clientID, 0);

}

/**
@name    : DBHttp服务器转发后后台http请求结果到场景服
@param 
@param 
*/
void DBHttpService::HandleDBHttpUrlResponse(void* data,size_t len)
{
	if (data == NULL || len < 0)
	{
		ErrorLn(__FUNCTION__":Recv DBHttpServer Url Result, len="<< len);
		return;
	}

	ibuffer buff(data,len);
	SMsgDBHttp_ResponseUrl msg;
	std::string strUrl;
	buff >> msg;
	buff >> strUrl;
	TMAP_UrlResponseNode::iterator it = m_mapUrlResponseNodes.find(msg.dwSessionID);
	if (it == m_mapUrlResponseNodes.end())
	{
		ErrorLn(__FUNCTION__":Url Session is exist,SessionID="<<msg.dwSessionID<< ",url="<< strUrl.c_str());
		return;
	}
	IUrlResponse* pUrlResponse = it->second;
    m_mapUrlResponseNodes.erase(it);
	if (pUrlResponse == NULL)
	{
		return;
	}
	pUrlResponse->onUrlResponse(strUrl.c_str(), strUrl.size());
}

///////////////////////////////////////////////////////////////////////////////


// 输出函数
API_EXPORT IDBHttpService* CreateDBHttpService(void)
{
    // 如果创建了，则直接返回
    if (g_pDBHttpService != NULL)
    {
        return g_pDBHttpService;
    }

    // 实例化系统模块
    DBHttpService * pDBHttpService = new DBHttpService();
    if (pDBHttpService == NULL || !pDBHttpService->create())
    {
        Safe_Release(pDBHttpService);
        return NULL;
    }

    g_pDBHttpService = pDBHttpService;

    return g_pDBHttpService;
}