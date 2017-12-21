#include "stdafx.h"
#include "ExternalHelper.h"
#include "ICenterConnectorService.h"
#include "MentorshipMsgDef.h"
#include "IServerGlobal.h"
#include "IActorMgr.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "ChatHelper.h"
#include "IChatSceneService.h"
#include "EntityHelper.h"
#include "Mentorship_ManagedDef.h"

using namespace Mentorship_ManagerDef;

using namespace rkt;

CExternalHelper	g_EHelper;

// 构造函数
CExternalHelper::CExternalHelper(void)
{

}

// 析构函数
CExternalHelper::~CExternalHelper(void)
{

}

// 发送消息
void CExternalHelper::sendMentorshipSocial(BYTE byKeyAction, int nActionID,void *pData, int nLen)
{
    // 社会服数据
    ICenterConnectorService *pCenterConnector = gServerGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnector)
    {
        ErrorLn(__FUNCTION__" failed( void * buf, size_t len )! nullptr == pCenterConnector");
        return;
    }

    obuf256 obMsg;

    SMsgMentorshipSubMsg subMsg;
    subMsg.dwMsgCode = nActionID;

    obMsg.push_back(&subMsg, sizeof(SMsgMentorshipSubMsg));
    obMsg.push_back(pData, nLen);

    pCenterConnector->sendToSocial(MSG_MODULEID_MENTORSHIP, byKeyAction, obMsg.data(), obMsg.size());
}

// 发送数据
void CExternalHelper::send2Client(DWORD dwPDBID, BYTE byKeyModule, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
	ActorHelper actorHelper(dwPDBID, ACTOR_DBID);
	if (actorHelper.m_ptr == NULL)
		return;

    // 通知客户端
    obuf256 obufData;

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = byKeyModule;	
    head.byKeyAction   = byKeyAction;

    obufData << head ;
    obufData.push_back(pszData,nDataLen);

    actorHelper.m_ptr->send2Client(PACKAGE_PTR(new string((const char *)obufData.data(),obufData.size())));
}

// 取得帮会常量信息
SMentorshipConfig* CExternalHelper::getConfig()
{
    ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return NULL;

    ISchemeMentorshipDataCenter *pSchemeMentorshipDataCenter = pSchemeCenter->getSchemeMentorshipDataCenter();
    if (pSchemeMentorshipDataCenter == NULL)
        return NULL;

    return pSchemeMentorshipDataCenter->getConfig();
}

/** 判断两个时间点之间相差几周
	@return
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

// 显示提示文字
void CExternalHelper::showSystemMessage(DWORD dwPDBID, EMChatTipID nTipID, LPCSTR szParam)
{
    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService == NULL)
    {
        return;
    }

    pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwPDBID, nTipID, szParam);
}