//==========================================================================
/**
** 文件名:	e:\Rocket\Server\MentorshipService\Src\Mentorship.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	易耀辉
** 日  期:	2016/11/28
** 版  本:	1.0
** 描述  :  师徒系统师徒关系类
*/
//==========================================================================


#ifndef __Mentorship_H__
#define __Mentorship_H__

class MentorshipService;
#include "MentorshipService.h"
#include "NameLenDef.h"
#include "MentorshipDef.h"
#include "MentorshipMsgDef.h"

class CMentorship
{
public:
	/// purpose: 构造函数
	CMentorship();

	/// purpose: 虚构函数
	virtual ~CMentorship();

public:
    /// purpose: 数据创建
    //@ pDBContext 创建现场（DBREQ_TABLE_MENTORSHIP结构）
    bool createFromDB(MentorshipService *pMentorshipService, DWORD dwMentorshipID, DBREQ_TABLE_MENTORSHIP *pDBContext);

public:
	/// purpose: 获取师徒关系信息
	SMentorshipInfo getMentorshipInfo();

	/// purpose: 获取师徒关系类型
	BYTE getMentorshipRelationType();

	/// purpose: 获取师徒关系状态
	BYTE getMentorshipState();

	/// purpose: 获取师徒关系提升标识
	bool getMentorshipPromoteFlag();

	/// purpose: 获取师徒师傅PDBID
	DWORD getMasterPDBID();

	/// purpose: 获取师徒学徒PDBID
	DWORD getPrenticePDBID();

	/// purpose: 获取师徒师傅名字
	LPCSTR getMasterName();

	/// purpose: 获取师徒学徒名字
	LPCSTR getPrenticeName();

	/// purpose: 获取师徒学徒建立时间
	DWORD getMentorshipBuildTime();

	/// purpose: 获取解散时间
	DWORD getMentorshipSeverTime();

    /// purpose: 师徒数据存数据库
    void updateDataToDB();

	/// purpose: 提升见习学徒为正式
	void promoteInternToFormal();

    /// purpose: 解除师徒关系
    void dismiss();

	/// purpose: 解散师徒消息处理
	void onClientDismiss(DWORD dwPDBID);

	/// purpose: 更新师徒关系到场景
	void updateMentorshipToScene(CSID dwServerID = INVALID_SERVER_ID);

	/// purpose: 更新师傅名字
	void changeMasterName(LPCSTR szName);

	/// purpose: 更新学徒名字
	void changePrenticeName(LPCSTR szName);

private:
	/// purpose: 更新师徒关系到客户端
	void updateMentorshipToClient(DWORD dwPDBID);

	/// purpose: 移除师徒关系到场景
	void removeMentorshipToScene();

	/// purpose: 移除师徒关系到客户端
	void removeMentorshipToClient(DWORD dwPDBID);

	/// purpose: 释放自己
	void release();

private:
    MentorshipService *m_pMentorshipService;        // 所属师徒服务         
	// 师徒数据
	DWORD   m_dwID;									// 关系ID
	DWORD   m_dwMasterPDBID;						// 师傅玩家ID
	char    m_szMasterName[ACTORNAME_MAXSIZE];		// 师傅玩家名
	DWORD   m_dwPrenticePDBID;						// 学徒玩家ID
	char    m_szPrenticeName[ACTORNAME_MAXSIZE];	// 学徒玩家名
	short	m_nFaceID;								// 学徒头像ID
	BYTE    m_nRelationType;						// 关系类型
	BYTE    m_nState;								// 关系状态
	bool	m_bPromoteFlag;							// 提升标识
	DWORD	m_dwBuildTime;							// 创建时间
	DWORD   m_dwSeverPDBID;							// 解除者玩家ID
	DWORD   m_dwSeverTime;							// 解除时间

};

#endif // __Mentorship_H__