#include "stdafx.h"
#include "Clan.h"
#include "ClanService.h"
#include "ClanInviteManager.h"
#include "ISocialGlobal.h"
#include "IDBEngineService.h"
#include "Event_ManagedDef.h"
#include "ExternalHelper.h"
#include "IKinService.h"
#include "IKin.h"
#include "IShareServer.h"
#include "KinHelper.h"
#include "ClanService.h"
#include "Kin_ManagedDef.h"
#include "EventDef.h"
#include "IChatManager.h"
#include "IChatService.h"
#include "ChatHelper.h"
#include "LegendCupHelper.h"
#include "ILegendCupService.h"
#include "LegendCup_ManagedDef.h"
#include "MailHelper.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"

using namespace Kin_ManagerDef;
using namespace Clan_ManagerDef;
using namespace LegendCup_ManagedDef;

// 构造函数
CClan::CClan()
{
    m_pClanService = NULL;
    m_dwReadFlag = 0;
    m_dwID = 0;
    memset(m_szName, 0, sizeof(m_szName));
    m_nState = emClanState_Normal;
	m_nFlag = 0;
    m_nLevel = 0;
    memset(m_szManifesto, 0, sizeof(m_szManifesto));
    memset(m_szNotice, 0, sizeof(m_szNotice));
    m_nEstablishTime = 0;
    m_nDismissApplyTime = 0;
    m_ApplyKinList.clear();
    m_nShaikhID = 0;
	m_ShaikhState = ClanShaikhState_Unknow;
    m_nMemberIDList.clear();
    m_nMemberCount = 0;
    m_nMemberLastLogoutTime = 0;
    memset(m_propAddress, 0, sizeof(m_propAddress));
    m_nEstablishTime = 0;
    m_nConstructDegree = 0;
    m_nGold = 0;
    m_nActivity = 0;
	m_nWeekActivity = 0;
	m_nWeekCount = 0;
    m_nPlace = 0;
    m_nWorldPlace = 0;
    m_bReadFinish = false;
    m_bSaveDB = false;
    m_nHistoryActivity = 0;
    m_nWorldID = 0;
    m_nKickTime=0;
    m_nKickNum =0;
	m_nRequestCardIndex = 0;
	m_nLegendCupCount = 0;
	m_nAutoAcceptFlag = 0;

    // 成员地址初始化
    m_propAddress[emClanProp_ID] = (int*)&m_dwID;
	m_propAddress[emClanProp_State] = (int*)&m_nState;
	m_propAddress[emClanProp_Flag] = &m_nFlag;
	m_propAddress[emClanProp_Level] = &m_nLevel;
	m_propAddress[emClanProp_EstablishTime] = &m_nEstablishTime;
	m_propAddress[emClanProp_DismissApplyTime] = &m_nDismissApplyTime;
	m_propAddress[emClanProp_ShaikhID] = &m_nShaikhID;
	m_propAddress[emClanProp_MemberLastLogoutTime] = &m_nMemberLastLogoutTime;
	m_propAddress[emClanProp_ReadFlag] = (int*)&m_dwReadFlag;
	m_propAddress[emClanProp_ConstructDegree] = (int*)&m_nConstructDegree;
	m_propAddress[emClanProp_Gold] = (int*)&m_nGold;
	m_propAddress[emClanProp_Activity] = (int*)&m_nActivity;
	m_propAddress[emClanProp_WeekActivity] = (int*)&m_nWeekActivity;
	m_propAddress[emClanProp_Place] = (int*)&m_nPlace;
	m_propAddress[emClanProp_WorldID] = (int*)&m_nWorldID;
	m_propAddress[emClanProp_KickTime] = (int*)&m_nKickTime;
	m_propAddress[emClanProp_KickNum]  = (int*)&m_nKickNum;	
	m_propAddress[emClanProp_MemberCount] = (int*)&m_nMemberCount;
	m_propAddress[emClanProp_LegendCupCount] = (int*)&m_nLegendCupCount;
	m_propAddress[emClanProp_AutoAcceptFlag] = (int*)&m_nAutoAcceptFlag;
	m_propAddress[emClanProp_Shaikh_State]   = (int*)&m_ShaikhState;

    // 状态地址初始化
    m_StateCtrl[emClanState_Normal] = &m_NormalState;
    m_StateCtrl[emClanState_Dismissing] = &m_DismissingState;

    m_bIsInit = false;
    m_bClanLegendDidaSend = false;
}

// 虚构函数
CClan::~CClan()
{

}

//////////////////////////TimerHandler///////////////////////////
void CClan::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
		case TimerID_SaveDB: // 存盘定时器
		{
			if (m_bSaveDB == true)
			{
				updateDataToDB();
				m_bSaveDB = false;
			}
		}
		break;

		default:
			break;
	}
}

// 创建，启动社会服时，从数据库中读取联盟数据来创建
bool CClan::onDBCreate(CClanService *pClanService, LPCSTR pszContext, int nLen)
{
    m_pClanService = pClanService;

    if (pszContext == NULL || nLen != sizeof(DBREQ_TABLE_CLAN))
    {
        return false;
    }

    // 设置数据
    DBREQ_TABLE_CLAN* pDBContext = (DBREQ_TABLE_CLAN*)pszContext;

    m_dwID				= pDBContext->dwClanID;				// 联盟ID
    m_nLevel			= pDBContext->nLevel;				// 联盟等级
	m_nFlag				= pDBContext->nFlag;				// 联盟旗帜
    m_nEstablishTime	= pDBContext->nEstablishTime;		// 联盟创立时间
    m_nDismissApplyTime = pDBContext->nDismissApplyTime;	// 发起解散的时间
    m_nShaikhID			= pDBContext->nShaikhID;			// 联盟ID
    m_nActivity			= pDBContext->nActivity;			// 活跃度
	m_nWeekActivity     = pDBContext->nWeekActivity;        // 周活跃度
    m_nPlace			= pDBContext->nPlace % 10000;		// 排名
    m_nWorldPlace		= pDBContext->nPlace / 10000;		// 世界排名
    m_nHistoryActivity	= pDBContext->nHisActivity;			// 历史活跃度
	m_nLegendCupCount	= pDBContext->nLegendCupCount;		// 可举办杯赛次数

    sstrcpyn(m_szName, pDBContext->szName, sizeof(m_szName));			// 联盟名
	sstrcpyn(m_szShaikhName, pDBContext->szShaikhName, sizeof(m_szShaikhName));	// 族长名字
	sstrcpyn(m_szManifesto, pDBContext->szManifesto, sizeof(m_szManifesto));	// 联盟宣言
	sstrcpyn(m_szNotice, pDBContext->szNotice, sizeof(m_szNotice));	// 联盟公告

    m_nWorldID = getThisGameWorldID();									// 游戏世界ID

    if (m_nHistoryActivity == 0)
    {
        m_nHistoryActivity = m_nActivity;
    }

    if (setCurState(pDBContext->nState) == false)
    {
        WarningLn(__FUNCTION__" invalid clan state = " << pDBContext->nState);
    }

    // 已读标志
    m_dwReadFlag = 0;
    m_dwReadFlag = m_dwReadFlag | emClanRead_Head;

    // 初始化联盟数据
    if (init() == false)
    {
        return false;
    }

    return true;
}

// 设置联盟成员数据
bool CClan::setDBMemberContext(LPCSTR pszContext, int nLen)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return false;

	if (pszContext == NULL || nLen < sizeof(DBREQ_RESULT_READMEMBERLIST))
		return false;

	DBREQ_RESULT_READMEMBERLIST* pRetData = (DBREQ_RESULT_READMEMBERLIST*)pszContext;
	if (nLen != sizeof(DBREQ_RESULT_READMEMBERLIST) + pRetData->nMemberCount * sizeof(DBREQ_TABLE_CLANMEMBER))
		return false;

	m_nMemberIDList.clear();
	m_nMemberCount = 0;

	// 设置联盟成员数据
	int offset = sizeof(DBREQ_RESULT_READMEMBERLIST);
	for (int i = 0; i < pRetData->nMemberCount; i ++)
	{
		DBREQ_TABLE_CLANMEMBER* pMember = (DBREQ_TABLE_CLANMEMBER*)(pszContext + offset);
		addMember(pMember->dwKinID);

		m_pClanService->addClanMember(pMember->dwClanID, pMember->dwKinID, false);

		// 登陆修改，往联盟战队数据缓存列表添加
		m_pClanService->addReadingKin(pMember->dwClanID, pMember->dwKinID);

		// TraceLn(_GT("联盟成员数据，联盟ID=") << pMember->dwClanID <<  _GT("战队ID=") << pMember->dwKinID);

		// 登陆修改 检查战队数据是否读出来，或者正在读取，如果没，进行读取并放到读取列表中
        SKinInfo sKin = pKinService->getKinInfo(pMember->dwKinID);
		if (sKin.nKinProp_ID != 0)
		{
			// 1、发现战队数据已经读取完成
			// 2、已经在读取数据的缓存列表
			DWORD dwKinReadFlag = sKin.nKinProp_ReadFlag;
			if (dwKinReadFlag == emKinRead_AllData)
			{
				offset += sizeof(DBREQ_TABLE_CLANMEMBER);
				m_pClanService->removeReadingKin(pMember->dwKinID);
				continue;
			}
		}

		offset += sizeof(DBREQ_TABLE_CLANMEMBER);
	}

	m_dwReadFlag = m_dwReadFlag | emClanRead_Member;

	return true;
}

// 创建，由场景服发来创建联盟对象（场景服中在中心库及区库中创建本联盟成功后发来社会服创建）
bool CClan::create(CClanService *pClanService, LPCSTR pszContext, int nLen)
{
    m_pClanService = pClanService;

    SClanConfig *pConfig = g_EHelper.getConfig();
    if (pConfig == NULL)
        return false;

    if (pszContext == NULL || nLen != sizeof(msg_clan_create_so))
        return false;
    msg_clan_create_so *pContext = (msg_clan_create_so*)pszContext;

	m_nAutoAcceptFlag = 1;  // 创建联盟时默认开启自动接收
    m_nLevel = 1;
	m_nLegendCupCount = 1;
    addMember(pContext->dwKinID);
    m_dwID = pContext->dwClanID;
	sstrcpyn(m_szName, pContext->szName, sizeof(m_szName));
	sstrcpyn(m_szManifesto, pContext->szManifesto, sizeof(m_szManifesto));
	m_nFlag = pContext->nClanFlag;
    m_nEstablishTime = pContext->nEstablishTime;
    m_dwReadFlag = emClanRead_AllData;
    m_nShaikhID = pContext->dwPDBID;
    sstrcpyn(m_szShaikhName, pContext->szShaikhName, sizeof(m_szShaikhName));
    m_nWorldID = getThisGameWorldID();

    setCurState(emClanState_Normal);

    // 初始化联盟数据
    if (init() == false)
        return false;

    // 把联盟的基本信息发送到客户端
    updateDataToClient(pContext->dwPDBID);

    // 发信息放到场景服
    updateDataToZone();

    // 发起联盟创建事件
    event_clan_create eventData;
    eventData.dwClanID = m_dwID;
    IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
    if (pEventEngine != NULL)
    {
        pEventEngine->FireExecute(EVENT_CLAN_CREATE, SOURCE_TYPE_CLAN, m_dwID, (LPCSTR)&eventData, sizeof(eventData));
    }

	m_nEstablishTime = time(NULL);

    return true;
}

// 初始化联盟数据
bool CClan::init()
{
    // 设置定时器存盘定时器 30分钟定时器
    g_EHelper.setTimer(TimerID_SaveDB, 1000 * 60 * 30, this, "CClan::init()");

    return true;
}

// 销毁
void CClan::release()
{
    // 结束当前状态
    if (m_nState >= 0 && m_nState < emClanState_Num)
    {
        m_StateCtrl[m_nState]->stop();
    }

    if (m_bSaveDB == true)
    {
        updateDataToDB();
        m_bSaveDB = false;
    }

    // 删除存盘定时器
    g_EHelper.killTimer(TimerID_SaveDB, this);

    // 关闭当前状态
    if (m_StateCtrl[m_nState] != NULL)
    {
        m_StateCtrl[m_nState]->stop();
    }

    delete this;
}

// 取得联盟名字
LPCSTR CClan::getName()
{
    return m_szName;
}

// 取得帮主名字
LPCSTR CClan::getShaikhName()
{
    return m_szShaikhName;
};

// 取得联盟宣言
LPCSTR CClan::getManifesto()
{
	return m_szManifesto;
}

// 取得联盟成员个数
int CClan::getKinCount()
{
    return m_nMemberCount ;
}

// 获取联盟成员列表
int CClan::getKinList(DWORD *pKinList, int nLen)
{
	if (pKinList == NULL || nLen <= 0)
		return 0;

	int nIndex = 0;
	list<int>::iterator iter = m_nMemberIDList.begin();
	for ( ; iter != m_nMemberIDList.end() && nIndex < nLen; ++iter)
	{
		pKinList[nIndex++] = (*iter);
	}
	return nIndex;
}

// 取得联盟战队成员个数
int CClan::getMemberCount()
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return 0;

	int nResult = 0;
	list<int>::iterator iter = m_nMemberIDList.begin();
	for ( ; iter != m_nMemberIDList.end(); ++iter)
	{
        SKinInfo sKin = pKinService->getKinInfo(*iter);
		if (sKin.nKinProp_ID != 0)
		{
			nResult += pKinService->getKinMemberCount(*iter);
		}
	}

	return nResult;
}

int CClan::getTotalFightScore()
{
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return 0;

    DWORD dwResult = 0;
    list<int>::iterator iter = m_nMemberIDList.begin();
    for ( ; iter != m_nMemberIDList.end(); ++iter)
    {
        SKinInfo sKin = pKinService->getKinInfo(*iter);
        if (sKin.nKinProp_ID != 0)
        {
            dwResult += pKinService->getTotalFightScore(*iter);
        }
    }

    return dwResult;
}


// 取得某个玩家的联盟身份
int CClan::getActorIdentity(DWORD dwPDBID)
{
	SKinMember* pKinMember = g_EHelper.getActorKinMemberInfo(dwPDBID);
	if (pKinMember == NULL)
	{
		return EMCLANIDENTITY_INVALID;
	}

	if (getNumProp(emClanProp_ShaikhID) == dwPDBID)
	{
		return EMCLANIDENTITY_SHAIKH;
	}
	else
	{
		if (pKinMember->nIdentity == emKinIdentity_Shaikh)
		{
			return EMCLANIDENTITY_ELDER;
		}
	}

	return EMCLANIDENTITY_MEMBER;
}

// 取得世界排名
int CClan::getWorldPlace()
{
	return m_nWorldPlace;
}

// 获得历史活跃度
int CClan::getHistoryActivity()
{
	return m_nHistoryActivity;
}

// 取得联盟的整型数据-TClanProp
int CClan::getNumProp(DWORD dwPropID)
{
	if (dwPropID < emClanProp_ID && dwPropID >= emClanProp_NumPropCount)
		return 0;

	if (m_propAddress[dwPropID] == 0)
		return 0;

	return *(m_propAddress[dwPropID]);
}

// 设置联盟的整型数据-TClanProp
typedef void * (CClan::* LEECHDOM_CHANGE_PROC)(DWORD dwPropID, int nValue);
bool CClan::setNumProp(DWORD dwPropID, int nValue, bool bSaveDB, bool bSyncToZone)
{
	static LEECHDOM_CHANGE_PROC changeproc[emClanProp_NumPropCount] =
	{	
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 联盟ID
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 状态
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 联盟等级
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 联盟创立时间
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 发起解散的时间

		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 召集令使用的时间
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 联盟帮主PDBID
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 成员的最后下线时间

		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 已经从数据库读出数据的标志

		(LEECHDOM_CHANGE_PROC)&CClan::increment_NoNegative,	// 建设度
		(LEECHDOM_CHANGE_PROC)&CClan::increment_NoNegative,	// 黄金
		(LEECHDOM_CHANGE_PROC)&CClan::increment_NoNegative,	// 活跃度
		(LEECHDOM_CHANGE_PROC)&CClan::increment_NoNegative,	// 周活跃度
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 排名
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 游戏世界ID
		0,
		0,
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 联盟战队数量
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 杯赛次数
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 自动接受标识
		(LEECHDOM_CHANGE_PROC)&CClan::cover_NoNegative,		// 盟主状态
	};

	if (dwPropID < emClanProp_ID || dwPropID >= emClanProp_NumPropCount || changeproc[dwPropID] == NULL)
		return false;

	if (m_propAddress[dwPropID] == 0)
		return false;

	(this->*changeproc[dwPropID])(dwPropID, nValue);

	if (bSaveDB == true)
	{
		updateDataToDB();
	}

	if (bSyncToZone)
	{
		// 更新属性到场景服
		obuf512 ob;
		fillClanData(ob, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATECLANPROP_ZONE);
		msg_clan_updateintprop_os sendData;
		sendData.dwClanID = m_dwID;
		sendData.nPropID = (BYTE)dwPropID;
		sendData.nValue = *(m_propAddress[dwPropID]);
		ob.push_back(&sendData, sizeof(sendData));
		g_EHelper.sendDataToZoneSvr(ob.data(), ob.size());
	}

	return true;
}

// 设置当前状态
bool CClan::setCurState(int nState)
{
    if (nState < 0 || nState >= Clan_ManagerDef::emClanState_Num)
        return false;

    // 关闭以前状态
    if (m_nState >= 0 && m_nState < Clan_ManagerDef::emClanState_Num)
    {
        m_StateCtrl[m_nState]->stop();
    }

    // 开启当前状态
    m_nState =  nState;
    m_StateCtrl[m_nState]->start(this);

    obuf512 ob;
    fillClanData(ob, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATECLANPROP_ZONE);
    msg_clan_updateintprop_os sendData;
    sendData.dwClanID = m_dwID;
    sendData.nPropID = emClanProp_State;
    sendData.nValue = m_nState;
    ob.push_back(&sendData, sizeof(sendData));
    g_EHelper.sendDataToZoneSvr(ob.data(), ob.size());

    return true;
}

// 设置当前排名
void CClan::setPlace(int nPlace, bool bSaveDB)
{
    setNumProp(emClanProp_Place, nPlace, bSaveDB, true);

    // 更新客户端
    msg_clan_updateintprop_oc sendData;
    sendData.nPropID = emClanProp_Place;
    sendData.nValue = m_nPlace;
    broadcastMsgToObserve(MSG_CLAN_UPDATECLANINTPROP, (LPCSTR)&sendData, sizeof(sendData));
}

// 设置世界排名
void CClan::setWorldPlace(int nPlace, int nHistoryActivity)
{
    m_nWorldPlace = nPlace;
    m_nHistoryActivity = nHistoryActivity;
}

// 设置帮主
void CClan::setShaikh(int nPDBID, char* szText)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
		return;

	DWORD dwOldShaikhID = m_nShaikhID;
	m_nShaikhID = nPDBID;

	SKinMember* pShaikh = g_EHelper.getActorKinMemberInfo(m_nShaikhID);
	if (pShaikh != NULL)
	{
		sstrcpyn(m_szShaikhName, pShaikh->szName, sizeof(m_szShaikhName));
	}

	updateDataToDB();

	//// 广播给客户端观察者
	SKinMember* pOldShaikh = g_EHelper.getActorKinMemberInfo(dwOldShaikhID);
	if (pOldShaikh != NULL)
	{
		SKinInfo sKin = pKinService->getKinInfo(pOldShaikh->dwKinID);
		if (sKin.nKinProp_ID != 0)
		{
			SClientClanMember sMember;
			fillClientMemberInfo(sMember, sKin, pOldShaikh);
			broadcastMsgToObserve(MSG_CLAN_UPDATEMEMBERINFO, (LPCSTR)&sMember, sizeof(sMember));
		}

		// 更新属性到客户端
		m_pClanService->updateMemberToClient_OC(m_dwID, pOldShaikh);
	}

	//// 广播给客户端观察者
	// 此行后pOldShaikh已作废
	pShaikh = g_EHelper.getActorKinMemberInfo(m_nShaikhID);
	if (pShaikh != NULL)
	{
		SKinInfo sKin = pKinService->getKinInfo(pShaikh->dwKinID);
		if (sKin.nKinProp_ID != 0)
		{
			SClientClanMember sMember;
			fillClientMemberInfo(sMember, sKin, pShaikh);
			broadcastMsgToObserve(MSG_CLAN_UPDATEMEMBERINFO, (LPCSTR)&sMember, sizeof(sMember));
		}

		// 更新属性到客户端
		m_pClanService->updateMemberToClient_OC(m_dwID, pShaikh);
	}

	// 更新帮主名字到客户端
	msg_clan_updatestringprop_oc sendStrData;
	sendStrData.nPropID = emClanShaikhName;
	sstrcpyn(sendStrData.szValue, m_szShaikhName, sizeof(sendStrData.szValue));
	broadcastMsgToObserve(MSG_CLAN_UPDATECLANSTRPROP, (LPCSTR)&sendStrData, sizeof(sendStrData));

	// 更新属性到客户端
	msg_clan_updateintprop_oc sendIntData;
	sendIntData.nPropID = emClanProp_ShaikhID;
	sendIntData.nValue = m_nShaikhID;
	broadcastMsgToObserve(MSG_CLAN_UPDATECLANINTPROP, (LPCSTR)&sendIntData, sizeof(sendIntData));

	// 把族长的数据更新到场景服
	updateDataToZone();

	// 发出事件
	event_clan_shaikh_change change;
	change.dwClanID = m_dwID;
	change.dwOldShaikh = dwOldShaikhID;
	change.dwNewShaikh = m_nShaikhID;
	sstrcpyn(change.szNewName, m_szShaikhName, sizeof(change.szNewName));
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine != NULL)
	{
		pEventEngine->FireExecute(EVENT_CLAN_SHAIKH_CHANGE, SOURCE_TYPE_CLAN, m_dwID, (LPCSTR)&change, sizeof(change));
	}

    // 帮主改变需要更新基础数据下去
    sendClanBaseData();

    if (szText)
    {
        SClanEvent newEvent;
        newEvent.nTime = (int)time(NULL);
        ssprintf_s(newEvent.szCoutext, sizeof(newEvent.szCoutext), szText);
        addClanEvent(newEvent);
    }
}

// 设置帮主名字
void CClan::setShaikhName(LPCSTR szName)
{
	if (szName == NULL)
		return;
	
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver = NULL)
		return;
	
	sstrcpyn(m_szShaikhName, szName, sizeof(m_szShaikhName));

	// 存DB
	updateDataToDB();

	// 广播给观察者
	SKinMember* pShaikh = g_EHelper.getActorKinMemberInfo(m_nShaikhID);
	if (pShaikh != NULL)
	{
		SKinInfo sKin = pKinService->getKinInfo(pShaikh->dwKinID);
		if (sKin.nKinProp_ID != 0)
		{
			SClientClanMember sMember;
			fillClientMemberInfo(sMember, sKin, pShaikh);
			broadcastMsgToObserve(MSG_CLAN_UPDATEMEMBERINFO, (LPCSTR)&sMember, sizeof(sMember));
		}

		// 更新属性到客户端
		m_pClanService->updateMemberToClient_OC(m_dwID, pShaikh);
	}

	// 更新帮主名字到客户端
	msg_clan_updatestringprop_oc sendStrData;
	sendStrData.nPropID = emClanShaikhName;
	sstrcpyn(sendStrData.szValue, m_szShaikhName, sizeof(sendStrData.szValue));
	broadcastMsgToObserve(MSG_CLAN_UPDATECLANSTRPROP, (LPCSTR)&sendStrData, sizeof(sendStrData));

	// 把族长的数据更新到场景服
	updateDataToZone();

	// 更新基础数据下去
	sendClanBaseData();
}

// 接收成员
void CClan::acceptMember(DWORD dwKinID)
{
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

    // 在共享服取一下信息
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
        return;

    // 添加到成员列表
    addMember(dwKinID);

    m_pClanService->addClanMember(m_dwID, dwKinID, true);

    SKinInfo sKin = pKinService->getKinInfo(dwKinID);
    if (sKin.nKinProp_ID == 0)
        return;

    PDBID pReturnArray[256] = {0};
    DWORD dwCount = pKinService->getKinMemberList(dwKinID, pReturnArray, 256);
    for (DWORD i = 0; i < dwCount; ++i)
    {
        DWORD dwPDBID = pReturnArray[i];

        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
        if (shareProperty.dwPDBID  == dwPDBID )
        {
            updateDataToClient(dwPDBID);
			// 通知杯赛
			broadcastClanLegendCupInfoToClient(dwPDBID);
        }

        SKinMember sKinMember = pKinService->getKinMemberInfo(dwPDBID);
        if (sKinMember.dwKinID == 0)
            continue;

		SClientClanMember clientMember;
		fillClientMemberInfo(clientMember, sKin, &sKinMember);
        broadcastMsgToObserve(MSG_CLAN_UPDATEMEMBERINFO, (LPCSTR)&clientMember, sizeof(clientMember));
    }

	// 添加联盟事件记录
	SClanEvent newEvent;
	newEvent.nTime = (int)time(NULL);
	ssprintf_s(newEvent.szCoutext, sizeof(newEvent.szCoutext), a2utf8("欢迎新成员[%s]加入了联盟"), sKin.szName);
	addClanEvent(newEvent);

	// 通知大家某人加入战队
	char szText[512] = { 0 };
	ssprintf_s(szText, sizeof(szText), "%s", sKin.szName);
	g_EHelper.broadcastClanSystemTips(m_dwID, CHAT_TIP_CLAN_ADMIT_ADD, szText);

    
}

// 删除成员
void CClan::delMember(DWORD dwKinID, DWORD dwKinShaikhID, bool bSaveDB)
{
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

    DWORD dwKinClanID = m_pClanService->findKinClan(dwKinID);
    if (dwKinClanID != m_dwID)
        return;

    // 开除成员
    m_nMemberIDList.remove(dwKinID);
    m_nMemberCount--;

    // 删除全局成员列表数据（里面已经通知客户端离开战队）
    m_pClanService->removeClanMember(dwKinID, bSaveDB);
	if (dwKinShaikhID == m_nShaikhID && m_nMemberIDList.empty() == false)
	{
		SKinMember kinMember;
		if (chooseNewShaikh(kinMember))
		{
			setShaikh(kinMember.dwPDBID);
		}
	}

	PDBID pReturnArray[256] = { 0 };
	DWORD dwCount = pKinService->getKinMemberList(dwKinID, pReturnArray, 256);
	for (DWORD i = 0; i < dwCount; ++i)
	{
		DWORD dwPDBID = pReturnArray[i];

		m_ObserveList.remove(dwPDBID);

		// 往客户端发消息
		msg_clan_removemember_oc sendData;
		sendData.dwPDBID = dwPDBID;
		broadcastMsgToObserve(MSG_CLAN_REMOVEMEMBERINFO, (LPCSTR)&sendData, sizeof(sendData));
	}

    // 成员数据已经读出
    if ((m_dwReadFlag & emClanRead_Member) == emClanRead_Member)
    {
        if (m_nMemberIDList.empty() == true)
        {
            dismiss(emClanDismiss_ZeroKin);
        }
    }
}

// 添加申请中成员, 不处理全局成员列表和数据
void CClan::addApplyMember(DWORD dwKinID)
{
    m_ApplyKinList.remove(dwKinID);
    m_ApplyKinList.push_back(dwKinID);
}

// 删除申请中成员, 不处理全局成员列表和数据
void CClan::removeApplyMember(DWORD dwKinID)
{
    m_ApplyKinList.remove(dwKinID);

    // 往客户端发出事件
    msg_clan_admit_oc sendData;
    sendData.dwClanID = m_dwID;
    sendData.dwKinID = dwKinID;
    broadcastMsgToObserve(MSG_CLAN_ADMIT, (LPCSTR)&sendData, sizeof(sendData));
}

void CClan::autoDismiss(int dismissActivity, int dismissWeeks)
{
	char szText[512] = { 0 };
	sprintf_s(szText, sizeof(szText), a2utf8("%s联盟本周活跃值%d点"), m_szName, m_nWeekActivity);
	IOSSLogService* pOSSLog = gSocialGlobal->getOSSLogService();
	pOSSLog->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_CLAN, m_dwID, szText);

	if (g_EHelper.DiffWeek((DWORD)m_nEstablishTime, (DWORD)time(NULL)))
	{
		if (m_nWeekActivity < dismissActivity)
		{
			if (++m_nWeekCount >= dismissWeeks)
			{				
				sprintf_s(szText, sizeof(szText), a2utf8("%s联盟活跃度连续%d周不满足%d点自动解散"), m_szName, dismissWeeks, dismissActivity);
				pOSSLog->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_CLAN, m_dwID, szText);
				dismiss(emClanDismiss_Activity);
			}
		}
		else
		{
			m_nWeekCount = 0;
		}
	}
}

// 解散联盟
void CClan::dismiss(TClanDismissType nReason)
{
	notifyDismiss(nReason);

    int nStartTick = getTickCount();

    // 通知成员解散联盟
    g_EHelper.broadcastClanSystemTips(m_dwID, CHAT_TIP_CLAN_DISMISS_SUCCEED);

    // 发起联盟解散事件
    event_clan_dismiss eventData;
    eventData.dwClanID = m_dwID;
    IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
    if (pEventEngine != NULL)
    {
        pEventEngine->FireExecute(EVENT_CLAN_DISMISS, SOURCE_TYPE_CLAN, m_dwID, (LPCSTR)&eventData, sizeof(eventData));
    }

    // 删除全局的成员数据
    list<int>::iterator it;
    for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++it)
    {
        // 删除成员
        m_pClanService->removeClanMember(*it, false);
    }

    // 通知数据库解散联盟
    m_pClanService->onDismissClan(m_dwID);

    // 从删除全局联盟对象
    m_pClanService->removeClan(m_dwID);

    int nCostTime = getTickCount() - nStartTick;
    TraceLn("[" << m_szName << "]clan dismiss，cost:" <<  nCostTime);

    // 通知所有场景删除联盟相关数据
    obuf512 obuf;
    msg_clan_removeclan_os sendData;
    sendData.dwClanID = m_dwID;
    fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_REMOVECLAN_ZONE);
    obuf.push_back(&sendData, sizeof(sendData));
    g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size());

	sendMail(nReason);

    // 释放自己，注意
    release();
}

void CClan::sendMail(TClanDismissType nReason)
{
	EMMailFillID mailID;
	if (nReason == emClanDismiss_Activity)
	{
		mailID = emMailFill_ClanAutoDismiss;
	}
	else
	{
		mailID = emMailFill_ClanDismiss;
	}
	
	SMailConfig * pMailConfig = g_EHelper.getMailConfig(mailID);
	if (!pMailConfig)
	{
		return;
	}

	MailHelper mailHelper;
	IMailService *pMailService = mailHelper.m_ptr;
	if (NULL == pMailService)
	{
		ErrorLn(__FUNCTION__": failed! pMailService == NULL");
		return;
	}

	SMailSendData mailInfo;
	mailInfo.nType = emMailType_System;							// 邮件类型
	mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
	sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName, sizeof(mailInfo.szSenderName));// 发件者姓名
	//mailInfo.dwMasterPDBID = m_nShaikhID;
	sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));
	mailInfo.nPlusMoney = 0;

	char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
	ssprintf_s(szParam, sizeof(szParam), a2utf8("%s"), pMailConfig->szContext);
	//pMailService->sendMail(mailInfo, mailID, szParam);

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;
	PDBID pReturnArray[256] = { 0 };
	for (list<int>::iterator it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++it)
	{
		DWORD dwCount = pKinService->getKinMemberList(*it, pReturnArray, sizeof(pReturnArray));
		for (DWORD i = 0; i < dwCount; ++i)
		{
			mailInfo.dwMasterPDBID = pReturnArray[i];
			pMailService->sendMail(mailInfo, mailID, szParam);
		}
	}
}

// 联盟数据存数据库（这个过程以后会改成其他类去做）
void CClan::updateDataToDB()
{
    if (isPublicGameWorld())
        return;

    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService == NULL)
        return;

    // 如果数据还没读出来就不处理了
    if ((m_dwReadFlag & emClanRead_Head) != emClanRead_Head)
        return;

    if (m_pClanService == NULL)
        return;

    DBREQ_PARAM_UPDATECLAN referData;
    referData.dwClanID = m_dwID;										// 联盟ID
    sstrcpyn(referData.szName, m_szName, sizeof(referData.szName));		// 联盟名
    referData.nShaikhID = m_nShaikhID;									// 帮主ID
    sstrcpyn(referData.szShaikhName, m_szShaikhName, sizeof(referData.szShaikhName));		// 帮主名
    referData.nState = m_nState;										// 联盟的状态：临时，正式，解散中
    referData.nLevel = m_nLevel;										// 联盟等级
    sstrcpyn(referData.szManifesto, m_szManifesto, sizeof(referData.szManifesto));	// 联盟的宣言
	referData.nFlag = m_nFlag;											// 联盟旗帜
    referData.nEstablishTime = m_nEstablishTime;						// 联盟创立时间
    referData.nDismissApplyTime = m_nDismissApplyTime;					// 发起解散的时间
    referData.nConstructDegree = m_nConstructDegree;					// 建设度
    referData.nGold = m_nGold;											// 黄金
	referData.nLegendCupCount = m_nLegendCupCount;						// 可举办杯赛次数
    referData.nActivity = m_nActivity;									// 活跃度
	referData.nWeekActivity = m_nWeekActivity;                          // 周活跃度
    referData.nHisActivity = m_nHistoryActivity;						// 历史活跃度
    referData.nPlace = m_nWorldPlace * 10000 + m_nPlace;				// 排名
    referData.nKickNum = m_nKickNum;									// 上一次踢人的次数
    referData.nKickTime= m_nKickTime;									// 上一次踢人的时间

    if (m_nShaikhID == 0)
        return;

    // 执行存储过程
    m_pClanService->onExecuteSP(GAMEDB_REQUEST_UPDATECLAN, m_dwID, (LPCSTR)&referData, sizeof(referData));

    m_bSaveDB = true;
}

// 同步联盟数据到场景服
void CClan::updateDataToZone(CSID ServerID)
{
    // 通知场景服添加联盟
    obuf512 ob;
    fillClanData(ob, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATECLAN_ZONE);

    msg_clan_updateclan_os sendData;
    sendData.dwID = m_dwID;									// 联盟ID
    sstrcpyn(sendData.szName, m_szName, sizeof(sendData.szName));		// 联盟名
    sendData.nState = m_nState;								// 联盟的状态：临时，正式，解散中
    sendData.nLevel = m_nLevel;								// 联盟等级
    sendData.nConstructDegree = m_nConstructDegree;			// 建设度
    sendData.nGold = m_nGold;								// 黄金
    sendData.nShaikhID = m_nShaikhID;						// 帮主PDBID
    sstrcpyn(sendData.szShaikhName , m_szShaikhName, sizeof(sendData.szShaikhName));	// 帮主名字
    sendData.nActivity = m_nActivity;						// 活跃度
	sendData.nWeekActivity = m_nWeekActivity;               // 周活跃度
    sendData.nPlace = m_nPlace;								// 排名
    sendData.nWorldID = m_nWorldID;							// 游戏世界ID
    sendData.nKickTime = m_nKickTime;						// 踢人时间
    sendData.nKickNum  = m_nKickNum;						// 踢人的次数
	sendData.nLegendCupCount = m_nLegendCupCount;			// 可举行杯赛次数
    ob.push_back(&sendData, sizeof(sendData));

    g_EHelper.sendDataToZoneSvr(ob.data(), ob.size(), ServerID);
}

// 同步联盟数据到客服端（只同步最重要的几个数据）
void CClan::updateDataToClient(DWORD dwPDBID, int nZoneID)
{
    msg_clan_updateclaninfo_oc updateClient;
    updateClient.dwID = m_dwID;								// 联盟ID
    updateClient.nState = (BYTE)m_nState;					// 联盟的状态：临时，正式，解散中
    sstrcpyn(updateClient.szName, m_szName, sizeof(updateClient.szName));// 联盟名
    updateClient.dwShaikhID = m_nShaikhID;					// 帮主PDBID
    sstrcpyn(updateClient.szShaikhName, m_szShaikhName, sizeof(updateClient.szShaikhName));		// 帮主名字

    obuf256 obuf;

    // 同步到指定场景服，由场景服转发给指定客户端
    if (nZoneID != -1)
    {
        msg_clan_zonepostmsg_os postMsg;
        postMsg.dwPDBID = dwPDBID;
        postMsg.dwMsgID = MSG_CLAN_UPDATECLIENT;

        fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_ZONEPOSTMSG);
        obuf.push_back(&postMsg, sizeof(postMsg));
        obuf.push_back(&updateClient, sizeof(updateClient));

        g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), nZoneID);
    }
    else	// 同步到指定客户端
    {
        fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_UPDATECLIENT);
        obuf.push_back(&updateClient, sizeof(updateClient));

        g_EHelper.sendDataToPerson(dwPDBID, obuf.data(), obuf.size());
    }
}

// 广播联盟数据到客服端（只同步最重要的几个数据）
void CClan::broadcastDataToClient()
{
    msg_clan_updateclaninfo_oc updateClient;
    updateClient.dwID = m_dwID;
    updateClient.nState = (BYTE)m_nState;
    sstrcpyn(updateClient.szName, m_szName, sizeof(updateClient.szName));
    updateClient.dwShaikhID = m_nShaikhID;
    sstrcpyn(updateClient.szShaikhName, m_szShaikhName, sizeof(updateClient.szShaikhName));

    obuf256 obuf;
    fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_UPDATECLIENT);
    obuf.push_back(&updateClient, sizeof(updateClient));

    list<int>::iterator it;
    for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
    {
        g_EHelper.sendDataToPerson(*it, obuf.data(), obuf.size());
    }
}

// 把属性改变广播给观察者
//@ param  : nEventID 事件ID
void CClan::broadcastMsgToObserve(int nEventID, LPCSTR pszData, size_t nLen)
{
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
	if (pShareReceiver == NULL || pGatewayAcceptorService == NULL)
		return;

    obuf256 obuf;
    fillClanData(obuf, MSG_ENDPOINT_CLIENT, nEventID);
    obuf.push_back(pszData, nLen);

	vector<ClientID> clientList;
	list<DWORD>::iterator it = m_ObserveList.begin();
	while (it != m_ObserveList.end())
	{
		DWORD dwPDBID = (*it);
		// 发现玩家不在线了，不广播，把它从观察者里面删除
        SSharePersonProperty shareProperty  = pShareReceiver->GetSharePerson(dwPDBID);
		if (shareProperty.dwPDBID == INVALID_PDBID)
		{
			it = m_ObserveList.erase(it);
			continue;
		}

		clientList.push_back(shareProperty.clientID);
		++it;
	}

	if (clientList.size() > 0)
		pGatewayAcceptorService->broadcast(clientList.data(), (WORD)clientList.size(), obuf.data(), obuf.size());
}

// 联盟杯赛提示
void CClan::broadcastClanLegendCupInfoToClient(DWORD dwPDBID)
{
	LegendCupHelper cupHelper;
	ILegendCupService *pLegendCupService = cupHelper.m_ptr;
	if (pLegendCupService == NULL)
		return;

	SCreaterLegendCupInfo sLegendCupInfo = pLegendCupService->getCreaterLegendCupInfo(emCupType_Clan, m_dwID);
	if (sLegendCupInfo.llLegendCupID == 0 || sLegendCupInfo.byState < SATE_REGISTER || sLegendCupInfo.byState >= SATE_END)
    {
        // 联盟检测杯赛发布
        clanCheckLegendCupInfoPublic(dwPDBID);
    }
    else
    {
        // 联盟杯赛信息广播
        clanLegendCupInfoBroadcast(dwPDBID);
    }

}

void CClan::clanCheckLegendCupInfoPublic(DWORD dwActorID)
{
    SClanConfig* pConfig = g_EHelper.getConfig();
    if (pConfig == NULL)
    {
        ErrorLn(__FUNCTION__": pConfig == NULL");
        return;
    }

    bool bCanPublicCup = true;
    do 
    {
        if (m_nLegendCupCount <= 0)
        {
            bCanPublicCup = false;
            break;
        }
        
        if (m_nWeekActivity < pConfig->nClanCupMiniWeekActivity)
        {
            bCanPublicCup = false;
            break;
        }
    } while (false);
    
    if (!bCanPublicCup)
    {
        return;
    }

    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        return;
    }
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwActorID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
        return;
    ChatHelper chatHelper;
    IChatService *pChatService = chatHelper.m_ptr;
    if (NULL == pChatService)
    {
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
        return;
    }
    if (dwActorID == m_nShaikhID)
    {
        // 是盟主
        string strParam = "";
        strParam += CHAT_MSG_ITEM_FLAG;

        char szText[256] = {0};
        ssprintf_s(szText, sizeof(szText), "%d;%s;", CHAT_CHANNEL_CLAN,strParam.c_str());
        pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwActorID, CHAT_TIP_CLAN_SHAIKH_CAN_PUBLIC_CUP, szText);

        // 发送dida
        obuf obuf;
        fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_LEGEND_CUP_DIDA);

        g_EHelper.sendDataToPerson(dwActorID, obuf.data(), obuf.size());

        m_bClanLegendDidaSend = true;
    }
    else
    {
        // 非盟主
        pChatService->broadcastSystemMessageToChatBox(CHAT_BROADCASTTYPE_SINGLE, shareProperty.dwPDBID, CHAT_TIP_CLAN_MEMBER_REQ_SHAIKH_PUBLIC_CUP, CHAT_CHANNEL_CLAN);
    }
}

void CClan::clanLegendCupInfoBroadcast(DWORD dwActorID)
{
    LegendCupHelper cupHelper;
    ILegendCupService *pLegendCupService = cupHelper.m_ptr;
    if (pLegendCupService == NULL)
    {
        ErrorLn(__FUNCTION__": pLegendCupService == NULL! dwActorID="<<dwActorID);
        return;
    }
    SCreaterLegendCupInfo sLegendCupInfo = pLegendCupService->getCreaterLegendCupInfo(emCupType_Clan, m_dwID);
    string strParam = "";
    strParam += CHAT_MSG_ITEM_FLAG;
    char szText[256] = {0};

    EMChatTipID nTipID = CHAT_TIP_CLAN_LEGEND_CUP_REGISTER_TIP;

    if (sLegendCupInfo.byState == SATE_REGISTER)
    {
        ssprintf_s(szText, sizeof(szText), "%d;%s;%s;%s;%lld;%d;%d", CHAT_CHANNEL_CLAN, m_szShaikhName, sLegendCupInfo.szCupName, strParam.c_str(), sLegendCupInfo.llLegendCupID, sLegendCupInfo.nRegisterKinCount, sLegendCupInfo.nRegisterKinMax);
    }
    else
    {
        nTipID = CHAT_TIP_CLAN_LEGEND_CUP_PROCEED_TIP;

        int nTmpNum = sLegendCupInfo.nRegisterKinCount;
        if (sLegendCupInfo.byState == SATE_32TO16)
        {
            nTmpNum = 32;
        }
        else if (sLegendCupInfo.byState == SATE_16TO8)
        {
            nTmpNum = 16;
        }
        else if (SATE_8TO4)
        {
            nTmpNum = 8;
        }
        else if (SATE_4TO2)
        {
            nTmpNum = 4;
        }
        else if (SATE_FINAL)
        {
            nTmpNum = 2;
        }

        ssprintf_s(szText, sizeof(szText), "%d;%s;%d;%s;%lld", CHAT_CHANNEL_CLAN, sLegendCupInfo.szCupName, nTmpNum, strParam.c_str(), sLegendCupInfo.llLegendCupID);
    }


    if (dwActorID == INVALID_PDBID)
    {
        list<int>::iterator it;
        for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
        {
            g_EHelper.showSystemMessage(*it, nTipID, szText);
        }
    }
    else
    {
        g_EHelper.showSystemMessage(dwActorID, nTipID, szText);
    }
}


// 联盟杯赛提示
void CClan::broadcastClanCreateLegendCupToClient(LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName)
{
	sendClanBaseData();
    KinHelper kinHelper;
    IKinService *pKinService = kinHelper.m_ptr;
    if (pKinService == NULL)
    {
        return;
    }
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        return;
    }


	string strParam = "";
	strParam += CHAT_MSG_ITEM_FLAG;
	char szText[256] = {0};
	ssprintf_s(szText, sizeof(szText), "%d;%s;%s;%s;%lld;%d;%d", CHAT_CHANNEL_CLAN, m_szShaikhName, pCupName, strParam.c_str(), llLegendCupID, 0, nMaxKinCount);

	for (list<int>::iterator it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
	{
        PDBID pReturnArray[256] = {0};
        DWORD dwCount = pKinService->getKinMemberList(*it, pReturnArray, 256);
        for (DWORD i = 0; i < dwCount; ++i)
        {
            DWORD dwPDBID = pReturnArray[i];
            SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
            if (shareProperty.dwPDBID == INVALID_PDBID)
                continue;

            g_EHelper.showSystemMessage(*it, CHAT_TIP_CLAN_LEGEND_CUP_REGISTER_TIP, szText);
        }

		
	}
}

// 是否有在线成员
bool CClan::hasOnLineMember()
{
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
		return false;

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return false;

    PDBID pReturnArray[256] = { 0 };
	list<int>::iterator it;
	for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
	{
		DWORD dwCount = pKinService->getKinMemberList(*it, pReturnArray, 256);
		for (DWORD i = 0; i < dwCount; ++i)
		{
			DWORD dwPDBID = pReturnArray[i];
            SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
			if (shareProperty.dwPDBID == dwPDBID)
			{
				return true;
			}
		}
	}

	return false;
}

// 是否读取
bool CClan::isReadFinish()
{
	return m_bReadFinish;
}

// 设置读取完成
void CClan::setReadFinish(bool bFinish)
{
	m_bReadFinish = bFinish;
}

// 成员登录调用
void CClan::onMemberLogin(SSharePersonProperty& shareProperty)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

    // 发送请求捐卡提示
    sendRequestTip(shareProperty.dwPDBID);
}

// 成员注销调用
void CClan::onMemberLogout(ISharePerson * pSharePerson, ELogoutMode nLineType)
{

}

// 联盟事件处理函数--添加联盟事件
void CClan::addClanEvent(const SClanEvent & sClanEven)
{
	// 如果已经超过上限了，删除第一个
	if (m_EvenList.size() >= CLAN_EVENT_LOG_NUM)
		m_EvenList.pop_front();

	m_EvenList.push_back(sClanEven);
}

// 获取联盟事件
void CClan::onClientUpdateClanEvent(DWORD dwPDBID)
{
	msg_clan_updateclanlog_oc headEvent;

	// 数据体
	obuf1024 obData;
	// 将事件信息按照倒序的方式发送给客户端，客户端保存将最近的时间的事件放在最上面
	for(list<SClanEvent>::iterator iter = m_EvenList.begin(); iter != m_EvenList.end(); ++iter)
	{
		SClanEvent & node = (*iter);
		int nTextLen = (int)strlen(node.szCoutext)+1;

		msg_clan_updateclanlog_single eventData;
		eventData.nTime  = node.nTime;		// 发生时间
		eventData.wLen = nTextLen; 			// 内容长度

		// 压入头部数据
		obData.push_back((&eventData), sizeof(eventData));
		// 压入内容
		obData.push_back(node.szCoutext, nTextLen);

		// 事件个数
		headEvent.wCount++;
		// 事件总长度
		headEvent.wLen += (sizeof(msg_clan_updateclanlog_single) + nTextLen);
	}

	if (!obData.good())
		return;

	obuf1024 ob;
	fillClanData(ob, MSG_ENDPOINT_CLIENT, MSG_CLAN_UPDATEEVENT);

	// 压入数据包头
	ob.push_back(&headEvent, sizeof(headEvent));

	// 有数据包才需要压入数据
	if (headEvent.wCount > 0)
	{
		ob.push_back(obData.data(), obData.size());
	}

	// 有日志信息并且buf拷贝失败才返回
	if (!ob.good())
	{
		return;
	}

	// 发送到客户端
	g_EHelper.sendDataToPerson(dwPDBID, ob.data(), ob.size());
}

// 更新联盟基本数据
void CClan::updateClientClanBaseData(DWORD dwPDBID)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

    SClanLevel* pLevelInfo = g_EHelper.getLevelInfo(m_nLevel);
    if (pLevelInfo == NULL)
        return;

	m_ob.reset();
	m_obdata.reset();
	// 不做处理
	// int offset = 0;
	SNetMsgHead msgHead;
	msgHead.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
	msgHead.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	msgHead.byKeyModule = MSG_MODULEID_CLAN;
	msgHead.byKeyAction = MSG_CLAN_CLANBASEDATA;
	m_ob.push_back(&msgHead, sizeof(SNetMsgHead));

	msg_clan_clanbasedata_oc sendData;
	sstrcpyn(sendData.szClanName, m_szName, sizeof(sendData.szClanName));
	sstrcpyn(sendData.szManifesto, m_szManifesto, sizeof(sendData.szManifesto));
	sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
	sstrcpyn(sendData.szNotice, m_szNotice, sizeof(sendData.szNotice));
	sendData.dwShaikhID = m_nShaikhID;
	sendData.nClanLevel = m_nLevel;
	sendData.nConstructDegree = m_nConstructDegree;
	sendData.nGold = m_nGold;
	sendData.nActivity = m_nActivity;
	sendData.nWeekActivity = m_nWeekActivity;
	sendData.nPlace = m_nPlace;
	sendData.nApplyMemberNum = (BYTE)m_ApplyKinList.size();
	sendData.nKickTime = m_nKickTime;
	sendData.nKickNum  = m_nKickNum;
    sendData.nTotalFight = getTotalFightScore();
    sendData.nKinCount = getKinCount();
    sendData.nMaxKinCount = pLevelInfo->nMaxKinCount;
	sendData.nLegendCupCount = m_nLegendCupCount;
	sendData.llLegendCupID = 0;
	sendData.nAutoAcceptFlag = m_nAutoAcceptFlag;
	sendData.nCurTime = (int)time(NULL);
    sendData.nDismissApplyTime = m_nDismissApplyTime;
    sendData.nState = m_nState;
	sendData.nShaikhState = m_ShaikhState;
	LegendCupHelper cupHelper;
	ILegendCupService *pLegendCupService = cupHelper.m_ptr;
	if (pLegendCupService != NULL)
	{
		SCreaterLegendCupInfo sLegendCupInfo = pLegendCupService->getCreaterLegendCupInfo(emCupType_Clan, m_dwID);
		if (sLegendCupInfo.llLegendCupID > 0)
			sendData.llLegendCupID = sLegendCupInfo.llLegendCupID;
	}

	size_t baseData_offset = m_obdata.size();
	m_obdata.push_back(&sendData, sizeof(sendData));

	// 获得玩家的共享数据，获得玩家的mapID
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	int nCount = 0;
	// TODO : 以后得在某处分页,不然数据过大
	list<int>::iterator it  = m_nMemberIDList.begin();
	for ( ; it != m_nMemberIDList.end(); ++it)
	{
		SKinInfo sKin = pKinService->getKinInfo(*it);
		if (sKin.nKinProp_ID == 0)
			continue;

		PDBID pReturnArray[256] = {0};
		DWORD dwCount = pKinService->getKinMemberList(*it, pReturnArray, 256);
		for (DWORD i=0; i < dwCount; ++i)
		{
			DWORD dwPDBID = pReturnArray[i];

			SKinMember sKinMember = pKinService->getKinMemberInfo(dwPDBID);
			if (sKinMember.dwKinID == 0)
				continue;

			SClientClanMember clientMember;
			fillClientMemberInfo(clientMember, sKin, &sKinMember);

			m_obdata.push_back(&clientMember, sizeof(SClientClanMember));
			++nCount;
		}
	}

	// 申请中的成员列表
	SApplyMember applyMember;
	list<DWORD>::iterator itApply;
	int nApplyCount = 0;
	for (itApply = m_ApplyKinList.begin(); itApply != m_ApplyKinList.end(); ++itApply)
	{
		SKinInfo sKin = pKinService->getKinInfo(*itApply);
		if (sKin.nKinProp_ID == 0)
			continue;

		memset(&applyMember, 0, sizeof(applyMember));
		SClanApply *pApply = g_ClanInviteManager.findApply(*itApply);
		if (pApply == NULL)
			continue;

		applyMember.dwKinID = pApply->dwKinID;
		sstrcpyn(applyMember.szName, pApply->szName, sizeof(applyMember.szName));
		sstrcpyn(applyMember.szShaikhName, pApply->szShaikhName, sizeof(applyMember.szShaikhName));
		applyMember.nMemberCount = pKinService->getKinMemberCount(*itApply);

		m_obdata.push_back(&applyMember, sizeof(SApplyMember));
		nApplyCount++;
	}

	msg_clan_clanbasedata_oc *pSendData = (msg_clan_clanbasedata_oc*)(m_obdata.data() + baseData_offset);
	pSendData->nMemberCount = nCount;
	pSendData->nApplyMemberNum = nApplyCount;

	if (!m_obdata.good())
		return;

	//TraceLn("联盟数据压缩前：" << m_obdata.size());
	Rle<1> rle;
	size_t sz = rle.compress(m_obdata.data(), m_obdata.size());
	//TraceLn("联盟数据压缩后：" << sz);
	m_ob.push_back(rle.data(), sz);

	if (m_ob.good())
	{
		g_EHelper.sendDataToPerson(dwPDBID, m_ob.data(), m_ob.size());
	}

	// 添加观察者
	m_ObserveList.remove(dwPDBID);
	m_ObserveList.push_back(dwPDBID);
}

// 填充ClientMember数据
void CClan::fillClientMemberInfo(SClientClanMember &clientMember, SKinInfo &KinInfo, SKinMember *pKinMember)
{
	clientMember.dwPDBID = pKinMember->dwPDBID;
	sstrcpyn(clientMember.szName, pKinMember->szName, sizeof(clientMember.szName));
	clientMember.dwKinID = pKinMember->dwKinID;
	sstrcpyn(clientMember.szKinName, KinInfo.szName, sizeof(clientMember.szKinName));
	clientMember.nFaceID = (BYTE)pKinMember->nFaceID;
	clientMember.nLevel = pKinMember->nLevel;
	clientMember.nIdentity = g_EHelper.getClanIdentity(pKinMember->dwPDBID, m_nShaikhID, pKinMember->nIdentity);
	clientMember.nIsOnline = pKinMember->bIsOnline ? 1 : 0;
	clientMember.nGameStatus = (BYTE)pKinMember->dwState;
	clientMember.nGrade = (BYTE)pKinMember->nGrade;
	clientMember.nDonateThisWeek = pKinMember->nClanCtrb;
	clientMember.nDonateTotal = pKinMember->nTotalClanCtrb;
	clientMember.dwBattleVal = pKinMember->dwBattleVal;
	clientMember.dwSrcBattleVal = pKinMember->dwSrcBattleVal;
	clientMember.dwBVOrder = (WORD)pKinMember->dwBVOrder;
	clientMember.dwLastOrder = (WORD)pKinMember->dwLastOrder;
	clientMember.nLogoutTime = pKinMember->nLogoutTime;
    clientMember.nSex = pKinMember->bySex;
	clientMember.dwWorldID = pKinMember->dwWorldID;
	clientMember.dwSceneID = pKinMember->dwSceneID;
	clientMember.serverID = pKinMember->serverID;
}

// 联盟服务器模块消息码
void CClan::onClanHandleMessage(DWORD dwServerID, DWORD dwMsgID, LPCSTR pszData, size_t nLen)
{
	// 服务器发过来的消息
	switch ( dwMsgID )
	{
	case MSG_CLAN_REQUEST_CARD:
		{
			// 场景服请求卡牌支援
			onSceneRequestCard(dwServerID, pszData, nLen);
		}
		break;
	case MSG_CLAN_DONATE_CARD:
		{
			// 场景服捐赠卡牌
			onSceneDonateCard(dwServerID, pszData, nLen);
		}
		break;
	case MSG_CLAN_UPDATECLANPROP_ZONE:
		{
			// 场景服更新属性
			onSceneUpdateProp(dwServerID, pszData, nLen);
		}
		break;
	default:
		{
			// 其他场景服消息
			ErrorLn(__FUNCTION__": unknown dwMsgID= " << dwMsgID);
		}
		break;
	}
}

// 联盟客户端模块消息码
void CClan::onClanClientMsg(DWORD dwClientID, DWORD dwMsgID, LPCSTR pszData, size_t nLen)
{
	// 在共享服取一下信息
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(dwClientID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	// 取得玩家PDBID
	DWORD dwPDBID = shareProperty.dwPDBID;

	// 客户端发过来的消息
	switch ( dwMsgID )
	{
	case MSG_CLAN_DISMISS:
		{
			onClientDismiss(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_CANEL_DISMISS:
		{
			onClientCanelDismiss(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_KICK:
		{
			onClientKick(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_KICKKINMEMBER:
		{
			onClientKickKinMember(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_APPLY:
		{
			onClientApply(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_ADMIT:
		{
			onClientAdmit(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_QUIT:
		{
			onClientQuit(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_CLANBASEDATA:
		{
			onClientClanBaseData(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_SETSTRINGPROP:
		{
			onClientSetStringProp(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_EXITOBSERVE:
		{
			onClientExitObserve(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_REQUEST_CARD_LIST:
		{
			onClientRequestCardList(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_REQUEST_CARD:
		{
			onClientRequestCard(dwPDBID, pszData, nLen);
		}
		break;
	case MSG_CLAN_DONATE_CARD:
		{
			onClientDonateCard(dwPDBID, pszData, nLen);
		}
		break;
    case MSG_CLAN_APPOINT:
        {
            onClientAppoint(dwPDBID, pszData, nLen);
        }
        break;
    case MSG_CLAN_UPDATEEVENT:
        {
            onClientUpdateClanEvent(dwPDBID);
        }
        break;
	case MSG_CLAN_SETINTPROP:
		{
			onClientSetIntProp(dwPDBID, pszData, nLen);
		}
		break;
	default:
		{
			// 其他消息
			ErrorLn(__FUNCTION__": unknown dwMsgID= " << dwMsgID);
		}
		break;
	}
}

// 场景服请求卡牌支援
void CClan::onSceneRequestCard(DWORD dwSeverID, LPCSTR pszData, size_t nLen)
{
	if (pszData == NULL || nLen != sizeof(msg_clan_requestcard_so))
	{
		CLAN_WARNINGLN(__FUNCTION__" nLen = " << nLen);
		return;
	}
	msg_clan_requestcard_so *pRecvData = (msg_clan_requestcard_so*)pszData;

	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	DWORD dwPDBID = pRecvData->dwPDBID;
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
	{
		CLAN_WARNINGLN(__FUNCTION__" pSharePerson == NULL pdbid = " << dwPDBID);
		return;
	}

	// 取得角色等级
	SClanCardSchemeInfo *pClanCardInfo = g_EHelper.getClanCardInfo(pRecvData->nLevel);
	if (pClanCardInfo == NULL)
	{
		CLAN_WARNINGLN(__FUNCTION__" pClanCardInfo == NULL nLevel = " << pRecvData->nLevel << "pdbid = " << dwPDBID);
		return;
	}

	// 移除当前请求卡牌
	REQUEST_CARD_LIST_ITER iter = findRequestCard(dwPDBID);
	if (iter != m_RequestCardList.end())
	{
		// 移除请求卡牌
		removeRequestCardToClient(dwPDBID, dwPDBID);
		// 从表中移除
		m_RequestCardList.erase(iter);
	}

	SRequestCardItem requestCard;
	requestCard.dwPDBID = dwPDBID;
	requestCard.nIndex = ++m_nRequestCardIndex;
	sstrcpyn(requestCard.szName, shareProperty.szName, sizeof(requestCard.szName));
	requestCard.nLevel = pRecvData->nLevel;
	requestCard.nCardID = pRecvData->nCardID;
	requestCard.nCurCount = 0;
	requestCard.nMaxCount = min(pClanCardInfo->nRequestSingleNumLimit, pClanCardInfo->nRequestDayNumLimit - pRecvData->nDayGetCount);
	// 增加请求卡牌
	m_RequestCardList.push_front(requestCard);
	const int nCardListMaxSize = 200;
	if (m_RequestCardList.size() > nCardListMaxSize)
		m_RequestCardList.pop_back();

	// 增加请求提示计数
	addRequestTipToClient(shareProperty, pRecvData->nCardID);
	// 更新卡牌列表到客户端
	updateCardListToClient(dwPDBID);
}

// 场景服捐赠卡牌
void CClan::onSceneDonateCard(DWORD dwSeverID, LPCSTR pszData, size_t nLen)
{
	if (pszData == NULL || nLen != sizeof(msg_clan_donatecard_so))
	{
		CLAN_WARNINGLN(__FUNCTION__" nLen = " << nLen);
		return;
	}

	msg_clan_donatecard_so *pRecvData = (msg_clan_donatecard_so*)pszData;

	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
		return;

	REQUEST_CARD_LIST_ITER iterRequestCard = findRequestCard(pRecvData->dwByDBID);
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pRecvData->dwByDBID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
	{
		// 被捐赠的角色不在线,写入数据库
		DBREQ_PARAM_ADDCLANDONATEINFO addDonateInfo;
		addDonateInfo.dwPDBID = pRecvData->dwByDBID;
		addDonateInfo.dwDonatePDBID = pRecvData->dwPDBID;
		sstrcpyn(addDonateInfo.szDonateActorName, pRecvData->szName, sizeof(addDonateInfo.szDonateActorName));
		if (iterRequestCard != m_RequestCardList.end())
		{
			addDonateInfo.dwIndex = iterRequestCard->nIndex * 100 + (int)iterRequestCard->vecIDList.size();
		}
		else
		{
			// 找不到先给个随机数。属于临时处理，以后有空时再想想怎么避免这种情况
			addDonateInfo.dwIndex = rand();
		}
		addDonateInfo.dwCardID = pRecvData->nCardID;
		addDonateInfo.dwCardNum = pRecvData->nCardCount;

		m_pClanService->onExecuteSP(GAMEDB_REQUEST_ADDCLANDONATEINFO, 0, (LPCSTR)&addDonateInfo, sizeof(addDonateInfo));
	}
	else
	{
		obuf obuf;
		fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_ADD_DONATE_CARD);

		msg_clan_adddonatecard_os sendData;
		sendData.dwPDBID = pRecvData->dwPDBID;
		sstrcpyn(sendData.szName, pRecvData->szName, sizeof(sendData.szName));
		sendData.dwByDBID = pRecvData->dwByDBID;
		sendData.nCardID = pRecvData->nCardID;
		sendData.nCardCount = pRecvData->nCardCount;
		obuf.push_back(&sendData, sizeof(sendData));
		g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
	}

	// 增加联盟贡献
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService != NULL)
	{
		pKinService->addClanCtrb(pRecvData->dwPDBID, pRecvData->nCardCount);
        // 取得成员信息
        SKinMember sKinMember = pKinService->getKinMemberInfo(pRecvData->dwPDBID);
		m_pClanService->updateMemberToClient_OC(m_dwID, &sKinMember);

        do 
        {
            if (sKinMember.dwKinID == 0)
            {
                break;
            }

            SKinInfo sKin = pKinService->getKinInfo(sKinMember.dwKinID);
            if (sKin.nKinProp_ID == 0)
            {
                break;
            }

            // 更新信息广播到观察者
            SClientClanMember clientMember;
            fillClientMemberInfo(clientMember, sKin, &sKinMember);
            broadcastMsgToObserve(MSG_CLAN_UPDATEMEMBERINFO, (LPCSTR)&clientMember, sizeof(clientMember));
        } while (false);
	}

	// 更新捐赠记录信息
	int nLogCount = 0;
	list<SDonateCardLogInfo>::iterator iterLog = m_DonateCardLogInfo.begin();
	bool bFound = false;
	// 只合并最近3条内的内容
	for ( ; iterLog != m_DonateCardLogInfo.end() && nLogCount < 3; ++iterLog, ++nLogCount)
	{
		if ((*iterLog).nGaveActorID == pRecvData->dwPDBID && (*iterLog).nTakeActorID == pRecvData->dwByDBID && (*iterLog).nCardID == pRecvData->nCardID)
		{
			(*iterLog).nCardNum += pRecvData->nCardCount;
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		SDonateCardLogInfo sLogInfo;
		sLogInfo.nGaveActorID = pRecvData->dwPDBID;
        SSharePersonProperty donateShareProperty = pShareReceiver->GetSharePerson(pRecvData->dwPDBID);
		if (donateShareProperty.dwPDBID == pRecvData->dwPDBID)
			sstrcpyn(sLogInfo.szGaveActorName, donateShareProperty.szName, sizeof(sLogInfo.szGaveActorName));
		sLogInfo.nTakeActorID = pRecvData->dwByDBID;
		if (iterRequestCard != m_RequestCardList.end())
			sstrcpyn(sLogInfo.szTakeActorName, iterRequestCard->szName, sizeof(sLogInfo.szTakeActorName));
		sLogInfo.nCardID = pRecvData->nCardID;
		sLogInfo.nCardNum = pRecvData->nCardCount;
		m_DonateCardLogInfo.push_front(sLogInfo);

		if (m_DonateCardLogInfo.size() > 20)
			m_DonateCardLogInfo.pop_back();

		iterLog = m_DonateCardLogInfo.begin();
	}

	{
		obuf obuf;
		fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_UPDATE_DONATE_LOG);

		msg_clan_updatedonatelog_oc sendLogData;
		sendLogData.nGaveActorID = (*iterLog).nGaveActorID;
		sstrcpyn(sendLogData.szGaveActorName, (*iterLog).szGaveActorName, sizeof(sendLogData.szGaveActorName));
		sendLogData.nTakeActorID = (*iterLog).nTakeActorID;
		sstrcpyn(sendLogData.szTakeActorName, (*iterLog).szTakeActorName, sizeof(sendLogData.szTakeActorName));
		sendLogData.nCardID = (*iterLog).nCardID;
		sendLogData.nCardNum = (*iterLog).nCardNum;
		obuf.push_back(&sendLogData, sizeof(sendLogData));
		g_EHelper.sendDataToPerson(pRecvData->dwPDBID, obuf.data(), obuf.size());
	}

	// 更新请求卡牌信息
	if (iterRequestCard != m_RequestCardList.end())
	{
		iterRequestCard->nCurCount++;
		iterRequestCard->vecIDList.push_back(pRecvData->dwPDBID);

		// 如果捐赠完成，则移除
		if (iterRequestCard->nCurCount >= iterRequestCard->nMaxCount)
		{
			// 从表中移除
			m_RequestCardList.erase(iterRequestCard);
			// 移除客户端请求卡牌
			removeRequestCardToClient(pRecvData->dwPDBID, pRecvData->dwByDBID);
		}
		else
		{
			// 更新客户端请求卡牌
			updateRequestCardToClient(pRecvData->dwPDBID, *iterRequestCard);
			updateRequestCardToClient(pRecvData->dwByDBID, *iterRequestCard);
		}
	}
	else
	{
		CLAN_WARNINGLN(__FUNCTION__" add card without requestList pdbid = " << pRecvData->dwByDBID << " donate pdbid = " << pRecvData->dwPDBID);
	}
}

// 场景服更新属性
void CClan::onSceneUpdateProp(DWORD dwSeverID, LPCSTR pszData, size_t nLen)
{
	if (pszData == NULL || nLen != sizeof(msg_clan_updateintprop_os))
	{
		CLAN_WARNINGLN(__FUNCTION__" nLen = " << nLen);
		return;
	}

	msg_clan_updateintprop_os *pRecvData = (msg_clan_updateintprop_os*)pszData;
	
	setNumProp(pRecvData->nPropID, pRecvData->nValue, true, true);
}

// 解散联盟消息处理									-- 来自客户端
void CClan::onClientDismiss(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    SClanConfig* pConfig = g_EHelper.getConfig();
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (pConfig == NULL || pShareReceiver == NULL || shareProperty.dwPDBID == INVALID_PDBID)
        return;

    // 判断玩家是否有解散联盟的权限
    if (dwPDBID != m_nShaikhID)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_DISMISS_PRIVILEGE);
        return;
    }

	if (m_nMemberIDList.size() == 1)
	{
		dismiss(emClanDismiss_ZeroKin);
		return;
	}

    if (m_nState != emClanState_Normal)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_DISMISS_STATE);
        return;
    }

    char szText[256]= {0};
    ssprintf_s(szText, sizeof(szText), "%s;%d", shareProperty.szName, pConfig->nDismissingLife / (3600 * 24));
    g_EHelper.broadcastClanSystemTips(m_dwID, CHAT_TIP_CLAN_DISMISS_STATE, szText);

    ostrbuf logBuf;
    logBuf.reserve(512);
    logBuf << a2utf8("解散联盟:角色ID:") << dwPDBID << a2utf8(",角色名:") << shareProperty.szName << a2utf8(",解散联盟名称:") << m_szName << a2utf8(",主动解散");
    g_EHelper.addClanGameNoteLog(m_dwID, logBuf.c_str());

    // 把联盟进入解散状态
    m_nDismissApplyTime = (int)time(NULL);
    setCurState(emClanState_Dismissing);
    broadcastPropToClient(emClanProp_State);

    // 往数据库存盘
    updateDataToDB();
}

// 取消解散联盟消息处理								-- 来自客户端
void CClan::onClientCanelDismiss(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
        return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
        return;

    // 判断玩家是否有解散联盟的权限
    if (dwPDBID != m_nShaikhID)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CANEL_DISMISS_PRIVILEGE);
        return;
    }

    if (m_nState != emClanState_Dismissing)
        return;

    // 把联盟进入解散状态
    setCurState(emClanState_Normal);
    broadcastPropToClient(emClanProp_State);

    // 往数据库存盘
    updateDataToDB();

    char szText[512]= {0};
    ssprintf_s(szText, sizeof(szText), "%s", shareProperty.szName);
    g_EHelper.broadcastClanSystemTips(m_dwID, CHAT_TIP_CLAN_CANEL_DISMISS_TIP, szText);

    ostrbuf logBuf;
    logBuf.reserve(512);
    logBuf << a2utf8("取消解散联盟:角色ID:") << dwPDBID << a2utf8(",角色名:") << shareProperty.szName << a2utf8(",取消解散联盟名称:") << m_szName << a2utf8(",主动取消");
    g_EHelper.addClanGameNoteLog(m_dwID, logBuf.c_str());
}

// 开除成员											-- 来自客户端
void CClan::onClientKick(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
        return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
        return;

	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	if (pszData == NULL || nLen != sizeof(msg_clan_kick_co))
		return;

	// 检测玩家是不是本联盟的成员
	DWORD dwKinClanID = m_pClanService->findKinClanByPDBID(dwPDBID);
	if (dwKinClanID != m_dwID)
		return;

	if (dwPDBID != m_nShaikhID)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_PRIVILEGE);
		return;
	}

	msg_clan_kick_co *pRecvData = (msg_clan_kick_co*)pszData;

	// 传进来的成员是否是本联盟成员
	SKinMember sKinMember = pKinService->getKinMemberInfo(pRecvData->dwMemberPDBID);
	if (sKinMember.dwKinID == 0)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_NO_KIN);
		return;
	}

	DWORD dwTargetKinClanID = m_pClanService->findKinClan(sKinMember.dwKinID);
	if (dwTargetKinClanID != m_dwID)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_NO_CLAN);
		return;
	}

	if (sKinMember.nIdentity != emKinIdentity_Shaikh)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_NO_KIN_SHAIKH);
		return;
	}

    // 现在的时间
    tm local_NowTime;
    tm local_time;

    DWORD dwNowTimes =(DWORD)time(NULL);
    time_t long_NowTime = (time_t)dwNowTimes; 
    localtime_s(&local_NowTime,&long_NowTime);

    // 上一次踢人时间
    time_t long_time = (time_t)m_nKickTime; 
    localtime_s(&local_time, &long_time);
    if (local_NowTime.tm_yday == local_time.tm_yday && local_NowTime.tm_year == local_time.tm_year)
    {
        SClanConfig *pConfig = g_EHelper.getConfig();
        if (pConfig == NULL)
            return;

        if (m_nKickNum >= pConfig->nKickNum)
        {
            char szText[32] = {0};
            ssprintf_s(szText, sizeof(szText), "%d", pConfig->nKickNum);
            g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_DAY_LIMIT_KICK_COUNT, szText);
            return;
        }
    }
    else
    {
        // 已经跨天了,重置之前的记录
        m_nKickTime = (int)time(NULL);
        m_nKickNum = 0;
    }
    m_nKickNum++;

    SKinInfo sKin = pKinService->getKinInfo(sKinMember.dwKinID);
	if (sKin.nKinProp_ID != 0)
	{
		// 通知客户端删除战队成功
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_SUCCEED, sKin.szName);

		char szText[256] = {0};
		ssprintf_s(szText, sizeof(szText), "%s", m_szName);
        g_EHelper.broadcastKinSystemTips(sKin.nKinProp_ID, CHAT_TIP_CLAN_BY_KICK, szText);

		// 添加联盟事件日志
		SClanEvent newEvent;
		newEvent.nTime = (int)time(NULL);
		ssprintf_s(newEvent.szCoutext, sizeof(newEvent.szCoutext), a2utf8("[%s]被[%s]联盟开除"), sKin.szName, m_szName);
		addClanEvent(newEvent);

        ostrbuf logBuf;
        logBuf.reserve(512);
        logBuf << a2utf8("踢出战队:角色ID:") << dwPDBID << a2utf8(",角色名:") << shareProperty.szName << a2utf8(",把战队名称:") << sKin.szName << a2utf8(",踢出联盟");
        g_EHelper.addClanGameNoteLog(m_dwID, logBuf.c_str());
	}

	// 删除成员
	delMember(sKinMember.dwKinID, sKin.nKinProp_ShaikhID, true);
}

// 联盟帮主开出战队成员
void CClan::onClientKickKinMember(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	SClanConfig *pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

	if (pszData == NULL || nLen != sizeof(msg_clan_kickkinmember_co))
		return;

	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	// 检测玩家是不是本联盟的成员
	DWORD dwKinClanID = m_pClanService->findKinClanByPDBID(dwPDBID);
	if (dwKinClanID != m_dwID)
		return;

	if (dwPDBID != m_nShaikhID)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_KM_PRIVILEGE);
		return;
	}

	msg_clan_kickkinmember_co *pRecvData = (msg_clan_kickkinmember_co*)pszData;

	// 传进来的成员是否是本联盟成员
	SKinMember sKinMember = pKinService->getKinMemberInfo(pRecvData->dwMemberPDBID);
	if (sKinMember.dwKinID == 0)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_KM_NO_KIN);
		return;
	}

	DWORD dwTargetKinClanID = m_pClanService->findKinClan(sKinMember.dwKinID);
	if (dwTargetKinClanID != m_dwID)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_KM_NO_CLAN);
		return;
	}

	if (sKinMember.nIdentity == emKinIdentity_Shaikh || sKinMember.nIdentity == emKinIdentity_Elder)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_KM_KIN_SHAIKH);
		return;
	}

	// 现在的时间
	tm local_NowTime;
	tm local_time;

	DWORD dwNowTimes =(DWORD)time(NULL);
	time_t long_NowTime = (time_t)dwNowTimes; 
	localtime_s(&local_NowTime,&long_NowTime);

	// 上一次踢人时间
	time_t long_time = (time_t)m_nKickTime; 
	localtime_s(&local_time, &long_time);
	if (local_NowTime.tm_yday == local_time.tm_yday && local_NowTime.tm_year == local_time.tm_year)
	{
		if (m_nKickNum >= pConfig->nKickNum)
		{
			char szText[32] = { 0 };
			ssprintf_s(szText, sizeof(szText), "%d", pConfig->nKickNum);
			g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_DAY_LIMIT_KICK_COUNT, szText);
			return;
		}
	}
	else
	{
		// 已经跨天了,重置之前的记录
		m_nKickTime = (int)time(NULL);
		m_nKickNum = 0;
	}

    SKinInfo sKin = pKinService->getKinInfo(sKinMember.dwKinID);
	if (sKin.nKinProp_ID == 0)
	{
		return ;
	}

	// 通知客户端删除人物成功
	g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_KICK_KM_SUCCEED, sKinMember.szName);
	char szText[512] = {0};
	ssprintf_s(szText, sizeof(szText), "%s;%s", sKinMember.szName, shareProperty.szName);
	g_EHelper.broadcastClanSystemTips(m_dwID, CHAT_TIP_CLAN_KICK_KIN_MEMBER, szText);

	// 真的删除
    pKinService->deleteKinMember(sKin.nKinProp_ID, pRecvData->dwMemberPDBID);

    ostrbuf logBuf;
    logBuf.reserve(512);
    logBuf << a2utf8("踢出战队成员:角色ID:") << dwPDBID << a2utf8(",角色名:") << shareProperty.szName << a2utf8(",把战队成员:") << sKinMember.szName << a2utf8(",踢出");
    g_EHelper.addClanGameNoteLog(m_dwID, logBuf.c_str());

	m_nKickNum++;
}

// 申请加入											-- 来自客户端
void CClan::onClientApply(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

    SClanConfig *pConfig = g_EHelper.getConfig();
    if (pConfig == NULL)
        return;

    IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
        return;
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
        return;

    SClanLevel *pLevelInfo = g_EHelper.getLevelInfo(m_nLevel);
    if (pLevelInfo == NULL)
        return;

    // 联盟已经在解散状态了，不能申请
    if (m_nState == emClanState_Dismissing)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_DISMISSING_NO_ACCEPT);
        return;
    }

    // 不是战队族长，不能申请
    SKinMember *pKinMember = g_EHelper.getActorKinMemberInfo(dwPDBID);
    if (pKinMember == NULL)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPLY_NO_KIN);
        return;
    }

    SKinInfo sKin = pKinService->getKinInfo(pKinMember->dwKinID);
    if (sKin.nKinProp_ID == 0 || sKin.nKinProp_State == emKinState_Informal)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPLY_NO_KIN);
        return;
    }

    if (pKinMember->nIdentity != emKinIdentity_Shaikh)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPLY_NO_KIN_SHAIKH);
        return;
    }

    // 本战队是否已经满人了
    if (m_nLevel <= 0 || m_nLevel > CLAN_MAX_LEVEL || m_nMemberIDList.size() >= (ulong)pLevelInfo->nMaxKinCount)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPLY_CLAN_MEMBER_FULL);
        return;
    }

    if (m_ApplyKinList.size() >= 10)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPLY_LIST_FULL);
        return;
    }

    // 申请者是否已经加入了其他联盟
    DWORD dwKinClanID = m_pClanService->findKinClan(pKinMember->dwKinID);
    if (dwKinClanID != 0)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPLY_HAVE_CLAN);
        return;
    }

	// 如果已经申请加入了其他战队
	DWORD dwOldAppClanID = 0;
    SClanApply* pClanApply = g_ClanInviteManager.findApply(pKinMember->dwKinID);
    if (pClanApply != NULL)
    {
        if (pClanApply->dwClanID != m_dwID)
        {
			dwOldAppClanID = pClanApply->dwClanID;
            g_ClanInviteManager.removeApply(this, pKinMember->dwKinID);	// pClanApply已经不能在使用了
        }
        else
        {
            g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPLY_HAVE_APPLY);
            return;
        }
	}

	// 自动加入
	if (m_nAutoAcceptFlag == 1)
	{
		acceptMember(pKinMember->dwKinID);

		ostrbuf logBuf;
		logBuf.reserve(512);
		logBuf << a2utf8("加入联盟 自动同意") << sKin.szName << a2utf8("加入联盟");
		g_EHelper.addClanGameNoteLog(m_dwID, logBuf.c_str());
		return;
	}

    // 战队成员数量
    int nMemberCount = pKinService->getKinMemberCount(pKinMember->dwKinID);

    SClanApply ClanApply;
    ClanApply.dwClanID = m_dwID;
    ClanApply.dwKinID = pKinMember->dwKinID;
    sstrcpyn(ClanApply.szName, sKin.szName, sizeof(ClanApply.szName));
    sstrcpyn(ClanApply.szShaikhName, shareProperty.szName, sizeof(ClanApply.szShaikhName));
	ClanApply.nMemberCount = nMemberCount;
    g_ClanInviteManager.addApply(this, &ClanApply);

    // 飘字提示
    g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPLY_SUCCEED, m_szName);

    // 给打开窗口的人变化
	msg_clan_apply_oc sendData;
	sendData.dwClanID = m_dwID;
	sendData.dwKinID = pKinMember->dwKinID;
	sstrcpyn(sendData.szName, sKin.szName, sizeof(sendData.szName));
	sstrcpyn(sendData.szShaikhName, shareProperty.szName, sizeof(sendData.szShaikhName));
    sendData.nMemberCount = nMemberCount;
    broadcastMsgToObserve(MSG_CLAN_APPLY, (LPCSTR)&sendData, sizeof(sendData));

	// 给申请人通知
	obuf appRetbuf;
	fillClanData(appRetbuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_APPLY_RET);

	msg_clan_apply_ret_oc appRet;
	appRet.dwOldClanID = dwOldAppClanID;
	appRet.dwClanID = m_dwID;
	appRet.nApplystate = CLAN_LIST_STATUS_REQUESTING;

	appRetbuf.push_back(&appRet, sizeof(msg_clan_apply_ret_oc));
	g_EHelper.sendDataToPerson(dwPDBID, appRetbuf.data(), appRetbuf.size());
}

// 接收成员											-- 来自客户端
void CClan::onClientAdmit(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
        return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
        return;

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

    if (pszData == NULL || nLen != sizeof(msg_clan_admit_co))
        return;

    // 联盟已经在解散状态了，不能招收
    if (m_nState == emClanState_Dismissing)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_DISMISSING_NO_ACCEPT);
        return;
    }

    // 检测玩家是否有招收成员的权限
    if (dwPDBID != m_nShaikhID)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_ADMIT_PRIVILEGE);
        return;
    }

    msg_clan_admit_co* pRecvData = (msg_clan_admit_co*)pszData;
    SClanApply* pClanApply = g_ClanInviteManager.findApply(pRecvData->dwKinID);
    if (pClanApply == NULL || pClanApply->dwClanID != m_dwID)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_ADMIT_NO_MY_CLAN);
        return;
    }

    SKinInfo sKin = pKinService->getKinInfo(pClanApply->dwKinID);
    if (sKin.nKinProp_ID == 0 || sKin.nKinProp_State == emKinState_Informal)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_ADMIT_NO_INFORMAL_KIN);
        return;
    }

    DWORD dwKinClanID = m_pClanService->findKinClan(pRecvData->dwKinID);
    if (dwKinClanID != 0)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_ADMIT_HAVE_CLAN);
        return;
    }

    // 判断列表是否已经满了
    SClanLevel* pLevelInfo = g_EHelper.getLevelInfo(m_nLevel);
    if (pLevelInfo == NULL)
        return;

    if (pRecvData->bAgree == true)
    {
        if (m_nMemberCount >= pLevelInfo->nMaxKinCount)
        {
            g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_ADMIT_MEMBER_MAX);
            return;
        }

        acceptMember(pRecvData->dwKinID);

        ostrbuf logBuf;
        logBuf.reserve(512);
        logBuf << a2utf8("加入联盟:角色ID:") << dwPDBID << a2utf8(",角色名:") << shareProperty.szName << a2utf8(",同意") << pClanApply->szName << a2utf8("加入联盟");
        g_EHelper.addClanGameNoteLog(m_dwID, logBuf.c_str());
    }
    else
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_ADMIT_REJECT, pClanApply->szName);

        // 飘字提示玩家
        g_EHelper.showSystemMessage(sKin.nKinProp_ShaikhID, CHAT_TIP_CLAN_ADMIT_BY_REJECT, m_szName);
    }

    g_ClanInviteManager.removeApply(this, pRecvData->dwKinID);
}

// 临时处理自动接收成员，非正式功能
void CClan::onForceAdmit(DWORD dwKinID)
{
	DWORD dwKinClanID = m_pClanService->findKinClan(dwKinID);
	if (dwKinClanID != 0)
		return;

	// 判断列表是否已经满了
	SClanLevel* pLevelInfo = g_EHelper.getLevelInfo(m_nLevel);
	if (pLevelInfo == NULL)
		return;

	if (m_nMemberCount >= pLevelInfo->nMaxKinCount)
		return;

	acceptMember(dwKinID);
}

// 退出联盟											-- 来自客户端
void CClan::onClientQuit(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;

	// 检测玩家是不是本联盟的成员
	SKinMember* pKinMember = g_EHelper.getActorKinMemberInfo(dwPDBID);
	if (pKinMember == NULL)
		return;

	DWORD dwKinClanID = m_pClanService->findKinClan(pKinMember->dwKinID);
    if (dwKinClanID != m_dwID)
        return;

    int nIdentity = getActorIdentity(dwPDBID);
    // 帮主不能退出联盟
    if (nIdentity == EMCLANIDENTITY_SHAIKH)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_QUIT_SHAIKH);
        return;
    }

    // 如果是长老
    if (nIdentity != EMCLANIDENTITY_ELDER)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_QUIT_ELDER);
        return;
    }

    SKinInfo sKin = pKinService->getKinInfo(pKinMember->dwKinID);
    if (sKin.nKinProp_ID != 0)
    {
        // 添加联盟事件记录
        SClanEvent newEvent;
        newEvent.nTime = (int)time(NULL);
        ssprintf_s(newEvent.szCoutext, sizeof(newEvent.szCoutext), a2utf8("[%s]主动退出联盟"), sKin.szName);
        addClanEvent(newEvent);

        ostrbuf logBuf;
        logBuf.reserve(512);
        logBuf << a2utf8("退出联盟:战队名字:") << sKin.szName << a2utf8(",战队退出") << m_szName << a2utf8("联盟");
        g_EHelper.addClanGameNoteLog(m_dwID, logBuf.c_str());
    }

    // 删除成员
    delMember(pKinMember->dwKinID, sKin.nKinProp_ShaikhID, true);
}

// 请求联盟数据										-- 来自客户端
void CClan::onClientClanBaseData(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    // 更新联盟基本数据
    updateClientClanBaseData(dwPDBID);
    // 发送请求捐卡提示
    // delete:登录时已获取提示，此处不需要再次获取，只在改变时主动发给客户端，客户端收到后就更新tip
	// sendRequestTip(dwPDBID);
}

// 修改宣言，公告										-- 来自客户端
void CClan::onClientSetStringProp(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    if (pszData == NULL || nLen != sizeof(msg_clan_setstringprop_co))
        return;

    // 检测玩家是否有招收成员的权限
    msg_clan_setstringprop_co *pRecvData = (msg_clan_setstringprop_co*)pszData;
    if (pRecvData->nPropID == emClanManifesto)
    {
        // 判断权限
        if (dwPDBID != m_nShaikhID)
        {
            g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_MOTIFY_MANIFESTO);
            return;
        }

        sstrcpyn(m_szManifesto, pRecvData->szText, sizeof(m_szManifesto));
    }
    else if (pRecvData->nPropID == emClanNotice)
    {
        // 判断权限
        if (dwPDBID != m_nShaikhID)
        {
            g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_MOTIFY_NOTICE);
            return;
        }

        sstrcpyn(m_szNotice, pRecvData->szText, sizeof(m_szNotice));
    }
    else
    {
        return;
    }

    // 往数据库存盘
    updateDataToDB();

    msg_clan_updatestringprop_oc sendData;
    sendData.nPropID = pRecvData->nPropID;
    sstrcpyn(sendData.szValue, pRecvData->szText, sizeof(sendData.szValue));
    broadcastMsgToObserve(MSG_CLAN_UPDATECLANSTRPROP, (LPCSTR)&sendData, sizeof(sendData));

	sendClanBaseData();
}

// 修改整形属性数据
void CClan::onClientSetIntProp(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
	if (pszData == NULL || nLen != sizeof(msg_clan_setintprop_co))
		return;

	msg_clan_setintprop_co *pRecvData = (msg_clan_setintprop_co*)pszData;

	if (pRecvData->nPropID == emClanProp_AutoAcceptFlag)
	{
		if (dwPDBID != m_nShaikhID)
			return;

		int nValue = (pRecvData->nValue == 0 ? 0 : 1);
		setNumProp(pRecvData->nPropID, nValue, true, true);

		broadcastPropToClient(pRecvData->nPropID);
	}
}

// 退出观察											-- 来自客户端
void CClan::onClientExitObserve(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    m_ObserveList.remove(dwPDBID);
}

void CClan::onClientRequestCardList(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    // 如果玩家没有联盟
    DWORD dwKinClanID = m_pClanService->findKinClanByPDBID(dwPDBID);
    if (dwKinClanID != m_dwID)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CARD_NO_MY_CLAN);
        return;
	}

    // 更新卡牌列表到客户端
	updateCardListToClient(dwPDBID);
}

// 请求卡牌客户端消息码
void CClan::onClientRequestCard(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
        return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
        return;

    if (pszData == NULL || nLen != sizeof(msg_clan_requestcard_co))
        return;
    msg_clan_requestcard_co *pRecvData = (msg_clan_requestcard_co*)pszData;

    // 如果玩家没有联盟
    DWORD dwKinClanID = m_pClanService->findKinClanByPDBID(dwPDBID);
    if (dwKinClanID != m_dwID)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CARD_NO_MY_CLAN);
        return;
    }
    
    obuf512 obuf;
    msg_clan_requestcard_os requestCard;
    requestCard.dwPDBID = dwPDBID;
    requestCard.nCardID = pRecvData->nCardID;

    fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_REQUEST_CARD);
    obuf.push_back(&requestCard, sizeof(requestCard));

    g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
}

void CClan::onClientDonateCard(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
        return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
        return;

    // 如果玩家没有联盟
    DWORD dwKinClanID = m_pClanService->findKinClanByPDBID(dwPDBID);
    if (dwKinClanID != m_dwID)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CARD_DONATE_NO_MY_CLAN);
        return;
    }

    if (pszData == NULL || nLen != sizeof(msg_clan_donatecard_co))
        return;
    msg_clan_donatecard_co *pRecvData = (msg_clan_donatecard_co*)pszData;

    // 判断参数
    if (pRecvData->nCardCount <= 0)
        return;

    if (pRecvData->dwByDBID == dwPDBID)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CARD_DONATE_SELF);
        return;
    }

    REQUEST_CARD_LIST_ITER iterRequestCard = findRequestCard(pRecvData->dwByDBID);
    if (iterRequestCard == m_RequestCardList.end())
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CARD_DONATE_CHANGE);
        return;
    }

    // 取得角色等级
	int nLevel = shareProperty.wLevel;
	SClanCardSchemeInfo *pSelfCardInfo = g_EHelper.getClanCardInfo(nLevel);
	if (pSelfCardInfo == NULL)
	{
		CLAN_WARNINGLN(__FUNCTION__" pSelfCardInfo == NULL nLevel = " << nLevel << " dbid = " << dwPDBID);
		return;
	}

	// 判断自己单次捐献是否达到上限
	vector<PDBID>::iterator iterSelfSingleLimit = iterRequestCard->vecIDList.begin();
	vector<PDBID>::iterator iterSelfSingleLimitEnd = iterRequestCard->vecIDList.end();
	int nSingleCount = 0;
	for ( ; iterSelfSingleLimit != iterSelfSingleLimitEnd; ++iterSelfSingleLimit)
	{
		if ((*iterSelfSingleLimit) == dwPDBID)
			++nSingleCount;
	}

	if (nSingleCount >= pSelfCardInfo->nDonateSingleNumLimit)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CARD_DONATE_SINGLE_LIMIT);
		return;
	}

    obuf obuf;
    fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_DONATE_CARD);

    msg_clan_donatecard_os sendData;
    sendData.dwPDBID = dwPDBID;
    sendData.dwByDBID = pRecvData->dwByDBID;
	sendData.nDonateLevel = nLevel;
	sendData.nTargetLevel = iterRequestCard->nLevel;
    sendData.nCardID = pRecvData->nCardID;
    sendData.nCardCount = pRecvData->nCardCount;
    obuf.push_back(&sendData, sizeof(sendData));
    g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
}

void CClan::onClientAppoint(DWORD dwPDBID, LPCSTR pszData, size_t nLen)
{
    if (pszData == NULL || nLen != sizeof(msg_clan_appoint_co))
    {
        return;
    }

    msg_clan_appoint_co* pRecvData = (msg_clan_appoint_co*)pszData;

	appointShaikh(dwPDBID, pRecvData->dwMemberPDBID);
}

void CClan::appointShaikh(DWORD dwPDBID, DWORD dwNewShaikhPDBID)
{
	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

	// 判断任命者是不是帮主
	if (dwPDBID != m_nShaikhID)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_MUST_SHAIKH_APPOINT);
		return;
	}

	if (dwNewShaikhPDBID == dwPDBID)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_SHAIKH_APPOINT_SELF);
		return;
	}

	SKinMember* pKinMember = g_EHelper.getActorKinMemberInfo(dwNewShaikhPDBID);
	if (pKinMember == NULL)
	{
		return;
	}

	DWORD dwKinClanID = m_pClanService->findKinClan(pKinMember->dwKinID);
	if (dwKinClanID != m_dwID)
	{
		return;
	}

	if (pKinMember->nIdentity != emKinIdentity_Shaikh)
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPOINT_FAIL);
		return;
	}

	// 给予玩家职位
	setShaikh(dwNewShaikhPDBID);

	char szText[512] = { 0 };
	ssprintf_s(szText, 512, "%s", g_EHelper.getIdentityName(EMCLANIDENTITY_SHAIKH).c_str());
	g_EHelper.showSystemMessage(dwNewShaikhPDBID, CHAT_TIP_CLAN_APPOINT_SHAIKH, szText);

	SKinMember* pNewShaikhKinMember = g_EHelper.getActorKinMemberInfo(dwNewShaikhPDBID);
	if (pNewShaikhKinMember == NULL)
	{
		return;
	}
	ssprintf_s(szText, 512, "%s;%s", pNewShaikhKinMember->szName, g_EHelper.getIdentityName(EMCLANIDENTITY_SHAIKH).c_str());
	g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_APPOINT_SUCCEED, szText);

	ostrbuf logBuf;
	logBuf.reserve(512);
	logBuf << a2utf8("盟主转让:角色ID:") << dwPDBID << a2utf8(",角色名:") << shareProperty.szName << a2utf8(",把") << g_EHelper.getIdentityName(EMCLANIDENTITY_SHAIKH).c_str() << a2utf8("转给") << pKinMember->szName;
	g_EHelper.addClanGameNoteLog(m_dwID, logBuf.c_str());
}

// 广播一个属性到客户端
void CClan::broadcastPropToClient(int nPropID)
{
	IGatewayAcceptorService *pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pGatewayAcceptorService == NULL || pShareReceiver == NULL || pKinService == NULL)
		return;

	obuf256 obuf;
	fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATECLANINTPROP);
	msg_clan_updateintprop_oc sUpdateInfo;
	sUpdateInfo.nPropID = nPropID;
	sUpdateInfo.nValue = getNumProp(nPropID);
	obuf.push_back(&sUpdateInfo, sizeof(sUpdateInfo));

	vector<ClientID> clientList;
	list<int>::iterator it = m_nMemberIDList.begin();
	for ( ; it != m_nMemberIDList.end(); ++it)
	{
		PDBID pReturnArray[256] = {0};
		DWORD dwCount = pKinService->getKinMemberList(*it, pReturnArray, 256);
		for (DWORD i = 0; i < dwCount; ++i)
		{
			DWORD dwPDBID = pReturnArray[i];

            SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
			if (shareProperty.dwPDBID == dwPDBID)
			{
				clientList.push_back(shareProperty.clientID);
			}
		}
	}

	if (clientList.size() > 0)
		pGatewayAcceptorService->broadcast(clientList.data(), (WORD)clientList.size(), obuf.data(), (WORD)obuf.size());
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
    LONGLONG old = *(m_propAddress[dwPropID]);
    if (old + (LONGLONG)nValue > MAX_INT_VALUE)
        return;

    *(m_propAddress[dwPropID]) += nValue;
    *(m_propAddress[dwPropID]) < 0 ? *(m_propAddress[dwPropID]) = 0 : 0;
}

// 添加成员
bool CClan::addMember(int nKinID)
{
	list<int>::iterator it;
	for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
	{
		if (nKinID == (*it))
			return true;
	}

	m_nMemberIDList.push_back(nKinID);
	m_nMemberCount++;
	return true;
}

// 更新卡牌列表到客户端
void CClan::updateCardListToClient(DWORD dwPDBID)
{
	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
	if (shareProperty.dwPDBID == INVALID_PDBID)
		return;

    // 取得角色等级
    int nLevel = shareProperty.wLevel;
    SClanCardSchemeInfo *pSelfCardInfo = g_EHelper.getClanCardInfo(nLevel);
    if (pSelfCardInfo == NULL)
    {
        CLAN_WARNINGLN(__FUNCTION__" pSelfCardInfo == NULL nLevel = " << nLevel << " dbid = " << dwPDBID);
        return;
    }

	m_ob.reset();
	{
		// 放进括号内以免宏里声明的变量重名。整理时这个宏应该改成函数
		fillClanData(m_ob, MSG_ENDPOINT_SCENE, MSG_CLAN_SENDREQUESTCARDINFO);

		msg_clan_sendrequestcardinfo_os sendData;
		sendData.dwPDBID = dwPDBID;
		m_ob.push_back(&sendData, sizeof(sendData));
		g_EHelper.sendDataToZoneSvr(m_ob.data(), m_ob.size());
	}

	m_ob.reset();
	{
		fillClanData(m_ob, MSG_ENDPOINT_SCENE, MSG_CLAN_REQUEST_CARD_LIST);

		msg_clan_requestcardlist_os requestCardList;
        requestCardList.dwPDBID = dwPDBID;
		requestCardList.nCount = (int)m_RequestCardList.size();
		m_ob.push_back(&requestCardList, sizeof(requestCardList));

		REQUEST_CARD_LIST_ITER iter = m_RequestCardList.begin();
		for ( ; iter != m_RequestCardList.end(); ++iter)
		{
            SRequestCardItem &requestCardInfo = *iter;

			msg_clan_requestcardlist_single node;
			node.dwPDBID = requestCardInfo.dwPDBID;
			sstrcpyn(node.szName, requestCardInfo.szName, sizeof(node.szName));
			node.nCardID = requestCardInfo.nCardID;
			node.nCurCount = requestCardInfo.nCurCount;
			node.nMaxCount = requestCardInfo.nMaxCount;
            node.nDonateCardCount = pSelfCardInfo->nDonateSingleNumLimit - findRequestCardDonateCount(requestCardInfo.dwPDBID, dwPDBID);
			m_ob.push_back(&node, sizeof(node));
		}
        // 因为要知道人物日捐赠上限，所以要发到场景服中转下
        g_EHelper.sendDataToZoneSvr(m_ob.data(), m_ob.size(), shareProperty.serverID);
	}

	// 一起更新捐赠记录到客户端
	m_ob.reset();
	{
		fillClanData(m_ob, MSG_ENDPOINT_CLIENT, MSG_CLAN_REQUEST_DONATE_LOG);

		msg_clan_requestdonatelog_oc sendLogHeadData;
		sendLogHeadData.nCount = (int)m_DonateCardLogInfo.size();
		m_ob.push_back(&sendLogHeadData, sizeof(sendLogHeadData));

		list<SDonateCardLogInfo>::iterator iterLog = m_DonateCardLogInfo.begin();
		for ( ; iterLog != m_DonateCardLogInfo.end(); ++iterLog)
		{
			msg_clan_updatedonatelog_oc sendLogData;
			sendLogData.nGaveActorID = (*iterLog).nGaveActorID;
			sstrcpyn(sendLogData.szGaveActorName, (*iterLog).szGaveActorName, sizeof(sendLogData.szGaveActorName));
			sendLogData.nTakeActorID = (*iterLog).nTakeActorID;
			sstrcpyn(sendLogData.szTakeActorName, (*iterLog).szTakeActorName, sizeof(sendLogData.szTakeActorName));
			sendLogData.nCardID = (*iterLog).nCardID;
			sendLogData.nCardNum = (*iterLog).nCardNum;
			m_ob.push_back(&sendLogData, sizeof(sendLogData));
		}

		g_EHelper.sendDataToPerson(dwPDBID, m_ob.data(), m_ob.size());
	}
}

// 更新请求卡牌到客户端
void CClan::updateRequestCardToClient(DWORD dwPDBID, const SRequestCardItem &requestCardInfo)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        return;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        return;
    }

    // 取得角色等级
    int nLevel = shareProperty.wLevel;
    SClanCardSchemeInfo *pSelfCardInfo = g_EHelper.getClanCardInfo(nLevel);
    if (pSelfCardInfo == NULL)
    {
        CLAN_WARNINGLN(__FUNCTION__" pSelfCardInfo == NULL nLevel = " << nLevel << " dbid = " << dwPDBID);
        return;
    }

    obuf obuf;
    fillClanData(obuf, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATE_REQUEST_CARD);

    msg_clan_updaterequestcard_os updateRequestCard;
    updateRequestCard.dwSendPDBID = dwPDBID;
    updateRequestCard.dwPDBID = requestCardInfo.dwPDBID;
    sstrcpyn(updateRequestCard.szName, requestCardInfo.szName, sizeof(updateRequestCard.szName));
    updateRequestCard.nCardID = requestCardInfo.nCardID;
    updateRequestCard.nCurCount = requestCardInfo.nCurCount;
    updateRequestCard.nMaxCount = requestCardInfo.nMaxCount;
    updateRequestCard.nDonateCardCount = pSelfCardInfo->nDonateSingleNumLimit - findRequestCardDonateCount(requestCardInfo.dwPDBID, dwPDBID);
    obuf.push_back(&updateRequestCard, sizeof(updateRequestCard));
    g_EHelper.sendDataToZoneSvr(obuf.data(), obuf.size(), shareProperty.serverID);
}

// 移除请求卡牌到客户端
void CClan::removeRequestCardToClient(DWORD dwPDBID, DWORD dwRemoveDBID)
{
    obuf obuf;
    fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_REMOVE_REQUEST_CARD);

    msg_clan_removerequestcard_oc removeRequestCard;
    removeRequestCard.dwPDBID = dwRemoveDBID;
    obuf.push_back(&removeRequestCard, sizeof(removeRequestCard));
    g_EHelper.sendDataToPerson(dwPDBID, obuf.data(), obuf.size());
}

// 增加请求提示计数
void CClan::addRequestTipToClient(SSharePersonProperty &shareProperty, int nCardID)
{
	obuf obuf;
	fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_ADD_REQUEST_TIP);

	msg_clan_addrequesttip_oc sendData;
	sendData.nIsInit = 0;
	sendData.nCount = 1;
	obuf.push_back(&sendData, sizeof(sendData));
	sendDataToAllClanMember(obuf.data(), obuf.size());
	
	ChatHelper chatHelper;
	IChatService *pChatService = chatHelper.m_ptr;
	if (NULL == pChatService)
	{
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
		return;
	}

	char szParam[32] = {0};
	ssprintf_s(szParam, sizeof(szParam), "%d", nCardID);
	pChatService->broadcastActorTipMessage(shareProperty, CHAT_CHANNEL_CLAN, CHAT_TIP_CLAN_ACTOR_REQUEST_CARD_TIP, szParam);
}

// 发送数据给所有在线成员
void CClan::sendDataToAllClanMember(char *pData, size_t nLen)
{
	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	IGatewayAcceptorService *pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pShareReceiver == NULL || pGatewayAcceptorService == NULL || pKinService == NULL)
		return;

    PDBID pReturnArray[256] = { 0 };
	vector<ClientID> targetList;
	list<int>::iterator it = m_nMemberIDList.begin();
	for ( ; it != m_nMemberIDList.end(); ++it)
	{
		DWORD dwKinID = *it;
		DWORD dwCount = pKinService->getKinMemberList(dwKinID, pReturnArray, 256);
		for (DWORD i = 0; i < dwCount; ++i)
		{
			DWORD dwPDBID = pReturnArray[i];
            SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwPDBID);
			if (shareProperty.dwPDBID == INVALID_PDBID)
				continue;
			targetList.push_back(shareProperty.clientID);
		}
	}

	pGatewayAcceptorService->broadcast(targetList.data(), (WORD)targetList.size(), pData, (WORD)nLen);
}

// 查找请求卡牌信息
REQUEST_CARD_LIST_ITER CClan::findRequestCard(PDBID targetID)
{
	REQUEST_CARD_LIST_ITER iter = m_RequestCardList.begin();
	for ( ; iter != m_RequestCardList.end(); ++iter)
	{
		if (iter->dwPDBID == targetID)
			break;
	}

	return iter;
}

// 查找对某个人捐赠卡牌次数
int CClan::findRequestCardDonateCount(DWORD dwRequestPDBID, DWORD dwDonatePDBID)
{
    int nCount = 0;

    REQUEST_CARD_LIST_ITER iter = m_RequestCardList.begin();
    for ( ; iter != m_RequestCardList.end(); ++iter)
    {
        if (iter->dwPDBID == dwRequestPDBID)
        {
            for (vector<PDBID>::iterator IterVec = iter->vecIDList.begin(); IterVec != iter->vecIDList.end(); ++IterVec)
            {
                if (*IterVec == dwDonatePDBID)
                {
                    ++nCount;
                }
            }
            break;
        }
    }

    return nCount;
}

// 通知解散联盟
void CClan::notifyDismiss(TClanDismissType nReason)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return;

	char szText[512] = {0};
	if (nReason == emClanDismiss_Dismiss)
		ssprintf_s(szText, sizeof(szText), a2utf8("[%s]联盟主动解散的时间到达，联盟解散"), m_szName);
	else if (nReason == emClanDismiss_ZeroKin)
		ssprintf_s(szText, sizeof(szText), a2utf8("您联盟[%s]已经没有战队了，自动解散"), m_szName);
	else if (nReason == emClanDismiss_Activity)
		ssprintf_s(szText, sizeof(szText), a2utf8("您联盟[%s]的繁荣度过低，自动解散"), m_szName);

	if ((m_dwReadFlag & emKinRead_Member) == emKinRead_Member)
	{
		// 往战队聊天广播消息
		list<int>::iterator it;
		for (it = m_nMemberIDList.begin(); it != m_nMemberIDList.end(); ++ it)
		{
		}
	}
}

void CClan::updateClanOrderInfo()
{
    IEventEngine* pEventEngine = gSocialGlobal->getEventEngine();
    if(!pEventEngine)
    {
        return;
    }

    obuf buf;
    event_clan_update_orderinfo eventData;
    eventData.dwClanID = m_dwID;			                // 战队ID
    eventData.dwTotalFightCapacity = getTotalFightScore();  // 战力总和
    eventData.dwShaikhID = m_nShaikhID;						// 族长ID
    eventData.dwPlayerCount = getMemberCount();       // 战队人数

    sstrcpyn(eventData.szShaikhName,m_szShaikhName,sizeof(eventData.szShaikhName));	        // 族长名字
    sstrcpyn(eventData.szClanName ,m_szName,sizeof(eventData.szClanName));                    // 战队名称	                 
    buf.push_back(&eventData, sizeof(eventData));
    pEventEngine->FireExecute(EVENT_CLAN_UPDATE_ORDERINFO, SOURCE_TYPE_CLAN, m_dwID, buf.data(), SIZE_INT(buf.size()));
}

void CClan::ResetWeekActivity()
{
	m_nWeekActivity = 0;
	updateDataToDB();

	// 更新属性到场景服
	obuf512 ob;
	fillClanData(ob, MSG_ENDPOINT_SCENE, MSG_CLAN_UPDATECLANPROP_ZONE);
	msg_clan_updateintprop_os sendDataToS;
	sendDataToS.dwClanID = m_dwID;
	sendDataToS.nPropID = (BYTE)emClanProp_WeekActivity;
	sendDataToS.nValue = *(m_propAddress[emClanProp_WeekActivity]);
	ob.push_back(&sendDataToS, sizeof(sendDataToS));
	g_EHelper.sendDataToZoneSvr(ob.data(), ob.size());

	// 更新客户端
	msg_clan_updateintprop_oc sendDataToC;
	sendDataToC.nPropID = emClanProp_WeekActivity;
	sendDataToC.nValue = 0;
	broadcastMsgToObserve(MSG_CLAN_UPDATECLANINTPROP, (LPCSTR)&sendDataToC, sizeof(sendDataToC));
}

// 帮主改变需要更新基础数据下去
void CClan::sendClanBaseData()
{
    SClanLevel* pLevelInfo = g_EHelper.getLevelInfo(m_nLevel);
    if (pLevelInfo == NULL)
        return;

    m_ob.reset();

    msg_clan_clanbasedata_oc sendData;
    sstrcpyn(sendData.szClanName, m_szName, sizeof(sendData.szClanName));
    sstrcpyn(sendData.szManifesto, m_szManifesto, sizeof(sendData.szManifesto));
    sstrcpyn(sendData.szShaikhName, m_szShaikhName, sizeof(sendData.szShaikhName));
	sstrcpyn(sendData.szNotice, m_szNotice, sizeof(sendData.szNotice));
    sendData.dwShaikhID = m_nShaikhID;
    sendData.nClanLevel = m_nLevel;
    sendData.nConstructDegree = m_nConstructDegree;
    sendData.nGold = m_nGold;
    sendData.nActivity = m_nActivity;
	sendData.nWeekActivity = m_nWeekActivity;
    sendData.nPlace = m_nPlace;
    sendData.nApplyMemberNum = 0;
    sendData.nKickTime = m_nKickTime;
    sendData.nKickNum  = m_nKickNum;
    sendData.nTotalFight = getTotalFightScore();
    sendData.nKinCount = getKinCount();
    sendData.nMaxKinCount = pLevelInfo->nMaxKinCount;
	sendData.nLegendCupCount = m_nLegendCupCount;
	sendData.llLegendCupID = 0;
	sendData.nAutoAcceptFlag = m_nAutoAcceptFlag;
	sendData.nCurTime = (int)time(NULL);
    sendData.nState = m_nState;
	sendData.nShaikhState = m_ShaikhState;

	LegendCupHelper cupHelper;
	ILegendCupService *pLegendCupService = cupHelper.m_ptr;
	if (pLegendCupService != NULL)
	{
		SCreaterLegendCupInfo sLegendCupInfo = pLegendCupService->getCreaterLegendCupInfo(emCupType_Clan, m_dwID);
		if (sLegendCupInfo.llLegendCupID > 0)
			sendData.llLegendCupID = sLegendCupInfo.llLegendCupID;
	}

    m_ob.push_back(&sendData, sizeof(sendData));

    // 显示基础数据
    broadcastMsgToObserve(MSG_CLAN_SET_CLANBASEDATA, m_ob.data(), m_ob.size());
}


// 发送请求捐卡提示
void CClan::sendRequestTip(DWORD dwPDBID)
{
    if (dwPDBID == INVALID_PDBID)
    {
        return;
    }

    obuf obuf;
    fillClanData(obuf, MSG_ENDPOINT_CLIENT, MSG_CLAN_ADD_REQUEST_TIP);
    msg_clan_addrequesttip_oc sendData;
    sendData.nIsInit = 1;
    sendData.nCount = (WORD)m_RequestCardList.size();
    obuf.push_back(&sendData, sizeof(sendData));
    g_EHelper.sendDataToPerson(dwPDBID, obuf.data(), obuf.size());
}

void CClan::enterOfflineState()
{
	SClanConfig* pConfig = g_EHelper.getConfig();
	if (pConfig == NULL)
		return;

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
        return;
    SKinMember kinMember = pKinService->getKinMemberInfo(m_nShaikhID);
    if (kinMember.dwPDBID == 0)
        return;

    DWORD dwInterval = time(NULL) - kinMember.nLogoutTime;
    DWORD offlineSecs = (pConfig->nOfflineDays > 0 ? pConfig->nOfflineDays : 0) * 3600 * 24;
    DWORD outgoingSecs = (pConfig->nOutgoingDays > 0 ? pConfig->nOutgoingDays : 0) * 3600 * 24;
    if (dwInterval >= offlineSecs + outgoingSecs)
    {
        m_ShaikhState = ClanShaikhState_Reigning;
        executeOutgoing();
    }
    else if (dwInterval >= offlineSecs)
    {
        m_ShaikhState = ClanShaikhState_Outgoing;
    }
    else
    {
        m_ShaikhState = ClanShaikhState_Offline;
    }
}

// 盟主自动卸任
void CClan::executeOutgoing()
{
	m_ShaikhState = ClanShaikhState_Reigning;
	SKinMember kinMember;
	if (chooseNewShaikh(kinMember))
	{
        char szText[256] = { 0 };
        sprintf_s(szText, sizeof(szText), a2utf8("%s由于长时间不在线，系统自动将盟主职位交与%s"), m_szShaikhName, kinMember.szName);
		setShaikh(kinMember.dwPDBID, szText);
	}
}

/*
* 盟主卸任规则：
* 盟主位置会给与联盟内的单人周捐献最多的战队长， 如果捐献一致，则使用战队周活跃读最高的战队队长，
* 如果周活跃度一致，则随机选择
*/
bool CClan::chooseNewShaikh(SKinMember & kinMemberOut)
{
	KinHelper helper;
	IKinService *pKinService = helper.m_ptr;
	if (pKinService == NULL)
		return false;

    SClanConfig* pConfig = g_EHelper.getConfig();
    if (pConfig == NULL)
        return false;

    DWORD offlineSecs = (pConfig->nOfflineDays > 0 ? pConfig->nOfflineDays : 0) * 3600 * 24;
    DWORD outgoingSecs = (pConfig->nOutgoingDays > 0 ? pConfig->nOutgoingDays : 0) * 3600 * 24;

	// 1. 找出联盟所有战队长列表（除去盟主所在战队）
	vector<SKinMember> kinMemberList;
	int maxDonate = -1;
	for (list<int>::iterator iter = m_nMemberIDList.begin(); iter != m_nMemberIDList.end(); ++iter)
	{
		SKinInfo kin = pKinService->getKinInfo(*iter);
		if (kin.nKinProp_ID != 0 && kin.nKinProp_ShaikhID != m_nShaikhID)
		{
			SKinMember kinMember = pKinService->getKinMemberInfo(kin.nKinProp_ShaikhID);
            if (kinMember.dwPDBID == 0)
                continue;
            DWORD dwInterval = time(NULL) - kinMember.nLogoutTime;
            if (dwInterval >= offlineSecs + outgoingSecs)
            {
                continue;
            }

			if (kinMember.nClanCtrb > maxDonate)
			{
				maxDonate = kinMember.nClanCtrb;
			}

			kinMemberList.push_back(kinMember);
		}
	}

    if (maxDonate < 0)
        return false;

	// 2. 找出捐献最多的战队长
	vector<SKinMember> maxDonateKinMemberList;
	for (vector<SKinMember>::iterator it = kinMemberList.begin(); it != kinMemberList.end(); ++it)
	{
		if (it->nClanCtrb == maxDonate)
		{
			maxDonateKinMemberList.push_back(*it);
		}
	}

	if (maxDonateKinMemberList.empty())
	{
		TraceLn("not find maxDonate Kin Shaikh.clanID=" << m_dwID << ", clanName=" << m_szName);
		return false;
	}

	if (maxDonateKinMemberList.size() == 1)
	{
		kinMemberOut = maxDonateKinMemberList.at(0);
		return true;
	}

	// 3. 在捐献最多的战队长列表里，选择所在战队周活跃度最高的战队长
	int maxWeekActivity = -1;
	for (vector<SKinMember>::iterator it = maxDonateKinMemberList.begin(); it != maxDonateKinMemberList.end(); ++it)
	{
		int weekActivity = pKinService->getWeekActivity(it->dwKinID);
		if (weekActivity > maxWeekActivity)
		{
			maxWeekActivity = weekActivity;
		}
	}

    if (maxWeekActivity < 0)
        return false;

	vector<SKinMember> maxWeekActivityKinList;
	for (vector<SKinMember>::iterator it = maxDonateKinMemberList.begin(); it != maxDonateKinMemberList.end(); ++it)
	{
		int weekActivity = pKinService->getWeekActivity(it->dwKinID);
		if (weekActivity == maxWeekActivity)
		{
			maxWeekActivityKinList.push_back(*it);
		}
	}

	if (maxWeekActivityKinList.empty())
	{
        return false;
	}

	int index;
	if (maxWeekActivityKinList.size() == 1)
	{
		index = 0;
	}
	else
	{
		// 4. 在相同周活跃度列表里随机选择
		index = rkt::mt_range_rand(0, maxWeekActivityKinList.size() - 1);
	}

	kinMemberOut = maxWeekActivityKinList.at(index);

    TraceLn(__FUNCTION__ << "maxWeekActivity=" << maxWeekActivity << ",count=" << maxWeekActivityKinList.size() << ",newShaikh(actorID=" << kinMemberOut.dwPDBID << ",actorName=" << kinMemberOut.szName << ")");

	return true;
}

// 检测盟主状态
void CClan::CheckShaikhState()
{
    KinHelper kinHelper;
    SKinMember kinMember = kinHelper.m_ptr->getKinMemberInfo(m_nShaikhID);
    if (kinMember.dwPDBID == m_nShaikhID && kinMember.bIsOnline)
    {
        return;
    }

    enterOfflineState();
}

void CClan::recCreateLegendCupEvent(LPCSTR pCupName)
{
    // 添加联盟事件记录
    SClanEvent newEvent;
    newEvent.nTime = (int)time(NULL);
    ssprintf_s(newEvent.szCoutext, sizeof(newEvent.szCoutext), a2utf8("盟主[%s]创建了联盟杯赛[%s]！"),m_szShaikhName, pCupName);
    addClanEvent(newEvent);
}

void CClan::resetClanLegendDidaSend()
{
    m_bClanLegendDidaSend = false;
}
