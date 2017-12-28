//==========================================================================
/**
* @file	  : PersonLegendCupMgr.cpp
* @author : 秦其勇
* created : 2010-4-29
* purpose : 个人杯赛相关
*/
//==========================================================================
#include "stdafx.h"
#include "SharePropertyDef.h"
#include "IShareServer.h"
#include "IGatewayAcceptorService.h"
#include "LegendCupInfoMgr.h"

CLegendCupInfoMgr::CLegendCupInfoMgr()
{	
   m_byCreateLegendCupType = emCupType_None;
   m_mapLegendID2Legend.clear();
   m_mapKin2LegendID.clear();
   m_mapCreateIDHaveLegendCup.clear();
}

CLegendCupInfoMgr::~CLegendCupInfoMgr()
{
    
}

void CLegendCupInfoMgr::release()
{
    m_mapKin2LegendID.clear();
    m_mapCreateIDHaveLegendCup.clear();
    map<LONGLONG, CLegendCup*>::iterator iter = m_mapLegendID2Legend.begin();
    for (;iter != m_mapLegendID2Legend.end(); ++iter)
    {
        iter->second->Release();
        iter->second = NULL;
    }
    
    m_mapLegendID2Legend.clear();
	delete this;
}

CLegendCup* CLegendCupInfoMgr::findCup(LONGLONG llLegendCupID)
{
    map<LONGLONG, CLegendCup*>::iterator iter = m_mapLegendID2Legend.find(llLegendCupID);
    if (iter == m_mapLegendID2Legend.end())
    {
        return NULL;
    }
    
    return iter->second;
}

bool CLegendCupInfoMgr::addLegendCup(CLegendCup* pLegendCup)
{
    if (pLegendCup == NULL)
    {
        return false;
    }

    if (m_mapLegendID2Legend.find(pLegendCup->getLegendCupID()) != m_mapLegendID2Legend.end())
    {
        return false;
    }

    m_mapLegendID2Legend.insert(pair<LONGLONG, CLegendCup*>(pLegendCup->getLegendCupID(), pLegendCup));

    // 检查是否结束
    if (!pLegendCup->isEnd())
    {
        if ( pLegendCup->getLegendCupBaseInfo()->bIsEnd != true)
        {
            DWORD dwCreateID = 0;
            switch(m_byCreateLegendCupType)
            {
            case emCupType_Personal:
                {
                    dwCreateID = pLegendCup->getLegendCupBaseInfo()->dwCreataActorID;
                }
                break;
            case emCupType_Clan:
                {
                    dwCreateID = pLegendCup->getLegendCupBaseInfo()->dwClanID;
                }
                break;
            case emCupType_System:
                {

                }
                break;
            }

            if (m_byCreateLegendCupType == emCupType_System)
            {
                return true;
            }

            m_mapCreateIDHaveLegendCup[dwCreateID] = pLegendCup->getLegendCupID();
        }
    }
    return true; 
}

void CLegendCupInfoMgr::addCupAllKinMapedLegendID(CLegendCup *pLegendCup)
{
    if (pLegendCup == NULL)
    {
        return;
    }

    //获取正在参赛队伍列表
    LONGLONG llLegendCupID = pLegendCup->getLegendCupID();
    map<DWORD,SCupRegisterInfo> RegisterKinList;
    pLegendCup->getRegisterKinList(RegisterKinList);
    map<DWORD,SCupRegisterInfo>::iterator iter = RegisterKinList.begin();
    for (;iter != RegisterKinList.end(); ++iter)
    {
        m_mapKin2LegendID[iter->first] = llLegendCupID;
    }
}

void CLegendCupInfoMgr::removeKinMapedLegendID(DWORD dwKinID)
{
    m_mapKin2LegendID.erase(dwKinID);
}


bool CLegendCupInfoMgr::canRequestJoinCup(DWORD dwKinID)
{
    if (m_mapKin2LegendID.find(dwKinID) != m_mapKin2LegendID.end())
    {
        return false;
    }

    return true;
}

void CLegendCupInfoMgr::exitRegisterCup(SMsgExitCup* pMsgInfo)
{
    CLegendCup* pLegendCup = findCup(pMsgInfo->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find LegendCup legendCupID="<<pMsgInfo->llLegendCupID<<" byCreateCupType="<<pMsgInfo->byCreateCupType);
        return;
    }

    if (!pLegendCup->exitRegister(pMsgInfo->dwActorID, pMsgInfo->dwKinID))
    {
        return;
    }

    // 移除对应的战队映射
    removeKinMapedLegendID(pMsgInfo->dwKinID);
}

void CLegendCupInfoMgr::registerCupCheck(SMsgJoinCup* pMsgInfo)
{
    if (!canRequestJoinCup(pMsgInfo->dwKinID))
    {
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_CANT_JOINED_SAME_TYPE_CUP,"");
        return;
    }

    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pMsgInfo->dwActorID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": can't find person!");
        return;
    }

    CLegendCup* pLegendCup = findCup(pMsgInfo->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find LegendCup legendCupID="<<pMsgInfo->llLegendCupID<<" byCreateCupType="<<pMsgInfo->byCreateCupType);
        return;
    }
    if (!pLegendCup->registerCheck(pMsgInfo))
    {
        return;
    }

    pMsgInfo->nCreateConfigID = pLegendCup->getLegendCupBaseInfo()->nCreateConfigID;
    pMsgInfo->dwRegisterNeedTicket = pLegendCup->getLegendCupBaseInfo()->dwRegisterNeedTicket;

    // 发送到场景服扣除申请参赛相关
    obuf256 obMsg;
    SNetMsgHead head;
    head.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    head.byDestEndPoint	= MSG_ENDPOINT_SCENE;
    head.byKeyModule	= MSG_MODULEID_LEGENDCUP;
    head.byKeyAction	= OS_CUP_REQUEST_JOIN_CUP;

    obMsg.push_back( &head, sizeof(head) );
    obMsg.push_back(pMsgInfo, sizeof(SMsgJoinCup));

    //取出场景服
    ICenterConnectorService *pCenterConnector = gSocialGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnector)
    {
        ErrorLn("MatchService::send2SceneServer failed( void * buf, size_t len )! nullptr == pCenterConnector");
        return;
    }

    //压入
    pCenterConnector->postMessage(shareProperty.serverID, obMsg.data(),(int)obMsg.size());
}

void CLegendCupInfoMgr::addToBlackList(SMsgAddToBlackList* pMsgInfo)
{
    CLegendCup* pLegendCup = findCup(pMsgInfo->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find LegendCup legendCupID="<<pMsgInfo->llLegendCupID<<" byCreateCupType="<<pMsgInfo->byCreateCupType);
        return;
    }

    // 是否是创建者操作
    if (pMsgInfo->dwOperateID != pLegendCup->getLegendCupBaseInfo()->dwCreataActorID)
    {
        // 不是战队队长无法申请
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwOperateID, CHAT_TIP_LEGENDCUP_HAVE_NORIGHT_AT_THIS_OPERATE,"");
        return ;
    }

    if (!pLegendCup->addToBlackList(pMsgInfo->dwOperateID, pMsgInfo->dwKinID))
    {
        return;
    }

    // 移除LegendCupID对应的战队
    removeKinMapedLegendID(pMsgInfo->dwKinID);
    
}


void CLegendCupInfoMgr::removeBlackList(SMsgRemoveBlackList* pMsgInfo)
{
    CLegendCup* pLegendCup = findCup(pMsgInfo->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find LegendCup legendCupID="<<pMsgInfo->llLegendCupID<<" byCreateCupType="<<pMsgInfo->byCreateCupType);
        return;
    }

    // 是否是创建者操作
    if (pMsgInfo->dwOperateID != pLegendCup->getLegendCupBaseInfo()->dwCreataActorID)
    {
        // 不是战队队长无法申请
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwOperateID, CHAT_TIP_LEGENDCUP_HAVE_NORIGHT_AT_THIS_OPERATE,"");
        return ;
    }

    if (!pLegendCup->removeBlackList(pMsgInfo->dwOperateID, pMsgInfo->dwKinID))
    {
        return;
    }
}


void CLegendCupInfoMgr::publicCompetition(SMsgPublicCompetition* pMsgInfo)
{
    CLegendCup* pLegendCup = findCup(pMsgInfo->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find LegendCup legendCupID="<<pMsgInfo->llLegendCupID<<" byCreateCupType="<<pMsgInfo->byCreateCupType);
        return;
    }

    //线下才能发布比赛
    if (pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
    {
        // 线上无法发布比赛
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwOperateID, CHAT_TIP_LEGENDCUP_ONLINE_CUP_CANT_PUBLIC,"");
        return;
    }

    // 是否是创建者操作
    if (pMsgInfo->dwOperateID != pLegendCup->getLegendCupBaseInfo()->dwCreataActorID)
    {
        // 不是创建者无法申请
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwOperateID, CHAT_TIP_LEGENDCUP_HAVE_NORIGHT_AT_THIS_OPERATE,"");
        return ;
    }

    if (!pLegendCup->bIsCanPublic(INVALID_CLIENT))
    {
        return;
    }
    pLegendCup->publishCompetition();
}


void CLegendCupInfoMgr::addKinMapedLegendID(DWORD dwKinID, LONGLONG llLegendCupID)
{
    m_mapKin2LegendID[dwKinID] = llLegendCupID;
}

int CLegendCupInfoMgr::getAllCupBaseInfo(obuf &out, DWORD dwRequestKinID)
{
    DWORD dwNowTime = (DWORD)time(NULL);
    int CupCount = 0;
    map<LONGLONG, CLegendCup*>::iterator iter = m_mapLegendID2Legend.begin();
    for (;iter != m_mapLegendID2Legend.end(); ++iter)
    {
		if (iter->second == NULL)
		{
			continue;
		}

        SLegendCupBaseInfo* pCupBaseInfo = iter->second->getLegendCupBaseInfo();
        SMsgLegendCupInfo CupInfo;
		
        CupInfo.llLegendCupID = pCupBaseInfo->llLegendCupID;                        // 杯赛ID
        CupInfo.nCreateConfigID= pCupBaseInfo->nCreateConfigID;                    // 创建的配置ID
        CupInfo.byCreateCupType= pCupBaseInfo->byCreateCupType;                    // 杯赛类型 ELegendCupType
        CupInfo.bIsOnlineCup= pCupBaseInfo->bIsOnlineCup;                           // 线上比赛 线下比赛
        CupInfo.dwCreataActorID= pCupBaseInfo->dwCreataActorID;                     // 创建玩家 系统的为 0
        CupInfo.byMaxKinNumber= pCupBaseInfo->byMaxKinNumber;                       // 最大参赛队伍
        CupInfo.nCurrentKinNum = iter->second->getRegisterCount();                  // 杯赛参赛队伍数量
        CupInfo.dwRegisterLeftTime = iter->second->getRegisterLeftTime();
        CupInfo.bClanLimit= pCupBaseInfo->bClanLimit;                               // 只有联盟限制才能参加
        CupInfo.nLevelLimit= pCupBaseInfo->nLevelLimit;                             // 等级限制
        CupInfo.eCupState= pCupBaseInfo->eCupState;                                 // 状态 ECupState
        CupInfo.bIsEnd= pCupBaseInfo->bIsEnd;                                       // 是否结束
        CupInfo.dwEndTime= pCupBaseInfo->dwEndTime;                                 // 结束日期
        CupInfo.bHavePassword= iter->second->havePassword() ;                       // 是否有密码
        CupInfo.bIsJoinedCup = iter->second->bKinInRegister(dwRequestKinID);                      // 是否加入了的比赛
        sstrcpyn(CupInfo.szCupName, pCupBaseInfo->szCupName,sizeof(CupInfo.szCupName));     // 杯赛名称
        sstrcpyn(CupInfo.szCreaterName, pCupBaseInfo->szCreaterName,sizeof(CupInfo.szCreaterName));     // 创建者
		CupInfo.nNeedRoundCount = pCupBaseInfo->nNeedRoundCount;                    // 杯赛需要打几轮
		CupInfo.nBeginRound = pCupBaseInfo->nBeginRound;                        // 本场比赛由第几轮开始
		CupInfo.nCurrentRound = pCupBaseInfo->nCurrentRound;                      // 当前打到第几轮
        CupInfo.dwTotalCompetitionBonus = pCupBaseInfo->dwTotalRegisterTicket + pCupBaseInfo->dwContriBonus; // 比赛总奖金
        CupInfo.dwRegistGold = pCupBaseInfo->dwRegisterNeedTicket;
		CupInfo.dwClanID	= pCupBaseInfo->dwClanID;
       
        out<<CupInfo;

		CupCount++;
      
    }
    return CupCount;
}

CLegendCup* CLegendCupInfoMgr::findKinJoinedLegendCup(DWORD dwKinID)
{
    map<DWORD, LONGLONG>::iterator iter = m_mapKin2LegendID.find(dwKinID);
    if(iter == m_mapKin2LegendID.end())
    {
        return NULL;
    }

    CLegendCup* pLegendCup = findCup(iter->second);
    return pLegendCup;
}

void CLegendCupInfoMgr::endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID)
{
    // 移除战队对应的杯赛关系
    removeLegendCupIDAllKinMap(llLegendCupID);

    // 移除玩家创建杯赛对应的关系
    removeLegendCupIDCreaterMap(llLegendCupID);

}

void CLegendCupInfoMgr::removeServiceCupInfo(LONGLONG llLegendCupID)
{
    // 移除战队对应的杯赛关系
    removeLegendCupIDAllKinMap(llLegendCupID);

    // 移除玩家创建杯赛对应的关系
    removeLegendCupIDCreaterMap(llLegendCupID);

    // 释放杯赛相关
	removeLegendCupIDRec(llLegendCupID);
}

void CLegendCupInfoMgr::removeLegendCupIDAllKinMap(LONGLONG llLegendCupID)
{
    // 移除所有战队对应的LegendCupID
    for(map<DWORD, LONGLONG>::iterator iter = m_mapKin2LegendID.begin();iter != m_mapKin2LegendID.end();)
    {
        if (iter->second == llLegendCupID)
        {
            m_mapKin2LegendID.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

void CLegendCupInfoMgr::removeLegendCupIDCreaterMap(LONGLONG llLegendCupID)
{
    map<LONGLONG, CLegendCup*>::iterator iterCup = m_mapLegendID2Legend.find(llLegendCupID);
    if (iterCup == m_mapLegendID2Legend.end())
    {
        ErrorLn(__FUNCTION__": can't find cup cupID="<<llLegendCupID);
        return;
    }

    if (iterCup->second == NULL)
    {
        ErrorLn(__FUNCTION__": can't find cup cupID="<<llLegendCupID);
        return;
    }

    DWORD dwCreateID = 0;
    SLegendCupBaseInfo* pCupBaseInfo = iterCup->second->getLegendCupBaseInfo();
    switch(m_byCreateLegendCupType)
    {
    case emCupType_Personal:
        {
            dwCreateID = pCupBaseInfo->dwCreataActorID;
        }
        break;
    case emCupType_Clan:
        {
            dwCreateID = pCupBaseInfo->dwClanID;
        }
        break;
    case emCupType_System:
        {
            return;
        }
        break;
    default:
        {
            ErrorLn(__FUNCTION__": can't find cup CupType cupID="<<llLegendCupID);
        }
        break;
    }

    m_mapCreateIDHaveLegendCup.erase(dwCreateID);
}

void CLegendCupInfoMgr::removeLegendCupIDRec(LONGLONG llLegendCupID)
{
    map<LONGLONG, CLegendCup*>::iterator iterCup = m_mapLegendID2Legend.find(llLegendCupID);
    if (iterCup == m_mapLegendID2Legend.end())
    {
        ErrorLn(__FUNCTION__": can't find cup cupID="<<llLegendCupID);
        return;
    }

    if (iterCup->second != NULL)
    {
        iterCup->second = NULL;
    }

    m_mapLegendID2Legend.erase(iterCup);
}


void CLegendCupInfoMgr::inital(ELegendCupType byCreateLegendCupType)
{
    m_byCreateLegendCupType = byCreateLegendCupType;
}

bool CLegendCupInfoMgr::canCreateLegendCup(SMsgCreateCup* pCreateCup)
{
    if (m_byCreateLegendCupType == emCupType_System)
    {
        return true;
    }

    DWORD dwCreateID = 0;
    switch(pCreateCup->byCreateCupType)
    {
    case emCupType_Personal:
        {
            dwCreateID = pCreateCup->dwCreataActorID;
        }
        break;
    case emCupType_Clan:
        {
            dwCreateID = pCreateCup->dwClanID;
        }
        break;
    case emCupType_System:
        {
            return true;
        }
        break;
    default:
        {
            ErrorLn(__FUNCTION__":byCreateCupType = "<< pCreateCup->byCreateCupType);
            return false;
        }
        break;
    }

    map<DWORD, LONGLONG>::iterator iter = m_mapCreateIDHaveLegendCup.find(dwCreateID);
    if (iter == m_mapCreateIDHaveLegendCup.end())
    {
        return true;
    }

    g_ExternalFacade.showSystemMessage(pCreateCup->dwCreataActorID, CHAT_TIP_LEGENDCUP_CANT_REPEAT_CREATE_CUP,"");
    return false;
}

void CLegendCupInfoMgr::addRegisterCup(SMsgJoinCup* pMsgInfo)
{
    CLegendCup* pLegendCup = findCup(pMsgInfo->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find LegendCup legendCupID="<<pMsgInfo->llLegendCupID<<" byCreateCupType="<<pMsgInfo->byCreateCupType);
        return;
    }

    if (!pLegendCup->addRegister(pMsgInfo))
    {
        return;
    }
    // 添加对应的战队映射
    addKinMapedLegendID(pMsgInfo->dwKinID, pMsgInfo->llLegendCupID);
}

void CLegendCupInfoMgr::offlineEndRegister(SMsgOfflineEndRegister* pMsgInfo)
{
    CLegendCup* pLegendCup = findCup(pMsgInfo->llLegendCupID);
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": can't find LegendCup legendCupID="<<pMsgInfo->llLegendCupID<<" byCreateCupType="<<pMsgInfo->byCreateCupType);
        return;
    }

    if (!pLegendCup->preEndRegisterState(pMsgInfo->dwActorID))
    {
        return;
    }

}

SCreaterLegendCupInfo CLegendCupInfoMgr::findCreateIDLegendCupInfo(DWORD dwCreateID)
{
    SCreaterLegendCupInfo sCupInfo;
    map<DWORD, LONGLONG>::iterator iter = m_mapCreateIDHaveLegendCup.find(dwCreateID);
    if (iter == m_mapCreateIDHaveLegendCup.end())
    {
        return sCupInfo;
    }
    CLegendCup* pLegendCup = findCup(iter->second);
    if (pLegendCup == NULL)
    {
        return sCupInfo;
    }
	SLegendCupBaseInfo *pLegendCupBaseInfo = pLegendCup->getLegendCupBaseInfo();
	if (pLegendCupBaseInfo == NULL)
	{
		return sCupInfo;
	}

    sCupInfo.llLegendCupID = pLegendCup->getLegendCupID();
	sstrcpyn(sCupInfo.szCupName, pLegendCupBaseInfo->szCupName, sizeof(sCupInfo.szCupName));
    sCupInfo.byState = pLegendCupBaseInfo->eCupState;
    sCupInfo.nRegisterKinCount = pLegendCup->getRegisterCount();
    sCupInfo.nRegisterKinMax = pLegendCupBaseInfo->byMaxKinNumber;

    return sCupInfo;
}

void CLegendCupInfoMgr::onActorLogOut(DWORD dwActorID)
{
    map<LONGLONG, CLegendCup*>::iterator iter = m_mapLegendID2Legend.begin();
    for (;iter != m_mapLegendID2Legend.end(); ++iter)
    {
        iter->second->removeLegendCupViewer(dwActorID);
    }
}


