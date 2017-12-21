/*******************************************************************
** 文件名:	ClanPropLock.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/1/2016	10:37
** 版  本:	1.0
** 描  述:	帮会属性加锁类
********************************************************************/

#ifndef __CLANPROPLOCK_H__
#define __CLANPROPLOCK_H__

#include "ClanDef.h"

// 属性锁信息
struct SPropLockInfo
{
	bool bIsLock;	// 是否锁定
	int nKey;		// 密匙
	char szRemark[64];

    SPropLockInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};

// (目前还没看明白其存在意义，确定无用后就删)
// 帮会属性加锁类
class CClanPropLock
{
public:
	/// purpose: 构造函数
	CClanPropLock();

	/// purpose: 虚构函数
	virtual ~CClanPropLock();

	/// purpose: 锁定
	//@ param  : nPropID 要锁定的属性ID
	//@ return : 解锁的密匙， -1表示失败
	DWORD lock(int nPropID, char* szRemark);

	/// purpose: 解锁
	//@ param  : nPropID 要锁定的属性ID
	//@ param  : nKey 解锁的密匙
	bool unLock(int nPropID, int nKey);
	
	/// purpose: 判断某个属性是否锁定
	bool isLock(int nPropID);

protected:
	SPropLockInfo m_PropList[emClanProp_NumPropCount];		// 属性的加锁信息
};


#endif // __CLANPROPLOCK_H__