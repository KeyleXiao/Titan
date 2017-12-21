/*******************************************************************
** 文件名: DevilKingMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/29
** 版  本: 1.0
** 描  述: 大逃杀-战场魔王玩法
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once

#include "IDevilKingMgr.h"

class IWar;

class CDevilKingMgr : public IDevilKingMgr, public TimerHandler, public IEventExecuteSink
{
	// 自动加Buff定时器
	enum ETimerEventID
	{
		ETimerEventID_BcastDevilKingPos,			// 定时广播魔王位置
		ETimerEventID_CloseTime,					// 关闭魔王玩法
	};

	enum ETimerInternal
	{
	};

public:
	CDevilKingMgr(IWar* pWar);
	~CDevilKingMgr();

public:
	//////////////////////TimerHandler///////////////////
	virtual void OnTimer(unsigned long dwTimerID) override;

	///////////////////////////////////////IEventExecuteSink///////////////////////////////////
	/** 容器事件触发后执行函数
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

public:
	////////////////////////IDevilKingMgr start/////////////////////////
	virtual bool Init(int nMatchType) override;

	virtual void Release() override;

	// 战场正式开始
	virtual void onWarRealStart() override;

	// 战场结束
	virtual void onWarEnd() override;

	// 玩家击杀了人
	virtual void onPlayerDie() override;

	// 有玩家击杀了人
	virtual void onPlayerKillPerson(UID uidMurder, UID uidDead);

	////////////////////////IDevilKingMgr end/////////////////////////

	// 功能函数
private:
	// 广播魔王小地图视野
	void bcastDevilKingPos();
	// 召唤魔王NPC
	void createDevilKingMonster(UID uidMurder);

	// 事件处理
private:
	// 产生了新的魔王
	void onNewDevilKing(UID uid);

	// 获取配置数据
private:
	// 根据英雄ID获取需要召唤的怪物ID
	int getDevilKingMonsterID(int nHeroID);
	// 获取魔王配置
	SWarDevilKing* getDevilKingScheme();
	// 获取达成魔王击杀人数条件
	int getDevilKingKillNumCond();
	// 获取魔王位置广播间隔
	int getBcastDevilKingPosInternal();
	// 获取魔王玩法关闭时间
	int getCloseTime();

	// 发送消息到客户端
private:
	// 通知客户端当前魔王
	void sendDevilKingToAll();
	// 告诉重连玩家当前魔王
	void sendDevilKingToClient(UID uid);
private:
	// 战场
	IWar*	m_pWar;

	int m_nMatchTypeID;

	// 当前魔王
	UID m_nCurDevilKing;

	// 魔王玩法是否关闭了
	bool m_isClose;

	// 是否已经产生第一个魔王
	bool m_isHasFirstDevilKing;
};