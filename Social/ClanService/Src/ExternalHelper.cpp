#include "StdAfx.h"
#include "ExternalHelper.h"
#include "ITimerService.h"
#include "ISocialGlobal.h"
#include "IShareServer.h"
#include "ICenterConnectorService.h"
#include "IGatewayAcceptorService.h"
#include "KinDef.h"
#include "ClanMessageDef.h"
#include "ISchemeCenter.h"
#include "IChatManager.h"
#include "IChatService.h"
#include "Kin_ManagedDef.h"
#include "KinHelper.h"
#include "ClanHelper.h"
#include "Clan_ManagedDef.h"
#include "ChatHelper.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"

using namespace Kin_ManagerDef;
using namespace Clan_ManagerDef;

CExternalHelper	g_EHelper;

// 启用定时器
bool CExternalHelper::setTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo)
{
	ITimerService *pTimer = gSocialGlobal->getTimerService();
	if (pTimer == NULL)
		return false;

	return pTimer->SetTimer(timerID, interval, handler, INFINITY_CALL, debugInfo);
}

// 销毁定时器
bool CExternalHelper::killTimer(DWORD timerID, ITimerHandler * handler)
{
	ITimerService *pTimerAxis = gSocialGlobal->getTimerService();
	if (pTimerAxis == NULL)
		return false;

	return pTimerAxis->KillTimer(timerID, handler);
}

// 发送数据到场景服，ServerID不填，则发送到所有的场景服
void CExternalHelper::sendDataToZoneSvr(LPCSTR pData, size_t nLen, CSID ServerID)
{
    ICenterConnectorService *pCenterConnector = gSocialGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnector)
    {
        ErrorLn(__FUNCTION__" failed! pCenterConnector == null");
        return;
    }

	DWORD count = pCenterConnector->getAllServersCount();
	if (ServerID == INVALID_SERVER_ID)
	{
        const int SERVER_COUNT = 256;
		ServerData* svrInfo[SERVER_COUNT] = { NULL };
		int count = pCenterConnector->getServerList(svrInfo, SERVER_COUNT, INVALID_SERVER_ID, MSG_ENDPOINT_SCENE);
		if (count <= 0)
			return;

		for (int i = 0; i < count; ++i)
		{
			pCenterConnector->postMessage(svrInfo[i]->dwServerID, (void*)pData, (int)nLen);
		}	
	}
	else
	{
		pCenterConnector->postMessage(ServerID, (void*)pData, (int)nLen);
	}
}

// 发送数据到某个玩家 
void CExternalHelper::sendDataToPerson(DWORD dwPDBID, LPCSTR pData, size_t nLen)
{
    IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL || dwPDBID == INVALID_PDBID)
        return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
        return;

    IGatewayAcceptorService *pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
    if (nullptr == pGatewayAcceptorService)
    {
        ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService PDBID=" << dwPDBID);
        return;
    }

    pGatewayAcceptorService->sendData(shareProperty.clientID, PACKAGE_PTR(new std::string((char const *)pData, nLen)) );
}

// 取得帮会职位
int CExternalHelper::getClanIdentity(DWORD dwPDBID, int nShaikhID, int nKinIdentity)
{
    if (dwPDBID == nShaikhID)
    {
        return EMCLANIDENTITY_SHAIKH;
    }

    if (nKinIdentity == emKinIdentity_Shaikh)
    {
        return EMCLANIDENTITY_ELDER;
    }

    return EMCLANIDENTITY_MEMBER;
}

// 取得帮会每级信息
SClanLevel* CExternalHelper::getLevelInfo(int nClanLevel)
{
    ISchemeCenter *pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return NULL;

    ISchemeClanDataCenter *pSchemeClanDataCenter = pSchemeCenter->getSchemeClanDataCenter();
    if (pSchemeClanDataCenter == NULL)
        return NULL;

    return pSchemeClanDataCenter->getLevelInfo(nClanLevel);
}

// 取得帮会常量信息
SClanConfig *CExternalHelper::getConfig()
{
    ISchemeCenter *pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return NULL;

    ISchemeClanDataCenter *pSchemeClanDataCenter = pSchemeCenter->getSchemeClanDataCenter();
    if (pSchemeClanDataCenter == NULL)
        return NULL;

    return pSchemeClanDataCenter->getConfig();
}

// 帮会卡牌信息
SClanCardSchemeInfo *CExternalHelper::getClanCardInfo(int nLevel)
{
    ISchemeCenter *pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return NULL;

    ISchemeClanDataCenter *pSchemeClanDataCenter = pSchemeCenter->getSchemeClanDataCenter();
    if (pSchemeClanDataCenter == NULL)
        return NULL;

    return pSchemeClanDataCenter->getClanCardInfo(nLevel);
}

// 显示提示文字
void CExternalHelper::showSystemMessage(DWORD dwPDBID, EMChatTipID nTipID, LPCSTR szParam)
{
	ChatHelper chatHelper;
	IChatService *pChatService = chatHelper.m_ptr;
	if (NULL == pChatService)
	{
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
		return;
	}

    pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwPDBID, nTipID, szParam);
}

// 发送系统提示到帮会
void CExternalHelper::broadcastClanSystemTips(DWORD dwClanID, EMChatTipID nTipID, LPCSTR szParam)
{
	ChatHelper chatHelper;
	IChatService *pChatService = chatHelper.m_ptr;
	if (NULL == pChatService)
	{
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
		return;
	}

	pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_CLAN, dwClanID, nTipID, szParam);
}

// 发送系统提示到帮会
void CExternalHelper::broadcastKinSystemTips(DWORD dwKinID, EMChatTipID nTipID, LPCSTR szParam)
{
	ChatHelper chatHelper;
	IChatService *pChatService = chatHelper.m_ptr;
	if (NULL == pChatService)
	{
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
		return;
	}

    pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_KIN, dwKinID, nTipID, szParam);
}

// 获取角色战队信息
SKinMember* CExternalHelper::getActorKinMemberInfo(DWORD dwPDBID)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return NULL;

	SKinMember sKinMember = pKinService->getKinMemberInfo(dwPDBID);
	if (sKinMember.dwKinID == NULL)
		return NULL;

	static SKinMember sTempKinMember;
	sTempKinMember = sKinMember;
	return &sTempKinMember;
}

// 取得某个身份的名字
string CExternalHelper::getIdentityName(int nIdentity)
{
    if (nIdentity == EMCLANIDENTITY_INVALID)
    {
        return a2utf8("无效职位");
    }

    if (nIdentity == EMCLANIDENTITY_SHAIKH)
    {
        return a2utf8("盟主");
    }
    else if (nIdentity == EMCLANIDENTITY_ELDER)
    {
        return a2utf8("长老");
    }
    else
    {
        return a2utf8("成员");
    }
}

// 取得某个玩家的帮会身份
int CExternalHelper::getIdentity(DWORD dwPDBID)
{
    SKinMember* pKinMember = g_EHelper.getActorKinMemberInfo(dwPDBID);
    if (pKinMember == NULL)
    {
        return EMCLANIDENTITY_INVALID;
    }

    ClanHelper clanHelper;
    IClanService *pClanService = clanHelper.m_ptr;
    if (pClanService == NULL)
    {
        return EMCLANIDENTITY_INVALID;
    }

    DWORD dwKinClanID = pClanService->findKinClan(pKinMember->dwKinID);
    if (dwKinClanID == 0)
    {
        return EMCLANIDENTITY_INVALID;
    }

    if (pClanService->getClanNumProp(dwKinClanID, emClanProp_ShaikhID) == dwPDBID)
    {
        return EMCLANIDENTITY_SHAIKH;
    }
    else
    {
        if (pKinMember->nIdentity == emKinIdentity_Shaikh)
        {
            return EMCLANIDENTITY_ELDER;
        }
        else
        {
            return EMCLANIDENTITY_MEMBER;
        }
    }

    return EMCLANIDENTITY_MEMBER;
}

// 增加帮会日志
void CExternalHelper::addClanGameNoteLog(DWORD dwClanID, const char *szParam)
{
    if (dwClanID == 0 || szParam == NULL)
    {
        return;
    }

    IOSSLogService *pOSSLogService = gSocialGlobal->getOSSLogService();
    if ( pOSSLogService == NULL )
    {
        return;
    }

    pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_CLAN, dwClanID, szParam);
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
	DWORD dwTimeFinal1 = dwTime1 - (nDiffMonday1 * nDaySec) - (tmTime1.tm_hour * 3600 + tmTime1.tm_min * 60 + tmTime1.tm_sec);
	DWORD dwTimeFinal2 = dwTime2 - (nDiffMonday2 * nDaySec) - (tmTime2.tm_hour * 3600 + tmTime2.tm_min * 60 + tmTime2.tm_sec);

	int nDiffWeeks = ((int)(dwTimeFinal1 - dwTimeFinal2)) / (7 * nDaySec);

	return nDiffWeeks;
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