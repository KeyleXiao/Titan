//==========================================================================
/**
* @file	  : ExternalHelper.cpp
* @author : xiaobao
* created : 2009-3-7
* purpose : 战队模块公用函数实现类
*/
//==========================================================================
#include "StdAfx.h"
#include "ExternalHelper.h"
#include "IManagerConnector.h "
#include "ISchemeCenter.h"
#include "Api.h"
#include "ISocialGlobal.h"
#include "ITimerService.h"
#include "ICenterConnectorService.h"
#include "IBridgeConnectorService.h"
#include "KinDef.h"
#include "IShareServer.h"
#include "IGatewayAcceptorService.h"
#include "ISocialGlobal.h"
#include "IChatManager.h"
#include "IChatService.h"
#include "Kin_ManagedDef.h"
#include "Chat_ManagedDef.h"
#include "ChatHelper.h"

using namespace Kin_ManagerDef;

CExternalHelper	g_EHelper;

// 启用定时器
bool CExternalHelper::SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo)
{
	ITimerService * pTimer = gSocialGlobal->getTimerService();
	if(pTimer == NULL)
	{
		return false;
	}

	return pTimer->SetTimer(timerID, interval, handler, INFINITY_CALL, debugInfo);
}

// 销毁定时器
bool CExternalHelper::KillTimer(DWORD timerID, ITimerHandler * handler)
{
	ITimerService * pTimerAxis = gSocialGlobal->getTimerService();
	if(pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(timerID, handler);
}

// 发送数据到场景服，ServerID不填，则发送到所有的场景服
void CExternalHelper::SendDataToSceneSvr(LPCSTR pData, size_t nLen, CSID ServerID)
{
    ICenterConnectorService *pCenterConnector = gSocialGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnector)
    {
        ErrorLn(__FUNCTION__" failed! pCenterConnector == null");
        return;
    }

	if(ServerID == INVALID_SERVER_ID)
	{
        const int SERVER_COUNT = 256;
		ServerData* svrInfo[SERVER_COUNT];
		int count = pCenterConnector->getServerList(svrInfo, SERVER_COUNT, INVALID_SERVER_ID, MSG_ENDPOINT_SCENE);
		if(count <= 0)
		{		
			return;
		}

		for(int i  = 0; i < count; i ++)
		{
			pCenterConnector->postMessage(svrInfo[i]->dwServerID, (void *)pData, (int)nLen);
		}	
	}
	else
	{
		pCenterConnector->postMessage(ServerID, (void *)pData, (int)nLen);
	}
}


// 发送数据到某个玩家 
void CExternalHelper::SendDataToPerson(DWORD dwPDBID, LPCSTR pData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }
    if (dwPDBID == INVALID_PDBID)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID != dwPDBID)
    {
        return;
    }

    IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
    if (nullptr == pGatewayAcceptorService)
    {
        ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService PDBID=" << dwPDBID);
        return;
    }

    pGatewayAcceptorService->sendData(shareProperty.clientID, PACKAGE_PTR(new std::string((char const *)pData, nLen)) );
}

// 取得某种身份的权限
int CExternalHelper::GetIdentityPopedom(int nIdentity)
{
	// 族长所有权限
	if (nIdentity == emKinIdentity_Shaikh)
	{
		return emKinPopedom_ShaikhPopedom;
	}
	// 副族长有除了任命族长和任命副族长的其他权限
	else if (nIdentity == emKinIdentity_Elder)
	{
		return emKinPopedom_ElderPopedom;
	}
	else
	{
		return 0;
	}

	return 0;
}

// 发送客户端事件
void CExternalHelper::FireClientEvent(DWORD dwPDBID, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, size_t nLen)
{
	SMsgKin_EventHead_OC eventHead;
	eventHead.wEventID = wEventID;
	eventHead.bSrcType = bSrcType;
	eventHead.dwSrcID = dwSrcID;

	obuf2048 ob;
	fillKinData(ob, MSG_ENDPOINT_CLIENT, MSG_KIN_CLIENTEVENT);
	ob.push_back(&eventHead, sizeof(SMsgKin_EventHead_OC));
	if (nLen > 0 && pszContext != NULL)
	{
		ob.push_back(pszContext, nLen);
	}
	
	g_EHelper.SendDataToPerson(dwPDBID, ob.data(), ob.size());	
}


// 取得当前时间的时间字符串
LPCSTR CExternalHelper::GetCurTimeString(void)
{
	static char buf[16] = {0};

	buf[0] = 0;
	time_t t = time(NULL);
	struct tm* local = localtime(&t);

	strftime(buf, 16, "%H:%M", local);

	return buf;
}


/// purpose: 根据字符串取得时间值
//@ note   : 如果参数为-1，表示用当前的时间
long CExternalHelper::GetLongTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSecond)
{
	time_t t = time(NULL);
	struct tm* local = localtime(&t);
	local->tm_year  =   (nYear != -1) ? (nYear - 1900) : local->tm_year;
	local->tm_mon   =   (nMonth != -1) ? (nMonth - 1) : local->tm_mon;
	local->tm_mday  =   (nDay != -1) ? nDay : local->tm_mday;
	local->tm_hour  =   (nHour != -1) ? nHour : local->tm_hour;
	local->tm_min  =   (nMin != -1) ? nMin : local->tm_min;
	local->tm_sec  =   (nSecond != -1) ? nSecond : local->tm_sec;

	time_t lTime   =   mktime(local);   

	return (int)lTime;
}

/// purpose: 2个时间点是不是在同一天
bool CExternalHelper::InOneDay(time_t time1, time_t time2)
{
	tm tmTime1, tmTime2;
	memcpy(&tmTime1, localtime(&time1), sizeof(tm));
	memcpy(&tmTime2, localtime(&time2), sizeof(tm));

	if (tmTime1.tm_year != tmTime2.tm_year
		|| tmTime1.tm_mon != tmTime2.tm_mon
		|| tmTime1.tm_mday != tmTime2.tm_mday)
	{
		return false;
	}

	return true;
}


/// purpose: 返回时间长度
//@ return : 格式 x天x小时x分钟
LPCSTR CExternalHelper::GetSecondString(int nDay, int nHour, int nMinute, int nSecond)
{
	static char szRet[128] = {0};
	memset(szRet, 0, sizeof(szRet));


	if (nDay > 0)
	{
		if (nHour > 0)
		{
			sprintf_s(szRet, 128, "%d天%d小时", nDay, nHour);
			return szRet;
		}
		else
		{
			sprintf_s(szRet, 128, "%d天%d小时", nDay);
			return szRet;
		}
	}
	else
	{
		if (nHour > 0)
		{	
			if (nMinute > 0)
			{
				sprintf_s(szRet, 128, "%d小时%d分", nHour, nMinute);
				return szRet;
			}
			else
			{
				sprintf_s(szRet, 128, "%d小时", nHour, nMinute);
				return szRet;
			}
		}
		else
		{
			if (nMinute > 0)
			{
				if (nSecond > 0)
				{
					sprintf_s(szRet, 128, "%d分%d秒", nMinute, nSecond);
					return szRet;

				}
				else
				{
					sprintf_s(szRet, 128, "%d分", nMinute);
					return szRet;
				}
			}
			else
			{
				if (nSecond > 0)
				{
					sprintf_s(szRet, 128, "%d秒", nSecond);
					return szRet;
				}
				else
				{
					return "0";
				}
			}
		}
	}

	return "0";
}

/// purpose: 返回时间长度
//@ return : 格式 x天x小时x分钟
LPCSTR CExternalHelper::GetSecondString(int nSecond)
{
	int nDay = nSecond / (3600 * 24);
	int nHour = (nSecond % (3600 * 24)) / 3600;
	int nMinute = (nSecond % 3600) / 60;
	int nSec = nSecond % 60;

	return  GetSecondString(nDay, nHour, nMinute, nSec);
}

/// purpose: 计算2个日期之间的天数
int	CExternalHelper::DiffDay(int time1, int time2)
{
	time_t t1 = time1;
	time_t t2 = time2;
	tm tmTime1, tmTime2;
	tm* tmpTime1= localtime(&t1);
	tm* tmpTime2= localtime(&t2);
	if (tmpTime1 == NULL || tmpTime2 == NULL)
	{
		return 0;
	}

	memcpy(&tmTime1, localtime(&t1), sizeof(tm));
	memcpy(&tmTime2, localtime(&t2), sizeof(tm));

	int nDaySec = 3600 * 24;

	// 取得当天的时间差
	time1 -= (tmTime1.tm_hour * 3600 + tmTime1.tm_min * 60 + tmTime1.tm_sec);
	time2 -= (tmTime2.tm_hour * 3600 + tmTime2.tm_min * 60 + tmTime2.tm_sec);

	int nDiffDay =  abs(time2 - time1) / nDaySec;

	return nDiffDay;
}

/** 判断两个时间点之间相差几周
@ 算法说明: 去掉两个时间的星期几相对于星期一零点的秒数，得到整的天数，计算这两个整天数
            之间相隔几周即可。
@return 同一周则返回0，第一个时间在第二个时间前N周则返回负数，反之，正数。
*/
int CExternalHelper::DiffWeek(DWORD dwTime1, DWORD dwTime2)
{
	// 一天的秒数
	int nDaySec = 3600 * 24;

	// 格式化
	time_t tTime1 = (DWORD)dwTime1;
	time_t tTime2 = (DWORD)dwTime2;

	tm tmTime1;
	memcpy(&tmTime1, localtime(&tTime1), sizeof(tmTime1));

	tm tmTime2;
	memcpy(&tmTime2, localtime(&tTime2), sizeof(tmTime2));

	// 星期几的处理(按照中国大陆的习惯，星期一算一个星期的第一天)
	int nWeekday1 = tmTime1.tm_wday;
	int nWeekday2 = tmTime2.tm_wday;
	int nDiffMonday1 = 0;		// nWeekday1与星期一相差几天
	int nDiffMonday2 = 0;		// nWeekday2与星期一相差几天
	if (nWeekday1 == 0)
	{
		nDiffMonday1 = 6;
	}
	else
	{
		nDiffMonday1 = nWeekday1 - 1;
	}
	if (nWeekday2 == 0)
	{
		nDiffMonday2 = 6;
	}
	else
	{
		nDiffMonday2 = nWeekday2 - 1;
	}

	// 得到dwTime1和dwTime2去掉零头后的秒数(相对于星期一零点，多出来的都是零头)
	DWORD dwTimeFinal1 = dwTime1 - (nDiffMonday1 * nDaySec) - (tmTime1.tm_hour*3600 + tmTime1.tm_min * 60 + tmTime1.tm_sec);
	DWORD dwTimeFinal2 = dwTime2 - (nDiffMonday2 * nDaySec) - (tmTime2.tm_hour*3600 + tmTime2.tm_min * 60 + tmTime2.tm_sec);

	int nDiffWeeks = ((int)(dwTimeFinal1 - dwTimeFinal2)) / (7*nDaySec);

	return nDiffWeeks;
}

/// purpose: 往公共区发送数据
bool CExternalHelper::SendToPublicServer(void* pData, size_t nLen)
{
	if (isPublicGameWorld() == true)
	{
		return false;
	}

	IBridgeConnectorService* pBridgeConnector = gSocialGlobal->getBridgeConnectorService();
	if (pBridgeConnector == NULL)
	{
		return false;
	}

	DWORD dwpubworldid = pBridgeConnector->getPublicWorldID();
	DWORD sendServerid = pBridgeConnector->getGameServerIDByTypeSubID(dwpubworldid,MSG_ENDPOINT_SOCIAL,0);
	if (sendServerid == 0)
	{
		return false;
	}

    ibuffer t_in(pData,nLen);
	pBridgeConnector->sendDataToServer(sendServerid, t_in);

	return true;
}

/// purpose: 往公共区发送数据
bool CExternalHelper::SendToPublicServer(BYTE byKeyAction, void* pData, size_t nLen)
{
	if (isPublicGameWorld() == true)
	{
		return false;
	}

	obuf4096 ob;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
	head.byDestEndPoint = MSG_ENDPOINT_SOCIAL;
	head.byKeyModule = MSG_MODULEID_KIN;
	head.byKeyAction = byKeyAction;

	ob.push_back(&head, sizeof(head));
	if (pData != NULL && nLen > 0)
	{
		ob.push_back(pData, nLen);
	}

	SendToPublicServer(ob.data(), ob.size());

	return true;
}


/** 显示提示文字
*/
void CExternalHelper::showSystemMessage(PDBID PlayerID, EMChatTipID nTipID, LPCSTR szParam)
{
  
	ChatHelper chatHelper;
	IChatService *pChatService = chatHelper.m_ptr;
    if ( NULL == pChatService )
    {
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
        return;
    }

    pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, PlayerID, nTipID, szParam);
}

SMailConfig* CExternalHelper::getMailConfig(int mailID)
{
	ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
	if (NULL == pSchemeCenter)
	{
		ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
		return NULL;
	}

	ISchemeMailDataCenter *pSchemeMailDataCenter = pSchemeCenter->getSchemeMailDataCenter();
	if (NULL == pSchemeMailDataCenter)
	{
		ErrorLn(__FUNCTION__" nullptr == pSchemeMailDataCenter");
		return NULL;
	}

	SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(mailID);
	if (NULL == pMailConfig)
	{
		ErrorLn(__FUNCTION__" nullptr == pMailConfig");
		return NULL;
	}

	return pMailConfig;
}

SKinConfig* CExternalHelper::getConfig()
{
	ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return NULL;

	ISchemeKinDataCenter *pSchemKinDataCenter = pSchemeCenter->getSchemeKinDataCenter();
	if (pSchemKinDataCenter == NULL)
		return NULL;

	return pSchemKinDataCenter->getConfig();
}
