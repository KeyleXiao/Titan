#include "stdafx.h"
#include "ClanPropLock.h"

/// purpose: 构造函数
CClanPropLock::CClanPropLock()
{
	memset(m_PropList, 0, sizeof(m_PropList));
}

/// purpose: 虚构函数
CClanPropLock::~CClanPropLock()
{

}

/// purpose: 锁定
//@ param  : nPropID 要锁定的属性ID
//@ return : 解锁的密匙, -1表示失败
DWORD CClanPropLock::lock(int nPropID, char* szRemark)
{
	if (nPropID < 0 || nPropID >= emClanProp_NumPropCount)
	{
		return - 1;
	}

	m_PropList[nPropID].bIsLock = true;
	m_PropList[nPropID].nKey ++;
	if (szRemark != NULL)
	{
		sstrcpyn(m_PropList[nPropID].szRemark, szRemark, 64);
	}

	return m_PropList[nPropID].nKey;
}

/// purpose: 解锁
//@ param  : nPropID 要锁定的属性ID
//@ param  : nKey 解锁的密匙
bool CClanPropLock::unLock(int nPropID, int nKey)
{
	if (nPropID < 0 || nPropID >= emClanProp_NumPropCount)
	{
		return false;
	}

	if (m_PropList[nPropID].nKey != nKey)
	{
		return false;
	}

	m_PropList[nPropID].bIsLock = false;
	sstrcpyn(m_PropList[nPropID].szRemark, "", 64);

	return true;
}

/// purpose: 判断某个属性是否锁定
bool CClanPropLock::isLock(int nPropID)
{
	if (nPropID < 0 || nPropID >= emClanProp_NumPropCount)
	{
		return false;
	}

	return m_PropList[nPropID].bIsLock;
}
