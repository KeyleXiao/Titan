#include "StdAfx.h"
#include "Clan.h"
#include "ExternalHelper.h"
#include "ClanScene.h"
#include "ClanMessageDef.h"
#include "KinHelper.h"
#include "ISchemeCenter.h"
#include "LegendCupSchemeDef.h"
#include "EventDef.h"
#include "IEventEngine.h"
#include "IServerGlobal.h"

// 构造函数
CClan::CClan(void)
{
	m_dwReadFlag = 0;										// 已经从数据库读出数据的标志
	m_dwID = 0;												// 联盟ID
	memset(m_szName, 0, sizeof(m_szName));					// 联盟名
	memset(m_szShaikhName, 0, sizeof(m_szShaikhName));		// 联盟盟主名
	m_nState = Clan_ManagerDef::emClanState_Invaild;		// 联盟的状态：临时，正式，解散中
	m_nLevel = 0;											// 联盟等级
	m_nShaikhID = 0;										// 盟主PDBID
	m_nConstructDegree = 0;									// 建设度
	m_nGold = 0;											// 黄金
	m_nActivity = 0;										// 活跃度
	m_nPlace = 0;											// 排名
	m_nWorldID = 0;											// 游戏世界ID
	m_nLegendCupCount = 0;									// 在线成员列表
	m_nWeekActivity = 0;                                    // 周活跃度
	m_MemberList.clear();									// 在线成员列表
	memset(m_propAddress, 0, sizeof(m_propAddress));	

	// 成员地址初始化
	m_propAddress[emClanProp_ID] = (int*)&m_dwID;
	m_propAddress[emClanProp_State] = (int*)&m_nState;
	m_propAddress[emClanProp_Level] = &m_nLevel;
	m_propAddress[emClanProp_ShaikhID] = &m_nShaikhID;
	m_propAddress[emClanProp_ReadFlag] = (int*)&m_dwReadFlag;
	m_propAddress[emClanProp_ConstructDegree] = (int*)&m_nConstructDegree;
	m_propAddress[emClanProp_Gold] = (int*)&m_nGold;
	m_propAddress[emClanProp_Activity] = (int*)&m_nActivity;
	m_propAddress[emClanProp_WeekActivity] = (int*)&m_nWeekActivity;
	m_propAddress[emClanProp_Place] = (int*)&m_nPlace;
	m_propAddress[emClanProp_WorldID] = (int*)&m_nWorldID;
	m_propAddress[emClanProp_LegendCupCount] = (int*)&m_nLegendCupCount;
}

// 析构函数
CClan::~CClan(void)
{

}

// 创建
bool CClan::create(LPCSTR pszData, int nLen)
{
	return updateData(pszData, nLen);
}

// 关闭
void CClan::release(void)
{
	delete this;
}

// 取得联盟的整型数据
int CClan::getNumProp(DWORD dwPropID)
{
	if (dwPropID < emClanProp_ID && dwPropID >= emClanProp_NumPropCount)
		return -1;

	if (m_propAddress[dwPropID] == 0)
		return -1;

	return *(m_propAddress[dwPropID]);
}


// 设置联盟的整型数据（覆盖型）
bool CClan::coverNumProp(DWORD dwPropID, int nValue)
{
	if (dwPropID < emClanProp_ID && dwPropID >= emClanProp_NumPropCount)
		return false;

	if (m_propAddress[dwPropID] == 0)
		return false;

    int nOldValue = *(m_propAddress[dwPropID]); 
	*(m_propAddress[dwPropID]) = nValue;

    if(dwPropID == emClanProp_ShaikhID && nOldValue != nValue)
    {
        // 发出盟主改变事件
        event_clan_shaikhid_changed evSkIDChanged;
        evSkIDChanged.nOldID = nOldValue;
        evSkIDChanged.nNewID = nValue; 
        gServerGlobal->getEventEngine()->FireExecute(EVENT_CLAN_SHAIKHID_CHANGED, SOURCE_TYPE_CLAN, m_dwID, LPCSTR(&evSkIDChanged), sizeof(evSkIDChanged));
    }

	return true;
}

// 取得联盟名字
LPCSTR CClan::getName()
{
	return m_szName;
}

// 取得盟主名字
LPCSTR CClan::getShaikhName()
{
	return m_szShaikhName;
}

// 更新数据
bool CClan::updateData(LPCSTR pszData, int nLen)
{
	if (pszData == NULL || nLen != sizeof(msg_clan_updateclan_os))
		return false;
	msg_clan_updateclan_os *pContext = (msg_clan_updateclan_os*)pszData;

	m_dwID = pContext->dwID;
	sstrcpyn(m_szName, pContext->szName, sizeof(m_szName));
	m_nState = pContext->nState;
	m_nLevel = pContext->nLevel;
	m_nConstructDegree = pContext->nConstructDegree;
	m_nGold = pContext->nGold;
    int nOldShaikhID = m_nShaikhID;
	m_nShaikhID = pContext->nShaikhID;
	sstrcpyn(m_szShaikhName, pContext->szShaikhName, sizeof(m_szShaikhName));
	m_nActivity = pContext->nActivity;
	m_nWeekActivity = pContext->nWeekActivity;
	m_nPlace = pContext->nPlace;
	m_nWorldID = pContext->nWorldID;
	m_nLegendCupCount = pContext->nLegendCupCount;

    if(nOldShaikhID != m_nShaikhID)
    {
        // 发出盟主改变事件
        event_clan_shaikhid_changed evSkIDChanged;
        evSkIDChanged.nOldID = nOldShaikhID;
        evSkIDChanged.nNewID = m_nShaikhID; 
        gServerGlobal->getEventEngine()->FireExecute(EVENT_CLAN_SHAIKHID_CHANGED, SOURCE_TYPE_CLAN, m_dwID, LPCSTR(&evSkIDChanged), sizeof(evSkIDChanged));
    }

	return true;
}

// 添加一个成员
void CClan::addMember(DWORD dwMemberID)
{
	m_MemberList.remove(dwMemberID);
	m_MemberList.push_back(dwMemberID);
}

// 删除一个成员
void CClan::removeMember(DWORD dwMemberID)
{
	m_MemberList.remove(dwMemberID);
}

// 覆盖修改 非负数
void CClan::cover_NoNegative(DWORD dwPropID, int nValue)
{
	nValue < 0 ? nValue = 0 : 0;

	*(m_propAddress[dwPropID]) = nValue;
}

// 增量修改 非负数
void CClan::increment_NoNegative(DWORD dwPropID, int nValue)
{
	*(m_propAddress[dwPropID]) += nValue;
	*(m_propAddress[dwPropID]) < 0 ? *(m_propAddress[dwPropID]) = 0 : 0;
}

// 检查杯赛
bool CClan::checkCreateLegendCup(int nLegendCupConfigID)
{
    ISchemeLegendCup* pSchemeLegendCup = gServerGlobal->getSchemeCenter()->getSchemeLegendCup();
    if (pSchemeLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": pSchemeLegendCup == NULL");
        return true;
    }
    SSchemeLegendCupLimit* pLimitConfig = pSchemeLegendCup->getLimitConfig(nLegendCupConfigID);
    if (pLimitConfig == NULL)
    {
        ErrorLn(__FUNCTION__": pConfig == NULL nCreateConfigID= "<<nLegendCupConfigID);
        return true;
    }

	KinHelper helper;
	IKinSceneService *pKinSceneService = helper.m_ptr;
	if (pKinSceneService == NULL)
		return false;

    bool bCanCreate = true;
    do 
    {
        // 人数及等级限制
        int nPlayerCount = 0;
        DWORD dwKinMemberMaxSize = 256;
        PDBID* pReturnArray = new PDBID[dwKinMemberMaxSize];
        list<DWORD>::iterator it = m_MemberList.begin();
        for ( ; it != m_MemberList.end(); ++it)
        {
            DWORD dwKinID = *it;
            DWORD dwCount = pKinSceneService->getKinMemberList(dwKinID, pReturnArray, dwKinMemberMaxSize);
            for (DWORD i = 0; i < dwCount; ++i)
            {
                DWORD dwPDBID = pReturnArray[i];
                SSceneKinMember kinMember = pKinSceneService->getKinMember(dwPDBID);
                if (kinMember.dwPDBID != dwPDBID)
                    continue;

                if (kinMember.nLevel >= pLimitConfig->m_nLevelLimit)
                {
                    ++nPlayerCount;
                }
            }
        }

        delete []pReturnArray;
        pReturnArray = NULL;
        bCanCreate = nPlayerCount >= pLimitConfig->m_nLevelLimitPersonCount;

        if (!bCanCreate)
        {
            break;
        }

        // 活跃度限制
        int nWeekActivity = getNumProp(emClanProp_WeekActivity);
        bCanCreate = nWeekActivity >= pLimitConfig->m_nLimitWeekActivity;
        if (!bCanCreate)
        {
            break;
        }

    } while (false);
	

	return bCanCreate;
}
