/*******************************************************************
** 文件名: WarCheckBadBehavior.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/6/23
** 版  本: 1.0
** 描  述: 战场检测恶劣行为：挂机,消极游戏等
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once
#include "StdAfx.h"
#include "ITimerService.h"


#define CHECK_BAD_BEHAVIOR_INTERNAL 30*1000					// 检测恶劣行为定时器间隔:30s（单位：ms）
#define CHECK_BOTING_INTERNAL 4								// 挂机行为检测时间:4*30s=120s（单位：ms）

class CWarCheckBadBehavior : public TimerHandler
{
	// 检测恶劣行为定时器
	enum
	{
		ETimerEventID_CheckBotting,			// 检测挂机
	};

public:
	CWarCheckBadBehavior(IEntity* pEntity);
	~CWarCheckBadBehavior();

public:
	//////////////////////TimerHandler///////////////////
	virtual void OnTimer(unsigned long dwTimerID);

public:
	bool Init(int nBottingCount);
	void Release();
	bool onCommand(int cmdid, void * data, size_t len);
	bool onMessage(int msgID, void *data, size_t len);

private:
	void onMessageBottingNotify(void* data, size_t len);
		
private:

	// 保存玩家位置
	void savePlayerLocation();

	// 判断5分钟内玩家是否挂机（位置是否改动过,误差1m）
	bool isBotting();

private:
	// 所属实体
	IEntity *				m_pMaster;

	// 玩家每分钟的位置
	std::deque<Vector3> m_LocationArray;

	// 挂机次数
	int m_nBottingCount;
};