/*******************************************************************
** 文件名:	e:\Rocket\Server\KinServer\Src\Kin.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2016/3/9
** 版  本:	1.0
** 描  述:	战队对象
** 应  用:	
********************************************************************/

#include "StdAfx.h"
#include "Kin.h"
#include "ExternalHelper.h"
#include "KinMember.h"
#include "KinScene.h"
#include "EventDef.h"
#include "IEventEngine.h"
#include "IServerGlobal.h"

/// purpose: 构造函数
CKin::CKin(void)
{
	m_dwReadFlag = 0;
	m_dwID = 0;
	memset(m_szName, 0, sizeof(m_szName));
	memset(m_szShaikhzName, 0, sizeof(m_szShaikhzName));
	m_nState = emKinState_Invaild;
	m_nLevel = 0;
	m_nShaikhID = 0;

	m_nFightCount = 0;
	m_nMoney = 0;
	m_nPlace = 0;
	memset(m_propAddress, 0, sizeof(m_propAddress));
	m_MemberList.clear();
	m_MembertIt = m_MemberList.begin();
	m_nWorldID = 0;
	m_nActivity = 0;
	m_nWeekActivity = 0;
	m_nChangeLand = 0;

	// 成员地址初始化
	m_propAddress[emKinProp_ID] = (int*)&m_dwID;
	m_propAddress[emKinProp_State] = (int*)&m_nState;
	m_propAddress[emKinProp_Level] = &m_nLevel;
	m_propAddress[emKinProp_ShaikhID] = &m_nShaikhID;
	m_propAddress[emKinProp_ReadFlag] = (int*)&m_dwReadFlag;
	m_propAddress[emKinProp_Activity] = (int*)&m_nActivity;
	m_propAddress[emKinProp_WeekActivity] = (int*)&m_nWeekActivity;
	m_propAddress[emKinProp_Place] = (int*)&m_nPlace;
	m_propAddress[emKinProp_WorldID] = (int*)&m_nWorldID;
	m_propAddress[emKinProp_Money] = (int*)&m_nMoney;
	m_propAddress[emKinProp_ChangeLand] = (int*)&m_nChangeLand;
}

/// purpose: 析构函数
CKin::~CKin(void)
{

}


/// purpose: 创建
bool CKin::create(KinScenService *pKinScenService, LPCSTR pszData, size_t nLen)
{
	if (nLen != sizeof(SMsgKin_UpdateKinScene_OS))
	{
        ErrorLn(__FUNCTION__": nLen != sizeof(SMsgKin_UpdateKinScene_OS)");
		return false;
	}
    m_pKinScenService = pKinScenService;

	SMsgKin_UpdateKinScene_OS* pContext = (SMsgKin_UpdateKinScene_OS*)pszData;
	m_dwID = pContext->dwID;
	sstrcpyn(m_szName, pContext->szName, sizeof(m_szName));
	m_nState = pContext->nState;
	m_nLevel = pContext->nLevel;
	m_nActivity = pContext->nActivity;
	m_nWeekActivity = pContext->nWeekActivity;
	m_nPlace = pContext->nPlace;
	m_nShaikhID = pContext->nShaikhID;
	sstrcpyn(m_szShaikhzName, pContext->szShaikhName, sizeof(m_szShaikhzName));
	m_nWorldID = pContext->nWorldID;
	m_nMoney = pContext->nMoney;
	sstrcpyn(m_szLabel, pContext->szKinLabel, sizeof(m_szLabel));

	return true;
}

/// purpose: 关闭
void CKin::release(void)
{
	m_MemberList.clear();

	delete this;
}

/// purpose: 取得战队的整型数据
int CKin::getNumProp(DWORD dwPropID)
{
    if (dwPropID < emKinProp_ID && dwPropID >= emKinProp_NumPropCount)
    {
        return -1;
    }

    if (m_propAddress[dwPropID] == 0)
    {
        return -1;
    }

    return *(m_propAddress[dwPropID]);  
}

/// purpose: 设置战队的整型数据（覆盖型）
//@ param  : dwPropID 战队属性ID
//@ param  : nValue 属性值
//@ param  : bToSo 是否需要同步到社会服
bool CKin::coverNumProp(DWORD dwPropID, int nValue)
{
    if (dwPropID < emKinProp_ID && dwPropID >= emKinProp_NumPropCount)
    {
		return false;
    }

    if (m_propAddress[dwPropID] == 0)
    {
		return false;
    }
    
    int nOldValue = *(m_propAddress[dwPropID]); 
    *(m_propAddress[dwPropID]) = nValue;

    if(dwPropID == emKinProp_ShaikhID && nOldValue != nValue)
    {
        // 发出战队长改变事件
        event_kin_shaikhid_changed evSkIDChanged;
        evSkIDChanged.nOldID = nOldValue;
        evSkIDChanged.nNewID = nValue; 
        gServerGlobal->getEventEngine()->FireExecute(EVENT_KIN_SHAIKHID_CHANGED, SOURCE_TYPE_KIN, m_dwID, LPCSTR(&evSkIDChanged), sizeof(evSkIDChanged));
    }

    return true;
}

/// purpose: 取得战队名字
LPCSTR CKin::getName()
{
	return m_szName;
}

/// purpose: 更新数据
bool CKin::updateData(LPCSTR pszData, size_t nLen)
{
	if (nLen != sizeof(SMsgKin_UpdateKinScene_OS))
	{
		return false;
	}
	SMsgKin_UpdateKinScene_OS* pContext = (SMsgKin_UpdateKinScene_OS*)pszData;
	m_dwID = pContext->dwID;
	sstrcpyn(m_szName, pContext->szName, sizeof(m_szName));
	m_nState = pContext->nState;
	m_nLevel = pContext->nLevel;
	m_nActivity = pContext->nActivity;
	m_nWeekActivity = pContext->nWeekActivity;
	m_nShaikhID = pContext->nShaikhID;
	sstrcpyn(m_szShaikhzName, pContext->szShaikhName, sizeof(m_szShaikhzName));
	m_nPlace = pContext->nPlace;
	m_nMoney = pContext->nMoney;
	sstrcpyn(m_szLabel, pContext->szKinLabel, sizeof(m_szLabel));

	return true;
}

/// purpose: 添加一个成员
void CKin::addMember(DWORD dwMemberID)
{
    list<DWORD>::iterator it = m_MemberList.begin();
    for (; it != m_MemberList.end(); ++ it)
    {
        if (dwMemberID == (*it))
        {
            return;
        }
    }

	m_MemberList.push_back(dwMemberID);
}

/// purpose: 删除一个成员
void CKin::removeMember(DWORD dwMemberID)
{
	m_MemberList.remove(dwMemberID);
}

/// purpose: 取得成员个数
int CKin::getMemberCount()
{
	return (int)(m_MemberList.size());
}

DWORD CKin::getMemberList(PDBID* pReturnArray, DWORD dwArrayMaxSize)
{
    //获取成员列表数量
    DWORD dwArraySize = 0;
    list<DWORD>::iterator iter = m_MemberList.begin();
    for (;iter != m_MemberList.end(); ++iter)
    {
        if ( dwArraySize >= dwArrayMaxSize)
        {
            return dwArrayMaxSize;
        }
        pReturnArray[dwArraySize++] = *iter;
    }

    return dwArraySize;
}