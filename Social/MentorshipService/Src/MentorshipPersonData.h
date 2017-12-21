/*******************************************************************
** 文件名:	e:\Rocket\Server\MentorshipService\Src\MentorshipPersonData.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	易耀辉
** 日  期:	2016/11/28
** 版  本:	1.0
** 描  述:	师徒系统个人数据对象类
** 应  用:	
**************************************1******************************/

#ifndef __MentorshipPersonData_h__
#define __MentorshipPersonData_h__

#include "IMentorshipService.h"
#include "IMentorshipPerson.h"
#include "IGameDatabaseDef.h"
#include "MentorshipService.h"
#include "MentorshipMsgDef.h"

// 战队对象实现类
class CMentorshipPersonData : public IMentorshipPerson
{
public:
	/// purpose: 构造函数
	CMentorshipPersonData();

	/// purpose: 析构函数
	virtual ~CMentorshipPersonData(void);

public:
	/////////////////////////////IMentorshipPerson/////////////////////////////////////////////
	/// purpose: 取得师徒个人的整型数据
	virtual int getNumProp(DWORD dwPropID);

	/////////////////////////////CMentorshipPersonData////////////////////////////////////////////
	//
	void release();

	/// purpose: 获取PDBID
	DWORD getPDBID();

	/// purpose: 设置师徒个人的整型数据
	bool setNumProp(DWORD dwPropID, int nValue, bool bSaveDB = false);

	/// purpose: 覆盖修改 非负数
	void cover_NoNegative(DWORD dwPropID, int nValue);

	/// purpose: 增量修改 非负数
	void increment_NoNegative(DWORD dwPropID, int nValue);

	/// purpose: 设置DB数据信息
	bool setDBDataContext(MentorshipService *pMentorshipService, DBREQ_RESULT_READMENTORSHIPPERSON *pRetData);

	/// purpose: 更新个人师徒数据存数据库
	void updateDataToDB();

	/// purpose: 更新数据通过不同的周
	bool updateDataByDiffWeek();

	/// purpose: 添加师徒值
	void addMentorshipValue(SMsgMentorship_Add_Member_ShipVal_SO *pRecvData);

private:
	// 获取属性对应时间属性
	int getPropToTimeProp(int nProp);

private:
	MentorshipService *m_pMentorshipService;        // 所属师徒服务   

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

	bool  m_bDBPsersonDataFinal;					// DB个人数据是否已加载

	int*  m_propAddress[emMshipPersonProp_NumPropCount];    // 整型数据的地址数组
};

#endif // __MentorshipPersonData_h__