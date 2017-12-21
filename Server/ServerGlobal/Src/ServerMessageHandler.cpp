/*******************************************************************
** 文件名:	ServerMessageHandler.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-9-14
** 版  本:	1.0
** 描  述:	服务器系统消息处理器
            
********************************************************************/

#include "stdafx.h"
#include "IServerGlobal.h"
#include "IMessageDispatch.h"
#include "ServerMessageHandler.h"



////////////////// CServerMessageHandler ////////////////////////////////////////////////////////
CServerMessageHandler::CServerMessageHandler()
{

}

CServerMessageHandler::~CServerMessageHandler()
{

}

bool CServerMessageHandler::create()
{
    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__": Can not found the IMessageDispatch!!");
        return false;
    }
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_COMMON, this);

    return true;
}

void CServerMessageHandler::release()
{
    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__": Can not found the IMessageDispatch! ");
        return;
    }
    pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_COMMON);
}

///////////////////////////////////ITransmitHandler/////////////////////////////////////////////
/** 消息处理
@param server	源服务器ID
@param uMsgID	消息ID
@param head		消息头
@param data		消息
@param len		消息长度
*/
void CServerMessageHandler::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    switch(uMsgID)
    {
    case SS_MSG_SYSTEM_NEWDAY:
        {
            onTransmitNewDay(server, uMsgID, head, data, len);
        }
        break;

    default:
        {
            ErrorLn( __FUNCTION__ ":has a message not handle! from="<<head->bySrcEndPoint<<",module="<<head->byKeyModule <<",msg="<< uMsgID);
        }
        break;
    }
}

/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void CServerMessageHandler::onServerListUpdated()
{

}

/** 通知服务器信息更新
@param nState		: SERVER_STATE
@param pServerData	: ServerData
@brief        : 例如服务器人数变化等等
*/
void CServerMessageHandler::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

//////////////////////////////////////////////////////////////////////////
void CServerMessageHandler::onTransmitNewDay(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
    if(pEventEngine == nullptr)
    {
        ErrorLn(__FUNCTION__": pEventEngine == nullptr");
        return;
    }

    // 触发新一天事件
    pEventEngine->FireExecute(EVENT_NEW_DAY, SOURCE_TYPE_SYSTEM, 0, "", 0);
    TraceLn("Fire New Day Event");
}