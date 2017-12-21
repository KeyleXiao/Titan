/*******************************************************************
** 文件名:	Mentorship.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	12/2/2016
** 版  本:	1.0
** 描  述:	师徒对象类
********************************************************************/

#ifndef __MENTORSHIP_H__
#define __MENTORSHIP_H__

#include "IMentorshipClient.h"
#include "MentorshipDef.h"
#include "GameViewDef.h"

// 帮会对象类
class CMentorship
{
public:
	// 构造函数
	CMentorship();

	// 析构函数
	virtual ~CMentorship();
	
	// 创建
	bool create(void);
	// 释放
	void release(void);
	// 清空所有数据
	void reset(void);
	// 是否有效
	bool isVaild(void);

	// 取得帮会的整型数据
	int getNumProp(DWORD dwPropID);
	// 设置帮会的整型数据
	bool setNumProp(DWORD dwPropID, int nValue);
	// 获取师徒职务
	BYTE getMentorshipPost();
	// 获取正式个数
	int getFormalCount(BYTE nPost);
	// 获取见习个数
	int getInternCount(BYTE nPost);

	// 更新数据到显示
	void updatePersonDataToView();

	// 更新基础数据到显示
	void updatePersonBaseDataToView();

	// 更新师徒数据到显示
	void updatePersonShipToView();

	// 移除师徒数据到显示
	void removePersonShipToView(DWORD dwMentorshipID);

    ////////////////////////IMessageHandler////////////////////
	// 消息处理
	void onMessage(SNetMsgHead *pHead, void *pData, size_t nLen);

private:
	// 请求师徒数据返回处理
	void onMentorshipPersonData(LPCSTR pszMsg, int nLen);
	// 师徒个人基础数据
	void onMentorshipPersonBaseData(LPCSTR pszMsg, int nLen);
	// 更新师徒关系数据
	void onUpdateMentorshipData(LPCSTR pszMsg, int nLen);
	// 添加师徒关系数据
	void onAddMentorshipData(LPCSTR pszMsg, int nLen);
	// 删除师徒关系数据
	void onRemoveMentorshipData(LPCSTR pszMsg, int nLen);
	// 师徒成员上下线
	void onMemberLoginAndLogout(LPCSTR pszMsg, int nLen);

private:
	// 师徒数据
	DWORD m_dwPDBID;								// 玩家ID
	int   m_nMetorshipPost;							// 关系职位
	int   m_nAutoIssueMFlag;						// 自动发布师傅信息标识
	int   m_nAutoIssuePFlag;						// 自动发布学徒信息标识

	int	  m_nMSumShipVal;							// 导师关系总值
	int	  m_nMWeekShipVal;							// 导师本周关系值
	int   m_nMLastWeekShipVal;						// 导师上周关系值
	int   m_nMLastSVOrder;							// 导师上周关系排名
	int   m_nMRecordTime;							// 导师关系记录时间

	int	  m_nPSumShipVal;							// 学徒关系总值
	int	  m_nPWeekShipVal;							// 学徒本周关系值
	int   m_nPLastWeekShipVal;						// 学徒上周关系值
	int   m_nPLastSVOrder;							// 学徒上周关系排名
	int   m_nPRecordTime;							// 学徒关系记录时间

	int   m_nMFWinAwardPreCount;					// 导师首胜奖励可领取次数
	int   m_nMFWinAwardCount;						// 导师首胜奖励次数
	int   m_nMFWinAwardTime;						// 导师首胜奖励时间

	int   m_nPFWinAwardPreCount;					// 学徒首胜奖励可领取次数
	int   m_nPFWinAwardCount;						// 学徒首胜奖励次数
	int   m_nPFWinAwardTime;						// 学徒首胜奖励时间

	bool  m_bLoad;									// 个人数据是否已加载

	int*  m_propAddress[emMshipPersonProp_NumPropCount];    // 整型数据的地址数组

	// 师徒数据
	vector<SClientMentorShip>	m_MentorshipVec;
};

extern CMentorship* g_pMentorship;

#endif // __MENTORSHIP_H__