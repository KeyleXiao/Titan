/*******************************************************************
** 文件名: BattleWar_SafetyZone.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/1
** 版  本: 1.0
** 描  述: 大逃杀-安全区/毒气区逻辑
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "GameDef.h"
#include "WarDef_Internal.h"
#include "TimerHandler.h"
#include "SafetyZoneHelper.h"
#include "ISafetyZoneMgr.h"
#include <vector>

class IWar;
struct SWarSafetyZone;

class CBattleWarSafetyZone : public ISafetyZoneMgr, public TimerHandler, public IEventExecuteSink
{
	typedef std::map<UID, Vector3> ACTORLOCMAP;
	typedef ACTORLOCMAP::iterator ACTORLOCMAP_ITER;

	// 定时器ID
	enum TimerID
	{
		TimerID_READY = 0,				// 游戏准备定时器
		TimerID_SPREAD_COUNTDOWN,		// 毒气扩散倒计时
		TimerID_SPREAD_OVER,			// 毒气传播结束
		TimerID_CHECK_INNOX,			// 检测是否在毒气区
		TimerID_SPREAD_COUNTDOWN_TIPS,	// 扩散倒计时提示
	};

	// 定时器间隔时间
	enum TimerInternal
	{
		TimerInternal_READY = 1000 * 120,		// 游戏准备时间，该时间到达后开始出现白色安全圈
		TimerInternal_CHECKINNOX = 1000 * 2,		// 定时检测是否在毒气区
	};

	
public:
	CBattleWarSafetyZone(IWar* pWar);
	~CBattleWarSafetyZone();

public:
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
	////////////////////////ISafetyZoneMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 获取毒圈内随机坐标
	virtual Vector3 getRandomPosOfNoxZone();

	// 获取安全圈内随机坐标
	// @fDistance: 离圆边的距离，默认为0是整个安全圈大小内随机一个点
	virtual Vector3 getRandomPosOfSafetyZone(float fDistance = 0.f);
	////////////////////////ISafetyZoneMgr end/////////////////////////

	// 获取安全圈内随机坐标
	virtual Cicle getSafetyZone();
public:
	////////////////////////////////////TimerHandler/////////////////////////////////////// 
	virtual void OnTimer(unsigned long dwTimerID);

private:
	// 准备阶段结束
	void onReadyOver();

	// 扩散倒计时结束
	void onSpreadCountDownOver();

	// 毒气扩散结束
	void onSpreadOver();

	// 检测玩家是否在毒气区
	void checkActorInNox();

private:
	// 播放毒气扩散倒计时广播提示
	void bcastSpreadCountDownTips(int nCountDownTime);

	// 播放毒气扩散开始广播提示
	void bcastSpreadBeginTipsTips();

private:
	// 填充安全圈消息
	void fillSafetyZoneInfo(obuf& ob);

	// 填充扩散倒计时信息
	void fillNoxSpreadCDInfo(obuf& ob);

	// 向客户端发送安全圈信息
	void sendSafetyZoneInfo2Client();

	// 向客户端发送毒气扩散倒计时信息
	void sendSpreadCDInfo2Client();

private:
	// 计算新的安全区圆心位置
	void calcNewSafetyZoneInfo();

	// 毒气是否在扩散（毒气圈在缩小）
	bool isNoxSpreading();

	// 获取当前轮次的安全区信息
	SWarSafetyZone* getCurRoundSafetyZone();

	// 获取当前毒圈
	Cicle getCurNoxZone();
private:

	// 战场
	IWar*	m_pWar;

	// 开始了
	bool m_isStart;

	// 当前轮数
	int m_nCurRound;

	// 当前毒气圈
	Cicle m_curNoxZone;

	// 当前安全圈
	Cicle m_curSafetyZone;

	// 扩散开始时间
	DWORD m_SpreadBeginTick;

	// 扩散时间
	DWORD m_SpreadTime;

	// 所有玩家坐标
	ACTORLOCMAP m_mapActorLoc;

	// 玩家是否被加Buff
	std::map<UID, bool> m_mapIsAddBuff;

	// 上次广播时间
	int m_LastSpreadTipsTime;
	// 当前广播提示时间下标
	int m_SpreadTipsTimeIndex;

	// 扩散倒计时开始的Tick数
	DWORD m_dwSpreadCDTick;
	// 扩散倒计时总时间
	DWORD m_dwSpreadCDTotalTime;

	// 最后一轮毒气是否扩散完成
	bool m_isAllOver;
};