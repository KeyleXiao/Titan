/*******************************************************************
** 文件名: ForceEnd.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/28
** 版  本: 1.0
** 描  述: 战场强制结算
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once
#include "IForceEndMgr.h"

class IWar;
struct SWarForceEndInfo;

//5分钟倒计时（一分钟间隔）
#define ForceEnd5MinTime 60000

//10秒倒计时（一秒间隔）
#define ForceEnd10SecTime 1000

// 强制结算定时器
enum 
{
    ETimerEventID_ForceEnd,					// 强制结束时间
    ETimerEventID_ForceEnd5Min_Start,		// 强制结束5分钟倒计时开始
    ETimerEventID_ForceEnd10Sec_Start,		// 强制结束10秒倒计时开始
    ETimerEventID_ForceEnd5Min,				// 5分钟倒计时
    ETimerEventID_ForceEnd10Sec,			// 10秒倒计时
};

class CForceEnd : public IForceEndMgr, public TimerHandler
{
public:
	CForceEnd(IWar* pWar);
	~CForceEnd();

public:
	//////////////////////TimerHandler///////////////////
	virtual void OnTimer(unsigned long dwTimerID);

public:
	////////////////////////IForceEndMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();
	////////////////////////IForceEndMgr end/////////////////////////

private:
	// 计算某个阵营的输赢参考分
	int getScore(int nCamp);

	// 获取当前阵营的击杀人头
	int getCampKillNum(int nCamp);

	// 获取当前阵营的推塔数
	int getCampDestroyTowerNum(int nCamp);

	// 获取敌方阵营
	int getEnemyCamp(int nCamp);

	// 强制结算处理
	void onForceEnd();
private:

	// 战场
	IWar*	m_pWar;

	// 强制结算信息
	SWarForceEndInfo* m_pForceEndInfo;

	// 当前5分钟倒计时剩余数
	int m_nCurRemain5Min;

	// 当前10秒倒计时剩余数
	int m_nCurRemain10Sec;
};