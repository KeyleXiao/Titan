#include "StdAfx.h"
#include "Clan.h"
#include "ExternalHelper.h"
#include "ClanClient.h"
#include "ClanMessageDef.h"
#include "Event_ManagedDef.h"
#include "IBankPart.h"
#include "IClientEntity.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"

CClan* g_pClan = NULL;

bool Less_Member_Name(const SClientClanMember& member1, const SClientClanMember& member2)
{
	if (member1.nIsOnline != member2.nIsOnline)
	{
		return (member1.nIsOnline > member2.nIsOnline);
	}
	if (member1.nIdentity != member2.nIdentity)
	{
		return (member1.nIdentity > member2.nIdentity);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

bool Less_Member_Kin(const SClientClanMember& member1, const SClientClanMember& member2)
{
	if (member1.dwKinID != member2.dwKinID)
	{
		int ret = strncmp(member1.szKinName, member2.szKinName, sizeof(member1.szKinName));
		return (ret < 0);
	}

	return Less_Member_Name(member1, member2);
}

bool Less_Member_Level(const SClientClanMember& member1, const SClientClanMember& member2)
{
	if (member1.nLevel != member2.nLevel)
	{
		return (member1.nLevel > member2.nLevel);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

bool Less_Member_Online(const SClientClanMember& member1, const SClientClanMember& member2)
{
	if (member1.nIsOnline != member2.nIsOnline)
	{
		return (member1.nIsOnline > member2.nIsOnline);
	}

	if (member1.nLogoutTime != member2.nLogoutTime)
	{
		return (member1.nLogoutTime > member2.nLogoutTime);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

// 构造函数
CClan::CClan()
{
	m_MemberVector.reserve(32);

	reset();
	
	nDismissApplyTime = 0;							// 战队解散时间
	m_nKickTime = 0;								// 踢人时间
	m_nKickNum = 0;									// 踢人次数

	m_propAddress[emClanProp_ID] = (int*)&m_dwID;
	m_propAddress[emClanProp_State] = (int*)&m_nState;
	m_propAddress[emClanProp_Level] = &m_nLevel;
	m_propAddress[emClanProp_EstablishTime] = &m_nEstablishTime;
	m_propAddress[emClanProp_ShaikhID] = &m_nShaikhID;
	m_propAddress[emClanProp_ConstructDegree] = (int*)&m_nConstructDegree;
	m_propAddress[emClanProp_Gold] = (int*)&m_nGold;
	m_propAddress[emClanProp_KickTime] = (int*)&m_nKickTime;
	m_propAddress[emClanProp_KickNum] = (int*)&m_nKickNum;
	m_propAddress[emClanProp_DismissApplyTime] = (int*)&nDismissApplyTime;
	m_propAddress[emClanProp_Activity] = (int*)&m_nActivity;
	m_propAddress[emClanProp_WeekActivity] = (int*)&m_nWeekActivity;
	m_propAddress[emClanProp_Place] = (int*)&m_nPlace;
	m_propAddress[emClanProp_LegendCupCount] = (int*)m_nLegendCupCount;
	m_propAddress[emClanProp_AutoAcceptFlag] = (int*)m_nAutoAcceptFlag;
}

// 析构函数
CClan::~CClan()
{

}

// 创建
bool CClan::create(void)
{
	return true;
}

// 释放
void CClan::release(void)
{
}

// 清空所有数据
void CClan::reset(void)
{
	memset(m_szManifesto, 0, sizeof(m_szManifesto));		// 帮会的宣言
	memset(m_szNotice, 0, sizeof(m_szNotice));				// 帮会的每日公告
	memset(m_szName, 0, sizeof(m_szName));					// 帮会名
	memset(m_szShaikhName, 0, sizeof(m_szShaikhName));		// 帮主名字

	m_ClanVaild = false;									// 帮会对象是否有效
	m_dwID = 0;												// 帮会ID
	m_nState = Clan_ManagerDef::emClanState_Invaild;		// 帮会的状态：临时，正式，解散中
	m_nEstablishTime = 0;									// 帮会创立时间

	m_nShaikhID = 0;										// 帮会族长PDBID
	m_nLevel = 0;											// 帮会等级
	m_nConstructDegree = 0;									// 建设度
	m_nGold = 0;											// 黄金
	m_nActivity = 0;										// 活跃度
	m_nPlace = 0;											// 排名
	m_nLegendCupCount = 0;									// 举办杯赛次数
	m_nAutoAcceptFlag = 0;

	m_MemberVector.clear();									// 成员数据
	
	m_ClanEventVector.clear();								// 清空帮会事件
}

// 是否有效
bool CClan::isVaild(void)
{
	return m_ClanVaild;
}

// 取得帮会的整型数据
int CClan::getNumProp(DWORD dwPropID)
{
	if (dwPropID < emClanProp_ID && dwPropID >= emClanProp_NumPropCount)
	{
		return 0;
	}

	// 先特殊处理下，以后再整理
	if (dwPropID == emClanProp_ID)
	{
		IClientEntity *pHero = gClientGlobal->getHero();
		if (pHero == NULL)
			return 0;
		return pHero->getIntProperty(PROPERTY_CLAN);
	}

	if (m_propAddress[dwPropID] == 0)
	{
		return 0;
	}

	return *(m_propAddress[dwPropID]);
}

// 设置帮会的整型数据
bool CClan::setNumProp(DWORD dwPropID, int nValue)
{
	if (dwPropID < emClanProp_ID && dwPropID >= emClanProp_NumPropCount)
	{
		return false;
	}

	if (m_propAddress[dwPropID] == 0)
	{
		return false;
	}

	*(m_propAddress[dwPropID]) = nValue;

	return true;
}

// 取得帮会的字符型数据
string CClan::getStringProp(DWORD dwPropID)
{
	switch (dwPropID)
	{
	// 帮会名
	case emClanProp_Name:
		{
			return m_szName;
		}
	// 帮会的宣言
	case emClanManifesto:
		{
			return m_szManifesto;
		}
	// 帮会的每日公告
	case emClanNotice:
		{
			return m_szNotice;
		}
	// 帮主名字
	case emClanShaikhName:
		{
			return m_szShaikhName;
		}
	}

	return "";
}

// 设置帮会的字符型数据
bool CClan::setStringProp(DWORD dwPropID, string strValue)
{
	switch (dwPropID)
	{
		// 帮会名
	case emClanProp_Name:
		{
			sstrcpyn(m_szName, strValue.c_str(), CLAN_NAME_SIZE);
			return true;
		}
		// 帮会的宣言
	case emClanManifesto:
		{
			sstrcpyn(m_szManifesto, strValue.c_str(), sizeof(m_szManifesto));
			return true;
		}
		// 帮会的每日公告
	case emClanNotice:
		{
			sstrcpyn(m_szNotice, strValue.c_str(), sizeof(m_szNotice));
			return true;
		}
		// 帮主名字
	case emClanShaikhName:
		{
			sstrcpyn(m_szShaikhName, strValue.c_str(), sizeof(m_szShaikhName));
			return true;
		}
	}

	return false;
}

// 取得成员的个数
int CClan::getMemberCount()
{
	return m_MemberVector.size();
}

// 取得成员的数据
SClientClanMember* CClan::getMemberByID(DWORD dwPDBID)
{
	vector<SClientClanMember>::iterator it;
	for (it = m_MemberVector.begin(); it != m_MemberVector.end(); ++ it)
	{	
		SClientClanMember *pMember = &(*it);
		if (pMember->dwPDBID == dwPDBID)
		{
			return pMember;
		}
	}

	return NULL;
}

// 删除成员
void CClan::removeMember(DWORD dwPDBID)
{
    bool bFind = false;
	vector<SClientClanMember>::iterator it;
	for (it = m_MemberVector.begin(); it != m_MemberVector.end(); ++it)
	{	
		SClientClanMember *pMember = &(*it);
		if (pMember->dwPDBID == dwPDBID)
		{
			m_MemberVector.erase(it);

            bFind = true;
			break;
		}
	}

    if (bFind)
    {
        obuf obData;
        cmd_clan_remove_member removemember;
        removemember.nDBID = dwPDBID;
        obData.push_back(&removemember, sizeof(removemember));
        // 帮会移除成员
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_REMOVEMEMBER, 0, 0, obData.data(), obData.size());
    }
}

// 排序
void CClan::sortMember(int nSortType)
{
	if (m_MemberVector.empty() == true)
	{
		return;
	}

	if (nSortType == emClanSort_Name)
	{
		std::sort(m_MemberVector.begin(), m_MemberVector.end(), Less_Member_Name);
	}
	else if (nSortType == emClanSort_Kin)
	{
		std::sort(m_MemberVector.begin(), m_MemberVector.end(), Less_Member_Kin);
	}
	else if (nSortType == emClanSort_Level)
	{
		std::sort(m_MemberVector.begin(), m_MemberVector.end(), Less_Member_Level);
	}
	else if (nSortType == emClanSort_Online)
	{
		std::sort(m_MemberVector.begin(), m_MemberVector.end(), Less_Member_Online);
	}
}

// 取得申请成员的个数
int CClan::getApplyMemberCount()
{
	return m_ApplyMemberVector.size();
}

// 取得申请成员的数据
SApplyMember* CClan::getApplyMemberByIndex(int nIndex)
{
	if (nIndex < 0 || (DWORD)nIndex >= m_ApplyMemberVector.size())
		return NULL;

	return &m_ApplyMemberVector[nIndex];
}

// 增加申请成员
void CClan::addApplyMember(SApplyMember* pApplyMember)
{
    if (pApplyMember == NULL)
    {
        return;
    }

	m_ApplyMemberVector.push_back(*pApplyMember);

    obuf obData;
    cmd_clan_add_apply addApply;
    addApply.nKinID = pApplyMember->dwKinID;
    sstrcpyn(addApply.szName, pApplyMember->szName, sizeof(addApply.szName));
    sstrcpyn(addApply.szShaikhName, pApplyMember->szShaikhName, sizeof(addApply.szShaikhName));
    addApply.nMemberCount = pApplyMember->nMemberCount;
    obData.push_back(&addApply, sizeof(addApply));
    // 增加帮会申请
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_ADDAPPLY, 0, 0, obData.data(), obData.size());
}

// 删除申请成员
void CClan::removeApplyMember(DWORD dwKinID)
{
    bool bFind = false;

	vector<SApplyMember>::iterator it;
	for (it = m_ApplyMemberVector.begin(); it != m_ApplyMemberVector.end(); ++it)
	{	
		SApplyMember* pMember = &(*it);
		if (pMember->dwKinID == dwKinID)
		{
			m_ApplyMemberVector.erase(it);

            bFind = true;
			break;
		}
	}

    if (bFind)
    {
        obuf obData;
        cmd_clan_remove_apply removeApply;
        removeApply.nKinID = dwKinID;
        obData.push_back(&removeApply, sizeof(removeApply));
        // 移除帮会申请
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_REMOVEAPPLY, 0, 0, obData.data(), obData.size());
    }
}

// 获得帮会事件的个数
int CClan::getClanEventCount()
{
	return m_ClanEventVector.size();
}

// 获得帮会事件的Txt信息
string CClan::getClanEventContext(int index)
{
	if (index < 0 || index >= (int)m_ClanEventVector.size())
	{
		return "";
	}

	time_t lTime = m_ClanEventVector[index].nTime;
	tm* tmTime = localtime(&lTime);
	char szContext[512] = {0};
	ssprintf_s(szContext, sizeof(szContext), "[green]%02d-%02d %02d:%02d[/green]&nbsp;&nbsp;%s", (tmTime->tm_mon + 1), tmTime->tm_mday, tmTime->tm_hour, tmTime->tm_min, m_ClanEventVector[index].szCoutext);

	return szContext;
}

////////////////////////IMessageHandler////////////////////
// 消息处理
void CClan::onMessage(SNetMsgHead *pHead, void *pData, size_t nLen)
{
	// 客户端消息case宏
#define MSG_CASE(proc, msgID) \
		case msgID:\
		{ proc((LPCSTR)pData, nLen);	break;}

	switch (pHead->byKeyAction)
	{
		// 更新帮会数字属性
		MSG_CASE(onUpdateClanIntProp, MSG_CLAN_UPDATECLANINTPROP)
		// 更新帮会字符串属性
		MSG_CASE(onUpdateClanStrProp, MSG_CLAN_UPDATECLANSTRPROP)

		// 更新帮会基本数据
 		MSG_CASE(onUpdateClanBaseData, MSG_CLAN_CLANBASEDATA)

		// 更新帮会成员数据
 		MSG_CASE(onUpdateMemberInfo, MSG_CLAN_UPDATEMEMBERINFO)
		// 移除帮会成员数据
 		MSG_CASE(onRemoveMemberInfo, MSG_CLAN_REMOVEMEMBERINFO)

		// 更新客户端数据
		MSG_CASE(onClanUpdateClient, MSG_CLAN_UPDATECLIENT)
		
		// 更新帮会事件
		MSG_CASE(onClanUpdateEvent, MSG_CLAN_UPDATEEVENT)

        // 请求卡牌列表事件
        MSG_CASE(onClanRequestCardList, MSG_CLAN_REQUEST_CARD_LIST)
        // 更新请求卡牌事件
        MSG_CASE(onClanUpdateRequestCard, MSG_CLAN_UPDATE_REQUEST_CARD)
        // 移除请求卡牌事件
        MSG_CASE(onClanRemoveRequestCard, MSG_CLAN_REMOVE_REQUEST_CARD)
        // 请求捐赠记录事件
		MSG_CASE(onClanRequestDonateLog, MSG_CLAN_REQUEST_DONATE_LOG)
		// 更新捐赠记录事件
		MSG_CASE(onClanUpdateDonateLog, MSG_CLAN_UPDATE_DONATE_LOG)
		// 增加请求提示事件
		MSG_CASE(onClanAddRequestTip, MSG_CLAN_ADD_REQUEST_TIP)
		// 捐卡信息消息
		MSG_CASE(onSendRequestCardInfo, MSG_CLAN_SENDREQUESTCARDINFO)

        // 设置帮会基础数据
        MSG_CASE(onSetClanBaseData, MSG_CLAN_SET_CLANBASEDATA)

        // 盟主创建杯赛dida
        MSG_CASE(onClanCreateLegendCupDida, MSG_CLAN_LEGEND_CUP_DIDA)

	default:
		break;
	}
}

// 更新帮会数字属性
void CClan::onUpdateClanIntProp(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_updateintprop_oc))
		return;

	msg_clan_updateintprop_oc* pInfo = (msg_clan_updateintprop_oc*)pszMsg;
	setNumProp(pInfo->nPropID, pInfo->nValue);

	onUpdatePropToView(pInfo->nPropID);
}

// 更新帮会字符串属性
void CClan::onUpdateClanStrProp(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_updatestringprop_oc))
		return;

	msg_clan_updatestringprop_oc* pInfo = (msg_clan_updatestringprop_oc*)pszMsg;
	setStringProp(pInfo->nPropID, pInfo->szValue);
}

// 设置自动标示到显示层
void CClan::onUpdatePropToView(DWORD dwPropID)
{
	if (dwPropID == emClanProp_AutoAcceptFlag)
	{
		obuf obData;
		cmd_clan_set_autoacceptflag cmdData;
		cmdData.nAutoAcceptFlag = m_nAutoAcceptFlag;

		obData.push_back(&cmdData, sizeof(cmd_clan_set_autoacceptflag));

		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_SET_AUTOACCEPT_FLAG, 0, 0, obData.data(), obData.size());
	}
    else if (dwPropID == emClanProp_State)
    {       
        obuf obData;
        cmd_clan_set_state cmdData;
        cmdData.nState = m_nState;

        obData.push_back(&cmdData, sizeof(cmd_clan_set_state));
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_STATE_NOTICE, 0, 0, obData.data(), obData.size());
    }
}

// 更新帮会基本数据
void CClan::onUpdateClanBaseData(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL)	
		return;

	Rle<1> rle;
	size_t sz = rle.decompress(pszMsg, nLen);
	if (sz < sizeof(msg_clan_clanbasedata_oc))
		return;
	pszMsg  = (LPCSTR)rle.data();

	msg_clan_clanbasedata_oc *pRecvData = (msg_clan_clanbasedata_oc*)pszMsg;
	int nExpectLen = sizeof(msg_clan_clanbasedata_oc);
	nExpectLen += (pRecvData->nMemberCount * sizeof(SClientClanMember) + pRecvData->nApplyMemberNum*sizeof(SApplyMember));
	if (sz != nExpectLen)
		return;

	int offset = sizeof(msg_clan_clanbasedata_oc);
	
 	sstrcpyn(m_szName, pRecvData->szClanName, sizeof(m_szName));
	sstrcpyn(m_szManifesto, pRecvData->szManifesto, sizeof(m_szManifesto));
	sstrcpyn(m_szShaikhName, pRecvData->szShaikhName, sizeof(m_szShaikhName));
	sstrcpyn(m_szNotice, pRecvData->szNotice, sizeof(m_szNotice));
	m_nShaikhID = pRecvData->dwShaikhID;
	m_nLevel = pRecvData->nClanLevel;
	m_nActivity = pRecvData->nActivity;
	m_nWeekActivity = pRecvData->nWeekActivity;
	m_nShaikhState = pRecvData->nShaikhState;
	m_nPlace = pRecvData->nPlace;
	m_nConstructDegree = pRecvData->nConstructDegree;
	m_nGold = pRecvData->nGold;
	m_nKickTime = pRecvData->nKickTime;
	m_nKickNum	= pRecvData->nKickNum;
	m_nLegendCupCount = pRecvData->nLegendCupCount;
	m_nAutoAcceptFlag = pRecvData->nAutoAcceptFlag;
	nDismissApplyTime = pRecvData->nDismissApplyTime;
    m_nState = pRecvData->nState;

	m_MemberVector.clear();
	for (int i = 0; i < pRecvData->nMemberCount; i ++)
	{
		SClientClanMember* pListMember = (SClientClanMember*)(pszMsg + offset);
 		m_MemberVector.push_back(*pListMember);
		offset += sizeof(SClientClanMember);
	}
	
	// 对数据进行一次排序
	std::sort(m_MemberVector.begin(), m_MemberVector.end(), Less_Member_Kin);

	// 申请成员数据
	m_ApplyMemberVector.clear();
	for (int i = 0; i < pRecvData->nApplyMemberNum; i ++ )
	{
		SApplyMember* pApplyMemberInfo = (SApplyMember*)(pszMsg + offset);
		m_ApplyMemberVector.push_back(*pApplyMemberInfo);
		offset += sizeof(SApplyMember);
	}

    int nMemberCount = m_MemberVector.size();
    int nApplyMemberCount = m_ApplyMemberVector.size();
    obuf obData;
    cmd_clan_basedata baseData;
    sstrcpyn(baseData.szClanName, pRecvData->szClanName, sizeof(baseData.szClanName));
    sstrcpyn(baseData.szManifesto, pRecvData->szManifesto, sizeof(baseData.szManifesto));
    sstrcpyn(baseData.szShaikhName, pRecvData->szShaikhName, sizeof(baseData.szShaikhName));
	sstrcpyn(baseData.szNotice, pRecvData->szNotice, sizeof(baseData.szNotice));	
    baseData.nShaikhID = pRecvData->dwShaikhID;
    baseData.nClanLevel = pRecvData->nClanLevel;
    baseData.nActivity = pRecvData->nActivity;
	baseData.nWeekActivity = pRecvData->nWeekActivity;
    baseData.nPlace = pRecvData->nPlace;
    baseData.nConstructDegree = pRecvData->nConstructDegree;
    baseData.nGold = pRecvData->nGold;
    baseData.nMemberCount = nMemberCount;
    baseData.nApplyMemberNum = nApplyMemberCount;
    baseData.nTotalFight = pRecvData->nTotalFight;
    baseData.nKinCount = pRecvData->nKinCount;
    baseData.nMaxKinCount = pRecvData->nMaxKinCount;
	baseData.nLegendCupCount = pRecvData->nLegendCupCount;
	baseData.lLegendCupID = pRecvData->llLegendCupID;
	baseData.nAutoAcceptFlag = pRecvData->nAutoAcceptFlag;
	baseData.nServerTime = pRecvData->nCurTime;
    baseData.nDismissApplyTime = pRecvData->nDismissApplyTime;
    baseData.nState = pRecvData->nState;
	baseData.nShaikhState = pRecvData->nShaikhState;

    obData.push_back(&baseData, sizeof(baseData));

    for (int i = 0; i < nMemberCount; ++i)
    {
		cmd_clan_memberlist memberList;
		fillClanMemberCmd(memberList, &m_MemberVector[i]);
        obData.push_back(&memberList, sizeof(memberList));
    }

    for (int i = 0; i < nApplyMemberCount; ++i)
    {
        SApplyMember &sApplyMember = m_ApplyMemberVector[i];

        cmd_clan_applymember applymember;
        applymember.nKinID = sApplyMember.dwKinID;
        sstrcpyn(applymember.szName, sApplyMember.szName, sizeof(applymember.szName));
        sstrcpyn(applymember.szShaikhName, sApplyMember.szShaikhName, sizeof(applymember.szShaikhName));
		applymember.nMemberCount = sApplyMember.nMemberCount;
        obData.push_back(&applymember, sizeof(applymember));
    }

    // 帮会基础数据
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_BASEDATA, 0, 0, obData.data(), obData.size());
}

// 设置帮会基本数据
void CClan::onSetClanBaseData(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_clanbasedata_oc) )	
        return;

    msg_clan_clanbasedata_oc *pRecvData = (msg_clan_clanbasedata_oc*)pszMsg;

    obuf obData;
    cmd_clan_basedata baseData;
    sstrcpyn(baseData.szClanName, pRecvData->szClanName, sizeof(baseData.szClanName));
    sstrcpyn(baseData.szManifesto, pRecvData->szManifesto, sizeof(baseData.szManifesto));
    sstrcpyn(baseData.szShaikhName, pRecvData->szShaikhName, sizeof(baseData.szShaikhName));
	sstrcpyn(baseData.szNotice, pRecvData->szNotice, sizeof(baseData.szNotice));
    baseData.nShaikhID = pRecvData->dwShaikhID;
    baseData.nClanLevel = pRecvData->nClanLevel;
    baseData.nActivity = pRecvData->nActivity;
	baseData.nWeekActivity = pRecvData->nWeekActivity;
    baseData.nPlace = pRecvData->nPlace;
    baseData.nConstructDegree = pRecvData->nConstructDegree;
    baseData.nGold = pRecvData->nGold;
    baseData.nTotalFight = pRecvData->nTotalFight;
    baseData.nKinCount = pRecvData->nKinCount;
    baseData.nMaxKinCount = pRecvData->nMaxKinCount;
	baseData.nLegendCupCount = pRecvData->nLegendCupCount;
	baseData.lLegendCupID = pRecvData->llLegendCupID;
	baseData.nAutoAcceptFlag = pRecvData->nAutoAcceptFlag;
	baseData.nServerTime = pRecvData->nCurTime;
    baseData.nDismissApplyTime = pRecvData->nDismissApplyTime;
    baseData.nState = pRecvData->nState;
	baseData.nShaikhState = pRecvData->nShaikhState;
    obData.push_back(&baseData, sizeof(baseData));

    // 设置帮会基础数据
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_SET_BASEDATA, 0, 0, obData.data(), obData.size());
}

// 更新成员信息
void CClan::onUpdateMemberInfo(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(SClientClanMember))
		return;

	SClientClanMember* pMemberInfo = (SClientClanMember*)pszMsg;
	SClientClanMember* pClanMember = getMemberByID(pMemberInfo->dwPDBID);
	if (pClanMember != NULL)
	{
		*pClanMember = *pMemberInfo;
	}
	else
	{
		m_MemberVector.push_back(*pMemberInfo);
	}

	obuf obData;
	cmd_clan_memberlist memberlist;
	fillClanMemberCmd(memberlist, pMemberInfo);
	obData.push_back(&memberlist, sizeof(memberlist));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_ADDMEMBER, 0, 0, obData.data(), obData.size());
}

// 移除成员信息
void CClan::onRemoveMemberInfo(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_removemember_oc))
		return;

	msg_clan_removemember_oc* pInfo = (msg_clan_removemember_oc*)pszMsg;
	removeMember(pInfo->dwPDBID);
}

// 更新客户端资源信息
void CClan::onClanUpdateClient(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_updateclaninfo_oc))
		return;

	msg_clan_updateclaninfo_oc *pRecvData = (msg_clan_updateclaninfo_oc*)pszMsg;

	m_ClanVaild = true;
	m_dwID = pRecvData->dwID;
	m_nState = pRecvData->nState;
	sstrcpyn(m_szName, pRecvData->szName, sizeof(m_szName));
	m_nShaikhID = pRecvData->dwShaikhID;
	sstrcpyn(m_szShaikhName, pRecvData->szShaikhName, sizeof(m_szShaikhName));	

    cmd_clan_selfdata cmdData;
    cmdData.nShaikhID = pRecvData->dwShaikhID;
    sstrcpyn(cmdData.szClanName, pRecvData->szName, sizeof(cmdData.szClanName));
    sstrcpyn(cmdData.szShaikhName, pRecvData->szShaikhName, sizeof(cmdData.szShaikhName));

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_UPDATECLIENT, 0, 0, &cmdData, sizeof(cmd_clan_selfdata));
}

// 更新帮会事件
void CClan::onClanUpdateEvent(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen < sizeof(msg_clan_updateclanlog_oc))
	{
		ErrorLn("onClanUpdateEvent nLen="<<nLen<<", HeadSize="<< sizeof(msg_clan_updateclanlog_oc));
		return;
	}

	msg_clan_updateclanlog_oc *pHeadData = (msg_clan_updateclanlog_oc*)pszMsg;
	if (nLen != sizeof(msg_clan_updateclanlog_oc) + pHeadData->wLen)
	{
		ErrorLn("onClanUpdateEvent nLen=" << nLen << ", HeadSize=" << sizeof(msg_clan_updateclanlog_oc) << ", DataLen=" << pHeadData->wLen);
		return;
	}

	// 获得帮会事件信息
	m_ClanEventVector.clear();

	// 事件个数
	WORD wEvenLogNum = pHeadData->wCount;
	if (wEvenLogNum > 0)
	{
		msg_clan_updateclanlog_single *pMemInfo = (msg_clan_updateclanlog_single*)(pszMsg + sizeof(msg_clan_updateclanlog_oc));
		for (int i = 0; i < wEvenLogNum; ++i)
		{
			if (pMemInfo == NULL)
				break;

			SClanEvent clanEvent;
			clanEvent.nTime = pMemInfo->nTime;
			WORD wLen = pMemInfo->wLen;
			if (wLen > 0)
			{
				LPCSTR pContent = (LPCSTR)pMemInfo+sizeof(msg_clan_updateclanlog_single);
				memcpy(clanEvent.szCoutext, pContent, wLen);
			}

			pMemInfo = (msg_clan_updateclanlog_single*)((LPCSTR)pMemInfo + sizeof(msg_clan_updateclanlog_single) + wLen);

			m_ClanEventVector.push_back(clanEvent);
		}
	}

    int nCount = m_ClanEventVector.size();
    obuf1024 obData;
    for (int i=0; i<nCount; ++i)
    {
        cmd_clan_eventlist eventlist;
        eventlist.nTime = m_ClanEventVector[i].nTime;
        sstrcpyn(eventlist.szCoutext, m_ClanEventVector[i].szCoutext, sizeof(eventlist.szCoutext));
        obData.push_back(&eventlist, sizeof(eventlist));
    }
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_EVENT_LIST, nCount, 0, obData.data(), obData.size());
}

// 请求卡牌列表
void CClan::onClanRequestCardList(LPCSTR pszMsg, int nLen)
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
        return;

    IBankPart *pBankPart = (IBankPart *)pClientEntity->getEntityPart(PART_BANK);
    if (pBankPart == NULL)
        return;

    if (pszMsg == NULL || nLen < sizeof(msg_clan_requestcardlist_sc))
    {
        ErrorLn("onClanRequestCardList nLen="<<nLen<<", HeadSize="<< sizeof(msg_clan_requestcardlist_sc));
        return;
    }

    msg_clan_requestcardlist_sc *pHeadData = (msg_clan_requestcardlist_sc*)pszMsg;
    if (nLen != sizeof(msg_clan_requestcardlist_sc) + pHeadData->nCount*sizeof(msg_clan_requestcardlist_single) )
    {
        ErrorLn("onClanRequestCardList nLen=" << nLen << ", HeadSize=" << sizeof(msg_clan_requestcardlist_sc) << ", DataLen=" << pHeadData->nCount * sizeof(msg_clan_requestcardlist_single));
        return;
    }

    obuf obData;

    // 卡牌列表头结构
	cmd_clan_request_cardlist_head head;
    head.nCount = pHeadData->nCount;
    obData.push_back(&head, sizeof(head));

    int offset = sizeof(msg_clan_requestcardlist_sc);
    for (int i = 0; i < pHeadData->nCount; ++i)
    {
        msg_clan_requestcardlist_single *pRequestCardNode = (msg_clan_requestcardlist_single*)(pszMsg + offset);

        DBREQ_PARAM_HERO_CARD HeroCard = pBankPart->getHeroCardInfo(pRequestCardNode->nCardID);
        int nNeedCount = 0;
        SHeroStarSchemeInfo *pHeroStar = pBankPart->getHeroCardInfoConfig(pRequestCardNode->nCardID);
        if (pHeroStar != NULL)
        {
            nNeedCount = pHeroStar->nNeedCount;
        }

        cmd_clan_request_cardlist requestCardList;
        requestCardList.nPDBID = pRequestCardNode->dwPDBID;
        sstrcpyn(requestCardList.szName, pRequestCardNode->szName, sizeof(requestCardList.szName));
        requestCardList.nCardID = pRequestCardNode->nCardID;
        requestCardList.nMyCurCount = HeroCard.nCardCount;
        requestCardList.nMyMaxCount = nNeedCount;
        requestCardList.nCurCount = pRequestCardNode->nCurCount;
        requestCardList.nMaxCount = pRequestCardNode->nMaxCount;
        requestCardList.nDonateCardCount = pRequestCardNode->nDonateCardCount;
        requestCardList.nDayDonateCardCount = pRequestCardNode->nDayDonateCardCount;
        obData.push_back(&requestCardList, sizeof(requestCardList));

        offset += sizeof(msg_clan_requestcardlist_single);
    }

    // 请求卡牌列表
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_REQUEST_CARDLIST, 0, 0, obData.data(), obData.size());
}

// 更新请求卡牌事件
void CClan::onClanUpdateRequestCard(LPCSTR pszMsg, int nLen)
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
        return;

    IBankPart *pBankPart = (IBankPart*)pClientEntity->getEntityPart(PART_BANK);
    if (pBankPart == NULL)
        return;

    if (pszMsg == NULL || nLen != sizeof(msg_clan_updaterequestcard_sc))
        return;
    msg_clan_updaterequestcard_sc *pUpdateRequestCard = (msg_clan_updaterequestcard_sc*)pszMsg;

    DBREQ_PARAM_HERO_CARD HeroCard = pBankPart->getHeroCardInfo(pUpdateRequestCard->nCardID);
    int nNeedCount = 0;
    SHeroStarSchemeInfo *pHeroStar = pBankPart->getHeroCardInfoConfig(pUpdateRequestCard->nCardID);
    if (pHeroStar != NULL)
    {
        nNeedCount = pHeroStar->nNeedCount;
    }

    obuf obData;

    cmd_clan_update_request_card card;
    card.nPDBID = pUpdateRequestCard->dwPDBID;
    sstrcpyn(card.szName, pUpdateRequestCard->szName, sizeof(card.szName));
    card.nCardID = pUpdateRequestCard->nCardID;
    card.nMyCurCount = HeroCard.nCardCount;
    card.nMyMaxCount = nNeedCount;
    card.nCurCount = pUpdateRequestCard->nCurCount;
    card.nMaxCount = pUpdateRequestCard->nMaxCount;
    card.nDonateCardCount = pUpdateRequestCard->nDonateCardCount;
    card.nDayDonateCardCount = pUpdateRequestCard->nDayDonateCardCount;
    obData.push_back(&card, sizeof(card));

    // 更新请求卡牌
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_REQUEST_CARD, 0, 0, obData.data(), obData.size());
}

// 移除请求卡牌事件
void CClan::onClanRemoveRequestCard(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_removerequestcard_oc))
        return;
    msg_clan_removerequestcard_oc *pRemoveRequestCard = (msg_clan_removerequestcard_oc*)pszMsg;

    obuf obData;
    cmd_clan_remove_request_card card;
    card.nPDBID = pRemoveRequestCard->dwPDBID;
    obData.push_back(&card, sizeof(card));

    // 移除请求卡牌
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_REMOVE_REQUEST_CARD, 0, 0, obData.data(), obData.size());
}

// 请求捐赠列表事件
void CClan::onClanRequestDonateLog(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen < sizeof(msg_clan_requestdonatelog_oc))
	{
		ErrorLn(__FUNCTION__" nLen=" << nLen << ", HeadSize=" << sizeof(msg_clan_requestdonatelog_oc));
		return;
	}

	msg_clan_requestdonatelog_oc *pHeadData = (msg_clan_requestdonatelog_oc*)pszMsg;
	if (nLen != sizeof(msg_clan_requestdonatelog_oc) + pHeadData->nCount * sizeof(msg_clan_updatedonatelog_oc) )
	{
		ErrorLn(__FUNCTION__" nLen=" << nLen << ", HeadSize=" << sizeof(msg_clan_requestdonatelog_oc) << ", DataLen=" << pHeadData->nCount * sizeof(msg_clan_updatedonatelog_oc));
		return;
	}

	ISchemeHeroCard *pSchemeHeroCard = gClientGlobal->getSchemeCenter()->getSchemeHeroCard();
	if (pSchemeHeroCard == NULL)
		return;

	obuf obData;

	// 卡牌列表头结构
	cmd_clan_request_donatelog_head head;
	head.nCount = pHeadData->nCount;
	obData.push_back(&head, sizeof(head));

	int offset = sizeof(msg_clan_requestdonatelog_oc);
	for (int i=0; i<pHeadData->nCount; ++i)
	{
		msg_clan_updatedonatelog_oc *pUpdateDonateLog = (msg_clan_updatedonatelog_oc*)(pszMsg + offset);
		cmd_clan_update_donatelog donateLog;
		donateLog.nGaveActorID = pUpdateDonateLog->nGaveActorID;
		sstrcpyn(donateLog.szGaveActorName, pUpdateDonateLog->szGaveActorName, sizeof(donateLog.szGaveActorName));
		donateLog.nTakeActorID = pUpdateDonateLog->nTakeActorID;
		sstrcpyn(donateLog.szTakeActorName, pUpdateDonateLog->szTakeActorName, sizeof(donateLog.szTakeActorName));
		donateLog.nCardID = pUpdateDonateLog->nCardID;
		SHeroCardScheme *pCardScheme = pSchemeHeroCard->getByCardID(pUpdateDonateLog->nCardID);
		if (pCardScheme != NULL)
			sstrcpyn(donateLog.szCardName, pCardScheme->szCardName, sizeof(donateLog.szCardName));
		donateLog.nCardNum = pUpdateDonateLog->nCardNum;
		obData.push_back(&donateLog, sizeof(donateLog));

		offset += sizeof(msg_clan_updatedonatelog_oc);
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_REQUEST_DONATELOG, 0, 0, obData.data(), obData.size());
}

// 更新捐赠列表事件
void CClan::onClanUpdateDonateLog(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_updatedonatelog_oc))
		return;

	ISchemeHeroCard *pSchemeHeroCard = gClientGlobal->getSchemeCenter()->getSchemeHeroCard();
	if (pSchemeHeroCard == NULL)
		return;

	msg_clan_updatedonatelog_oc *pUpdateDonateLog = (msg_clan_updatedonatelog_oc*)pszMsg;

	obuf obData;
	cmd_clan_update_donatelog donateLog;
	donateLog.nGaveActorID = pUpdateDonateLog->nGaveActorID;
	sstrcpyn(donateLog.szGaveActorName, pUpdateDonateLog->szGaveActorName, sizeof(donateLog.szGaveActorName));
	donateLog.nTakeActorID = pUpdateDonateLog->nTakeActorID;
	sstrcpyn(donateLog.szTakeActorName, pUpdateDonateLog->szTakeActorName, sizeof(donateLog.szTakeActorName));
	donateLog.nCardID = pUpdateDonateLog->nCardID;
	SHeroCardScheme *pCardScheme = pSchemeHeroCard->getByCardID(pUpdateDonateLog->nCardID);
	if (pCardScheme != NULL)
		sstrcpyn(donateLog.szCardName, pCardScheme->szCardName, sizeof(donateLog.szCardName));
	donateLog.nCardNum = pUpdateDonateLog->nCardNum;
	obData.push_back(&donateLog, sizeof(donateLog));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_UPDATE_DONATELOG, 0, 0, obData.data(), obData.size());
}

// 增加请求提示事件
void CClan::onClanAddRequestTip(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_addrequesttip_oc))
		return;

	ISchemeHeroCard *pSchemeHeroCard = gClientGlobal->getSchemeCenter()->getSchemeHeroCard();
	if (pSchemeHeroCard == NULL)
		return;

	msg_clan_addrequesttip_oc *pMsg = (msg_clan_addrequesttip_oc*)pszMsg;
	obuf obData;
	cmd_clan_add_requestcard_tip cmdData;
	cmdData.nIsInit = pMsg->nIsInit;
	cmdData.nCount = pMsg->nCount;
	obData.push_back(&cmdData, sizeof(cmdData));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_ADD_REQUESTCARD_TIP, 0, 0, obData.data(), obData.size());
}

// 捐卡信息消息
void CClan::onSendRequestCardInfo(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen != sizeof(msg_clan_sendrequestcardinfo_sc))
		return;
	msg_clan_sendrequestcardinfo_sc *pData = (msg_clan_sendrequestcardinfo_sc*)pszMsg;

	cmd_clan_requestcard_info info;
	info.nLeftCooldownTime = pData->nCooldownTime;
	info.nLeftRequestNum = pData->nLeftRequestNum;
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_REQUESTCARD_INFO, 0, 0, &info, sizeof(info));
}

// 填充显示层帮会成员信息
void CClan::fillClanMemberCmd(cmd_clan_memberlist &sCmd, SClientClanMember *pMemberInfo)
{
	sCmd.nPDBID = pMemberInfo->dwPDBID;
	sstrcpyn(sCmd.szName, pMemberInfo->szName, sizeof(sCmd.szName));
	sCmd.nKinID = pMemberInfo->dwKinID;
	sstrcpyn(sCmd.szKinName, pMemberInfo->szKinName, sizeof(sCmd.szKinName));
	sCmd.nFaceID = pMemberInfo->nFaceID;
	sCmd.nLevel = pMemberInfo->nLevel;
	sCmd.nIdentity = pMemberInfo->nIdentity;
	sCmd.nIsOnline = pMemberInfo->nIsOnline;
	sCmd.nGameStatus = pMemberInfo->nGameStatus;
	sCmd.nDonateThisWeek = pMemberInfo->nDonateThisWeek;
	sCmd.nDonateTotal = pMemberInfo->nDonateTotal;
	sCmd.dwBattleVal = pMemberInfo->dwBattleVal;
	sCmd.dwSrcBattleVal = pMemberInfo->dwSrcBattleVal;
	sCmd.dwBVOrder = pMemberInfo->dwBVOrder;
	sCmd.dwLastOrder = pMemberInfo->dwLastOrder;
	sCmd.nLogoutTime = pMemberInfo->nLogoutTime;
    sCmd.nSex = pMemberInfo->nSex;
	sCmd.dwWorldID = pMemberInfo->dwWorldID;
	sCmd.dwSceneID = pMemberInfo->dwSceneID;
	sCmd.serverID = pMemberInfo->serverID;

	// 获得段位信息
	ISchemeMatchRankConfigInfo *pMatchRankConfigInfo = gClientGlobal->getSchemeCenter()->getSchemeMatchRankConfig();
	if (pMatchRankConfigInfo != NULL)
	{
		const SMatchRankConfigSchemeInfo *pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(MatchType_Rank, pMemberInfo->nGrade);
		if (pSchemeInfo != NULL)
		{
			sstrcpyn(sCmd.szGrade, pSchemeInfo->szDes, sizeof(sCmd.szGrade));
		}
	}
}

void CClan::onClanCreateLegendCupDida(LPCSTR pszMsg, int nLen)
{
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CLAN_CREATE_LEGENDCUP_DIDA, 0, 0, NULL, 0);
}
