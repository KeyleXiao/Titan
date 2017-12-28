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
        // ��������Ϣ�ַ���ע��
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_LEGENDCUP);
        // �ͻ�����Ϣ�ַ���ע��
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_LEGENDCUP);
    }
    IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
    if (pEventEngine)
    {

        // ���� ��������������������
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0);
        pEventEngine->UnSubscibe(this, EVENT_KIN_READ_DB_FINISHED, SOURCE_TYPE_KIN, 0);
        pEventEngine->UnSubscibe(this, EVENT_KIN_LOGIN, SOURCE_TYPE_KIN, 0);

    }

    IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->UnregisterHandler(this);
    }

    // ���ݿ�ӿڷ�ע��
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
        // ��������Ϣ�ַ�ע��
        pMessageDispatch->registerTransmitHandler(MSG_MODULEID_LEGENDCUP,this);
        // �ͻ�����Ϣ�ַ�ע��
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


    // ���ݿ�ӿ�ע��
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->registerHandler(this);
    }

    // ��������������Ϣ
    for (BYTE i = (BYTE)emCupType_None + 1; i <(BYTE)emCupType_Max ; ++i)
    {
        CLegendCupInfoMgr* pCupInfoMgr = new CLegendCupInfoMgr();
        if (pCupInfoMgr == NULL)
        {
            ErrorLn(__FUNCTION__": pCupInfoMgr == NULL emCupType  ="<<i);
        }

        pCupInfoMgr->inital(ELegendCupType(i));
        //��ӽ�������
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
    case GAMEDB_REQUEST_READ_ALL_LEGEND_CUP:			// ����ϵͳ������Լ��� ϵͳ����
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
                return;
            }

            onReturnReadAllCupDataBaseInfo(pOutData,nOutLen);
        }
        break;

        
    case GAMEDB_REQUEST_EX_LIST_DATA_INFO:			    // �������� ���������б� �������б� �����б�...
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
                return;
            }

            onReturnCupEXListDataInfo(pOutData,nOutLen);
        }
        break;
    case GAMEDB_REQUEST_LEGEND_CUP_MEMBERLIST:			// �������� ���������Ϣ
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
                return;
            }

            onReturnCupMemberList(pOutData,nOutLen);
        }
        break;
    case GAMEDB_REQUEST_READ_ALL_CUP_COMPETITION_NODE:			// �������� ������Ϣ
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
                return;
            }

            onReturnCupCompetitionNode(pOutData,nOutLen);
        }
        break;
    case GAMEDB_REQUEST_READ_ALL_KININFO:			// �������� ���������Ϣ
        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
                return;
            }

            onReturnCupKinInfo(pOutData,nOutLen);
               
        }
        break;

    case GAMEDB_REQUEST_READ_ROUND_GROUP_KIN_POS:           // ��÷����б�

        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
                return;
            }

            onReturnCupRoundKinPos(pOutData,nOutLen);
                
        }
        break;
    case GAMEDB_REQUEST_READ_CUP_ROUND_TIMEINFO:           // ���ÿ��ʱ���б�

        {
            if (nDBRetCode < 0)
            {
                ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
                return;
            }

            onReturnCupRoundTimeInfo(pOutData,nOutLen);

        }
        break;
	case GAMEDB_REQUEST_READ_PLAYER_LEGENDCUP_DATA:

		{
			if (nDBRetCode < 0)
			{
				ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
				return;
			}

			onReturnLegendCupPlayerInfo(pOutData,nOutLen);
		}
		break;
	case GAMEDB_REQUEST_UPDATE_PLAYER_LEGENDCUP_DATA:

		{
			if (nDBRetCode < 0)
			{
				ErrorLn(_GT("��������ID��")<<nCmdID<<_GT(" ���ݿⷵ���룺")<<nDBRetCode<<_GT(" ��Ϣ������")<<pszDBRetDesc);
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

    // ������ݷ��س���
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

    // ������������
    int offset = sizeof(DBREQ_RESULT_READCUPLIST);
    char szText[512] = {0};
    static bool bOrder = false;
    for (int i = 0; i < pRetData->nLegendCupCount; i ++)
    {

        DBREQ_RESULT_LEGEND_CUP_BASE_INFO* pCupData = (DBREQ_RESULT_LEGEND_CUP_BASE_INFO*)(pOutData + offset);

        if (pCupData->bIsEnd && (DWORD)time(NULL) - pCupData->dwEndTime > 2*24*60*60)
        {
            // ������ �޶�֮��Ĳ��ö�ȡ��
            offset += sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO);
            continue;
        }

        if (pCupData->eCupState == SATE_SYSTEMCANCEL)
        {
            offset += sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO);
            continue;
        }

        // ������������
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

        // �ѱ���������ӵ���������б�
        addLegendCupToMgr(pCup);
        
        //��ȡ��������
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

	// ��ȡ�����������
	int nMemberCount = 0;
	obuf obNode;

    // ����ѳ�Ա��ӵ�m_KinMemberList��
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
		// ��ȡ��ұ���������Ϣ
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

    // ����ѳ�Ա��ӵ�������Ϣ��
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

    // ����ѳ�Ա��ӵ�������Ϣ��
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

    // ����ÿ��ս��λ����Ϣ
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

    // ����ÿ��ս��λ����Ϣ
    if (pCup->setCupDBRoundTimeInfo(pOutData, nOutLen) == false)
    {
        WarningLn(__FUNCTION__": set setCupDBCompetitionContext error!");
    }

    if (pCup->getLegendCupDBReadFlag() == emLegendRead_AllData)
    {
        readCupInfoDBFinished(llLegendCupID);
    }
}

// ������ұ���������Ϣ
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
                pDateTriggerService->RegisterDateTrigger(DCT_DataTrigger_LegendCupUpdate, this, sTriggerInfo);	// ����ʱ�̶�ʱ����ÿ��00��00��
            }

            // ��ȡ���б������ݿ���Ϣ
            //readAllCupInfoFromDB();
        }
        break;

    case EVENT_KIN_READ_DB_FINISHED:
        {
            // ս����Ϣ��ȡ��Ϻ��ȡ���б������ݿ���Ϣ
            readAllCupInfoFromDB();
        }
        break;

    case EVENT_KIN_DELMEMBER:
        {
            // ս�ӳ�Ա�Ƴ�
            onEventKinMemberDelte(pszContext, nLen);
        }
        break;

    case EVENT_KIN_QUIT:
        {
            // ս���˳�
            onEventKinMemberQuit(pszContext, nLen);
        }
        break;

    case EVENT_KIN_LOGIN:
        {
            // ս�ӳ�Ա��½
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
	// ��̬���� ����ʾ
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

	// ��ȡ��ұ���������Ϣ
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
    // ����˳�
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

// ���ñ����������
void LegendCupService::setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult)
{
    //���ñ����������
    CLegendCup* pLegendCup = findLegendCup(sNodeWarResult.llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<sNodeWarResult.llLegendCupID);
        return;
    }
    
    // ��¼�����������
    pLegendCup->recordCupWarResult(&sNodeWarResult);
}

void LegendCupService::setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState)
{
    // �ı䱭���ڵ�״̬
    CLegendCup* pLegendCup = findLegendCup(sNodeState.llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": cant find the legendCupID ="<<sNodeState.llLegendCupID);
        return;
    }
    
    // ��¼�����������
    pLegendCup->setLegendCupNodeState(&sNodeState);
}

void LegendCupService::handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen)
{
    //LEDENDCUP_TRACELN(__FUNCTION__": serverID = " << serverID<<" byKeyAction ="<<head.byKeyAction );
    // todo ת����������

    // ������ת�����Լ��Ĺ�����
    void* data = pData;
    size_t len = nLen;
    switch( head.byKeyAction )
    {

    case SO_CUP_CREATECUP_CHECK:                     // �����������    
        {
            onServerMsgCreateCupCheck(serverID, &head, data, len);
        }
        break;

    case SO_CUP_CREATE_CUP_RESLUT:                     // ����������������    
        {
            onServerMsgCreateCupResult(serverID, &head, data, len);
        }
        break;

    case SO_CUP_REQ_JOIN_LEGENDCUP_ROOM:               // ������뱭������
        {
            onServerMsgReqJoinLegendcupRoom(serverID, &head, data, len);
        }
        break;

	case SO_CUP_RECORD_WAR_RESULT:					// �����������
		{
			onServerMsgCupRecordWarResult(serverID, &head, data, len);
		}
		break;

    case SO_CUP_REQ_JOIN_CUP_SUCCESS:               // ������뱭���ɹ�
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
    // ������ת�����Լ��Ĺ�����
    void* data = pData;
    size_t len = nLen;
    switch( head.byKeyAction )
    {
        
    case CO_REQUEST_CUP_LIST:                               // �鿴���б����б�    
        {
            onClientRequestCupList(client, &head, data, len);
        }
        break;

    case CO_REQ_JOIN_CUP:                               // �����������뱭��    
        {
            onClientMsgReqJoinCup(client, &head, data, len);
        }
        break;

    case CO_REQ_EXIT_CUP:                               //  �����˳�����    
        {
            onClientMsgExitCup(client, &head, data, len);
        }
        break;

    case CO_REQ_ADD_TO_BLACKLIST:                     // �����ƶ���������    
        {
            onClientMsgMoveToBlackList(client, &head, data, len);
        }
        break;

    case CO_REQ_REMOVE_BLACKLIST:                     // �����ƶ���������    
        {
            onClientMsgRemoveToBlackList(client, &head, data, len);
        }
        break;

    case CO_REQ_LOOK_OVER_LEGENDCUP:                     // ����鿴���в���������Ϣ    
        {
            onClientMsgLoockOverLegendCup(client, &head, data, len);
        }
        break;

    case CO_REQ_ALL_BALCKLIST:                     // �����ƶ���������    
        {
            onClientMsgRequestAllBlackKinlist(client, &head, data, len);
        }
        break;

    case CO_REQ_KIN_MEMBER_LIST:                     // ����ս�ӳ�Ա�б�   
        {
            onClientMsgRequestKinMemberList(client, &head, data, len);
        }
        break;

    case CO_REQ_CUP_JUDGE_SINGAL_WAR_RESULT:         // ����μӱ���    
        {

            onClientMsgRequestJudgeWarResult(client, &head, data, len);
        }
        break;

    case CO_REQ_LOOK_UP_CUP_NODE_WARINFO:            // ����鿴�����ڵ�    
        {
            onClientMsgRequestLookUpCupNodeWarInfo(client, &head, data, len);
        }
        break;

    case CO_REQ_PUBLIC_COMPETTIONNODE:               // ���󷢲�����   
        {
            onClientPublicCompetition(client, &head, data, len);
        }
        break;
    case CO_REQ_QUIT_OBSERVE:                       // �˳��鿴����
        {
            onClientCloseCupView(client, &head, data, len);
        }
        break;

    case CO_REQ_OFFLINE_PRE_END_REGISTER:           // ����Ԥ�Ƚ������±��������׶�
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

    // ��ӵ��۲���
    DWORD dwActorID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;
    pLegendCup->addLegendCupViewer(dwActorID);

	//�·���������
	pLegendCup->updateSingleCupBaseInfo2Client(sShareProperty.clientID, sShareProperty.dwKinID);
    // ���ݱ����׶��·�����
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
    // ����ڵ���
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

    //����鿴�ڵ���Ϣ
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

    // ��ȡ�����б� �������б� �����б�...
    DBREQ_REQUEST_EX_LIST_DATA_INFO readEXList;
    readEXList.dwWorldID = getThisGameWorldID();
    readEXList.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_EX_LIST_DATA_INFO, 0, (LPCSTR)(&readEXList), sizeof(readEXList), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_EX_LIST_DATA_INFO request error!");
    }

    // ����Ѿ��е�״̬���ö�ȡ����б� �� ������Ϣ
    if (pCup->getLegendCupState() < SATE_GROUPING)
    {
        pCup->setLegendCupDBReadFlag(emLegendRead_AllData);
        m_ReadDBRecord.erase(llLegendCupID);
        return;
    }

    // ��ȡ����������������Ϣ
    // ��ȡÿ�ֱ���ʱ����Ϣ
    DBREQ_PARAM_READ_CUP_ROUND_TIMEINFO readRoundTime;
    readRoundTime.dwWorldID = getThisGameWorldID();
    readRoundTime.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_CUP_ROUND_TIMEINFO, 0, (LPCSTR)(&readRoundTime), sizeof(readRoundTime), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_READ_CUP_ROUND_TIMEINFO request error!");
    }


    // ��ȡս�ӳ�Ա�б�����
    DBREQ_PARAM_READCUPMEMBERLIST readMemberList;
    readMemberList.dwWorldID = getThisGameWorldID();
    readMemberList.llLegendCupID = llLegendCupID;

    if (pDBEngine->executeSP(GAMEDB_REQUEST_LEGEND_CUP_MEMBERLIST, 0, (LPCSTR)(&readMemberList), sizeof(readMemberList), this) == false)
    {
        ErrorLn("GAMEDB_REQUEST_LEGEND_CUP_MEMBERLIST request error!");
    }

    //// ��ȡ����ս����Ϣ
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

    //// ��ȡ�������б�����Ϣ
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
    // �������б���Ϣ
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

    // ���б�����Ϣ���·�
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
        // ��ȡ���б������ Ĭ��ϵͳ��������ǰ��
		if (iter->second == NULL)
		{
			continue;
		}

        LegendCuphead.nCount += iter->second->getAllCupBaseInfo(LegendCupBuf,dwRequestKinID);
    }
    // ��֯�������
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
    // ��������
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(pMsg->byCreateCupType);
    if (iter == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__":can't find LegendCupMgr byCreateCupType="<<pMsg->byCreateCupType);
        return;
    }

    //�������������
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

    // ������뱭��
    map<BYTE, CLegendCupInfoMgr*>::iterator iter = m_AllLegendCupMgr.find(pMsg->byCreateCupType);
    if (iter == m_AllLegendCupMgr.end())
    {
        ErrorLn(__FUNCTION__":can't find LegendCupMgr byCreateCupType="<<pMsg->byCreateCupType);
        return;
    }

    //���������������
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
    // �ӳ��ж�
    KinHelper kinHelper;
    if (kinHelper.m_ptr == NULL)
    {
        ErrorLn(__FUNCTION__ ": kinHelper.m_ptr == NULL!");
        return;
    }

    // �˳�����
    SMsgExitCup* pMsg = (SMsgExitCup*) pData;
    pMsg->dwActorID = pShareReceiver->GetSharePersonByClientID(clientID).dwPDBID;
    pMsg->dwKinID = pShareReceiver->GetSharePersonByClientID(clientID).dwKinID;

    SKinInfo sKinInfo = kinHelper.m_ptr->getKinInfo(pMsg->dwKinID);
    if (sKinInfo.nKinProp_ShaikhID != pMsg->dwActorID)
    {
        // ����ս�Ӷӳ��޷�����
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

    // ����������˳�����
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
    // �ӳ��ж�
    KinHelper kinHelper;
    if (kinHelper.m_ptr == NULL)
    {
        ErrorLn(__FUNCTION__ ": kinHelper.m_ptr == NULL!");
        return;
    }

    // ������������׶�
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

    // ����������˳�����
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
    //�����ദ��
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
    //�����ദ��
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

    //�������ڵ㷿��
    if (!pLegendCup->haveRightJoinCupRoom(pMsg))
    {
        return;
    }
    if (!pLegendCup->joinLegendCupRoom(pMsg))
    {
        // �޷���������ڵ�
        return;
    }

    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return;
    }

    //���ý����������ӿ�
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

    // ���Դ������� ���͸��������������� �۳����
    obuf256 obMsg;

    SNetMsgHead head;
    head.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    head.byDestEndPoint	= MSG_ENDPOINT_SCENE;
    head.byKeyModule	= MSG_MODULEID_LEGENDCUP;
    head.byKeyAction	= OS_CUP_CAN_CREATECUP;

    obMsg.push_back( &head, sizeof(head) );
    obMsg.push_back(pMsg, stLen);

    //ȡ��������
    ICenterConnectorService *pCenterConnector = gSocialGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnector)
    {
        ErrorLn("MatchService::send2SceneServer failed( void * buf, size_t len )! nullptr == pCenterConnector");
        return;
    }

    //ѹ��
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

    // ������������
    CLegendCup* pCup = new CLegendCup();
    if (pCup == NULL)
    {
        ErrorLn(__FUNCTION__": pCup == NULL  LegendID" << pMsg->llLegendCupID<<" dwCreataActorID = "<<pMsg->dwCreataActorID );
        return;
    }

    SLegendCupBaseInfo basInfo;
    basInfo.llLegendCupID           = pMsg->llLegendCupID;                      // ����ID
    basInfo.nCreateConfigID         = pMsg->nCreateConfigID;                    // ����������ID
    basInfo.byCreateCupType         = pMsg->byCreateCupType;                    // �������� 1 ϵͳ 2 ����
    basInfo.bIsOnlineCup            = pMsg->bIsOnlineCup;                       // ���ϱ��� ���±���
    basInfo.dwCreataActorID         = pMsg->dwCreataActorID;                    // ������� ϵͳ��Ϊ 0
    basInfo.byMaxKinNumber          = pMsg->byMaxKinNumber;                     // ����������
    basInfo.dwRegisterDeadline       = pMsg->dwRegisterDeadline;                // ������ֹʱ��
    basInfo.dwConfirmRegisterDeadline = pMsg->dwConfirmRegisterDeadline;        // ȷ�����ձ�����ֹʱ��
    basInfo.bClanLimit              = pMsg->bClanLimit;                         // ֻ���������Ʋ��ܲμ�
    basInfo.dwClanID                = pMsg->dwClanID;                           // ����ID
    basInfo.nNeedRoundCount         = pMsg->nNeedRoundCount;                    // ������Ҫ����
    basInfo.nLevelLimit             = pMsg->nLevelLimit;                        // �ȼ�����
    basInfo.dwContriBonus           = pMsg->dwContriBonus;                      // ���ʽ���
    basInfo.dwTotalRegisterTicket   = pMsg->dwTotalRegisterTicket;              // �����ܹ��ۻ��ĵ��
    basInfo.dwRegisterNeedTicket    = pMsg->dwRegisterNeedTicket;               // ������Ҫ�ĵ��
    basInfo.byTicketAllocationMode  = pMsg->byTicketAllocationMode;             // �������䷽ʽ 1�� ϵͳ���� 2������Զ��� ETicketAllocateMode  
    
    basInfo.bHaveRewards            = false;                                    // ���Ž�����־
    basInfo.eCupState               = SATE_REGISTER;                            // ״̬ ECupState
    basInfo.nCurrentRound           = 0;                                        // ��ǰ�򵽵ڼ���
    basInfo.bIsEnd                  = false;                                    // �Ƿ����
    basInfo.dwEndTime               = 0;                                        // ��������

    memcpy(basInfo.byRankTicketRatio, pMsg->byRankTicketRatio, sizeof(basInfo.byRankTicketRatio));// ���佱������
    sstrcpyn(basInfo.szCupName, pMsg->szCupName, sizeof(basInfo.szCupName));    // ��������
    sstrcpyn(basInfo.szPassword, pMsg->szPassword, sizeof(basInfo.szPassword)); // ��������
    sstrcpyn(basInfo.szCreaterName, pMsg->szCreaterName, sizeof(basInfo.szCreaterName)); // ��������

    if (pCup->Create(this, &basInfo) == false)
    {
        ErrorLn(__FUNCTION__": LegendID" << pMsg->llLegendCupID);
        return;
    }

    // todo �㲥֪ͨ������ʲô����
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

    // ����������־
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        string strPassWord = pCup->havePassword() == true? a2utf8("��Ҫ����"): a2utf8("����Ҫ����");
        string strClanLimit = basInfo.bClanLimit == true? a2utf8("�����޶�"): a2utf8("�������޶�");
        string strOnlineOffLine = basInfo.bIsOnlineCup == true? a2utf8("����"): a2utf8("����");
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
        ssprintf_s(szText, sizeof(szText), a2utf8("�ٰ챭��(%s)��[%u] ��ɫ��[%s] �ٰ챭��������id[%u] ����[%s] ��������[%u]��%s ����:(%s)��%s, ����ʱ��: [%s], ȷ������ʱ��:[%s]"), 
            strOnlineOffLine.c_str(),shareProperty.dwPDBID, shareProperty.szName, basInfo.llLegendCupID, basInfo.szCupName, basInfo.dwContriBonus, strPassWord.c_str(), pCup->getLegendCupBaseInfo()->szPassword, strClanLimit.c_str(),tRegisterDeadline ,tConfirmRegisterDeadline);

        pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, basInfo.llLegendCupID, szText);
    }

	// ��������˱��� ��֪ͨ����
	if (pMsg->byCreateCupType == emCupType_Clan && pMsg->dwClanID > 0)
	{
		ClanHelper clanHelper;
		IClanService *pClanService = clanHelper.m_ptr;
		if (pClanService != NULL)
		{
			pClanService->notifyCreateClanLegendCup(pMsg->dwClanID, pMsg->llLegendCupID, pMsg->byMaxKinNumber, pMsg->szCupName);
		}
	}

	// ��ͨ�����뱭�����Զ���һ����������
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

    // �ҵ���������ӱ���
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

// �������������������
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
	// ��¼ս��ս�����
	IKinService *pKinService = KinHelper().m_ptr;
	if (pKinService == NULL)
	{
		ErrorLn(__FUNCTION__ ": pKinService == NULL");
		return;
	}
	SKinWarRecordNode recordNode;
	recordNode.llWarID = pMsg->llWarID;							// ս��ID
	recordNode.dwKin1ID = pMsg->dwKin1ID;						// ս��1ID
	recordNode.dwKin2ID = pMsg->dwKin2ID;						// ս��2ID
	recordNode.dwBeginTime = pMsg->dwBeginTime;					// ս����ʼʱ��
	recordNode.dwWinKin = pMsg->dwWinnerKinID;					// ʤ��ս��ID
	sstrcpyn(recordNode.szKin1Name, pKinService->getKinInfo(pMsg->dwKin1ID).szName, sizeof(recordNode.szKin1Name)); // ս��1����
	sstrcpyn(recordNode.szKin2Name, pKinService->getKinInfo(pMsg->dwKin2ID).szName, sizeof(recordNode.szKin2Name)); // ս��2����
	memcpy(recordNode.nKin1Glory, pMsg->nKin1Glory, sizeof(recordNode.nKin1Glory));
	memcpy(recordNode.nKin2Glory, pMsg->nKin2Glory, sizeof(recordNode.nKin2Glory));
	pKinService->addKinLegendWarResult(recordNode);

	// ��¼�����������
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
   
    // ��ӵ�ӳ��
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

    // ��������˳�ս��
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
    // ��������˳�ս��
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

    // ��̬��ͼ�в�����
    if (shareProperty.dwSceneID > MAP_MAXCOUNT)
    {
        return;
    }

    // ����ս�ӽ�ɫ��½
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

    // �ҵ�����������
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

    // �ҵ�����������
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


// ���汭���������toDB
void LegendCupService::SaveLegendCupPlayerInfoToDB(DWORD dwPDBID)
{
	TMap_LegendCupPlayerListIt it = m_LegendCupPlayerList.find(dwPDBID);
	if (it == m_LegendCupPlayerList.end())
		return;

	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return;

	// ������ұ���������Ϣ
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

// ��ȡ������˱����Ľ�������
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

// ����������˱����Ľ�������
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