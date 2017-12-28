/*******************************************************************
** 文件名:	e:\Rocket\Client\Kin\Src\Kin.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	xiaobao
** 日  期:	2009/3/9
** 版  本:	1.0
** 描  述:	战队客户端
** 应  用: 
********************************************************************/
#include "StdAfx.h"
#include "Kin.h"
#include "ExternalHelper.h"
#include "KinClient.h"
#include "KinEvent.h"
#include "Event_ManagedDef.h"
#include "GameViewDef.h"
#include "ISchemeCenter.h"
#include "MatchSchemeDef.h"
#include "IEntityProperty.h"
#include "IClientEntity.h"


CKin* g_pKin = NULL;

// 成员名字排序
bool Less_Member_Name(const SClientKinMember& member1, const SClientKinMember& member2)
{
	if (member1.bIsOnline != member2.bIsOnline)
	{
		return (member1.bIsOnline > member2.bIsOnline);
	}
	if (member1.nIdentity != member2.nIdentity)
	{
		return (member1.nIdentity > member2.nIdentity);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

// 等级排序
bool Less_Member_Level(const SClientKinMember& member1, const SClientKinMember& member2)
{
	if (member1.nLevel != member2.nLevel)
	{
		return (member1.nLevel > member2.nLevel);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

// 职业排序
bool Less_Member_Vocation(const SClientKinMember& member1, const SClientKinMember& member2)
{
	if (member1.nVocation != member2.nVocation)
	{
		return (member1.nVocation > member2.nVocation);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}


// 最后在线排序
bool Less_Member_Online(const SClientKinMember& member1, const SClientKinMember& member2)
{
	if (member1.bIsOnline != member2.bIsOnline)
	{
		return (member1.bIsOnline > member2.bIsOnline);
	}

	if (member1.nLogoutTime != member2.nLogoutTime)
	{
		return (member1.nLogoutTime > member2.nLogoutTime);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

// 最后发送状态排序
bool Less_Member_BoonState(const SClientKinMember& member1, const SClientKinMember& member2)
{


	if (member1.nLogoutTime != member2.nLogoutTime)
	{
		return (member1.nLogoutTime > member2.nLogoutTime);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

// 最后发送状态排序
bool Less_Member_BoonNum(const SClientKinMember& member1, const SClientKinMember& member2)
{

	if (member1.nLogoutTime != member2.nLogoutTime)
	{
		return (member1.nLogoutTime > member2.nLogoutTime);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

bool Less_Member_BattleValue(const SClientKinMember& member1, const SClientKinMember& member2)
{
	// 直接比较当前战力值
	if (member1.dwBattleVal != member2.dwBattleVal)
	{
		return (member1.dwBattleVal > member2.dwBattleVal);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

bool Less_Member_BattleOrder(const SClientKinMember& member1, const SClientKinMember& member2)
{
	if (member1.dwBVOrder == 0 || member2.dwBVOrder == 0)
	{
		return (member1.dwBVOrder > member2.dwBVOrder);
	}
	if (member1.dwBVOrder != member2.dwBVOrder)
	{
		return (member1.dwBVOrder < member2.dwBVOrder);
	}

	return (strncmp(member1.szName, member2.szName, ACTORNAME_MAXSIZE) < 0);
}

/// purpose: 构造函数
CKin::CKin()
{
	m_MemberList.reserve(32);

	reset();

	m_propAddress[emKinProp_ID]				= (int*)&m_dwID;
	m_propAddress[emKinProp_State]			= (int*)&m_nState;
	m_propAddress[emKinProp_Level]			= &m_nLevel;
	m_propAddress[emKinProp_EstablishTime]	= &m_nEstablishTime;
	m_propAddress[emKinProp_Money]			= (int*)&m_nMoney;
	m_propAddress[emKinProp_ShaikhID]		= &m_nShaikhID;
	m_propAddress[emKinProp_Activity]		= (int*)&m_nActivity;
	m_propAddress[emKinProp_WeekActivity]   = (int*)&m_nWeekActivity;
	m_propAddress[emKinProp_Place]			= (int*)&m_nPlace;
	m_propAddress[emKinProp_DismissApplyTime] = &nDismissApplyTime;

	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_APPLY_LIST,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_AUTOACCEPT_SAVE,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_AUTOACCEPT_REQUEST,this );
    gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_REQUEST_KIN_EVENTLIST,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_REQUESTKINGLORY,this );
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_KIN_REQ_WARRECORD_TOTAL,this );
	
    
    
    
}

/// purpose: 虚构函数
CKin::~CKin()
{
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_APPLY_LIST );
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_AUTOACCEPT_SAVE );
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_AUTOACCEPT_REQUEST );
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_REQUEST_KIN_EVENTLIST );
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_REQUESTKINGLORY );
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_KIN_REQ_WARRECORD_TOTAL );
    
    
}

/// purpose: 创建
bool CKin::create(void)
{
    

	return true;
}

void CKin::release()
{
    
}

/// purpose: 释放
void CKin::close(void)
{
	// 释放资源
    
}

/// purpose: 清空所有数据
void CKin::reset(void)
{
	WarningLn("CKin::Reset");
	m_KinVaild = false;
	memset(m_szManifesto, 0, sizeof(m_szManifesto));
	memset(m_szNotice, 0, sizeof(m_szNotice));
	memset(m_szName, 0, sizeof(m_szName));

	m_dwID = 0;
	m_dwCountryID = 0;
	m_nState = emKinState_Invaild;
	m_nEstablishTime = 0;

	
	m_nShaikhID = 0;
	m_nLevel = 0;
	m_nServiceOpenFlag = 0;

	m_nActivity = 0;
	m_nWeekActivity = 0;
	m_nMoney = 0;

	memset(m_szSkillData, 0, sizeof(m_szSkillData));
	m_MemberList.clear();
	m_ApplyMemberList.clear();	

	m_bFirstOpen	= true;

	memset(m_szSlogan, 0, sizeof(m_szSlogan));
	memset(&m_AutoSetting, 0, sizeof(m_AutoSetting));
}


/// purpose: 是否有效
bool CKin::IsVaild(void)
{
	return m_KinVaild;
}

/// purpose: 取得战队的整型数据
int CKin::getNumProp(DWORD dwPropID)
{
	if (dwPropID < emKinProp_ID && dwPropID >= emKinProp_NumPropCount)
	{
		return 0;
	}

	if (m_propAddress[dwPropID] == 0)
	{
		return 0;
	}

	return *(m_propAddress[dwPropID]);
}

/// purpose: 设置战队的整型数据
bool CKin::setNumProp(DWORD dwPropID, int nValue)
{
	if (dwPropID < emKinProp_ID && dwPropID >= emKinProp_NumPropCount)
	{
		return false;
	}

	if (m_propAddress[dwPropID] == 0)
	{
		return false;
	}

	*(m_propAddress[dwPropID]) = nValue;


	//更新到显示
	cmd_kin_numprop_update headData;
	headData.nPropID = dwPropID;
	headData.nValue = nValue;

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_NUM_PROP_UPDATE, 0, "", &headData, sizeof(cmd_kin_numprop_update));

	return true;
}

/// purpose: 取得战队的字符型数据
string CKin::getStringProp(DWORD dwPropID)
{
	switch (dwPropID)
	{
	// 战队名
	case emKinProp_Name:
		{
			return m_szName;
		}
	// 战队的宣言
	case emKinManifesto:
		{
			return m_szManifesto;
		}
	// 战队的每日公告
	case emKinNotice:
		{
			return m_szNotice;
		}
	// 族长名字
	case emKinShaikhName:
		{
			return m_szShaikhName;
		}
	// 宣传口号
	case emKinSlogan:
		{
			return m_szSlogan;
		}
	}

	return "";
}

/// purpose: 设置战队的字符型数据
bool CKin::setStringProp(DWORD dwPropID, string strValue)
{
	switch (dwPropID)
	{
		// 战队名
	case emKinProp_Name:
		{
			sstrcpyn(m_szName, strValue.c_str(), KIN_NAME_SIZE);
			return true;
		}
		// 战队的宣言
	case emKinManifesto:
		{
			sstrcpyn(m_szManifesto, strValue.c_str(), KIN_MANIFESTO_LEN);
			return true;
		}
		// 战队的每日公告
	case emKinNotice:
		{
			sstrcpyn(m_szNotice, strValue.c_str(), KIN_NOTICE_LEN);
		}
		break;
		// 战队族长名字
	case emKinShaikhName:
		{
			sstrcpyn(m_szShaikhName, strValue.c_str(), sizeof(m_szShaikhName));
		}
		break;
		// 宣传口号
	case emKinSlogan:
		{
			sstrcpyn(m_szSlogan, strValue.c_str(), sizeof(m_szSlogan));
			return true;
		}
	}

    //更新到显示
    cmd_kin_stringprop_update headData;
    headData.nPropID = dwPropID;
    memcpy(headData.szText,strValue.c_str(),sizeof(headData.szText));

    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_STRING_PROP_UPDATE, 0, "", &headData, sizeof(cmd_kin_stringprop_update));

	return true;
}

/// purpose: 取得成员的个数
int CKin::getMemberCount()
{
	return m_MemberList.size();
}

/// purpose: 取得成员的数据
//@ param  : 下标，第几个成员
SClientKinMember* CKin::getMemberByIndex(int nIndex)
{
	if (nIndex < 0 || (DWORD)nIndex >= m_MemberList.size())
	{
		return NULL;
	}

	// 取得成员数据
	return &m_MemberList[nIndex];
}

/// purpose: 取得成员的数据
//@ param  : 玩家ID
SClientKinMember* CKin::getMemberByID(DWORD dwPDBID)
{
	vector<SClientKinMember>::iterator it;
	for (it = m_MemberList.begin(); it != m_MemberList.end(); ++ it)
	{	
		SClientKinMember* pMember = &(*it);
		if (pMember->dwPDBID == dwPDBID)
		{
			return pMember;
		}
	}

	return NULL;
}

/// purpose: 删除成员
void CKin::removeMember(DWORD dwPDBID)
{
	vector<SClientKinMember>::iterator it;
	for (it = m_MemberList.begin(); it != m_MemberList.end(); ++ it)
	{	
		SClientKinMember* pMember = &(*it);
		if (pMember->dwPDBID == dwPDBID)
		{
			m_MemberList.erase(it);
			return;
		}
	}
}

/// purpose: 取得申请成员的个数
int CKin::getApplyMemberCount()
{
	return m_ApplyMemberList.size();
}

/// purpose: 取得成员的数据
//@ param  : 下标，第几个成员
SKinClientApplyMember* CKin::getApplyMemberByIndex(int nIndex)
{
	if (nIndex < 0 || (DWORD)nIndex >= m_ApplyMemberList.size())
	{
		return NULL;
	}

	// 取得成员数据
	return &m_ApplyMemberList[nIndex];
}

/// purpose: 删除删除成员
void CKin::removeApplyMember(DWORD dwPDBID)
{
	vector<SKinClientApplyMember>::iterator it;
	for (it = m_ApplyMemberList.begin(); it != m_ApplyMemberList.end(); ++ it)
	{	
		SKinClientApplyMember* pMember = &(*it);
		if (pMember->dwPDBID == dwPDBID)
		{
			m_ApplyMemberList.erase(it);
			return;
		}
	}
}


/// purpose: 排序
//@ param  : nSortType 参照TKinSortType
void CKin::sortMember(int nSortType)
{
	if (m_MemberList.empty() == true)
	{
		return;
	}

	if (nSortType == emKinSort_Name)
	{
		std::sort(m_MemberList.begin(), m_MemberList.end(), Less_Member_Name);
	}
	else if (nSortType == emKinSort_Level)
	{
		std::sort(m_MemberList.begin(), m_MemberList.end(), Less_Member_Level);
	}
	else if (nSortType == emKinSort_Vocation)
	{
		std::sort(m_MemberList.begin(), m_MemberList.end(), Less_Member_Vocation);
	}
	else if (nSortType == emKinSort_Online)
	{
		std::sort(m_MemberList.begin(), m_MemberList.end(), Less_Member_Online);
	}
	else if (nSortType == emKinSort_BattleValue)
	{
		std::sort(m_MemberList.begin(), m_MemberList.end(), Less_Member_BattleValue);
	}
	else if (nSortType == emKinSort_BattleOrder)
	{
		std::sort(m_MemberList.begin(), m_MemberList.end(), Less_Member_BattleOrder);
	}
}


/// purpose: 取得排名
int CKin::getPlace()
{
	return m_nPlace;
}



/// purpose: 取得口号信息
SAutoAccept* CKin::getAutoAccept()
{
	return &m_AutoSetting;
}



// purpose: 更新战队属性
void CKin::onUpdateKinProp(LPCSTR pszMsg, int nLen)
{
 	if (nLen < sizeof(int) * 3)
	{
		ErrorLn("CKin::onUpdateKinProp  2");
		return;
	}

	int offest = 0;
	m_dwID = *(DWORD*)pszMsg;
	offest += sizeof(DWORD);
	int nPropID = *(int*)(pszMsg + offest);
	offest += sizeof(int);
	int nDataLen = *(int*)(pszMsg + offest);
	offest += sizeof(int);
	if (nLen != nDataLen + sizeof(int) * 3)
	{
		ErrorLn("CKin::onUpdateKinProp  1");
		return;
	}

	WarningLn("CKin::onUpdateKinProp");
	m_KinVaild = true;

	// 整型属性类型
	if (nPropID >= emKinProp_ID && nPropID < emKinProp_NumPropCount)
	{
		if (nDataLen != sizeof(int))
		{
			return;
		}

		setNumProp(nPropID, *(int *)(pszMsg + offest));
		return;
	}

	switch (nPropID)
	{
	case emKinProp_Name:
		{
			sstrcpyn(m_szName, (pszMsg + offest), nDataLen);
		}
		break;
	case emKinManifesto:
		{
			sstrcpyn(m_szManifesto, (pszMsg + offest), nDataLen);
		}
		break;
	case emKinNotice:
		{
			sstrcpyn(m_szNotice, (pszMsg + offest), nDataLen);
		}
		break;
	}
}

//接收战队基础信息
void CKin::onRecvKinBaseData(LPCSTR pszMsg, int nLen)
{
    if (nLen < sizeof(SMsgKin_KinBaseData_OC))
    {
        return;
    }

    SMsgKin_KinBaseData_OC* pRecvData = (SMsgKin_KinBaseData_OC*)pszMsg;
    if (nLen != sizeof(SMsgKin_KinBaseData_OC) + pRecvData->nApplyMemberNum * sizeof(SKinClientApplyMember) + pRecvData->nEventDataLen)
    {
        return;
    }
    int offset  = sizeof(SMsgKin_KinBaseData_OC);
    m_nShaikhID     = pRecvData->dwShaikhID;
    m_nLevel        = pRecvData->nKinLevel;
    m_nSysTime      = pRecvData->nSysTime;
    m_nTickCount    = getTickCount();
    m_nPlace        = pRecvData->nPlace;
    m_nActivity     = pRecvData->nActivity;
	m_nWeekActivity = pRecvData->nWeekActivity;
    m_nMoney        = pRecvData->nMoney;
    sstrcpyn(m_szName, pRecvData->szKinName, KIN_NAME_SIZE);
    sstrcpyn(m_szShaikhName, pRecvData->szShaikhName, sizeof(m_szShaikhName));
    sstrcpyn(m_szNotice, pRecvData->szNotice, KIN_NOTICE_LEN);

    // 申请成员数据
    m_ApplyMemberList.clear();
    for (int i = 0; i < pRecvData->nApplyMemberNum; i ++ )
    {
        SKinClientApplyMember* pApplyMemberInfo = (SKinClientApplyMember*)(pszMsg + offset);
        m_ApplyMemberList.push_back(*pApplyMemberInfo);
        offset += sizeof(SKinClientApplyMember);
    }
    // 对数据进行一次排序
    std::sort(m_MemberList.begin(), m_MemberList.end(), Less_Member_Name);

    // 显示层战队数据更新数据组装
    obuf obData;
    cmd_kin_basedata headData;
    headData.nKinID = pRecvData->dwKinID;
    headData.nKinLevel = pRecvData->nKinLevel;              // 战队等级 
    headData.dwShaikhID = pRecvData->dwShaikhID;            // 族长
    headData.nMemberCount = pRecvData->nMemberCount;        // 成员数量
    headData.nOnlineCount = pRecvData->nOnlineCount;
    headData.nKinMaxMember = pRecvData->nKinMaxMember;
    headData.nApplyCount = pRecvData->nApplyMemberNum;
    headData.nTotalFight    = pRecvData->nTotalFight;                          // 战队总战力
    headData.nFightOrder    = pRecvData->nTotalFightOrder;                     // 战队战力排名
    sstrcpyn(headData.szKinName, pRecvData->szKinName, sizeof(headData.szKinName));  // 战队名
    sstrcpyn(headData.szShaikhName, pRecvData->szShaikhName, sizeof(headData.szShaikhName));  // 族长名
    sstrcpyn(headData.szNotice, pRecvData->szNotice, sizeof(headData.szNotice));  // 战队的每日公告
	sstrcpyn(headData.szKinLabel, pRecvData->szKinLabel, sizeof(headData.szKinLabel));
	headData.nMatchWinNum = pRecvData->nLegendWinCount;
	headData.nMatchTotalNum = pRecvData->nLegendTotaLCount;
	headData.nGoldNum = pRecvData->nGoldNum;
	headData.nSilverNum = pRecvData->nSilverNum;
	headData.nWeekActivity = pRecvData->nWeekActivity;

    obData.push_back(&headData, sizeof(cmd_kin_basedata));

    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REQUESTBASEDATA, obData.size(), 0, obData.data(), obData.size());
}

// purpose: 更新战队基本数据
void CKin::onUpdateKinBaseData(LPCSTR pszMsg, int nLen)
{

	SMsgKin_KinBaseData_OC* pRecvData = (SMsgKin_KinBaseData_OC*)pszMsg;
 	if (nLen != sizeof(SMsgKin_KinBaseData_OC))
	{
		return;
	}
	int offset  = sizeof(SMsgKin_KinBaseData_OC);
	m_nShaikhID     = pRecvData->dwShaikhID;
	m_nLevel        = pRecvData->nKinLevel;
	m_nSysTime      = pRecvData->nSysTime;
	m_nTickCount    = getTickCount();
	m_nPlace        = pRecvData->nPlace;
	m_nActivity     = pRecvData->nActivity;
	m_nWeekActivity = pRecvData->nWeekActivity;
	m_nMoney        = pRecvData->nMoney;
    sstrcpyn(m_szName, pRecvData->szKinName, KIN_NAME_SIZE);
    sstrcpyn(m_szShaikhName, pRecvData->szShaikhName, sizeof(m_szShaikhName));
    sstrcpyn(m_szNotice, pRecvData->szNotice, KIN_NOTICE_LEN);

    // 显示层战队数据更新数据组装
    obuf obData;
    cmd_kin_basedata headData;
    headData.nKinID = pRecvData->dwKinID;
    headData.nKinLevel = pRecvData->nKinLevel;              // 战队等级 
    headData.dwShaikhID = pRecvData->dwShaikhID;            // 族长
    headData.nMemberCount = pRecvData->nMemberCount;        // 成员数量
    headData.nOnlineCount = pRecvData->nOnlineCount;
    headData.nKinMaxMember = pRecvData->nKinMaxMember;
    headData.nApplyCount = pRecvData->nApplyMemberNum;
    headData.nTotalFight    = pRecvData->nTotalFight;                          // 战队总战力
    headData.nFightOrder    = pRecvData->nTotalFightOrder;                     // 战队战力排名
    sstrcpyn(headData.szKinName, pRecvData->szKinName, sizeof(headData.szKinName));  // 战队名
    sstrcpyn(headData.szShaikhName, pRecvData->szShaikhName, sizeof(headData.szShaikhName));  // 族长名
    sstrcpyn(headData.szNotice, pRecvData->szNotice, sizeof(headData.szNotice));  // 战队的每日公告
	sstrcpyn(headData.szKinLabel, pRecvData->szKinLabel, sizeof(headData.szKinLabel));
	headData.nMatchWinNum = pRecvData->nLegendWinCount;
	headData.nMatchTotalNum = pRecvData->nLegendTotaLCount;
	headData.nGoldNum = pRecvData->nGoldNum;
	headData.nSilverNum = pRecvData->nSilverNum;
	headData.nWeekActivity = pRecvData->nWeekActivity;

    obData.push_back(&headData, sizeof(cmd_kin_basedata));

     // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REQUESTBASEDATA, obData.size(), 0, obData.data(), obData.size());

}

// purpose: 更新战队成员数据
void CKin::onUpdateKinMemData(LPCSTR pszMsg, int nLen)
{

    ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (NULL == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
        return;
    }
    ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
    if (NULL == pMatchRankConfigInfo)
    {
        ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
        return;
    }

	if (nLen < sizeof(SMsgKin_KinBaseData_OC))
	{
		return;
	}

	SMsgKin_KinBaseData_OC* pRecvData = (SMsgKin_KinBaseData_OC*)pszMsg;
	if (nLen != sizeof(SMsgKin_KinBaseData_OC) + pRecvData->nMemberCount * sizeof(SClientKinMember))
	{
		return;
	}
	int offset = sizeof(SMsgKin_KinBaseData_OC);

    // 显示层战队数据更新数据组装
    obuf obData;
    int nPosNum = pRecvData->nMemberCount;
    cmd_kin_basedata headData;
    headData.nKinID = pRecvData->dwKinID;
    headData.nKinLevel = pRecvData->nKinLevel;              // 战队等级 
    headData.dwShaikhID = pRecvData->dwShaikhID;            // 族长
    headData.nMemberCount = pRecvData->nMemberCount;        // 成员数量
    headData.nOnlineCount = pRecvData->nOnlineCount;
    headData.nKinMaxMember = pRecvData->nKinMaxMember;
    headData.nTotalFight    = pRecvData->nTotalFight;                          // 战队总战力
    headData.nFightOrder    = pRecvData->nTotalFightOrder;                     // 战队战力排名
    sstrcpyn(headData.szKinName, pRecvData->szKinName, sizeof(headData.szKinName));  // 战队名
    sstrcpyn(headData.szShaikhName, pRecvData->szShaikhName, sizeof(headData.szShaikhName));  // 族长名
    sstrcpyn(headData.szNotice, pRecvData->szNotice, sizeof(headData.szNotice));  // 战队的每日公告
	sstrcpyn(headData.szKinLabel, pRecvData->szKinLabel, sizeof(headData.szKinLabel));
	headData.nMatchWinNum = pRecvData->nLegendWinCount;
	headData.nMatchTotalNum = pRecvData->nLegendTotaLCount;
	headData.nGoldNum = pRecvData->nGoldNum;
	headData.nSilverNum = pRecvData->nSilverNum;
	headData.nWeekActivity = pRecvData->nWeekActivity;

    obData.push_back(&headData, sizeof(cmd_kin_basedata));

	m_MemberList.clear();
	for (int i = 0; i < pRecvData->nMemberCount; ++i)
	{
		SClientKinMember* pListMember = (SClientKinMember*)(pszMsg + offset);
		m_MemberList.push_back(*pListMember);

        //组装信息到显示层
        cmd_kin_membernode nodeData;
		nodeData.dwUDBID        = pListMember->dwUDBID;                     // 账号ID
        nodeData.dwPDBID        = pListMember->dwPDBID;						// 玩家ID
        nodeData.nLevel         = pListMember->nLevel;						// 玩家等级
        nodeData.nIdentity      = pListMember->nIdentity;					// 族内职位
        nodeData.bIsOnline      = pListMember->bIsOnline;					// 玩家是否在线
        nodeData.nLogoutTime    = pListMember->nLogoutTime;					// 玩家最后下线时间
        nodeData.nVocation      = pListMember->nVocation;					// 玩家职业
        nodeData.nFaceID        = pListMember->nFaceID;                     // 头像ID
        nodeData.bIsPublicOnline= pListMember->bIsPublicOnline;				// 玩家是否在公共区在线
        nodeData.dwBattleVal    = pListMember->dwBattleVal;					// 战力值
        nodeData.dwSrcBattleVal = pListMember->dwSrcBattleVal;				// 上一次战力值
        nodeData.dwBVOrder      = pListMember->dwBVOrder;					// 战力值排行
        nodeData.dwLastOrder    = pListMember->dwLastOrder;					// 上一次战力值排行
        nodeData.nSex           = pListMember->bySex;						// 性别
        nodeData.byGameState    = pListMember->dwState;                     // 游戏状态
		nodeData.dwWorldID      = pListMember->dwWorldID;                   // 世界ID
		nodeData.dwSceneID      = pListMember->dwSceneID;                   // 场景ID
		nodeData.serverID       = pListMember->serverID;                    // 服务器ID

        // 获得段位信息
        const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(pListMember->nMatchType, pListMember->nGrade);
        if (pSchemeInfo != NULL)
        {
            sstrcpyn( nodeData.szGradeName,(char *)pSchemeInfo->szDes, sizeof(nodeData.szGradeName));
        }
        else
        {    
            sstrcpyn( nodeData.szGradeName,a2utf8("无"), sizeof(nodeData.szGradeName));
        }
        sstrcpyn(nodeData.szName, pListMember->szName, sizeof(nodeData.szName));  // 玩家名字
        obData.push_back(&nodeData, sizeof(nodeData));

		offset += sizeof(SClientKinMember);
	}

    // 由于第5个参数传不过去，  用第二个参数代替长度
    if(m_KinVaild)
    {
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REQUESTMEMBERLIST, obData.size(), 0, obData.data(), obData.size());
    }
}


// purpose: 更新客户端资源信息
void CKin::onKinUpdateClient(LPCSTR pszMsg, int nLen)
{


	if (nLen != sizeof(SMsgKin_UpdateClient_OC))
	{
		ErrorLn(__FUNCTION__": length error!");
		return;
	}

	SMsgKin_UpdateClient_OC* pRecvData = (SMsgKin_UpdateClient_OC*)pszMsg;
	m_KinVaild = true;
	m_dwID = pRecvData->dwID;
	m_nState = pRecvData->nState;
	m_nShaikhID = pRecvData->dwShaikhID;
	sstrcpyn(m_szName, pRecvData->szName, sizeof(m_szName));
	sstrcpyn(m_szShaikhName, pRecvData->szShaikhName, sizeof(m_szShaikhName));
	sstrcpyn(m_szLabel, pRecvData->szKinLabel, sizeof(m_szLabel));

    // 下发基础信息到显示层
    obuf obData;
    cmd_kin_basedata headData;
    headData.nKinID = pRecvData->dwID;
    headData.nKinLevel = pRecvData->nKinLevel;                                                             // 战队等级 
    headData.dwShaikhID= pRecvData->dwShaikhID;                                         // 族长
    sstrcpyn(headData.szKinName, m_szName, sizeof(headData.szKinName));
    sstrcpyn(headData.szShaikhName, m_szShaikhName, sizeof(headData.szShaikhName));		   
    headData.nMemberCount = pRecvData->nMemberCount;
    headData.nOnlineCount = pRecvData->nOnlineCount;
    headData.nKinMaxMember = pRecvData->nKinMaxMember;
    headData.nApplyCount = pRecvData->nApplyCount;
	sstrcpyn(headData.szKinLabel, m_szLabel, sizeof(headData.szKinLabel));
    obData.push_back(&headData,sizeof(headData));

    // 请求下自动收人配置
    g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_AUTO_ACCEPT_REQUEST, m_dwID, 0, 0);

    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REQUESTBASEDATA, obData.size(), 0, obData.data(), obData.size());


	
}

// purpose: 更新留言数据
void CKin::onKinUpdateWord(LPCSTR pszMsg, int nLen)
{
	int offset = 0;
	char szWord[1024] = {0};
	ostrbuf osb;
	while(true)
	{
		if (offset + sizeof(DWORD) + sizeof(short) > (ulong)nLen)
		{
			break;
		}

		DWORD dwPDBID = *(DWORD*)(pszMsg+offset);
		offset += sizeof(DWORD);

		short nWordLen = *(short*)(pszMsg+offset);
		offset += sizeof(short);

		if (offset + nWordLen > nLen)
		{
			break;
		}
		sstrcpyn(szWord, (pszMsg+offset), nWordLen+1);

	}
}



void CKin::onMessage(SNetMsgHead* head, void* data, size_t len)
{
    switch ( head->byKeyAction )
    {
        
    case MSG_KIN_MEMBER_LOGIN :
        {
            onKinMemberLogin((LPCSTR)data,len);
        }
        break;

    case MSG_KIN_UPDATECLIENT :
        {
            onKinUpdateClient((LPCSTR)data,len);
        }
        break;
        
    case MSG_KIN_UPATEKINPORP :
        {
            onUpdateKinProp((LPCSTR)data,len);
        }
        break;

    case MSG_KIN_KINBASEDATA :
        {
            onRecvKinBaseData((LPCSTR)data,len);
        }
        break;

    case MSG_KIN_KINBASEDATA_UPDATE :
        {
            onUpdateKinBaseData((LPCSTR)data,len);
        }
        break;

    case MSG_KIN_DISMISS :
        {
            onKinDisMiss((LPCSTR)data,len);
        }
        break;
        
    case MSG_KIN_KINMEMBERDATA :
        {
            onUpdateKinMemData((LPCSTR)data,len);
        }
        break;

    case MSG_KIN_REQUEST_KINEVENT_LIST :
        {
            onUpdateKinEventList((LPCSTR)data,len);
        }
        break;
	case MSG_KIN_REQUEST_KINGLORYDATA:
		{
			onUpdateKinGloryData((LPCSTR)data,len);
		}
		break;
	case MSG_KIN_REQUEST_WARRECORD_TOTAL:
		{
			onUpdateKinWarRecordTotal((LPCSTR)data,len);
		}
		break;
    default:
        {
            ErrorLn(__FUNCTION__": unknown head->byKeyAction= " << head->byKeyAction);
        }
    }
}

void CKin::onKinDisMiss(LPCSTR pszMsg, int nLen)
{
    reset();
    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_DISMISS, 0, 0, NULL, 0);
}


bool CKin::onViewEvent(int32 eventID,int nParam,const char * strParam,void * ptrParam)
{
    switch ( eventID )
    {
    case  GVIEWCMD_KIN_APPLY_LIST :
        {
            // 邀请答复 
            onViewApplyListRequest();
        }
        break;  
    case  GVIEWCMD_KIN_AUTOACCEPT_SAVE :
        {
            if (strParam != NULL)
            {
                // 自动接收设置保存
                cmd_kin_auto_accept *headData =(cmd_kin_auto_accept *)ptrParam;
                SMsgKin_EditAutoAccept data;
                data.bAutoAccept = headData->bAutoAccept == 1? true: false;
                data.nAcceptLevel = headData->nAcceptLevel;
                data.dwAcceptFlag = headData->dwAcceptFlag;
                g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_AUTO_ACCEPT_EDIT, m_dwID, &data, sizeof(data));
            }
            
        }
        break;

    case  GVIEWCMD_KIN_AUTOACCEPT_REQUEST :
        {
            // 自动接收设置请求
            onViewAutoAcceptRequest();
        }
        break;

    case  GVIEWCMD_REQUEST_KIN_EVENTLIST :
        {
            // 请求战队事件列表
            g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_REQUEST_KINEVENT_LIST, m_dwID, NULL, 0);
        }
        break;

	case  GVIEWCMD_KIN_REQUESTKINGLORY :
		{
			// 请求战队荣誉信息
			g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_REQUEST_KINGLORYDATA, m_dwID, NULL, 0);
		}
		break;
	case GVIEWCMD_KIN_REQ_WARRECORD_TOTAL:
		{
			// 请求战队总历史战绩
			g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_REQUEST_WARRECORD_TOTAL, m_dwID, NULL, 0);
		}
		break;

    default:
        ErrorLn( __FUNCTION__": unhandled eventID=" << eventID );
        return false;
    }

    return true;
}

bool CKin::dealClientEvent(LPCSTR pszMsg, int nLen)
{
    if (nLen < sizeof(SMsgKin_EventHead_OC))
    {
        return false;
    }

    SMsgKin_EventHead_OC* pRecvHead = (SMsgKin_EventHead_OC*)pszMsg;
    LPCSTR szContext = pszMsg + sizeof(SMsgKin_EventHead_OC);
    int nContextLen = nLen - sizeof(SMsgKin_EventHead_OC);

    switch (pRecvHead->wEventID)
    {
        // 成员加入
    case EVENT_KIN_ADDMEMBER:
        {
            ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
            if (NULL == pSchemeCenter)
            {
                ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
                return false;
            }
            ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
            if (NULL == pMatchRankConfigInfo)
            {
                ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
                return false;
            }
            if (nContextLen != sizeof(SEventKin_AddMember_OC))
            {
                return false;
            }

            SEventKin_AddMember_OC* pEventCoutext = (SEventKin_AddMember_OC*)szContext;

            removeMember(pEventCoutext->dwPDBID);
            SClientKinMember member;
            member.dwUDBID  = pEventCoutext->dwUDBID;
            member.dwPDBID = pEventCoutext->dwPDBID;
            sstrcpyn(member.szName, pEventCoutext->szName, ACTORNAME_MAXSIZE);
            member.nLevel		 = pEventCoutext->nLevel;
            member.nIdentity	 = pEventCoutext->nIdentity;
            member.bIsOnline	 = pEventCoutext->bIsOnline;
            member.nLogoutTime	 = pEventCoutext->nLogoutTime;
            member.nVocation	 = pEventCoutext->nVocation;
            member.nFaceID      = pEventCoutext->nFaceID;
            member.dwBattleVal	 = pEventCoutext->dwBattleVal;
            member.dwSrcBattleVal= pEventCoutext->dwSrcBattleVal;
            member.dwBVOrder	 = pEventCoutext->dwBVOrder;
            member.dwLastOrder	 = pEventCoutext->dwLastOrder;
            //段位信息
            member.nMatchType	    = pEventCoutext->nMatchType;
            member.nRankScore	    = pEventCoutext->nRankScore;
            member.nGrade	        = pEventCoutext->nGrade;

            m_MemberList.push_back(member);

            // 发到显示层显示
            obuf obData;
            cmd_kin_membernode nodeData;
			nodeData.dwUDBID        = pEventCoutext->dwUDBID;                           // 账号ID
            nodeData.dwPDBID        = pEventCoutext->dwPDBID;							// 玩家ID
            nodeData.nLevel         = pEventCoutext->nLevel;							// 玩家等级
            nodeData.nIdentity      = pEventCoutext->nIdentity;							// 族内职位
            nodeData.bIsOnline      = pEventCoutext->bIsOnline == true? 1: 0;			// 玩家是否在线
            nodeData.nLogoutTime    = pEventCoutext->nLogoutTime;						// 玩家最后下线时间
            nodeData.nVocation      = pEventCoutext->nVocation;							// 玩家职业
            nodeData.nFaceID        = pEventCoutext->nFaceID;                           // 头像ID
            nodeData.dwBattleVal    = pEventCoutext->dwBattleVal;						// 战力值
            nodeData.dwSrcBattleVal = pEventCoutext->dwSrcBattleVal;				    // 上一次战力值
            nodeData.dwBVOrder      = pEventCoutext->dwBVOrder;							// 战力值排行
            nodeData.dwLastOrder    = pEventCoutext->dwLastOrder;						// 上一次战力值排行
            nodeData.nSex           = pEventCoutext->bySex;						        // 性别
            nodeData.byGameState    = pEventCoutext->dwState;                     // 游戏状态
            sstrcpyn(nodeData.szName, pEventCoutext->szName, sizeof(nodeData.szName));

            // 获得段位信息
            const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(pEventCoutext->nMatchType, pEventCoutext->nGrade);
            if (pSchemeInfo != NULL)
            {
                sstrcpyn( nodeData.szGradeName,(char *)pSchemeInfo->szDes, sizeof(nodeData.szGradeName));
            }
            else
            {
                sstrcpyn( nodeData.szGradeName,a2utf8("无"), sizeof(nodeData.szGradeName));
            }

            obData.push_back(&nodeData, sizeof(nodeData));
            // 由于第5个参数传不过去，  用第二个参数代替长度
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_ADDMEMBER, obData.size(), 0, obData.data(), obData.size());

        }
        break;
        // 删除成员
    case EVENT_KIN_REMOVEMEMBER:
        {
            if (nContextLen != sizeof(SEventKin_RemoveMember_OC))
            {
                return false;
            }

            SEventKin_RemoveMember_OC* pEventCoutext = (SEventKin_RemoveMember_OC*)szContext;
            removeMember(pEventCoutext->dwPDBID);

            // 发到显示层显示
            obuf obData;
            cmd_kin_remove_member headData;
            headData.nPlayerPDBID = pEventCoutext->dwPDBID;										// 移除人员
            obData.push_back(&headData, sizeof(cmd_kin_remove_member));

            // 由于第5个参数传不过去，  用第二个参数代替长度
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REMOVEMEMBER, obData.size(), 0, obData.data(), obData.size());

        }
        break;
        // 申请加入
    case EVENT_KIN_ADDAPPLY:
        {

            if (nContextLen != sizeof(SEventKin_AddApply_OC))
            {
                ErrorLn(__FUNCTION__": data length is wrong!");
                return false;
            }
           
            SEventKin_AddApply_OC* pEventCoutext = (SEventKin_AddApply_OC*)szContext;
            removeApplyMember(pEventCoutext->dwPDBID);
            SKinClientApplyMember applyMemberInfo;
            applyMemberInfo.dwPDBID = pEventCoutext->dwPDBID;
            sstrcpyn(applyMemberInfo.szName, pEventCoutext->szName, sizeof(applyMemberInfo.szName));
            applyMemberInfo.nLevel = pEventCoutext->nLevel;
            applyMemberInfo.nVocation = pEventCoutext->nVocation;
            m_ApplyMemberList.push_back(applyMemberInfo);

            // 发到显示层显示
            obuf obData;
            cmd_kin_applylist_node headData;
            headData.nLevel         = pEventCoutext->nLevel;
            headData.nPlayerPDBID   = pEventCoutext->dwPDBID;
            headData.nPower         = pEventCoutext->dwPower;
            sstrcpyn(headData.szName, pEventCoutext->szName, sizeof(headData.szName));  // 玩家名
            obData.push_back(&headData, sizeof(cmd_kin_applylist_node));

            // 由于第5个参数传不过去，  用第二个参数代替长度
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_ADDAPPLY, obData.size(), 0, obData.data(), obData.size());
        }
        break;

        // 删除申请中成员
    case EVENT_KIN_REMOVEAPPLY:
        {
            if (nContextLen != sizeof(SEventKin_RemoveApply_OC))
            {
                return false;
            }

            SEventKin_RemoveApply_OC* pEventCoutext = (SEventKin_RemoveApply_OC*)szContext;
            removeApplyMember(pEventCoutext->dwPDBID);

            // 发到显示层显示
            obuf obData;
            cmd_kin_remove_apply headData;
            headData.nPlayerPDBID = pEventCoutext->dwPDBID;										// 移除人员
            obData.push_back(&headData, sizeof(cmd_kin_remove_apply));

            // 由于第5个参数传不过去，  用第二个参数代替长度
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REMOVEAPPLY, obData.size(), 0, obData.data(), obData.size());
        }
        break;
    case EVENT_KIN_UPDATEMEMBER:
        {
            ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
            if (NULL == pSchemeCenter)
            {
                ErrorLn(__FUNCTION__": NULL == pSchemeCenter");
                return false;
            }
            ISchemeMatchRankConfigInfo* pMatchRankConfigInfo = pSchemeCenter->getSchemeMatchRankConfig();
            if (NULL == pMatchRankConfigInfo)
            {
                ErrorLn(__FUNCTION__": NULL == pMatchRankConfigInfo");
                return false;
            }
            if (nContextLen != sizeof(SEventKin_UpdateMember_C))
            {
                return false;
            }

            SEventKin_UpdateMember_C* pEventCoutext = (SEventKin_UpdateMember_C*)szContext;
            SClientKinMember* pKinMember = getMemberByID(pEventCoutext->dwPDBID);
            if (pKinMember != NULL)
            {
                pKinMember->nLevel = pEventCoutext->nLevel;
                pKinMember->nIdentity = pEventCoutext->nIdentity;
                pKinMember->bIsOnline = pEventCoutext->bIsOnline;
                pKinMember->nLogoutTime = pEventCoutext->nLogoutTime;
                pKinMember->nVocation = pEventCoutext->nVocation;
                pKinMember->nFaceID = pEventCoutext->nFaceID;
                pKinMember->dwBattleVal	 = pEventCoutext->dwBattleVal;
                pKinMember->dwSrcBattleVal= pEventCoutext->dwSrcBattleVal;
                pKinMember->dwBVOrder	 = (BYTE)pEventCoutext->dwBVOrder;
                pKinMember->dwLastOrder	 = (BYTE)pEventCoutext->dwLastOrder;
                pKinMember->dwState	    = (BYTE)pEventCoutext->dwState;

                pKinMember->nMatchType  = pEventCoutext->nMatchType;
                pKinMember->nRankScore  = pEventCoutext->nRankScore;
                pKinMember->nGrade      = pEventCoutext->nGrade;
                pKinMember->bySex       = pEventCoutext->bySex;
                sstrcpyn(pKinMember->szName, pEventCoutext->szName, ACTORNAME_MAXSIZE);

                // 发到显示层显示
                obuf obData;
                cmd_kin_membernode nodeData;
                nodeData.dwUDBID        = pKinMember->dwUDBID;
                nodeData.dwPDBID        = pEventCoutext->dwPDBID;							// 玩家ID
                nodeData.nLevel         = pEventCoutext->nLevel;							// 玩家等级
                nodeData.nIdentity      = pEventCoutext->nIdentity;							// 族内职位
                nodeData.bIsOnline      = pEventCoutext->bIsOnline;							// 玩家是否在线
                nodeData.nLogoutTime    = pEventCoutext->nLogoutTime;						// 玩家最后下线时间
                nodeData.nVocation      = pEventCoutext->nVocation;							// 玩家职业
                nodeData.nFaceID        = pEventCoutext->nFaceID;                           // 头像ID
                nodeData.dwBattleVal    = pEventCoutext->dwBattleVal;						// 战力值
                nodeData.dwSrcBattleVal = pEventCoutext->dwSrcBattleVal;				    // 上一次战力值
                nodeData.dwBVOrder      = pEventCoutext->dwBVOrder;							// 战力值排行
                nodeData.dwLastOrder    = pEventCoutext->dwLastOrder;						// 上一次战力值排行
                nodeData.nSex           = pEventCoutext->bySex;						        // 性别
                nodeData.byGameState    = pEventCoutext->dwState;                     // 游戏状态
                sstrcpyn(nodeData.szName, pEventCoutext->szName, sizeof(nodeData.szName));


                // 获得段位信息
                const SMatchRankConfigSchemeInfo* pSchemeInfo = pMatchRankConfigInfo->getMatchRankConfigShemeInfo(pEventCoutext->nMatchType, pEventCoutext->nGrade);
                if (pSchemeInfo != NULL)
                {
                    sstrcpyn( nodeData.szGradeName,(char *)pSchemeInfo->szDes, sizeof(nodeData.szGradeName));
                }
                else
                {
                    sstrcpyn( nodeData.szGradeName,a2utf8("无"), sizeof(nodeData.szGradeName));
                }

                obData.push_back(&nodeData, sizeof(nodeData));
                // 由于第5个参数传不过去，  用第二个参数代替长度
                gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_UPDATEMEMBER, obData.size(), 0, obData.data(), obData.size());

            }
        }
        break;
        // 战队整型属性改变
    case EVENT_KIN_UPDATE_NUMPROP:
        {
            if (nContextLen != sizeof(SEventKin_UpdateNumProp_C))
            {
                return false;
            }

            SEventKin_UpdateNumProp_C* pEventCoutext = (SEventKin_UpdateNumProp_C*)szContext;
            setNumProp(pEventCoutext->nPropID, pEventCoutext->nValue);
            
        }
        break;
        // 战队字符型属性改变
    case EVENT_KIN_UPDATE_STRINGPROP:
        {
            if (nContextLen != sizeof(SEventKin_UpdateStringProp_C))
            {
                return false;
            }

            SEventKin_UpdateStringProp_C* pEventCoutext = (SEventKin_UpdateStringProp_C*)szContext;
            setStringProp(pEventCoutext->nPropID, pEventCoutext->szValue);
            
        }
        break;
        // 编辑自动接收
    case EVENT_KIN_EDITAUTOACCEPT:
        {
            if (nContextLen != sizeof(SEventKin_EditAutoAccept))
            {
                return false;
            }
            SEventKin_EditAutoAccept* pEvent = (SEventKin_EditAutoAccept*)szContext;

            m_AutoSetting.bAutoAccept = pEvent->bAutoAccept;
            m_AutoSetting.dwAcceptFlag = pEvent->dwAcceptFlag;
            m_AutoSetting.nAcceptLevel = pEvent->nAcceptLevel;

            // 下发到客户端显示
            cmd_kin_auto_accept headData;
            headData.bAutoAccept = m_AutoSetting.bAutoAccept == true? 1: 0;
            headData.dwAcceptFlag = m_AutoSetting.dwAcceptFlag;
            headData.nAcceptLevel = m_AutoSetting.nAcceptLevel;

            // 由于第5个参数传不过去，  用第二个参数代替长度
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_AUTOACCEPT_SAVE, 0, 0, &headData, sizeof(cmd_kin_auto_accept));
        }
        break;
        // 请求自动设置数据
    case EVENT_KIN_AUTOACCEPTREQUEST:
        {

			if (nContextLen != sizeof(SEventKin_AutoAcceptRequest))
            {
                return false;
            }
            SEventKin_AutoAcceptRequest* pEvent = (SEventKin_AutoAcceptRequest*)szContext;
            m_AutoSetting.dwAcceptFlag = pEvent->dwAcceptFlag;
            m_AutoSetting.nAcceptLevel = pEvent->nAcceptLevel;
            m_AutoSetting.bAutoAccept = pEvent->bAutoAccept;
            
        }
        break;
    case EVENT_KIN_CREATE_SUCCESS:
        {
            // 发到显示层显示
            if (nContextLen != sizeof(SEventKin_CreateSuccess_OC))
            {
                return false;
            }

            SEventKin_CreateSuccess_OC* pEventCoutext = (SEventKin_CreateSuccess_OC*)szContext;

            // 发到显示层显示
            obuf obData;
            cmd_kin_create_success headData;
            headData.bSuccess = pEventCoutext->bSuccess;
            obData.push_back(&headData, sizeof(cmd_kin_create_success));

            // 由于第5个参数传不过去，  用第二个参数代替长度
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_CREATE_SUCCESS, obData.size(), 0, obData.data(), obData.size());
            
        }
        break;
    case EVENT_KIN_PERSONJOINEDKIN:
        {
            // 发到显示层显示
            if (nContextLen != sizeof(SEventKin_PersonJoinedKin))
            {
                return false;
            }

            SEventKin_PersonJoinedKin* pEventCoutext = (SEventKin_PersonJoinedKin*)szContext;

            // 发到显示层显示
            obuf obData;
            cmd_kin_join_success cmdData;
            cmdData.bSuccess = pEventCoutext->bInSuccess;
            obData.push_back(&cmdData, sizeof(cmd_kin_join_success));

            // 由于第5个参数传不过去，  用第二个参数代替长度
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_JOIN_SUCCESS, obData.size(), 0, obData.data(), obData.size());

        }
        break;
    case EVENT_KIN_ANSWERINVITE_SUCCESS:
        {
            // 由于第5个参数传不过去，  用第二个参数代替长度
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_ANSWERINVITE_SUCCESS, 0, 0, 0, 0);

        }
        break;
        
    default:
        break;
    }

    return true;
}

void CKin::onViewApplyListRequest()
{
    obuf obData;
    obuf obNodeData;
    cmd_kin_applylist_head headData;
    sstrcpyn(headData.szSlogan,"我只是来测试口号的", sizeof(headData.szSlogan));
    vector<SKinClientApplyMember>::iterator iter = m_ApplyMemberList.begin();
    for (; iter != m_ApplyMemberList.end() ; ++iter )
    {
        // 显示层战队数据更新数据组装
        cmd_kin_applylist_node nodeData;
        nodeData.nLevel = iter->nLevel;
        nodeData.nPlayerPDBID = iter->dwPDBID;
        nodeData.nPower = 0;
        sstrcpyn(nodeData.szName, iter->szName, sizeof(nodeData.szName));  // 玩家名

        obNodeData.push_back(&nodeData, sizeof(nodeData));
        headData.nCount++;
    }
   
    obData.push_back(&headData, sizeof(cmd_kin_applylist_head));
    obData.push_back(obNodeData.data(), obNodeData.size());

    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_APPLY_LIST, obData.size(), 0, obData.data(), obData.size());
}

void CKin::onViewAutoAcceptRequest()
{
    // 下发到客户端显示
    cmd_kin_auto_accept headData;
    headData.bAutoAccept = m_AutoSetting.bAutoAccept == true? 1: 0;
    headData.dwAcceptFlag = m_AutoSetting.dwAcceptFlag;
    headData.nAcceptLevel = m_AutoSetting.nAcceptLevel;

    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_AUTOACCEPT_REQUEST, 0, 0, &headData, sizeof(cmd_kin_auto_accept));
}

void CKin::onKinMemberLogin(LPCSTR pszMsg, int nLen)
{
    /// 取得主角对象
    if (nLen != sizeof(SMsgKin_MemberLogin_OC))
    {
        ErrorLn(__FUNCTION__": length error!");
        return;
    }

    SMsgKin_MemberLogin_OC* pRecvData = (SMsgKin_MemberLogin_OC*)pszMsg;
    m_KinVaild = true;
    m_dwID = pRecvData->dwID;
    m_nState = pRecvData->nState;
    m_nShaikhID = pRecvData->dwShaikhID;
    sstrcpyn(m_szName, pRecvData->szName, sizeof(m_szName));
    sstrcpyn(m_szShaikhName, pRecvData->szShaikhName, sizeof(m_szShaikhName));

    // 下发基础信息到显示层
    obuf obData;
    cmd_kin_basedata headData;
    headData.nKinID = m_dwID;
    headData.nKinLevel = pRecvData->nKinLevel;                                          // 战队等级 
    headData.dwShaikhID= pRecvData->dwShaikhID;                                         // 族长
    sstrcpyn(headData.szKinName, m_szName, sizeof(headData.szKinName));
    sstrcpyn(headData.szShaikhName, m_szShaikhName, sizeof(headData.szShaikhName));		   
    headData.nMemberCount = pRecvData->nMemberCount;
    headData.nOnlineCount = pRecvData->nOnlineCount;
    headData.nKinMaxMember = pRecvData->nKinMaxMember;
    headData.nApplyCount = pRecvData->nApplyCount;
    obData.push_back(&headData,sizeof(headData));

    // 请求下自动收人配置
    g_EHelper.SendKinSocial(emMsgKin_Kin, MSG_KIN_AUTO_ACCEPT_REQUEST, m_dwID, 0, 0);

    // 由于第5个参数传不过去，  用第二个参数代替长度
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_REQUESTBASEDATA, obData.size(), 0, obData.data(), obData.size());

    // 通知给显示层战队引导信息
}

void CKin::onUpdateKinEventList(LPCSTR pszMsg, int nLen)
{
    if (nLen < sizeof(SKinEventHead))
    {
        return;
    }

    SKinEventHead* pRecvData = (SKinEventHead*)pszMsg;
    if (nLen != sizeof(SKinEventHead) + pRecvData->nCount * sizeof(SKinEvent))
    {
        return;
    }

    if (pRecvData->nCount <= 0)
    {
        return;
    }

    // 显示层战队数据更新数据组装
    obuf obData;
    SKinEvent *pData  = (SKinEvent *)(pRecvData + 1);
    for (int i = 0; i < pRecvData->nCount; ++i)
    {
        //组装信息到显示层
        cmd_kin_eventlist nodeData;
        nodeData.nYear  = pData->tTime.tm_year;							    // 年
        nodeData.nMonth = pData->tTime.tm_mon;							    // 月
        nodeData.nDay   = pData->tTime.tm_mday;							    // 日
        nodeData.nHour  = pData->tTime.tm_hour;							    // 小时
        nodeData.nMinute= pData->tTime.tm_min;							    // 分钟
        
        sstrcpyn(nodeData.szCoutext, pData->szCoutext , sizeof(nodeData.szCoutext));

        obData.push_back(&nodeData, sizeof(nodeData));
        pData++;
    }

    // 由于第5个参数传不过去，  用第二个参数代替长度
    if(m_KinVaild)
    {
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_RECV_KIN_EVENTLIST, pRecvData->nCount, 0, obData.data(), obData.size());
    }
}

// purpose: 更新战队荣誉数据
void CKin::onUpdateKinGloryData(LPCSTR pszMsg, int nLen)
{
	if (nLen < sizeof(SMsgKinGloryHead_OC))
	{
		return;
	}

	SMsgKinGloryHead_OC* pRecvData = (SMsgKinGloryHead_OC*)pszMsg;
	if (nLen != sizeof(SMsgKinGloryHead_OC) + pRecvData->nCount * sizeof(SMsgKinGloryNode_OC))
	{
		return;
	}

	// 显示层战队荣誉数据组装
	obuf obData;

	cmd_kin_glory_head gloryHead;
	sstrcpyn(gloryHead.szKinName, pRecvData->szKinName, sizeof(gloryHead.szKinName));
	gloryHead.nWinNum = pRecvData->nWinNum;
	gloryHead.nLoseNum = pRecvData->nLoseNum;
	gloryHead.nGodNum = pRecvData->nGodNum;
	gloryHead.n5KillNum = pRecvData->n5KillNum;
	gloryHead.n4KillNum = pRecvData->n4KillNum;
	gloryHead.n3KillNum = pRecvData->n3KillNum;
	gloryHead.nDeathNum = pRecvData->nDeathNum;
	gloryHead.nKillNum = pRecvData->nKillNum;
	gloryHead.nAssistNum = pRecvData->nAssistNum;
	gloryHead.nCount = pRecvData->nCount;
	obData.push_back(&gloryHead, sizeof(gloryHead));

	SMsgKinGloryNode_OC *pData  = (SMsgKinGloryNode_OC *)(pRecvData + 1);
	for (int i = 0; i < pRecvData->nCount; ++i)
	{
		cmd_kin_glory_node gloryNode;
		gloryNode.llLegendCupId = pData->llLegengCupID;
		gloryNode.nCupConfigID = pData->nCupConfigID;
		gloryNode.nEndTime = pData->dwCupEndTime;
		gloryNode.nCupRank = pData->byCupRank;
		sstrcpyn(gloryNode.szCupName, pData->szCupName , sizeof(gloryNode.szCupName));
		obData.push_back(&gloryNode, sizeof(gloryNode));

		pData++;
	}

	if(m_KinVaild)
	{
		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_RECV_KIN_GLORYDATA, 0, 0, obData.data(), obData.size());
	}
}

// purpose: 更新战队总历史战绩
void CKin::onUpdateKinWarRecordTotal(LPCSTR pszMsg, int nLen)
{
	if (nLen < sizeof(int))
	{
		return;
	}

	int* nCount = (int*)pszMsg;
	if (nLen != sizeof(int) + (*nCount) * sizeof(SMsgKinTotalWarNode_OC))
	{
		return;
	}

	// 显示层战队荣誉数据组装
	obuf obData;
	obData << (*nCount);

	SMsgKinTotalWarNode_OC *pData  = (SMsgKinTotalWarNode_OC *)(nCount + 1);
	for (int i = 0; i < (*nCount); ++i)
	{
		cmd_kin_warrecord_total_warnode warnode;
		warnode.nWarID = pData->nWarID;
		warnode.nWinKinID = pData->nWinKin;
		warnode.nBeginTime = pData->nWinKin;
		sstrcpyn(warnode.szKin1Name, pData->szKin1Name , sizeof(warnode.szKin1Name));
		sstrcpyn(warnode.szKin2Name, pData->szKin2Name , sizeof(warnode.szKin2Name));
		obData.push_back(&warnode, sizeof(warnode));

		pData++;
	}

	if(m_KinVaild)
	{
		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_KIN_RECV_WARRECORD_TOTAL, 0, 0, obData.data(), obData.size());
	}
}