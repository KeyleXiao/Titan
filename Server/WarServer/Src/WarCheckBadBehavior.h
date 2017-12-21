/*******************************************************************
** 文件名: WarCheckBadBehavior.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/6/19
** 版  本: 1.0
** 描  述: 战场检测恶劣行为：挂机,消极游戏等
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once

#include "ICheckBadBehaviorMgr.h"

class IWar;

class CWarCheckBadBehavior : public ICheckBadBehaviorMgr, public TimerHandler
{
	struct SBottingInfo
	{
		int nBottingCount;				// 本局比赛挂机次数
		int nLatestEffectiveOptTime;	// 最后一次有效操作时间：输出伤害,治疗队友等
		bool isBotting;					// 是否在挂机

		SBottingInfo()
		{
			memset(this, 0, sizeof(*this));
		}
	};

public:
	CWarCheckBadBehavior(IWar* pWar);
	~CWarCheckBadBehavior();

public:
	//////////////////////TimerHandler///////////////////
	virtual void OnTimer(unsigned long dwTimerID);

public:
	////////////////////////ICheckBadBehaviorMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	virtual void onMessage(BYTE byKeyAction, UID uid, void* data, size_t len);

    virtual void warEndBottingRec();

	////////////////////////ICheckBadBehaviorMgr end/////////////////////////
private:
	// 发送挂机提示到客户端
	void sendBottingTipToClient(UID uid, BYTE byType);

	// 是否已经被判定为挂机惩罚,不用再判断是否挂机
	bool isBottingPunish(UID uid);

	// 客户端挂机次数+1
	void increaseBottingCount(UID uid);

	// 设置玩家是否挂机状态
	void setBottingState(UID uid, bool isBotting);
private:
	// 战场
	IWar*	m_pWar;

	// 玩家是否挂机
	map<UID, bool> m_IsBottingMap;
};