#include "stdafx.h"
#include "LegendCupService.h"
//#include "ICenterConnectorService.h"
#include "ISocialGlobal.h"
#include "Event_ManagedDef.h"
#include "MatchDef.h"
#include "LegendCupEvent.h"
#include "LegendCupdef.h"
#include "LegendCupHelper.h"
#include "LegendCup.h"
#include "KinHelper.h"
#include "LegendCupMsgDef.h"
#include "MatchHelper.h"
#include "KinEvent.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "ClanHelper.h"
#include "IClanService.h"
#include "EntityDef.h"
#include "IDateTriggerService.h"
#include "ChatHelper.h"

class LegendCupService;

struct IDBEngineService;

LegendCupService::LegendCupService()
{
   
}


void LegendCupService::onClientDisconnect(ClientID clientID,DWORD dwReason)
{
	TraceLn(__FUNCTION__ ": ClienttID = " << clientID << "dwReason = " << dwReason);
}

void LegendCupService::Release()
{
	LEDENDCUP_TRACELN(__FUNCTION__);

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        // 服务器消息分发反注册
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_LEGENDCUP);
        // 客户端消息分发反注册
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_LEGENDCUP);
    }
    IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
    if (pEventEngine)
    {

        // 订阅 中心连接器服务已启动
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0);
        pEventEngine->UnSubscibe(this, EVENT_KIN_READ_DB_FINISHED, SOURCE_TYPE_KIN, 0);
        pEventEngine->UnSubscibe(this, EVENT_KIN_LOGIN, SOURCE_TYPE_KIN, 0);

    }

    IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->UnregisterHandler(this);
    }

    // 数据库接口反注册
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->unregisterHandler(this);
    }

    IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
    if (pDateTriggerService != NULL)
    {
        pDateTriggerService->UnRegisterDateTrigger(DCT_DataTrigger_LegendCupUpdate, this);
    }

    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.begin();
    for (;iter != m_AllLegendCupMgr.end(); ++iter)
    {
        iter->second->release();
        iter->second = NULL;
    }
    m_AllLegendCupMgr.clear();
    m_ReadDBRecord.clear();
    m_LegendCupTypeList.clear();
	m_LegendCupPlayerList.clear();
    delete this;
}

bool LegendCupService::Create()
{
	LEDENDCUP_TRACELN(__FUNCTION__);

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        // 服务器消息分发注册
        pMessageDispatch->registerTransmitHandler(MSG_MODULEID_LEGENDCUP,this);
        // 客户端消息分发注册
        pMessageDispatch->registerMessageHandler(MSG_MODULEID_LEGENDCUP, this);
    }

    IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
    if (pEventEngine)
    {
        pEventEngine->Subscibe(this, EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
        pEventEngine->Subscibe(this, EVENT_KIN_READ_DB_FINISHED, SOURCE_TYPE_KIN, 0, __FUNCTION__);
        pEventEngine->Subscibe(this, EVENT_KIN_LOGIN, SOURCE_TYPE_KIN, 0, __FUNCTION__);
        
    }

    IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->RegisterHandler(this, __FUNCTION__);
    }


    // 数据库接口注册
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->registerHandler(this);
    }

    // 创建杯赛管理信息
    for (BYTE i = (BYTE)emCupType_None + 1; i <(BYTE)emCupType_Max ; ++i)
    {
        CLegendCupInfoMgr* pCupInfoMgr = new CLegendCupInfoMgr();
        if (pCupInfoMgr == NULL)
        {
            ErrorLn(__FUNCTION__": pCupInfoMgr == NULL emCupType  ="<<i);
        }

        pCupInfoMgr->inital(ELegendCupType(i));
        //添加进管理类
        m_AllLegendCupMgr.insert(pair<BYTE, CLegendCupInfoMgr*>(i,pCupInfoMgr));
    }

    bReadDBCupInfoFinished = false;

	return true;
}

void LegendCupService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    LegendCupHelper cupHelper;
    ILegendCupService *pLegendCupService = cupHelper.m_ptr;
    if (pLegendCupService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    pLegendCupService->handleServerMsg( server, *head, data, len);
}


void LegendCupService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{
    LegendCupHelper cupHelper;
    ILegendCupService *pLegendCupService = cupHelper.m_ptr;
    if (pLegendCupService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    pLegendCupService->handleClientMsg( clientID, *head, data, len );
}



void LegendCupService::send2Client( ClientID clientID, obuf256& obuf)
{
    IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
	if (nullptr == pGatewayAcceptorService)
	{
		ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService PDBID=" << clientID);
		return;
	}

    if(clientID == INVALID_CLIENT)
	{
		ErrorLn(__FUNCTION__": failed! clientID == INVALID_CLIENT PDBID=" << clientID);
		return;
	}

	pGatewayAcceptorService->sendData(clientID, PACKAGE_PTR(new std::string((char const *)obuf.data(), obuf.size())));
}

void LegendCupService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    switch (nCmdID)
    {
    case GAMEDB_REQUEST_READ_ALL_LEGEND_CUP:			// 包括系统和玩家自己的 系统优先
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
                return;
            }

            onReturnReadAllCupDataBaseInfo(pOutData,nOutLen);
        }
        break;

        
    case GAMEDB_REQUEST_EX_LIST_DATA_INFO:			    // 附加数据 杯赛申请列表 黑名单列表 排名列表...
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
                return;
            }

            onReturnCupEXListDataInfo(pOutData,nOutLen);
        }
        break;
    case GAMEDB_REQUEST_LEGEND_CUP_MEMBERLIST:			// 附加数据 杯赛玩家信息
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
                return;
            }

            onReturnCupMemberList(pOutData,nOutLen);
        }
        break;
    case GAMEDB_REQUEST_READ_ALL_CUP_COMPETITION_NODE:			// 附加数据 比赛信息
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
                return;
            }

            onReturnCupCompetitionNode(pOutData,nOutLen);
        }
        break;
    case GAMEDB_REQUEST_READ_ALL_KININFO:			// 附加数据 杯赛玩家信息
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
                return;
            }

            onReturnCupKinInfo(pOutData,nOutLen);
               
        }
        break;

    case GAMEDB_REQUEST_READ_ROUND_GROUP_KIN_POS:           // 获得分组列表

        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
                return;
            }

            onReturnCupRoundKinPos(pOutData,nOutLen);
                
        }
        break;
    case GAMEDB_REQUEST_READ_CUP_ROUND_TIMEINFO:           // 获得每轮时间列表

        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
                return;
            }

            onReturnCupRoundTimeInfo(pOutData,nOutLen);

        }
        break;
	case GAMEDB_REQUEST_READ_PLAYER_LEGENDCUP_DATA:

		{
			if (nDBRetCode < 0)
			{
				ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
				return;
			}

			onReturnLegendCupPlayerInfo(pOutData,nOutLen);
		}
		break;
	case GAMEDB_REQUEST_UPDATE_PLAYER_LEGENDCUP_DATA:

		{
			if (nDBRetCode < 0)
			{
				ErrorLn(_GT("数据请求ID：")<<nCmdID<<_GT(" 数据库返回码：")<<nDBRetCode<<_GT(" 信息描述：")<<pszDBRetDesc);
				return;
			}
		}
		break;

    default:
        break;
    }
}

void LegendCupService::onReturnReadAllCupDataBaseInfo( char* pOutData, int nOutLen)
{

    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return;
    }

    // 检查数据返回长度
    if (nOutLen < sizeof(DBREQ_RESULT_READCUPLIST))
    {
        int tempLen = sizeof(DBREQ_RESULT_READCUPLIST);
        ErrorLn("[onReturnReadAllCupDataBaseInfo] error length outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READKINLIST) = " << tempLen);
        return;
    }

    DBREQ_RESULT_READCUPLIST* pRetData = (DBREQ_RESULT_READCUPLIST*)pOutData;
    EmphasisLn("return cup count =" << pRetData->nLegendCupCount);
    if (nOutLen != sizeof(DBREQ_RESULT_READCUPLIST) + pRetData->nLegendCupCount * sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO))
    {
        int tempLen = sizeof(DBREQ_RESULT_READCUPLIST) + pRetData->nLegendCupCount * sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO);
        ErrorLn("[onReturnReadAllCupDataBaseInfo] length wrong outlength = " << nOutLen << " sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO) = " << tempLen);
        return;
    }

    // 创建杯赛对象
    int offset = sizeof(DBREQ_RESULT_READCUPLIST);
    char szText[512] = {0};
    static bool bOrder = false;
    for (int i = 0; i < pRetData->nLegendCupCount; i ++)
    {

        DBREQ_RESULT_LEGEND_CUP_BASE_INFO* pCupData = (DBREQ_RESULT_LEGEND_CUP_BASE_INFO*)(pOutData + offset);

        if (pCupData->bIsEnd && (DWORD)time(NULL) - pCupData->dwEndTime > 2*24*60*60)
        {
            // 结束了 限定之外的不用读取了
            offset += sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO);
            continue;
        }

        if (pCupData->eCupState == SATE_SYSTEMCANCEL)
        {
            offset += sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO);
            continue;
        }

        // 创建杯赛对象
        CLegendCup* pCup = new CLegendCup();
        if (pCup == NULL)
        {
            ErrorLn(__FUNCTION__": pCup == NULL" << pCupData->szCupName << ", LegendID" << pCupData->llLegendCupID);
            offset += sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO);
            continue;
        }

        if (pCup->createFromDB(this,(LPCSTR)pCupData, sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO)) == false)
        {
            ErrorLn(__FUNCTION__": name =" << pCupData->szCupName << ", LegendID" << pCupData->llLegendCupID);
            offset += sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO);
            continue;
        }

        // 把杯赛对象添加到对象管理列表
        addLegendCupToMgr(pCup);
        
        //读取附加数据
        readIniCupPlusData(pCupData->llLegendCupID);
        
        m_ReadDBRecord.insert(pair<LONGLONG, bool>(pCupData->llLegendCupID, false));

        offset += sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO);
    }
    TraceLn(__FUNCTION__": onReturnReadAllCupDataBaseInfo finished! ");
}

void LegendCupService::onReturnCupMemberList(char* pOutData, int nOutLen)
{
  
    int nTempLen = sizeof(DBREQ_RESULT_READLEGENDCUP_MEMBERLIST);
    if (nOutLen < nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_READKINMEMBERLIST] length = " << nTempLen);
        return;
    }

    DBREQ_RESULT_READLEGENDCUP_MEMBERLIST* pRetData = (DBREQ_RESULT_READLEGENDCUP_MEMBERLIST*)pOutData;
    nTempLen = sizeof(DBREQ_RESULT_READLEGENDCUP_MEMBERLIST) + pRetData->nMemberCount * sizeof(DBREQ_PARAM_LEGENDCUP_MEMBER);
    if (nOutLen != nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
        return;
    }
    LONGLONG llLegendCupID = pRetData->llLegendCupID;

    CLegendCup* pCup = findLegendCup(llLegendCupID);
    if (pCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find the kin object, LegendID=" << llLegendCupID);
        return;
    }

	// 读取杯赛玩家数据
	int nMemberCount = 0;
	obuf obNode;

    // 里面把成员添加到m_KinMemberList里
    if (pCup->setCupDBMemberContext(pOutData, nOutLen, &obNode, nMemberCount) == false)
    {
        WarningLn(__FUNCTION__": set setCupDBMemberContext error!");
    }

    if (pCup->getLegendCupDBReadFlag() == emLegendRead_AllData)
    {
        readCupInfoDBFinished(llLegendCupID);
    }

	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine != NULL && nMemberCount > 0 && obNode.size() > 0)
	{
		// 读取玩家杯赛数据信息
		obuf ob;
		DBREQ_PARAM_READ_PLAYER_CUP_DATA_HEAD refer;
		refer.nCount = nMemberCount;

		int nNodeLen = refer.nCount * sizeof(DBREQ_PARAM_READ_PLAYER_CUP_INFO);

		ob.push_back(&refer, sizeof(DBREQ_PARAM_READ_PLAYER_CUP_DATA_HEAD));
		ob.push_back(&nNodeLen, sizeof(nNodeLen));
		ob.push_back(obNode.data(), obNode.size());

		if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_PLAYER_LEGENDCUP_DATA, 0, ob.data(), ob.size(), this) == false)
		{
			ErrorLn("GAMEDB_REQUEST_READ_PLAYER_LEGENDCUP_DATA request error!");
		}
	}	
}

void LegendCupService::onReturnCupKinInfo(char* pOutData, int nOutLen)
{
    int nTempLen = sizeof(DBREQ_RESULT_READLEGENDCUP_KINLIST);
    if (nOutLen < nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_READKINMEMBERLIST] length = " << nTempLen);
        return;
    }

    DBREQ_RESULT_READLEGENDCUP_KINLIST* pRetData = (DBREQ_RESULT_READLEGENDCUP_KINLIST*)pOutData;
    nTempLen = sizeof(DBREQ_RESULT_READLEGENDCUP_KINLIST) + pRetData->nKinCount * sizeof(DBREQ_REQUEST_UPDATE_CUP_KININFO);
    if (nOutLen != nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
        return;
    }
    LONGLONG llLegendCupID = pRetData->llLegendCupID;

    CLegendCup* pCup = findLegendCup(llLegendCupID);
    if (pCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find the kin object, LegendID=" << llLegendCupID);
        return;
    }

    // 里面把成员添加到杯赛信息里
    if (pCup->setCupDBKinDataContext(pOutData, nOutLen) == false)
    {
        WarningLn(__FUNCTION__": set setCupDBKinDataContext error!");
    }

    if (pCup->getLegendCupDBReadFlag() == emLegendRead_AllData)
    {
        readCupInfoDBFinished(llLegendCupID);
    }
}

void LegendCupService::onReturnCupCompetitionNode(char* pOutData, int nOutLen)
{
    int nTempLen = sizeof(DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE);
    if (nOutLen < nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_READKINMEMBERLIST] length = " << nTempLen);
        return;
    }

    DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE* pRetData = (DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE*)pOutData;
    int nRightLen = sizeof(DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE) + pRetData->nCount * sizeof(DBREQ_PARAM_COMPETITION_NODE);
    if (nOutLen != nRightLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
        return;
    }
    LONGLONG llLegendCupID = pRetData->llLegendCupID;

    CLegendCup* pCup = findLegendCup(llLegendCupID);
    if (pCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find the kin object, LegendID=" << llLegendCupID);
        return;
    }

    // 里面把成员添加到杯赛信息里
    if (pCup->setCupDBCompetitionContext(pOutData, nOutLen) == false)
    {
        WarningLn(__FUNCTION__": set setCupDBCompetitionContext error!");
    }

    if (pCup->getLegendCupDBReadFlag() == emLegendRead_AllData)
    {
        readCupInfoDBFinished(llLegendCupID);
    }
}

void LegendCupService::onReturnCupRoundKinPos(char* pOutData, int nOutLen)
{
    int nTempLen = sizeof(DBREQ_RESULT_ROUND_GROUP_KIN_POS);
    if (nOutLen < nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_ROUND_GROUP_KIN_POS] length = " << nTempLen);
        return;
    }

    DBREQ_RESULT_ROUND_GROUP_KIN_POS* pRetData = (DBREQ_RESULT_ROUND_GROUP_KIN_POS*)pOutData;
    nTempLen = sizeof(DBREQ_RESULT_ROUND_GROUP_KIN_POS) + pRetData->nCount * sizeof(DBREQ_PARAM_ROUND_GROUP_KIN_POS);
    if (nOutLen != nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
        return;
    }
    LONGLONG llLegendCupID = pRetData->llLegendCupID;

    CLegendCup* pCup = findLegendCup(llLegendCupID);
    if (pCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find the cup object, LegendID=" << llLegendCupID);
        return;
    }

    // 设置每轮战队位置信息
    if (pCup->setCupDBRoundKinPos(pOutData, nOutLen) == false)
    {
        WarningLn(__FUNCTION__": set setCupDBCompetitionContext error!");
    }

    if (pCup->getLegendCupDBReadFlag() == emLegendRead_AllData)
    {
        readCupInfoDBFinished(llLegendCupID);
    }
}

void LegendCupService::onReturnCupRoundTimeInfo(char* pOutData, int nOutLen)
{
    int nTempLen = sizeof(DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO);
    if (nOutLen < nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_ROUND_GROUP_KIN_POS] length = " << nTempLen);
        return;
    }

    DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO* pRetData = (DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO*)pOutData;
    nTempLen = sizeof(DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO) + pRetData->nCount * sizeof(DBREQ_PARAM_ROUND_TIMEINFO);
    if (nOutLen != nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
        return;
    }
    LONGLONG llLegendCupID = pRetData->llLegendCupID;

    CLegendCup* pCup = findLegendCup(llLegendCupID);
    if (pCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find the cup object, LegendID=" << llLegendCupID);
        return;
    }

    // 设置每轮战队位置信息
    if (pCup->setCupDBRoundTimeInfo(pOutData, nOutLen) == false)
    {
        WarningLn(__FUNCTION__": set setCupDBCompetitionContext error!");
    }

    if (pCup->getLegendCupDBReadFlag() == emLegendRead_AllData)
    {
        readCupInfoDBFinished(llLegendCupID);
    }
}

// 返回玩家杯赛数据信息
void LegendCupService::onReturnLegendCupPlayerInfo(char* pOutData, int nOutLen)
{
	int nTempLen = sizeof(DBREQ_RESULT_READ_PLAYER_CUP_DATA_HEAD);
	if (nOutLen < nTempLen)
	{
		ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_READ_PLAYER_CUP_DATA_HEAD] length = " << nTempLen);
		return;
	}

	DBREQ_RESULT_READ_PLAYER_CUP_DATA_HEAD* pRetData = (DBREQ_RESULT_READ_PLAYER_CUP_DATA_HEAD*)pOutData;
	nTempLen = sizeof(DBREQ_RESULT_READ_PLAYER_CUP_DATA_HEAD) + pRetData->nCount * sizeof(DBREQ_RESULT_READ_PLAYER_CUP_INFO);
	if (nOutLen != nTempLen)
	{
		ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << " right length = " << nTempLen);
		return;
	}

	int offset = sizeof(DBREQ_RESULT_READ_PLAYER_CUP_DATA_HEAD);
	for (int i = 0; i < pRetData->nCount; i ++)
	{
		DBREQ_RESULT_READ_PLAYER_CUP_INFO* pCupPlayerInfo = (DBREQ_RESULT_READ_PLAYER_CUP_INFO*)(pOutData + offset);
		TMap_LegendCupPlayerListIt it = m_LegendCupPlayerList.find(pCupPlayerInfo->dwPDBID);
		if (it == m_LegendCupPlayerList.end())
		{
			SLegendCupPlayerInfo sCupPlayerInfo;
			sCupPlayerInfo.dwPDBID = pCupPlayerInfo->dwPDBID;
			sCupPlayerInfo.nClanCupPrizeCount = pCupPlayerInfo->nClanCupPrizeCount;
			sCupPlayerInfo.nRecvPrizeTime = pCupPlayerInfo->nRecvPrizeTime;

			if (!g_ExternalFacade.IsSameMonth(sCupPlayerInfo.nRecvPrizeTime, (DWORD)time(NULL)))
			{
				sCupPlayerInfo.nClanCupPrizeCount = 0;
				sCupPlayerInfo.nRecvPrizeTime = (int)time(NULL);
			}

			m_LegendCupPlayerList.insert(pair<DWORD, SLegendCupPlayerInfo>(sCupPlayerInfo.dwPDBID, sCupPlayerInfo));
		}

		offset += sizeof(DBREQ_RESULT_READ_PLAYER_CUP_INFO);
	}
}

void LegendCupService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {

    case EVENT_SYSTEM_STARTFINISH:
        {
            IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
            if (pDateTriggerService != NULL)
            {
                SDateTriggerInfo sTriggerInfo;
                sTriggerInfo.year = -1;
                sTriggerInfo.month = -1;
                sTriggerInfo.day = -1;
                sTriggerInfo.hour = 0;
                sTriggerInfo.min = 0;
                sTriggerInfo.wday = -1;
                pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_LegendCupUpdate, this, sTriggerInfo);	// 订阅时刻定时器（每晚00：00）
            }

            // 读取所有比赛数据库信息
            //readAllCupInfoFromDB();
        }
        break;

    case EVENT_KIN_READ_DB_FINISHED:
        {
            // 战队信息读取完毕后读取所有杯赛数据库信息
            readAllCupInfoFromDB();
        }
        break;

    case EVENT_KIN_DELMEMBER:
        {
            // 战队成员移除
            onEventKinMemberDelte(pszContext, nLen);
        }
        break;

    case EVENT_KIN_QUIT:
        {
            // 战队退出
            onEventKinMemberQuit(pszContext, nLen);
        }
        break;

    case EVENT_KIN_LOGIN:
        {
            // 战队成员登陆
            onEventKinMemberLogin(pszContext, nLen);
        }
        break;
        
    
    default:
        break;
    }
}


void LegendCupService::onServerListUpdated()
{

}

void LegendCupService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}


void LegendCupService::OnLogin(ISharePerson * pSharePerson, ELoginMode nLineType)
{
	// 动态场景 不提示
	if (nLineType != elogin_online)
		return;

	PDBID dwPDBID = pSharePerson->GetNumProp(PROPERTY_ID);

	if (IsComputerPDBID(dwPDBID))
		return;

	TMap_LegendCupPlayerListIt it = m_LegendCupPlayerList.find(dwPDBID);
	if (it != m_LegendCupPlayerList.end())
		return;

	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return;

	// 读取玩家杯赛数据信息
	obuf ob;
	DBREQ_PARAM_READ_PLAYER_CUP_DATA_HEAD refer;
	refer.nCount = 1;

	DBREQ_PARAM_READ_PLAYER_CUP_INFO nodeInfo;
	nodeInfo.dwPDBID = dwPDBID;

	int nNodeLen = refer.nCount * sizeof(DBREQ_PARAM_READ_PLAYER_CUP_INFO);

	ob.push_back(&refer, sizeof(DBREQ_PARAM_READ_PLAYER_CUP_DATA_HEAD));
	ob.push_back(&nNodeLen, sizeof(nNodeLen));
	ob.push_back(&nodeInfo, sizeof(DBREQ_PARAM_READ_PLAYER_CUP_INFO));

	if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_PLAYER_LEGENDCUP_DATA, 0, ob.data(), ob.size(), this) == false)
	{
		ErrorLn(__FUNCTION__" : GAMEDB_REQUEST_READ_PLAYER_LEGENDCUP_DATA request error!");
	}	
}

void LegendCupService::OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType)
{
    // 玩家退出
    if (pSharePerson == NULL)
    {
        ErrorLn(__FUNCTION__": pSharePerson == NULL");
        return;
    }

    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.begin();
    for(;iter != m_AllLegendCupMgr.end(); ++iter )
    {
        iter->second->onActorLogOut(pSharePerson->GetNumProp(PROPERTY_ID));
    }

    return;
}

void LegendCupService::OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{

}

void LegendCupService::OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{

}

void LegendCupService::DateCome(unsigned long nTriggerID)
{
    if (nTriggerID == DCT_DataTrigger_LegendCupUpdate)
    {
        TraceLn("--------------------------Date Come Refresh LegendCupInfo--------------------------");
    }
}

// 设置杯赛比赛结果
void LegendCupService::setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult)
{
    //设置杯赛比赛结果
    CLegendCup* pLegendCup = findLegendCup(sNodeWarResult.llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<sNodeWarResult.llLegendCupID);
        return;
    }
    
    // 记录杯赛比赛结果
    pLegendCup->recordCupWarResult(&sNodeWarResult);
}

void LegendCupService::setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState)
{
    // 改变杯赛节点状态
    CLegendCup* pLegendCup = findLegendCup(sNodeState.llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<sNodeState.llLegendCupID);
        return;
    }
    
    // 记录杯赛比赛结果
    pLegendCup->setLegendCupNodeState(&sNodeState);
}

void LegendCupService::handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen)
{
    //LEDENDCUP_TRACELN(__FUNCTION__": serverID = " << serverID<<" byKeyAction ="<<head.byKeyAction );
    // todo 转发到公共区

    // 公共区转发到自己的公共区
    void* data = pData;
    size_t len = nLen;
    switch( head.byKeyAction )
    {

    case SO_CUP_CREATECUP_CHECK:                     // 创建杯赛检查    
        {
            onServerMsgCreateCupCheck(serverID, &head, data, len);
        }
        break;

    case SO_CUP_CREATE_CUP_RESLUT:                     // 请求社会服创建杯赛    
        {
            onServerMsgCreateCupResult(serverID, &head, data, len);
        }
        break;

    case SO_CUP_REQ_JOIN_LEGENDCUP_ROOM:               // 请求加入杯赛房间
        {
            onServerMsgReqJoinLegendcupRoom(serverID, &head, data, len);
        }
        break;

	case SO_CUP_RECORD_WAR_RESULT:					// 单个比赛结果
		{
			onServerMsgCupRecordWarResult(serverID, &head, data, len);
		}
		break;

    case SO_CUP_REQ_JOIN_CUP_SUCCESS:               // 请求加入杯赛成功
        {
            onServerMsgReqJoinCupSuccess(serverID, &head, data, len);
        }
        break;

    default:
        break;
    }
}



void LegendCupService::handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen)
{
    // 公共区转发到自己的公共区
    void* data = pData;
    size_t len = nLen;
    switch( head.byKeyAction )
    {
        
    case CO_REQUEST_CUP_LIST:                               // 查看所有杯赛列表    
        {
            onClientRequestCupList(client, &head, data, len);
        }
        break;

    case CO_REQ_JOIN_CUP:                               // 请求社会服加入杯赛    
        {
            onClientMsgReqJoinCup(client, &head, data, len);
        }
        break;

    case CO_REQ_EXIT_CUP:                               //  请求退出杯赛    
        {
            onClientMsgExitCup(client, &head, data, len);
        }
        break;

    case CO_REQ_ADD_TO_BLACKLIST:                     // 请求移动到黑名单    
        {
            onClientMsgMoveToBlackList(client, &head, data, len);
        }
        break;

    case CO_REQ_REMOVE_BLACKLIST:                     // 请求移动到黑名单    
        {
            onClientMsgRemoveToBlackList(client, &head, data, len);
        }
        break;

    case CO_REQ_LOOK_OVER_LEGENDCUP:                     // 请求查看所有参赛队伍信息    
        {
            onClientMsgLoockOverLegendCup(client, &head, data, len);
        }
        break;

    case CO_REQ_ALL_BALCKLIST:                     // 请求移动到黑名单    
        {
            onClientMsgRequestAllBlackKinlist(client, &head, data, len);
        }
        break;

    case CO_REQ_KIN_MEMBER_LIST:                     // 请求战队成员列表   
        {
            onClientMsgRequestKinMemberList(client, &head, data, len);
        }
        break;

    case CO_REQ_CUP_JUDGE_SINGAL_WAR_RESULT:         // 请求参加比赛    
        {

            onClientMsgRequestJudgeWarResult(client, &head, data, len);
        }
        break;

    case CO_REQ_LOOK_UP_CUP_NODE_WARINFO:            // 请求查看比赛节点    
        {
            onClientMsgRequestLookUpCupNodeWarInfo(client, &head, data, len);
        }
        break;

    case CO_REQ_PUBLIC_COMPETTIONNODE:               // 请求发布比赛   
        {
            onClientPublicCompetition(client, &head, data, len);
        }
        break;
    case CO_REQ_QUIT_OBSERVE:                       // 退出查看比赛
        {
            onClientCloseCupView(client, &head, data, len);
        }
        break;

    case CO_REQ_OFFLINE_PRE_END_REGISTER:           // 请求预先结束线下杯赛报名阶段
        {
            onClientOfflineEndRegisterState(client, &head, data, len);
        }
        break;

    default:
        break;
    }
    
}

void LegendCupService::onClientMsgLoockOverLegendCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgRequestLookOverCup))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    SMsgRequestLookOverCup* pMsg = (SMsgRequestLookOverCup*) pData;
    CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<pMsg->llLegendCupID);
        return;
    }
    
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        return;
    }
	SSharePersonProperty sShareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
	if (sShareProperty.dwPDBID == INVALID_PDBID)
	{
		return;
	}

    // 添加到观察者
    DWORD dwActorID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;
    pLegendCup->addLegendCupViewer(dwActorID);

	//下发基础数据
	pLegendCup->updateSingleCupBaseInfo2Client(sShareProperty.clientID, sShareProperty.dwKinID);
    // 根据比赛阶段下发数据
    if (pLegendCup->getLegendCupBaseInfo()->nCurrentRound <= 0)
    {
        pLegendCup->sendRegisterList2Client(clientID);
    }
    else
    {
        pLegendCup->sendAllRoundInfo2Client(clientID);
    }

}

void LegendCupService::onClientMsgRequestAllBlackKinlist(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgRequestAllBlackKinList))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    SMsgRequestAllBlackKinList* pMsg = (SMsgRequestAllBlackKinList*) pData;
    CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<pMsg->llLegendCupID);
        return;
    }

    pLegendCup->sendBlackList2Client(clientID);
    
}

void LegendCupService::onClientMsgRequestKinMemberList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgRequestKinMember))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    SMsgRequestKinMember* pMsg = (SMsgRequestKinMember*) pData;
    CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<pMsg->llLegendCupID);
        return;
    }

    pLegendCup->sendKinMember2Client(clientID, pMsg->dwKinID);

}

void LegendCupService::onClientMsgRequestJudgeWarResult(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgJudgeWarResult))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    SMsgJudgeWarResult* pMsg = (SMsgJudgeWarResult*) pData;
    pMsg->clientID = clientID;
    CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<pMsg->llLegendCupID);
        return;
    }
    // 处理节点结果
    pLegendCup->judgeNodeWarResult(pMsg);
    
}


void LegendCupService::onClientMsgRequestLookUpCupNodeWarInfo(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgLookUpCupNodeWarInfo))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    SMsgLookUpCupNodeWarInfo* pMsg = (SMsgLookUpCupNodeWarInfo*) pData;
    pMsg->clientID = clientID;
    
    CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<pMsg->llLegendCupID);
        return;
    }

    //请求查看节点信息
    pLegendCup->lookUpCupNodeWarInfo(pMsg);
}


void LegendCupService::readAllCupInfoFromDB()
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return;
    }

    DWORD dwWorldID = getThisGameWorldID();
	DBREQ_PARAM_READCUPLIST dbParam;
	dbParam.dwWordID = dwWorldID;
    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_ALL_LEGEND_CUP, dwWorldID, (LPCSTR)(&dbParam), sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(" executeSP GAMEDB_REQUEST_READ_ALL_LEGEND_CUP faild!");
        return;
    }

}

void LegendCupService::readIniCupPlusData(LONGLONG llLegendCupID)
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    
    if (pDBEngine == NULL)
    {
        return;
    }

    CLegendCup* pCup = (CLegendCup*)findLegendCup(llLegendCupID);
    if (pCup == NULL)
    {
        WarningLn("pCup == NULL llLegendCupID=" << llLegendCupID);
        return;
    }

    // 读取参赛列表 黑名单列表 排名列表...
    DBREQ_REQUEST_EX_LIST_DATA_INFO readEXList;
    readEXList.dwWorldID = getThisGameWorldID();
    readEXList.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_EX_LIST_DATA_INFO, 0, (LPCSTR)(&readEXList), sizeof(readEXList), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_EX_LIST_DATA_INFO request error!");
    }

    // 如果已经有的状态不用读取玩家列表 和 比赛信息
    if (pCup->getLegendCupState() < SATE_GROUPING)
    {
        pCup->setLegendCupDBReadFlag(emLegendRead_AllData);
        m_ReadDBRecord.erase(llLegendCupID);
        return;
    }

    // 读取单个杯赛的其他信息
    // 读取每轮比赛时间信息
    DBREQ_PARAM_READ_CUP_ROUND_TIMEINFO readRoundTime;
    readRoundTime.dwWorldID = getThisGameWorldID();
    readRoundTime.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_CUP_ROUND_TIMEINFO, 0, (LPCSTR)(&readRoundTime), sizeof(readRoundTime), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_READ_CUP_ROUND_TIMEINFO request error!");
    }


    // 读取战队成员列表请求
    DBREQ_PARAM_READCUPMEMBERLIST readMemberList;
    readMemberList.dwWorldID = getThisGameWorldID();
    readMemberList.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_LEGEND_CUP_MEMBERLIST, 0, (LPCSTR)(&readMemberList), sizeof(readMemberList), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_LEGEND_CUP_MEMBERLIST request error!");
    }

    //// 读取杯赛战队信息
    DBREQ_PARAM_READ_ALL_KININFO readKinInfo;
    readKinInfo.dwWorldID   = getThisGameWorldID();
    readKinInfo.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_ALL_KININFO, 0, (LPCSTR)(&readKinInfo), sizeof(readKinInfo), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_READ_ALL_KININFO request error!");
    }

    DBREQ_PARAM_REQUEST_READ_ROUND_GROUP_KIN_POS readInfo;
    readInfo.dwWorldID = getThisGameWorldID();
    readInfo.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_ROUND_GROUP_KIN_POS, 0, (LPCSTR)(&readInfo), sizeof(readInfo), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_READ_ROUND_GROUP_KIN_POS request error!");
    }

    //// 读取杯赛所有比赛信息
    DBREQ_PARAM_REQUEST_READ_CUP_COMPETITION_NODE readCompetitionInfo;
    readCompetitionInfo.dwWorldID = getThisGameWorldID();
    readCompetitionInfo.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_ALL_CUP_COMPETITION_NODE, 0, (LPCSTR)(&readCompetitionInfo), sizeof(readCompetitionInfo), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_READ_ALL_CUP_COMPETITION_NODE request error!");
    }
}

CLegendCup* LegendCupService::findLegendCup(LONGLONG llLegendCupID)
{
    map<LONGLONG, BYTE>::iterator iter = m_LegendCupTypeList.find(llLegendCupID);
    if (iter == m_LegendCupTypeList.end())
    {
        return NULL;
    }
    BYTE eCupType = iter->second;
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(eCupType);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        return NULL;
    }
    return iterCupMgr->second->findCup(llLegendCupID);
}


void LegendCupService::addCupAllKinMapedLegendID(CLegendCup *pLegendCup)
{
    if (pLegendCup == NULL)
    {
        return;
    }

    if (pLegendCup->isEnd())
    {
        return;
    }

    map<LONGLONG, BYTE>::iterator iter = m_LegendCupTypeList.find(pLegendCup->getLegendCupID());
    if (iter == m_LegendCupTypeList.end())
    {
        return;
    }
    BYTE eCupType = iter->second;
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(eCupType);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        return;
    }
 
    iterCupMgr->second->addCupAllKinMapedLegendID(pLegendCup);
}


void LegendCupService::readCupInfoDBFinished(LONGLONG llLegendCupID)
{
    m_ReadDBRecord.erase(llLegendCupID);

    if (m_ReadDBRecord.size() == 0)
    {
        bReadDBCupInfoFinished = true;
    }
}

void LegendCupService::onClientRequestCupList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    // 请求杯赛列表信息
    if (clientID == INVALID_CLIENT)
    {
        ErrorLn(__FUNCTION__ ": the clientID is invalid, clientID="<< clientID);
        return;
    }
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        return;
    }
    DWORD dwRequestKinID = pShareReceiver->GetSharePersonByClientID(clientID).dwKinID;

    // 所有杯赛信息并下发
    obuf ob;

    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_ALL_LEGENDCUP;
    ob<<NetMsgHead;

    SMsgLegendCupListHead LegendCuphead;
    LegendCuphead.nCount = 0;

    obuf LegendCupBuf;
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.begin();
    for(;iter != m_AllLegendCupMgr.end(); ++iter )
    {
        // 获取所有杯赛相关 默认系统比赛在最前面
		if (iter->second == NULL)
		{
			continue;
		}

        LegendCuphead.nCount += iter->second->getAllCupBaseInfo(LegendCupBuf,dwRequestKinID);
    }
    // 组织数据相关
    ob<<LegendCuphead;
    ob.push_back(LegendCupBuf.data(), LegendCupBuf.size());

    send2Client(clientID, ob);
}

void LegendCupService::onClientPublicCompetition(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgPublicCompetition))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    if (clientID == INVALID_CLIENT)
    {
        ErrorLn(__FUNCTION__ ": the clientID is invalid, clientID="<< clientID);
        return;
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }
    DWORD dwOperaterID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;

    SMsgPublicCompetition* pMsg = (SMsgPublicCompetition*) pData;
    pMsg->dwOperateID = dwOperaterID;
    // 发布比赛
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(pMsg->byCreateCupType);
    if (iter == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__":can't find LegendCupMgr byCreateCupType="<<pMsg->byCreateCupType);
        return;
    }

    //管理类进行申请
    iter->second->publicCompetition(pMsg);
}


void LegendCupService::onClientMsgReqJoinCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgJoinCup))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }

    SMsgJoinCup* pMsg = (SMsgJoinCup*) pData;
    pMsg->dwActorID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;
    pMsg->dwKinID = pShareReceiver->GetSharePersonByClientID(clientID).dwKinID;

    // 申请加入杯赛
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(pMsg->byCreateCupType);
    if (iter == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__":can't find LegendCupMgr byCreateCupType="<<pMsg->byCreateCupType);
        return;
    }

    //管理类进行申请检查
    iter->second->registerCupCheck(pMsg);

}

void LegendCupService::onClientMsgExitCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgExitCup))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__ ": pShareReceiver == NULL!");
        return;
    }
    // 队长判定
    KinHelper kinHelper;
    if (kinHelper.m_ptr == NULL)
    {
        ErrorLn(__FUNCTION__ ": kinHelper.m_ptr == NULL!");
        return;
    }

    // 退出申请
    SMsgExitCup* pMsg = (SMsgExitCup*) pData;
    pMsg->dwActorID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;
    pMsg->dwKinID = pShareReceiver->GetSharePersonByClientID(clientID).dwKinID;

    SKinInfo sKinInfo = kinHelper.m_ptr->getKinInfo(pMsg->dwKinID);
    if (sKinInfo.nKinProp_ShaikhID != pMsg->dwActorID)
    {
        // 不是战队队长无法申请
        g_ExternalFacade.showSystemMessage(pMsg->dwActorID, CHAT_TIP_LEGENDCUP_NOT_SHAIKH_CANT_REQUEST,"");
        return ;
    }

    map<LONGLONG, BYTE>::iterator iterType = m_LegendCupTypeList.find(pMsg->llLegendCupID);
    if (iterType == m_LegendCupTypeList.end())
    {
        ErrorLn(__FUNCTION__":can't find CreateCupType legendCupID ="<<pMsg->llLegendCupID);
        return;
    }

    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(iterType->second);
    if (iter == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__":can't find LegendCupMgr byCreateCupType="<<iterType->second);
        return;
    }

    // 管理类进行退出申请
    iter->second->exitRegisterCup(pMsg);

}

void LegendCupService::onClientOfflineEndRegisterState(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgOfflineEndRegister))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__ ": pShareReceiver == NULL!");
        return;
    }
    // 队长判定
    KinHelper kinHelper;
    if (kinHelper.m_ptr == NULL)
    {
        ErrorLn(__FUNCTION__ ": kinHelper.m_ptr == NULL!");
        return;
    }

    // 请求结束报名阶段
    SMsgOfflineEndRegister* pMsg = (SMsgOfflineEndRegister*) pData;
    pMsg->dwActorID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;
    pMsg->dwKinID = pShareReceiver->GetSharePersonByClientID(clientID).dwKinID;

    map<LONGLONG, BYTE>::iterator iterType = m_LegendCupTypeList.find(pMsg->llLegendCupID);
    if (iterType == m_LegendCupTypeList.end())
    {
        ErrorLn(__FUNCTION__":can't find CreateCupType legendCupID ="<<pMsg->llLegendCupID);
        return;
    }

    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(iterType->second);
    if (iter == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__":can't find LegendCupMgr byCreateCupType="<<iterType->second);
        return;
    }

    // 管理类进行退出申请
    iter->second->offlineEndRegister(pMsg);
}


void LegendCupService::onClientCloseCupView(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgReqQuitObserve))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__ ": pShareReceiver == NULL!");
        return;
    }

    SMsgReqQuitObserve* pMsg = (SMsgReqQuitObserve*)pData;
    PDBID dwPDBID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;
    LONGLONG llLegendCupID = pMsg->llLegendCupID;
    CLegendCup* pLegendCup = findLegendCup(llLegendCupID);
    if(pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__" can't find LegendCup llLegendCupID ="<<llLegendCupID);
        return;
    }
    pLegendCup->removeLegendCupViewer(dwPDBID);
}


void LegendCupService::onClientMsgMoveToBlackList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgAddToBlackList))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__ ": pShareReceiver == NULL!");
        return;
    }


    SMsgAddToBlackList* pMsg = (SMsgAddToBlackList*) pData;
    pMsg->dwOperateID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;
    
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(pMsg->byCreateCupType);
    if (iter == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__":can't find LegendCupMgr byCreateCupType="<<pMsg->byCreateCupType);
        return;
    }
    //管理类处理
    iter->second->addToBlackList(pMsg);
}

void LegendCupService::onClientMsgRemoveToBlackList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgRemoveBlackList))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__ ": pShareReceiver == NULL!");
        return;
    }

    SMsgRemoveBlackList* pMsg = (SMsgRemoveBlackList*) pData;
    pMsg->dwOperateID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;

    CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
    if (pLegendCup == NULL)
    {
		ErrorLn(__FUNCTION__": can't find legendcupID ="<<pMsg->llLegendCupID);
		return;

    }
    //管理类处理
    pLegendCup->removeBlackList(pMsg->dwOperateID, pMsg->dwKinID);
}


void LegendCupService::onServerMsgReqJoinLegendcupRoom(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgJoinLegendCupRoom))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }
    SMsgJoinLegendCupRoom* pMsg = (SMsgJoinLegendCupRoom*) pData;

    CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<pMsg->llLegendCupID);
        return;
    }

    //请求进入节点房间
    if (!pLegendCup->haveRightJoinCupRoom(pMsg))
    {
        return;
    }
    if (!pLegendCup->joinLegendCupRoom(pMsg))
    {
        // 无法进入比赛节点
        return;
    }

    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return;
    }

    //调用进入比赛房间接口
    bool bCanJoin = pMatchService->joinLegendCupRoom(*pMsg);
    if (!bCanJoin)
    {
        ErrorLn(__FUNCTION__": can't join room! legendCupID = "<<pLegendCup->getLegendCupBaseInfo()->llLegendCupID<<" serchID ="<<pMsg->nSerchID);
        return;
    }
}

bool LegendCupService::canCreateLegendCup(SMsgCreateCup* pCreateCup)
{
    if (pCreateCup == NULL)
    {
        return false;
    }
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(pCreateCup->byCreateCupType);
    if(iter == m_AllLegendCupMgr.end())
    {
       return false;
    }

    return iter->second->canCreateLegendCup(pCreateCup);
}


void LegendCupService::onServerMsgCreateCupCheck(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgCreateCup))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    SMsgCreateCup* pMsg = (SMsgCreateCup*) pData;
    if (!canCreateLegendCup(pMsg))
    {
        return;
    }

    // 可以创建杯赛 发送给场景服创建杯赛 扣除相关
    obuf256 obMsg;

    SNetMsgHead head;
    head.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    head.byDestEndPoint	= MSG_ENDPOINT_SCENE;
    head.byKeyModule	= MSG_MODULEID_LEGENDCUP;
    head.byKeyAction	= OS_CUP_CAN_CREATECUP;

    obMsg.push_back( &head, sizeof(head) );
    obMsg.push_back(pMsg, stLen);

    //取出场景服
    ICenterConnectorService *pCenterConnector = gSocialGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnector)
    {
        ErrorLn("MatchService::send2SceneServer failed( void * buf, size_t len )! nullptr == pCenterConnector");
        return;
    }

    //压入
    pCenterConnector->postMessage(dwServerID, obMsg.data(),(int)obMsg.size());
}

void LegendCupService::onServerMsgCreateCupResult(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(DBREQ_RESULT_CREATE_LEGEND_CUP))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }

    DBREQ_RESULT_CREATE_LEGEND_CUP* pMsg = (DBREQ_RESULT_CREATE_LEGEND_CUP*) pData;

    // 创建杯赛对象
    CLegendCup* pCup = new CLegendCup();
    if (pCup == NULL)
    {
        ErrorLn(__FUNCTION__": pCup == NULL  LegendID" << pMsg->llLegendCupID<<" dwCreataActorID = "<<pMsg->dwCreataActorID );
        return;
    }

    SLegendCupBaseInfo basInfo;
    basInfo.llLegendCupID           = pMsg->llLegendCupID;                      // 杯赛ID
    basInfo.nCreateConfigID         = pMsg->nCreateConfigID;                    // 创建的配置ID
    basInfo.byCreateCupType         = pMsg->byCreateCupType;                    // 杯赛类型 1 系统 2 个人
    basInfo.bIsOnlineCup            = pMsg->bIsOnlineCup;                       // 线上比赛 线下比赛
    basInfo.dwCreataActorID         = pMsg->dwCreataActorID;                    // 创建玩家 系统的为 0
    basInfo.byMaxKinNumber          = pMsg->byMaxKinNumber;                     // 最大参赛队伍
    basInfo.dwRegisterDeadline       = pMsg->dwRegisterDeadline;                // 报名截止时间
    basInfo.dwConfirmRegisterDeadline = pMsg->dwConfirmRegisterDeadline;        // 确定最终报名截止时间
    basInfo.bClanLimit              = pMsg->bClanLimit;                         // 只有联盟限制才能参加
    basInfo.dwClanID                = pMsg->dwClanID;                           // 联盟ID
    basInfo.nNeedRoundCount         = pMsg->nNeedRoundCount;                    // 杯赛需要打几轮
    basInfo.nLevelLimit             = pMsg->nLevelLimit;                        // 等级限制
    basInfo.dwContriBonus           = pMsg->dwContriBonus;                      // 出资奖励
    basInfo.dwTotalRegisterTicket   = pMsg->dwTotalRegisterTicket;              // 报名总共累积的点卷
    basInfo.dwRegisterNeedTicket    = pMsg->dwRegisterNeedTicket;               // 报名需要的点卷
    basInfo.byTicketAllocationMode  = pMsg->byTicketAllocationMode;             // 奖励分配方式 1、 系统设置 2、玩家自定义 ETicketAllocateMode  
    
    basInfo.bHaveRewards            = false;                                    // 发放奖励标志
    basInfo.eCupState               = SATE_REGISTER;                            // 状态 ECupState
    basInfo.nCurrentRound           = 0;                                        // 当前打到第几轮
    basInfo.bIsEnd                  = false;                                    // 是否结束
    basInfo.dwEndTime               = 0;                                        // 结束日期

    memcpy(basInfo.byRankTicketRatio, pMsg->byRankTicketRatio, sizeof(basInfo.byRankTicketRatio));// 分配奖励比例
    sstrcpyn(basInfo.szCupName, pMsg->szCupName, sizeof(basInfo.szCupName));    // 杯赛名称
    sstrcpyn(basInfo.szPassword, pMsg->szPassword, sizeof(basInfo.szPassword)); // 参赛密码
    sstrcpyn(basInfo.szCreaterName, pMsg->szCreaterName, sizeof(basInfo.szCreaterName)); // 参赛密码

    if (pCup->Create(this, &basInfo) == false)
    {
        ErrorLn(__FUNCTION__": LegendID" << pMsg->llLegendCupID);
        return;
    }

    // todo 广播通知创建了什么比赛
    if (pMsg->dwCreataActorID != INVALID_PDBID)
    {
        g_ExternalFacade.showSystemMessage(pMsg->dwCreataActorID, CHAT_TIP_LEGENDCUP_CREATE_SUCCESS,"");
    }

    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    SSharePersonProperty shareProperty= pShareReceiver->GetSharePerson(pMsg->dwCreataActorID);
    if(pShareReceiver != NULL)
    {
        pCup->updateSingleCupBaseInfo2Client(shareProperty.clientID, shareProperty.dwKinID, true); 
    }

    addLegendCupToMgr(pCup);

    // 创建杯赛日志
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        string strPassWord = pCup->havePassword() == true? a2utf8("需要密码"): a2utf8("不需要密码");
        string strClanLimit = basInfo.bClanLimit == true? a2utf8("联盟限定"): a2utf8("非联盟限定");
        string strOnlineOffLine = basInfo.bIsOnlineCup == true? a2utf8("线上"): a2utf8("线下");
        time_t dwRegisterDeadline = basInfo.dwRegisterDeadline;
        tm *pRegisterDeadline = localtime(&dwRegisterDeadline);
        char tRegisterDeadline[56] = {0};
        sprintf_s(tRegisterDeadline, sizeof(tRegisterDeadline), "%d-%.2d-%.2d %.2d:%.2d:%.2d", 
            1900+pRegisterDeadline->tm_year,
            pRegisterDeadline->tm_mon+1,
            pRegisterDeadline->tm_mday,
            pRegisterDeadline->tm_hour,
            pRegisterDeadline->tm_min,
            pRegisterDeadline->tm_sec);

        time_t dwConfirmRegisterDeadline = basInfo.dwConfirmRegisterDeadline;
        tm *pConfirmRegisterDeadline = localtime(&dwConfirmRegisterDeadline);
        char tConfirmRegisterDeadline[56] = {0};
        sprintf_s(tConfirmRegisterDeadline, sizeof(tConfirmRegisterDeadline), "%d-%.2d-%.2d %.2d:%.2d:%.2d", 
            1900+pRegisterDeadline->tm_year,
            pConfirmRegisterDeadline->tm_mon+1,
            pConfirmRegisterDeadline->tm_mday,
            pConfirmRegisterDeadline->tm_hour,
            pConfirmRegisterDeadline->tm_min,
            pConfirmRegisterDeadline->tm_sec);

        char szText[512] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("举办杯赛(%s)：[%u] 角色名[%s] 举办杯赛（比赛id[%u] 名称[%s] 赞助奖金[%u]）%s 密码:(%s)，%s, 报名时限: [%s], 确定参赛时限:[%s]"), 
            strOnlineOffLine.c_str(),shareProperty.dwPDBID, shareProperty.szName, basInfo.llLegendCupID, basInfo.szCupName, basInfo.dwContriBonus, strPassWord.c_str(), pCup->getLegendCupBaseInfo()->szPassword, strClanLimit.c_str(),tRegisterDeadline ,tConfirmRegisterDeadline);

        pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, basInfo.llLegendCupID, szText);
    }

	// 如果是联盟杯赛 则通知联盟
	if (pMsg->byCreateCupType == emCupType_Clan && pMsg->dwClanID > 0)
	{
		ClanHelper clanHelper;
		IClanService *pClanService = clanHelper.m_ptr;
		if (pClanService != NULL)
		{
			pClanService->notifyCreateClanLegendCup(pMsg->dwClanID, pMsg->llLegendCupID, pMsg->byMaxKinNumber, pMsg->szCupName);
		}
	}

	// 普通无密码杯赛，自动发一条邀请链接
	if (pMsg->byCreateCupType != emCupType_Clan	&& strcmp(pMsg->szPassword, "") == 0)
	{
		ChatHelper chatHelper;
		IChatService *pChatService = chatHelper.m_ptr;
		if (NULL == pChatService)
		{
			ErrorLn(__FUNCTION__": failed! nullptr == pChatService" );
			return;
		}

		string strParam = "";
		strParam += CHAT_MSG_ITEM_FLAG;

		char szText[256] = {0};
		ssprintf_s(szText, sizeof(szText), "%d;%s;%d;%d;%d;%s;%lld", CHAT_CHANNEL_WORLD, pMsg->szCupName, 0, pMsg->byMaxKinNumber, pMsg->dwRegisterNeedTicket, strParam.c_str(), pMsg->llLegendCupID);

		pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_WORLD, pMsg->dwCreataActorID, CHAT_TIP_LEGENDCUP_CREATE_INVITE_REGIST_TIP, szText);
	}
}

void LegendCupService::onServerMsgReqJoinCupSuccess(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
    if (pData == NULL || stLen != sizeof(SMsgJoinCup))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
        return;
    }
    SMsgJoinCup* pMsg = (SMsgJoinCup*) pData;

    CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<pMsg->llLegendCupID);
        return;
    }

    if (!pLegendCup->registerCheck(pMsg))
    {
        ErrorLn(__FUNCTION__": registerCheck cant find the legendCupID ="<<pMsg->llLegendCupID);
        return;
    }

    // 找到管理类添加杯赛
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(pLegendCup->getLegendCupBaseInfo()->byCreateCupType);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__": can't find LegendCupMgr llLegendCupID="<<pMsg->llLegendCupID);
        return;
    }

    if (iterCupMgr->second)
    {
        iterCupMgr->second->addRegisterCup(pMsg);
    }
    
}

// 处理场景服单个比赛结果
void LegendCupService::onServerMsgCupRecordWarResult(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen)
{
	if (pData == NULL || stLen != sizeof(SMsgRecordNodeWarResult))
	{
		ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
		return;
	}
	SMsgRecordNodeWarResult* pMsg = (SMsgRecordNodeWarResult*) pData;

	CLegendCup* pLegendCup = findLegendCup(pMsg->llLegendCupID);
	if (pLegendCup == NULL)
	{
		ErrorLn(__FUNCTION__": cant find the legendCupID ="<<pMsg->llLegendCupID);
		return;
	}
	// 记录战队战绩结果
	IKinService *pKinService = KinHelper().m_ptr;
	if (pKinService == NULL)
	{
		ErrorLn(__FUNCTION__ ": pKinService == NULL");
		return;
	}
	SKinWarRecordNode recordNode;
	recordNode.llWarID = pMsg->llWarID;							// 战场ID
	recordNode.dwKin1ID = pMsg->dwKin1ID;						// 战队1ID
	recordNode.dwKin2ID = pMsg->dwKin2ID;						// 战队2ID
	recordNode.dwBeginTime = pMsg->dwBeginTime;					// 战场开始时间
	recordNode.dwWinKin = pMsg->dwWinnerKinID;					// 胜利战队ID
	sstrcpyn(recordNode.szKin1Name, pKinService->getKinInfo(pMsg->dwKin1ID).szName, sizeof(recordNode.szKin1Name)); // 战队1名称
	sstrcpyn(recordNode.szKin2Name, pKinService->getKinInfo(pMsg->dwKin2ID).szName, sizeof(recordNode.szKin2Name)); // 战队2名称
	memcpy(recordNode.nKin1Glory, pMsg->nKin1Glory, sizeof(recordNode.nKin1Glory));
	memcpy(recordNode.nKin2Glory, pMsg->nKin2Glory, sizeof(recordNode.nKin2Glory));
	pKinService->addKinLegendWarResult(recordNode);

	// 记录杯赛比赛结果
	pLegendCup->recordCupWarResult(pMsg);
}

bool LegendCupService::addLegendCupToMgr(CLegendCup *pLegendCup)
{
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": pLegendCup == NULL");
        return false;
    }

    BYTE byCreateCupType = pLegendCup->getLegendCupBaseInfo()->byCreateCupType;
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(byCreateCupType);

    if (iter == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__": can't find LegendCupMgr LegendCupTypeID = "<<byCreateCupType);
        return false;
    }
    if (!iter->second->addLegendCup(pLegendCup))
    {
        ErrorLn(__FUNCTION__": can't find add to legendCupMgr error! LegendCupID = "<<pLegendCup->getLegendCupID());
    }
   
    // 添加到映射
    m_LegendCupTypeList[pLegendCup->getLegendCupID()] = byCreateCupType;
    return true;
}

void LegendCupService::onEventKinMemberDelte(LPCSTR pszContext, int nLen)
{
    if(pszContext == NULL || nLen != sizeof(SEventKin_DelMember))
    {
        return;
    }
    SEventKin_DelMember* pEvent = (SEventKin_DelMember*)pszContext;

    // 处理玩家退出战队
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.begin();
    for (;iter != m_AllLegendCupMgr.end(); ++iter)
    {
        CLegendCup* pLegendCup = iter->second->findKinJoinedLegendCup(pEvent->dwKinID);
        if(pLegendCup == NULL)
        {
            continue;
        }
        pLegendCup->deleteKinMember(pEvent->dwActorID);
    }
}

void LegendCupService::onEventKinMemberQuit(LPCSTR pszContext, int nLen)
{
    if(pszContext == NULL || nLen != sizeof(SEventKin_Quit))
    {
        return;
    }

    SEventKin_Quit* pEvent = (SEventKin_Quit*)pszContext;
    // 处理玩家退出战队
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.begin();
    for (;iter != m_AllLegendCupMgr.end(); ++iter)
    {
        CLegendCup* pLegendCup = iter->second->findKinJoinedLegendCup(pEvent->dwKinID);
        if(pLegendCup == NULL)
        {
            continue;
        }
        if (pLegendCup->getLegendCupBaseInfo()->bIsEnd)
        {
            continue;
        }
        pLegendCup->deleteKinMember(pEvent->dwActorID);
    }
}

void LegendCupService::onEventKinMemberLogin(LPCSTR pszContext, int nLen)
{
    
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }

    if(pszContext == NULL || nLen != sizeof(SEventKin_Quit))
    {
        return;
    }

    SEventKin_MemberLogin* pEvent = (SEventKin_MemberLogin*)pszContext;
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pEvent->dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        return;
    }

    // 动态地图中不触发
    if (shareProperty.dwSceneID > MAP_MAXCOUNT)
    {
        return;
    }

    // 处理战队角色登陆
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.begin();
    for (;iter != m_AllLegendCupMgr.end(); ++iter)
    {
        CLegendCup* pLegendCup = iter->second->findKinJoinedLegendCup(pEvent->dwKinID);
        if(pLegendCup == NULL)
        {
            continue;
        }

        if (pLegendCup->isEnd())
        {
            continue;
        }

        pLegendCup->onKinMemberLogin(pEvent->dwPDBID);
    }
    
}


void LegendCupService::endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID)
{
    map<LONGLONG, BYTE>::iterator iter = m_LegendCupTypeList.find(llLegendCupID);
    BYTE byCupType =  emCupType_None;
    if (iter == m_LegendCupTypeList.end())
    {
        ErrorLn(__FUNCTION__": can't find CupType llLegendCupID="<<llLegendCupID);
        return;
        
    }
    byCupType = iter->second;

    // 找到杯赛管理类
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(byCupType);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__": can't find LegendCupMgr llLegendCupID="<<llLegendCupID);
        return;
    }

    if (iterCupMgr->second)
    {
        iterCupMgr->second->endCupRemoveCreaterRecInfo(llLegendCupID);
    }

}
void LegendCupService::removeServiceCupInfo(LONGLONG llLegendCupID)
{
    map<LONGLONG, BYTE>::iterator iter = m_LegendCupTypeList.find(llLegendCupID);
    BYTE byCupType =  emCupType_None;
    if (iter == m_LegendCupTypeList.end())
    {
        ErrorLn(__FUNCTION__": can't find CupType llLegendCupID="<<llLegendCupID);
        return;

    }
    byCupType = iter->second;

    // 找到杯赛管理类
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(byCupType);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__": can't find LegendCupMgr llLegendCupID="<<llLegendCupID);
        return;
    }

    if (iterCupMgr->second)
    {
        iterCupMgr->second->removeServiceCupInfo(llLegendCupID);
    }

    m_LegendCupTypeList.erase(iter);
}


// 保存杯赛玩家数据toDB
void LegendCupService::SaveLegendCupPlayerInfoToDB(DWORD dwPDBID)
{
	TMap_LegendCupPlayerListIt it = m_LegendCupPlayerList.find(dwPDBID);
	if (it == m_LegendCupPlayerList.end())
		return;

	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return;

	// 更新玩家杯赛数据信息
	DBREQ_PARAM_UPDATE_PLAYER_LEGENDCUP_DATA refer;
	refer.dwPDBID = dwPDBID;
	refer.nClanCupPrizeCount = (*it).second.nClanCupPrizeCount;
	refer.nRecvPrizeTime = (*it).second.nRecvPrizeTime;

	if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_PLAYER_LEGENDCUP_DATA, 0, (LPCSTR)(&refer), sizeof(DBREQ_PARAM_UPDATE_PLAYER_LEGENDCUP_DATA), this) == false)
	{
		ErrorLn(__FUNCTION__" : GAMEDB_REQUEST_UPDATE_PLAYER_LEGENDCUP_DATA request error!");
	}	
}

void LegendCupService::luaAddCupRegister(LONGLONG llLegendCupID, PACKAGE_PTR pkKinList)
{
    map<LONGLONG, BYTE>::iterator iter = m_LegendCupTypeList.find(llLegendCupID);
    if (iter == m_LegendCupTypeList.end())
    {
        return;
    }
    BYTE eCupType = iter->second;
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(eCupType);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        return ;
    }
    CLegendCup* pLegendCup = iterCupMgr->second->findCup(llLegendCupID);
}

void LegendCupService::luaRemoveCupRegister(LONGLONG llLegendCupID)
{
    map<LONGLONG, BYTE>::iterator iter = m_LegendCupTypeList.find(llLegendCupID);
    if (iter == m_LegendCupTypeList.end())
    {
        return;
    }
    BYTE eCupType = iter->second;
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(eCupType);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        return;
    }

    CLegendCup* pLegendCup = iterCupMgr->second->findCup(llLegendCupID);
}

void LegendCupService::exitStartCompetitionNode(LONGLONG llLegendCupID, int nSerchID, DWORD dwNewStartTime)
{
    map<LONGLONG, BYTE>::iterator iter = m_LegendCupTypeList.find(llLegendCupID);
    if (iter == m_LegendCupTypeList.end())
    {
        ErrorLn(__FUNCTION__": can't find legendCup Type LegendCupID="<<llLegendCupID);
        return;
    }

    BYTE eCupType = iter->second;
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(eCupType);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__": can't find iterCupMgr LegendCupID="<<llLegendCupID);
        return;
    }

    CLegendCup* pLegendCup = iterCupMgr->second->findCup(llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find LegendCup LegendCupID="<<llLegendCupID);
        return;
    }
    
    pLegendCup->exitStartCompetitionNode( nSerchID, dwNewStartTime);
}

void LegendCupService::sendAbstentionKinMail(LONGLONG llLegendCupID, DWORD dwFailedKinID)
{
	map<LONGLONG, BYTE>::iterator iter = m_LegendCupTypeList.find(llLegendCupID);
	if (iter == m_LegendCupTypeList.end())
	{
		ErrorLn(__FUNCTION__": can't find legendCup Type LegendCupID=" << llLegendCupID);
		return;
	}

	BYTE eCupType = iter->second;
	map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(eCupType);
	if (iterCupMgr == m_AllLegendCupMgr.end())
	{
		ErrorLn(__FUNCTION__": can't find iterCupMgr LegendCupID=" << llLegendCupID);
		return;
	}

	CLegendCup* pLegendCup = iterCupMgr->second->findCup(llLegendCupID);
	if (pLegendCup == NULL)
	{
		ErrorLn(__FUNCTION__": can't find LegendCup LegendCupID=" << llLegendCupID);
		return;
	}

	pLegendCup->sendAbstentionKinMail(dwFailedKinID);
}

void LegendCupService::onReturnCupEXListDataInfo(char* pOutData, int nOutLen)
{
    int nTempLen = sizeof(DBREQ_RESULT_EX_LIST_DATA_INFO);
    if (nOutLen < nTempLen)
    {
        ErrorLn(__FUNCTION__": error length! nOutLen = " << nOutLen << ", [DBREQ_RESULT_READKINMEMBERLIST] length = " << nTempLen);
        return;
    }

    DBREQ_RESULT_EX_LIST_DATA_INFO* pRetData = (DBREQ_RESULT_EX_LIST_DATA_INFO*)pOutData;
    LONGLONG llLegendCupID = pRetData->llLegendCupID;

    CLegendCup* pCup = findLegendCup(llLegendCupID);
    if (pCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find the kin object, LegendID=" << llLegendCupID);
        return;
    }

    if (pCup->setCupDBEXlistContext(pOutData, nOutLen) == false)
    {
        WarningLn(__FUNCTION__": set setCupDBEXlistContext error!");
    }

    if (pCup->getLegendCupDBReadFlag() == emLegendRead_AllData)
    {
        readCupInfoDBFinished(llLegendCupID);
    }

    addCupAllKinMapedLegendID(pCup);
}

SCreaterLegendCupInfo LegendCupService::getCreaterLegendCupInfo(BYTE byCupCreateType,DWORD dwCreaterID)
{
    SCreaterLegendCupInfo ClanCupInfo;
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.find(emCupType_Clan);
    if (iterCupMgr == m_AllLegendCupMgr.end())
    {
        return ClanCupInfo;
    }
    ClanCupInfo = iterCupMgr->second->findCreateIDLegendCupInfo(dwCreaterID);

    return ClanCupInfo;
}

// 获取免费联盟杯赛的奖励次数
int LegendCupService::getPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID)
{
	int nRetCount = 0;
	TMap_LegendCupPlayerListIt it = m_LegendCupPlayerList.find(dwPDBID);
	if (it != m_LegendCupPlayerList.end())
	{
		if (!g_ExternalFacade.IsSameMonth((*it).second.nRecvPrizeTime, (DWORD)time(NULL)))
		{
			(*it).second.nClanCupPrizeCount = 0;
			(*it).second.nRecvPrizeTime = (int)time(NULL);
		}

		nRetCount = (*it).second.nClanCupPrizeCount;
	}

	return nRetCount;
}

// 设置免费联盟杯赛的奖励次数
bool LegendCupService::setPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID, int nRecvPrizeCount)
{
	TMap_LegendCupPlayerListIt it = m_LegendCupPlayerList.find(dwPDBID);
	if (it != m_LegendCupPlayerList.end())
	{
		(*it).second.nClanCupPrizeCount = nRecvPrizeCount;
		(*it).second.nRecvPrizeTime = (int)time(NULL);
	}
	else
	{
		SLegendCupPlayerInfo sCupPlayerInfo;
		sCupPlayerInfo.dwPDBID = dwPDBID;
		sCupPlayerInfo.nClanCupPrizeCount = nRecvPrizeCount;
		sCupPlayerInfo.nRecvPrizeTime = (int)time(NULL);

		m_LegendCupPlayerList.insert(pair<DWORD, SLegendCupPlayerInfo>(dwPDBID, sCupPlayerInfo));
	}

	SaveLegendCupPlayerInfoToDB(dwPDBID);

	return true;
}

bool LegendCupService::isKinInLegendCup(DWORD dwKinID)
{
    map<BYTE, CLegendCupInfoMgr*>::iterator iterCupMgr = m_AllLegendCupMgr.begin();
    for (; iterCupMgr != m_AllLegendCupMgr.end(); ++iterCupMgr)
    {
        CLegendCupInfoMgr* pCupInfoMgr = iterCupMgr->second;
        if (pCupInfoMgr == NULL)
        {
            continue;
        }

        CLegendCup* pLegendCup = pCupInfoMgr->findKinJoinedLegendCup(dwKinID);
        if (pLegendCup != NULL && !pLegendCup->isEnd())
        {
            return true;
        }
    }
    return false;
}