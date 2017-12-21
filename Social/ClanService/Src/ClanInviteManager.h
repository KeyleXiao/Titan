/*******************************************************************
** 文件名:	ClanInviteManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/1/2016	10:37
** 版  本:	1.0
** 描  述:	管理邀请玩家加入帮会的邀请对象
********************************************************************/

#ifndef __CLANINVITEMANAGER_H__
#define __CLANINVITEMANAGER_H__

#include "ClanDef.h"

// 邀请结构
struct SInvite
{
	DWORD dwInvitePDBID;	// 邀请者ID
	DWORD dwInviteClanID;	// 邀请者帮会ID
	DWORD dwAnswerPDBID;	// 邀请者PDBID

    SInvite()
    {
        memset(this, 0, sizeof(*this));
    }
};

struct SClanApply : public SApplyMember
{
	DWORD dwClanID;			// 申请帮会

    SClanApply()
    {
        memset(this, 0, sizeof(*this));
    }
};

class CClan;
// 邀请管理器
class CClanInviteManager
{
public:
	// 构造函数
	CClanInviteManager();

	// 析构函数
	virtual ~CClanInviteManager();

public:
	// 玩家下线处理
	//@ note  : 被邀请者下线，删除邀请关系，通知客户端删除dida和窗口
	void onUserLogout(DWORD dwPDBID);

	// 添加一个邀请对象
	void addInvite(DWORD dwInvitePDBID, DWORD dwInviteClanID, DWORD dwAnswerPDBID);

	// 删除一个邀请对象
	void removeInvite(DWORD dwInvitePDBID, DWORD dwAnswerPDBID);

	// 删除被邀请者ID为dwAnswerPDBID的所有邀请对象
	SInvite* findInvite(DWORD dwInvitePDBID, DWORD dwAnswerPDBID);

	// 添加一个申请对象
	void addApply(CClan *pClan, SClanApply* pKinApply);

	// 删除一个申请对象
	void removeApply(CClan *pClan, DWORD dwKinID);

	// 查找一个申请对象
	SClanApply* findApply(DWORD dwKinID);

private:
	list<SInvite> m_InviteList;			// 邀请对象列表
	list<SClanApply> m_ApplyList;		// 申请对象列表
};

// 全局帮会数据中心对象声明
extern CClanInviteManager g_ClanInviteManager;

#endif // __CLANINVITEMANAGER_H__