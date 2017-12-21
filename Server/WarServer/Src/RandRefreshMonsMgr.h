/*******************************************************************
** 文件名: RandomRefreshMons.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/15
** 版  本: 1.0
** 描  述: 战场随机刷怪接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IRandRefreshMonsMgr.h"
#include "TimerHandler.h"

class IWar;
struct SWarRandRefreshMons;

class CRandRefreshMonsMgr : public IRandRefreshMonsMgr, TimerHandler
{
	enum ETimerID
	{
		ETimerID_RefreshMonster = 0,	// 刷怪检测定时器
		ETimerID_MonsterLifeOver,		// 怪物生命是否结束检测定时器
	};

	enum ETimerInternal
	{
		ETimerInternal_RefreshMonster = 1000,    // 刷怪检测间隔
		ETimerInternal_MonsterLifeOver = 1000,   // 怪物生命是否结束检测定时器
	};

	// 每组怪物的刷怪信息
	struct SMonsterRandRefreshInfo
	{
		// 刷新的信息
		//SWarRandRefreshMons RandRefreshInfo;

		// 具体的怪物信息
		map<int, WarPointVect> mapWarPointInfo;

		// 创建的怪物列表<UID,LocalIndex>
		map<UID, int> createdMonUIDList;

		// WarPoint.csv的类型ID（每组怪物唯一）
		int nRefreshIndex;

		// 刷新时候的Tick时间（毫秒）
		DWORD dwRefreshTick;

		// 该组怪物是否已经刷新
		bool isRefresh;

		SMonsterRandRefreshInfo()
		{
			nRefreshIndex = 0;
			dwRefreshTick = 0;
			isRefresh = false;
		}
	};

	typedef std::map<int, SMonsterRandRefreshInfo> MonsterRandRefreshMap;

public:
	CRandRefreshMonsMgr(IWar* pWar);
	~CRandRefreshMonsMgr();

	////////////////////////////////////////IRandRefreshMonsMgr Start////////////////////////////
public:
	// 初始化
	virtual bool Init(int nMatchType);

	// 释放
	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 停止怪物AI
	virtual void stopAI();

	// 释放怪物
	virtual void releaseMonsterRes();

	////////////////////////////////////////IRandRefreshMonsMgr End////////////////////////////

	///////////////////////////////////////////////TimerHandler Start//////////////////////////////
	virtual void OnTimer(unsigned long dwTimerID);

	///////////////////////////////////////////////TimerHandler Stop//////////////////////////////

private:
	// 创建怪物
	void createMonster(SWarRandRefreshMons* randRefreshMonsInfo);

	// 检测是否刷怪
	void checkRefreshMonster();

	// 检测怪物生命周期
	void checkMonsterLife();



private:
	IWar* m_pWar;

	int m_nWarType;

	// 随机刷怪信息列表<nRefreshIndex,SMonsterRandRefreshInfo>
	MonsterRandRefreshMap m_RandRefreshInfoList;
};