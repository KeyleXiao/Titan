/*******************************************************************
** 文件名:	e:\Rocket\SocialServer\SocialServer\Src\ShareServerHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/2/12 21:26
** 版  本:	1.0
** 描  述:	共享辅助类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "ShareServerHelper.h"
#include "ISocialGlobal.h"
#include "IShareServer.h"


CShareServerHelper g_ShareServerHelper;
/** 
@param   
@param   
@return  
*/
CShareServerHelper::CShareServerHelper(void)
{

}

/** 
@param   
@param   
@return  
*/
CShareServerHelper::~CShareServerHelper(void)
{

}

/** 
@param   
@param   
@return  
*/
bool CShareServerHelper::Create(void)
{
	IMessageDispatch * pDispatch = gSocialGlobal->getMessageDispatch();
	if(pDispatch == NULL)
	{
		return false;
	}

	pDispatch->registerTransmitHandler(MSG_MODULEID_SHARE, static_cast<ITransmitHandler *>(this));

	return true;
}

/** 
@param   
@param   
@return  
*/
void CShareServerHelper::Close(void)
{
	IMessageDispatch * pDispatch = gSocialGlobal->getMessageDispatch();
	if(pDispatch != NULL)
	{
		pDispatch->unregisterTransmitHandler(MSG_MODULEID_SHARE);
	}
}

////// ITransmitHandler /////////////////////////////////////////////////////////////
/** 消息处理
@param server	源服务器ID
@param uMsgID	消息ID
@param head		消息头
@param data		消息
@param len		消息长度
*/
void CShareServerHelper::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
	if(head->bySrcEndPoint != MSG_ENDPOINT_SCENE)
	{
		return;
	}

	IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
	if(pShareReceiver != NULL)
	{
		pShareReceiver->onTransmit(server, head, data, len);
	}
}

/** 消息处理
@param msg 网络消息
*/
void CShareServerHelper::onMessage(ulong endpointId, ulong actionId, SNetMsgHead* head, void* data, size_t len)
{

}

void CShareServerHelper::onServerListUpdated()
{

}

/** 通知服务器信息更新
@param nState		: SERVER_STATE
@param pServerData	: ServerData
@brief        : 例如服务器人数变化等等
*/
void CShareServerHelper::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{
	IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
	if(pShareReceiver != NULL)
	{
		pShareReceiver->HandleServerInfoUpdated(ServerID, nState, pServerData);
	}
}



