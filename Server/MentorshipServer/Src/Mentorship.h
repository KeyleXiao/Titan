/*******************************************************************
** 文件名:	Mentorship.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/29/2016
** 版  本:	1.0
** 描  述:	师徒对象类
********************************************************************/

#ifndef __MENTORSHIP_H__
#define __MENTORSHIP_H__

#include "IMentorshipScene.h"
#include "MentorshipMsgDef.h"

// 师徒对象实现类
class CMentorship
{
public:
	// 构造函数
	CMentorship(void);

	// 析构函数
	~CMentorship(void);

public:
	// 创建
	bool create(DWORD dwMentorshipID, SMsgMentorship_UpdateMentorship_OS *pContext);

	// 关闭
	void release(void);

	// 更新数据
	bool updateData(SMsgMentorship_UpdateMentorship_OS *pContext);

	// 获取ID
	DWORD getMentorshipID();

	// 获取导师PDBID
	DWORD getMasterPDBID();

	// 获取导师PDBID
	DWORD getPrenticePDBID();

private:
	// 师徒数据
	DWORD	m_dwMentorshipID;
	DWORD   m_dwMasterPDBID;						// 师傅玩家ID
	DWORD   m_dwPrenticePDBID;						// 学徒玩家ID
	BYTE    m_nRelationType;						// 关系类型
	BYTE    m_nState;								// 关系状态
	bool	m_bPromoteFlag;							// 提升标识
	DWORD	m_dwBuildTime;							// 创建时间
	DWORD   m_dwSeverPDBID;							// 解除者玩家ID
	DWORD   m_dwSeverTime;							// 解除时间
};

#endif // __MENTORSHIP_H__