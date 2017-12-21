//==========================================================================
/**
** 文件名:	e:\Rocket\Server\MentorshipService\Src\MentorshipIssueManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	易耀辉
** 日  期:	2016/11/25
** 版  本:	1.0
** 描述 : 师徒发布信息管理器 
*/
//==========================================================================

#ifndef __MentorshipIssueManager_H__
#define __MentorshipIssueManager_H__

#include "MentorshipDef.h"
#include "MentorshipMsgDef.h"

// 师徒发布信息管理器
class CMentorshipIssueManager
{
// 发布信息列表
typedef stdext::hash_map<DWORD, SMentorshipIssue>					TMap_MentorshipIssue;
// 发布信息列表迭代器
typedef stdext::hash_map<DWORD, SMentorshipIssue>::iterator			TMap_MentorshipIssueIt;

public:
	/// purpose: 构造函数
	CMentorshipIssueManager();

	/// purpose: 虚构函数
	virtual ~CMentorshipIssueManager();

public:
	/// purpose: 添加一个师傅发布信息对象
	void addMasterIssue(SMentorshipIssue* pMasterIssue);

	/// purpose: 添加一个学徒发布信息对象
	void addPrenticeIssue(SMentorshipIssue* pPrenticeIssue);

	/// purpose: 删除一个师傅发布信息对象
	//@ param  : dwPDBID 玩家ID
	void removeMasterIssue(DWORD dwPDBID);

	/// purpose: 删除一个学徒发布信息对象
	//@ param  : dwPDBID 玩家ID
	void removePrenticeIssue(DWORD dwPDBID);

	/// purpose: 查找一个师傅发布信息对象
	//@ param  : dwPDBID 玩家ID
	SMentorshipIssue* findMasterIssue(DWORD dwPDBID);

	/// purpose: 查找一个学徒发布信息对象
	//@ param  : dwPDBID 玩家ID
	SMentorshipIssue* findPrenticeIssue(DWORD dwPDBID);

	/// purpose: 添加一个上线对象
	//@ param  : dwPDBID 玩家ID
	void addIssueOnlineMember(DWORD dwPDBID);

	/// purpose: 添加师傅上线对象
	//@ param  : dwPDBID 玩家ID
	void addMIssueOnlineMember(DWORD dwPDBID);

	/// purpose: 添加学徒上线对象
	//@ param  : dwPDBID 玩家ID
	void addPIssueOnlineMember(DWORD dwPDBID);

	/// purpose: 移除一个上线对象
	//@ param  : dwPDBID 玩家ID
	void removeIssueOnlineMember(DWORD dwPDBID);

	/// purpose: 移除师傅上线对象
	//@ param  : dwPDBID 玩家ID
	void removeMIssueOnlineMember(DWORD dwPDBID);

	/// purpose: 移除学徒上线对象
	//@ param  : dwPDBID 玩家ID
	void removePIssueOnlineMember(DWORD dwPDBID);

	/// purpose: 发送发布信息列表To客户端
	//@ param  : dwPDBID 玩家ID, 信息类型
	void sendIssueListToClient(DWORD dwPDBID, BYTE nIssueType);

private:
	/// purpose: 填充发布节点数据
	//@ param  : 节点
	bool fillMentorshipIssueNode(DWORD dwPDBID, SMsgMentorship_Issue_Node* pIssueNode);

private:
	TMap_MentorshipIssue	m_MasterIssueList;		// 师傅发布信息对象列表
	TMap_MentorshipIssue	m_PrenticeIssueList;	// 学徒发布信息对象列表

	vector<DWORD>			m_MIssueOnlineList;			// 师傅发布信息玩家在线列表
	vector<DWORD>			m_PIssueOnlineList;			// 学徒发布信息玩家在线列表
};

// 全局师徒发布信息管理器对象声明
extern CMentorshipIssueManager g_MentorshipIssueManager;

#endif // __MentorshipIssueManager_H__