//==========================================================================
/**
** 文件名:	e:\Rocket\Server\KinService\Src\KinInviteManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2009/3/9
** 版  本:	1.0
** 描述 : 邀请管理器  管理邀请玩家加入战队的邀请对象
*/
//==========================================================================

#ifndef __KinInviteManager_H__
#define __KinInviteManager_H__

#include "NameLenDef.h"
#include "KinService.h"

// 邀请结构
struct SKinInvite
{
	DWORD dwInvitePDBID;					// 邀请者ID
	DWORD dwInviteKinID;					// 邀请者战队ID
	DWORD dwAnswerPDBID;					// 邀请者战队ID
};

// 申请结构结构
struct SKinApply
{
    DWORD dwUDBID;                          // 申请者账号ID
	DWORD dwApplyPDBID;						// 申请者ID
	char  szApplyName[ACTORNAME_MAXSIZE];	// 申请者名字
	DWORD dwApplyKinID;						// 申请战队ID
	BYTE  nLevel;							// 玩家等级
	int   nVocation;						// 玩家职业
	BYTE  nNation;							// 国家
	char  szWord[128];						// 玩家留言
    int   nFaceID;                          // 头像ID
    BYTE  nSex;                             // 性别

	SKinApply()
	{
		memset(this, 0, sizeof(*this));
	}	
};


// 邀请管理器
class CKinInviteManager
{
public:
	/// purpose: 构造函数
	CKinInviteManager();

	/// purpose: 虚构函数
	virtual ~CKinInviteManager();

public:
    /// purpose: 玩家下线处理
    //@ note  : 被邀请者下线，删除邀请关系，通知客户端删除dida和窗口
    void setKinService(KinService *pKinService);

	/// purpose: 玩家下线处理
	//@ note  : 被邀请者下线，删除邀请关系，通知客户端删除dida和窗口
	void OnUserLogout(DWORD uPDBID);

	/// purpose: 添加一个邀请对象
	//@ param  : dwInvitePDBID 邀请者ID
	//@ param  : dwAnswerPDBID 被邀请者ID
	void AddInvite(DWORD dwInvitePDBID, DWORD dwInviteKinID, DWORD dwAnswerPDBID);

	/// purpose: 删除一个邀请对象
	//@ param  : dwInvitePDBID 邀请者ID
	//@ param  : dwAnswerPDBID 被邀请者ID
	void RemoveInvite(DWORD dwInvitePDBID, DWORD dwAnswerPDBID);

	/// purpose: 查找一个邀请对象
	//@ param  : dwInvitePDBID 邀请者ID
	//@ param  : dwAnswerPDBID 被邀请者ID
	SKinInvite* FindInvite(DWORD dwInvitePDBID, DWORD dwAnswerPDBID);

	/// purpose: 添加一个申请对象
	void AddApply(SKinApply* pKinApply);

	/// purpose: 删除一个角色所有申请对象的战队请求
	//@ param  : dwApplyPDBID 申请者ID
	void RemoveApply(DWORD dwApplyPDBID);

	/// purpose: 删除一个申请对象
	//@ param  : dwApplyPDBID 申请者ID
	void RemoveApply(DWORD dwApplyPDBID, DWORD dwKinID);

	/// purpose: 查找一个角色申请的战队数量
	//@ param  : dwKinID 战队ID
	int GetApplyKinCount(DWORD dwKinID);

	/// purpose: 查找一个申请对象
	//@ param  : dwApplyPDBID 申请者ID
	//@ param  : dwKinID 战队ID
	SKinApply* FindApply(DWORD dwApplyPDBID, DWORD dwKinID);

private:
	list<SKinInvite> m_InviteList;		// 邀请对象列表
	list<SKinApply>  m_ApplyList;		// 申请对象列表
    KinService      *m_pKinService;      // 战队服务对象
};

// 全局战队数据中心对象声明
extern CKinInviteManager g_KinInviteManager;

#endif // __KinInviteManager_H__