/*******************************************************************
** 文件名:	e:\Rocket\Client\WarClient\Src\WarClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/7/28  21:25
** 版  本:	1.0
** 描  述:	战场客户端
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "WarClient.h"
#include "ExternalFacade.h"
#include "WarPart.h"
#include "MobaGoodsPart.h"
#include "GameViewDef.h"

CWarClient *	g_pWarClient = NULL;
/** 构造
@param   
@param   
@return  
*/
CWarClient::CWarClient(void)
{

}

/** 析构
@param   
@param   
@return  
*/
CWarClient::~CWarClient(void)
{

}

/** 创建
@param   
@param   
@return  
*/
bool CWarClient::Create(void)
{
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_WAR, this);

	return true;
}

/** 释放
@param   
@param   
@param   
@return  
@note     释放所有资源，并且销毁此对像
@warning 
@retval buffer 
*/
void CWarClient::release(void)
{
	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_WAR, this);

	// 全局指针NULL
	g_pWarClient = NULL;

	delete this;
}

// 模块消息处理
void CWarClient::onMessage(SNetMsgHead* head, void* data, size_t len)
{
	switch (head->byKeyModule)
	{
	case MSG_MODULEID_WAR:
		{
			// 战场模块消息处理
			onWarMessageID(head->byKeyAction, data, len);
		}
		break;
		// TODO...
	//case MSG_MODULEID_STATICWAR:
	//	{
	//		// 主城模块消息处理
	//		onWarStaticMessageID(head->byKeyAction, data, len);
	//	}
	//	break;
	default:
		break;
	}

}

bool CWarClient::onWarMessageID(int msgId, void * data, size_t len)
{
	switch (msgId)
	{
	case SC_MSG_WAR_SAFETYZONE_INFO:
		{
			// 大逃杀战场-安全圈信息
			onMessageWarSafetyZoneInfo(data, len);
		}
		break;
	case SC_MSG_WAR_PLAYER_RECONNECTION:
		{
			// 玩家掉线重连
			OnMessagePlayerReconnection(data, len);
		}
		break;
	case SC_MSG_WAR_SAFETYZONE_SPREAD_CD:
		{
			// 大逃杀战场-扩散倒计时信息
			onMessageWarNoxSpreadCDInfo(data, len);
		}
		break;
	case SC_MSG_WAR_SERVER_TOVIEW_MSG:
		{
			// 服务器直接发到显示层的消息
			onMessageWarToViewMsg(data, len);
		}
		break;
	default:break;
	}

	return true;
}


void CWarClient::onMessageWarSafetyZoneInfo(void * pData, size_t nLen)
{
	if (nLen != sizeof(msg_war_safetyzone_info))
	{
		return;
	}
	msg_war_safetyzone_info* pInfo = (msg_war_safetyzone_info*)pData;

	gamelogic_war_safetyzone_info cmd;
	cmd.fCenterOfNoxZoneX = pInfo->nCenterOfNoxZoneX * LOCATION_SCALE_RESTORE;
	cmd.fCenterOfNoxZoneY = pInfo->nCenterOfNoxZoneY * LOCATION_SCALE_RESTORE;
	cmd.fCenterOfNoxZoneZ = pInfo->nCenterOfNoxZoneZ * LOCATION_SCALE_RESTORE;
	cmd.fCenterOfSafetyZoneX = pInfo->nCenterOfSafetyZoneX * LOCATION_SCALE_RESTORE;
	cmd.fCenterOfSafetyZoneY = pInfo->nCenterOfSafetyZoneY * LOCATION_SCALE_RESTORE;
	cmd.fCenterOfSafetyZoneZ = pInfo->nCenterOfSafetyZoneZ * LOCATION_SCALE_RESTORE;
	cmd.fRadiusOfNoxZone = pInfo->nRadiusOfNoxZone * LOCATION_SCALE_RESTORE;
	cmd.fRadiusOfSafetyZone = pInfo->nRadiusOfSafetyZone * LOCATION_SCALE_RESTORE;
	cmd.nSpreadBeginTick = pInfo->nSpreadBeginTick;
	cmd.nSpreadTime = pInfo->nSpreadTime;
	cmd.isSpreadBegin = pInfo->nSpreadBeginTick > 0;

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_WAR_SAFETYZONE_INFO, 0, NULL, &cmd, sizeof(cmd));
}

void CWarClient::OnMessagePlayerReconnection(void* data, size_t len)
{
	if (data == NULL || len < sizeof(msg_war_player_reconnection))
	{
		ErrorLn(__FUNCTION__ << " : SC_MSG_WAR_PLAYER_RECONNECTION data is null or length is invalid! len = " << len << ", sizeof = " << sizeof(msg_war_player_reconnection));
		return;
	}

	msg_war_player_reconnection* pData = (msg_war_player_reconnection*)data;

	gamelogic_war_player_reconnection cmd;
	cmd.actorID = pData->actorID;
	cmd.heroID = pData->heroID;
	sstrcpyn(cmd.actorName, pData->actorName, sizeof(cmd.actorName));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_WAR_PLAYER_RECONNECTION, 0, NULL, &cmd, sizeof(cmd));
}

void CWarClient::onMessageWarNoxSpreadCDInfo(void * pData, size_t nLen)
{
	if (nLen != sizeof(msg_war_safetyzone_spread_cd))
	{
		return;
	}
	msg_war_safetyzone_spread_cd* pInfo = (msg_war_safetyzone_spread_cd*)pData;

	gamelogic_war_nox_spread_cd cmd;
	cmd.nSpreadCDTick = pInfo->nSpreadCDTick;
	cmd.nSpreadTotalCDTime = pInfo->nSpreadCDTime;

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_WAR_NOX_SPREAD_CD, 0, NULL, &cmd, sizeof(cmd));
}

void CWarClient::onMessageWarToViewMsg(void * pData, size_t nLen)
{
	if (nLen < sizeof(msg_war_server_toview_msg))
	{
		return;
	}
	msg_war_server_toview_msg* pMsg = (msg_war_server_toview_msg*)pData;

	gamelogic_war_server_toview_msg cmd;
	cmd.nKeyAction = pMsg->nKeyAction;

	obuf ob;
	ob << cmd;
	ob.push_back(pMsg + 1, nLen - sizeof(msg_war_server_toview_msg));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_WAR_SERVER_TO_VIEW_MSG, 0, NULL, ob.data(), ob.size());
}


/** 给实体创建冷却部件，重新new的部件才调此方法。如果是回收的，则不需要
@param   
@param   
@param   
@return   成功: 非NULL,失败: NULL
@note     
@warning 
@retval buffer 
*/
IWarPart * CWarClient::CreatePart(void)
{
	return new CWarPart();
}

IMobaGoodsPart * CWarClient::CreateMobaGoodsPart(void)
{
	return new CMobaGoodsPart();
}


/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT IWarClient * CreateWarClient(void)
{
	// 如果创建了，则直接返回
	if(g_pWarClient != NULL)
	{
		return g_pWarClient;
	}

	// 实例化BUFF客户端
	CWarClient * pWarClient = new CWarClient();
	if(pWarClient == NULL || !pWarClient->Create())
	{
		safeRelease(pWarClient);
		return NULL;
	}

	g_pWarClient = pWarClient;
	return g_pWarClient;
}