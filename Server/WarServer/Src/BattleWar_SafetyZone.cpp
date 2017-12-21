/*******************************************************************
** 文件名: BattleWar_SafetyZone.cpp
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

#include "StdAfx.h"
#include "GameDef.h"
#include "BattleWar_SafetyZone.h"
#include "BattleWar.h"
#include "SWPerson.h"
#include "WarLogRecord.h"
#include "ExternalHelper.h"
#include "ISchemeCenter.h"
#include "Vector3.h"

CBattleWarSafetyZone::CBattleWarSafetyZone(IWar * pWar)
	: m_pWar(pWar)
	, m_isStart(false)
	, m_nCurRound(0)
	, m_SpreadBeginTick(0)
	, m_SpreadTime(0)
	, m_SpreadTipsTimeIndex(0)
	, m_LastSpreadTipsTime(0)
	, m_dwSpreadCDTick(0)
	, m_dwSpreadCDTotalTime(0)
	, m_isAllOver(false)
{
}

CBattleWarSafetyZone::~CBattleWarSafetyZone()
{
	m_pWar = NULL;
}

void CBattleWarSafetyZone::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_GAME_ACTOR_CONTINUE_PLAY:
		{
			if (m_isStart == false)
				return;

			if (nLen != sizeof(event_entity_continue_play))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_continue_play) !" << ", nLen = " << nLen);
				return;
			}
			event_entity_continue_play* pEvent = (event_entity_continue_play*)pszContext;

			ActorHelper actorHelper(pEvent->dwDBID, ACTOR_DBID);
			if (actorHelper.m_ptr == NULL)
				return;

			UID uid = actorHelper.m_ptr->getUID();

			obuf ob1;
			fillNoxSpreadCDInfo(ob1);
			g_EHelper.sendMessageToClientWar(uid, SC_MSG_WAR_SAFETYZONE_SPREAD_CD, (LPCSTR)ob1.data(), ob1.size());

			obuf ob2;
			fillSafetyZoneInfo(ob2);
			g_EHelper.sendMessageToClientWar(uid, SC_MSG_WAR_SAFETYZONE_INFO, (LPCSTR)ob2.data(), ob2.size());

			WarningLn(__FUNCTION__":Continue player.");
		}
	}
}

bool CBattleWarSafetyZone::Init(int nMatchtype)
{
	if (m_pWar == NULL)
		return false;

	g_EHelper.Subscribe(m_pWar->getWarSceneID(), this, EVENT_GAME_ACTOR_CONTINUE_PLAY, TYPE_PLAYER_ROLE, 0, __FUNCTION__);

	return true;
}

void CBattleWarSafetyZone::Release()
{
	g_EHelper.KillTimer(TimerID_READY, this);
	g_EHelper.KillTimer(TimerID_CHECK_INNOX, this);
	g_EHelper.KillTimer(TimerID_SPREAD_OVER, this);
	g_EHelper.KillTimer(TimerID_SPREAD_COUNTDOWN, this);
	g_EHelper.KillTimer(TimerID_SPREAD_COUNTDOWN_TIPS, this);
	
	g_EHelper.UnSubscribe(m_pWar->getWarSceneID(), this, EVENT_GAME_ACTOR_CONTINUE_PLAY, TYPE_PLAYER_ROLE, 0);

	delete this;
}

void CBattleWarSafetyZone::onWarRealStart()
{
	// 设置准备时间定时器
	g_EHelper.SetTimer(TimerID_READY, TimerInternal_READY, dynamic_cast<TimerHandler*>(this), 1, "CBattleWarSafetyZone set timer.");
	// 设置定时检测是否在毒气区
	g_EHelper.SetTimer(TimerID_CHECK_INNOX, TimerInternal_CHECKINNOX, dynamic_cast<TimerHandler*>(this), INFINITY_CALL, "CBattleWarSafetyZone set timer.");

	// 当前轮数
	m_nCurRound = 1;

	// 设置毒气圈的初始圆心和半径
	SWarSafetyZone* pWarSafetyZone = g_EHelper.getWarSafetyZoneInfo(m_nCurRound);
	if (pWarSafetyZone == NULL)
		return ;
	m_curNoxZone.center = pWarSafetyZone->centerOfNoxiousZone;
	m_curNoxZone.radius = pWarSafetyZone->fRadiusOfNoxiousZone;


}

void CBattleWarSafetyZone::onWarEnd()
{
	g_EHelper.KillTimer(TimerID_READY, this);
	g_EHelper.KillTimer(TimerID_CHECK_INNOX, this);
	g_EHelper.KillTimer(TimerID_SPREAD_OVER, this);
	g_EHelper.KillTimer(TimerID_SPREAD_COUNTDOWN, this);
	g_EHelper.KillTimer(TimerID_SPREAD_COUNTDOWN_TIPS, this);
}

Vector3 CBattleWarSafetyZone::getRandomPosOfNoxZone()
{
	Cicle curNoxZone = getCurNoxZone();

	return CMathHelper::getRandomPosOfCicle(curNoxZone);
}

Vector3 CBattleWarSafetyZone::getRandomPosOfSafetyZone(float fDistance)
{
	Cicle curSafetyZone = m_curSafetyZone;

	return CMathHelper::getRandomPosOfCicle(curSafetyZone, fDistance);
}

Cicle CBattleWarSafetyZone::getSafetyZone()
{
	return m_curSafetyZone;
}

void CBattleWarSafetyZone::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case TimerID_READY:  // 准备阶段过后
		{
			onReadyOver();
		}
		break;
	case TimerID_SPREAD_COUNTDOWN:   // 毒气扩散倒计时到达后
		{
			onSpreadCountDownOver();
		}
		break;
	case TimerID_SPREAD_OVER:   // 毒气扩散结束
		{
			onSpreadOver();
		}
		break;
	case TimerID_CHECK_INNOX:  // 检测是否在毒气区
		{
			checkActorInNox();
		}
		break;
	case TimerID_SPREAD_COUNTDOWN_TIPS:
		{
			SWarSafetyZone* pWarSafetyZone = getCurRoundSafetyZone();
			if (pWarSafetyZone != NULL)
			{
				m_LastSpreadTipsTime = pWarSafetyZone->nSpreadTipsTime[m_SpreadTipsTimeIndex];
				// 播放提示广播
				bcastSpreadCountDownTips(pWarSafetyZone->nSpreadTipsTime[m_SpreadTipsTimeIndex]);

				m_SpreadTipsTimeIndex++;
				if (m_SpreadTipsTimeIndex < pWarSafetyZone->nSpreadTipsTime.size())
				{
					// 设置新的提示广播定时器
					int nInternal = m_LastSpreadTipsTime - pWarSafetyZone->nSpreadTipsTime[m_SpreadTipsTimeIndex];
					g_EHelper.SetTimer(TimerID_SPREAD_COUNTDOWN_TIPS, nInternal, this, 1);
				}
			}
		}
		break;
	default:
		{
			ErrorLn(__FUNCTION__":Unkonwn Timer, timerID=" << dwTimerID);
		}
	}
}

void CBattleWarSafetyZone::onReadyOver()
{
	m_SpreadBeginTick = 0;
	m_SpreadTime = 0;

	// 计算第一个安全圈位置
	calcNewSafetyZoneInfo();

	// 向客户端发送第一个安全圈位置信息
	sendSafetyZoneInfo2Client();

	SWarSafetyZone* pWarSafetyZone = getCurRoundSafetyZone();
	if (pWarSafetyZone != NULL)
	{
		m_dwSpreadCDTick = gServerGlobal->getTimeSyncService()->GetTick();
		m_dwSpreadCDTotalTime = pWarSafetyZone->nSpreadCountdown;

		// 设置毒气扩散倒计时定时器
		g_EHelper.SetTimer(TimerID_SPREAD_COUNTDOWN, pWarSafetyZone->nSpreadCountdown, this, 1);

		// 播放毒气扩散倒计时广播（即毒气圈开始缩放倒计时）
		bcastSpreadCountDownTips(pWarSafetyZone->nSpreadCountdown);

		// 通知客户端扩散倒计时信息
		sendSpreadCDInfo2Client();

		// 设置扩散提示广播定时器
		m_SpreadTipsTimeIndex = 0;
		if (pWarSafetyZone->nSpreadTipsTime.empty() == false)
		{
			m_LastSpreadTipsTime = pWarSafetyZone->nSpreadCountdown;
			g_EHelper.SetTimer(TimerID_SPREAD_COUNTDOWN_TIPS, m_LastSpreadTipsTime - pWarSafetyZone->nSpreadTipsTime[m_SpreadTipsTimeIndex], this, 1);
		}
	}

	m_isStart = true;
}

void CBattleWarSafetyZone::onSpreadCountDownOver()
{
	// 毒气扩散倒计时到达后开始扩散毒气（缩小毒气圈）
	m_SpreadBeginTick = gServerGlobal->getTimeSyncService()->GetTick();

	// 通知客户端毒气开始扩散（播放毒气圈缩小动画）
	sendSafetyZoneInfo2Client();

	// 播放毒气开始扩散广播
	bcastSpreadBeginTipsTips();

	SWarSafetyZone* pWarSafetyZone = getCurRoundSafetyZone();
	if (pWarSafetyZone != NULL)
	{
		// 设置毒气扩散结束定时器
		g_EHelper.SetTimer(TimerID_SPREAD_OVER, pWarSafetyZone->nSpreadTime, this, 1);
	}
}

void CBattleWarSafetyZone::onSpreadOver()
{
	// 毒气扩散完成后（毒气圈和安全圈一样大小）
	m_SpreadBeginTick = 0;
	m_SpreadTime = 0;

	// 设置新的毒气圈圆心和位置（和现在的安全圈一样）
	m_curNoxZone = m_curSafetyZone;

	// 轮数+1
	m_nCurRound++;

	SWarSafetyZone* pWarSafetyZone = getCurRoundSafetyZone();
	if (pWarSafetyZone == NULL)
	{
		m_isAllOver = true;
		m_nCurRound--;	// 回滚到上一次有效轮数
		return;
	}

	// 计算新的安全圈位置
	calcNewSafetyZoneInfo();

	// 通知客户端新的安全圈位置信息
	sendSafetyZoneInfo2Client();

	m_dwSpreadCDTick = gServerGlobal->getTimeSyncService()->GetTick();
	m_dwSpreadCDTotalTime = pWarSafetyZone->nSpreadCountdown;

	// 设置毒气扩散倒计时定时器
	g_EHelper.SetTimer(TimerID_SPREAD_COUNTDOWN, pWarSafetyZone->nSpreadCountdown, this, 1);

	// 播放毒气扩散倒计时广播（即毒气圈开始缩放倒计时）
	bcastSpreadCountDownTips(pWarSafetyZone->nSpreadCountdown);

	// 通知客户端扩散倒计时信息
	sendSpreadCDInfo2Client();

	// 设置扩散提示广播定时器
	m_SpreadTipsTimeIndex = 0;
	if (pWarSafetyZone->nSpreadTipsTime.empty() == false)
	{
		m_LastSpreadTipsTime = pWarSafetyZone->nSpreadCountdown;
		g_EHelper.SetTimer(TimerID_SPREAD_COUNTDOWN_TIPS, m_LastSpreadTipsTime - pWarSafetyZone->nSpreadTipsTime[m_SpreadTipsTimeIndex], this, 1);
	}
}

void CBattleWarSafetyZone::bcastSpreadCountDownTips(int nCountDownTime)
{
	SWarSafetyZone* pWarSafetyZone = getCurRoundSafetyZone();
	if (pWarSafetyZone == NULL)
		return;

	char buf[128];
	ssprintf_s(buf, sizeof(buf), pWarSafetyZone->szCDBroadcast, (int)(nCountDownTime/1000));
	g_EHelper.showSystemMessageToChatBox(m_pWar->getWarSceneID(), CHAT_TIP_CHATBOX_CUSTOMER_TIP, buf, __LINE__);

	g_EHelper.showSystemMessage(m_pWar->getWarSceneID(), CHAT_TIP_CUSTOMER_TIP, buf);

#ifdef WAR_SAFETYZONE_PRINT
	char log[256] = { 0 };
	sprintf_s(log, sizeof(log), "%s,nRound=%d, spreadCountDownTips, %s", __FUNCTION__, m_nCurRound, buf);
	WarningLn(log);
#endif
}

void CBattleWarSafetyZone::bcastSpreadBeginTipsTips()
{
	SWarSafetyZone* pWarSafetyZone = getCurRoundSafetyZone();
	if (pWarSafetyZone == NULL)
		return;

	g_EHelper.showSystemMessageToChatBox(m_pWar->getWarSceneID(), CHAT_TIP_CHATBOX_CUSTOMER_TIP, pWarSafetyZone->szSpreadBeginBcast, __LINE__);

	g_EHelper.showSystemMessage(m_pWar->getWarSceneID(), CHAT_TIP_CUSTOMER_TIP, pWarSafetyZone->szSpreadBeginBcast);

#ifdef WAR_SAFETYZONE_PRINT
	char log[256];
	sprintf_s(log, sizeof(log), "%s,nRound=%d, spreadBeginTips, %s", __FUNCTION__, m_nCurRound, pWarSafetyZone->szSpreadBeginBcast);
	WarningLn(log);
#endif
}

void CBattleWarSafetyZone::fillSafetyZoneInfo(obuf& ob)
{
	// 毒气圈圆心、半径、安全圈圆心、半径、扩散时间、扩散开始时间
	msg_war_safetyzone_info msg;
	msg.nCenterOfNoxZoneX = m_curNoxZone.center.x * LOCATION_SCALE;
	msg.nCenterOfNoxZoneY = m_curNoxZone.center.y * LOCATION_SCALE;
	msg.nCenterOfNoxZoneZ = m_curNoxZone.center.z * LOCATION_SCALE;
	msg.nCenterOfSafetyZoneX = m_curSafetyZone.center.x * LOCATION_SCALE;
	msg.nCenterOfSafetyZoneY = m_curSafetyZone.center.y * LOCATION_SCALE;
	msg.nCenterOfSafetyZoneZ = m_curSafetyZone.center.z * LOCATION_SCALE;
	msg.nRadiusOfNoxZone = m_curNoxZone.radius * LOCATION_SCALE;
	msg.nRadiusOfSafetyZone = m_curSafetyZone.radius * LOCATION_SCALE;
	msg.nSpreadTime = m_SpreadTime;
	// 扩散开始时间大于0，客户端才每帧把毒气圈缩小
	msg.nSpreadBeginTick = m_SpreadBeginTick;   // 取时间服务器的时间，客户端可以根据这个减少误差

	ob << msg;
}

void CBattleWarSafetyZone::fillNoxSpreadCDInfo(obuf& ob)
{
	msg_war_safetyzone_spread_cd msg;
	msg.nSpreadCDTick = m_dwSpreadCDTick;
	msg.nSpreadCDTime = m_dwSpreadCDTotalTime;

	ob << msg;
}

void CBattleWarSafetyZone::sendSafetyZoneInfo2Client()
{
	SWarSafetyZone* pWarSafetyZone = getCurRoundSafetyZone();
	if (pWarSafetyZone == NULL)
		return;
	
	obuf ob;
	fillSafetyZoneInfo(ob);
	m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_SAFETYZONE_INFO, (LPCSTR)ob.data(), ob.size());

#ifdef WAR_SAFETYZONE_PRINT
	char log[256] = { 0 };
	sprintf_s(log, sizeof(log), "%s,nRound=%d, NoxZone,(%f,%f,%f),radius=%f, spreadTime=%d", __FUNCTION__, m_nCurRound, m_curNoxZone.center.x, m_curNoxZone.center.y,
		m_curNoxZone.center.z, m_curNoxZone.radius, pWarSafetyZone->nSpreadTime);
	WarningLn(log);

	sprintf_s(log, sizeof(log), "%s,nRound=%d, SafetyZone,(%f,%f,%f),radius=%f, spreadCountDown=%d", __FUNCTION__, m_nCurRound, m_curSafetyZone.center.x, m_curSafetyZone.center.y,
		m_curSafetyZone.center.z, m_curSafetyZone.radius, pWarSafetyZone->nSpreadCountdown);
	WarningLn(log);
#endif
}

void CBattleWarSafetyZone::sendSpreadCDInfo2Client()
{
	obuf ob;
	fillNoxSpreadCDInfo(ob);
	m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_SAFETYZONE_SPREAD_CD, (LPCSTR)ob.data(), ob.size());

#ifdef WAR_SAFETYZONE_PRINT
	char log[256] = { 0 };
	sprintf_s(log, sizeof(log), "%s,nRound=%d,startTick=%d, spreadCountDown=%d", __FUNCTION__, m_nCurRound, m_dwSpreadCDTick, m_dwSpreadCDTotalTime);
	WarningLn(log);
#endif
}

void CBattleWarSafetyZone::calcNewSafetyZoneInfo()
{
	SWarSafetyZone* pWarSafetyZone = getCurRoundSafetyZone();
	if (pWarSafetyZone == NULL)
		return;

	// 新安全区半径
	m_curSafetyZone.radius = pWarSafetyZone->fRadiusOfSafetyZone;

	// 新安全区圆点
	m_curSafetyZone.center = g_SZHelper.getNewSafetyZoneCenter(m_curNoxZone, m_curSafetyZone.radius);

	// 扩散时间
	m_SpreadTime = pWarSafetyZone->nSpreadTime;
}

void CBattleWarSafetyZone::checkActorInNox()
{
	SWarSafetyZone* pSafetyZone = getCurRoundSafetyZone();
	if (pSafetyZone == NULL)
		return;

	if (pSafetyZone->nBuffID <= 0)
		return;

	int nCurBuffID = pSafetyZone->nBuffID;
	int nCurBuffLevel = pSafetyZone->nBuffIDLevel;

	// 获取所有玩家的位置
	UID uidList[128];
	int nCount = m_pWar->getAllPerson(uidList, sizeof(uidList));
	for (int i = 0; i < nCount; ++i)
	{
		UID uid = uidList[i];
		Vector3 loc = getLocation(uid);
		m_mapActorLoc[uid] = loc;
	}

	// 计算当前时间的毒气圈
	Cicle newNoxZone = getCurNoxZone();

#ifdef WAR_SAFETYZONE_PRINT
	char szLog[256] = { 0 };
	ssprintf_s(szLog, sizeof(szLog), "realTime noxious zone, (%f,%f,%f)-%f,(%f,%f,%f)-%f", newNoxZone.center.x, newNoxZone.center.y, newNoxZone.center.z,
		newNoxZone.radius, m_curSafetyZone.center.x, m_curSafetyZone.center.y, m_curSafetyZone.center.z, m_curSafetyZone.radius);
	WarningLn(szLog);
#endif

	ACTORLOCMAP_ITER iter = m_mapActorLoc.begin();
	for (; iter != m_mapActorLoc.end(); ++iter)
	{
		UID uid = iter->first;

		// 判断玩家是否在毒气区(即在毒气圈外就是毒气区)
		Vector3 tmpLoc = iter->second;
		tmpLoc.y = 0;  // 去除Y坐标
		bool isInNox = (newNoxZone.center.checkDistance(tmpLoc, newNoxZone.radius) == false);

		if (isInNox)
		{
			// 在病毒区则增加Buff
			AddBuff(uid, nCurBuffID, nCurBuffLevel, uid);

			m_mapIsAddBuff[uid] = true;

#ifdef WAR_SAFETYZONE_PRINT
			char szLog[256] = { 0 };
			ssprintf_s(szLog, sizeof(szLog), "isInNox, Add buff=%d, uid=%d, (%f,%f,%f)-%f,(%f,%f,%f)", nCurBuffID, uid, newNoxZone.center.x, newNoxZone.center.y, newNoxZone.center.z,
				newNoxZone.radius, iter->second.x, iter->second.y, iter->second.z);
			WarningLn(szLog);
#endif
		}
		else
		{
			// 在非毒气区给有Buff的人删Buff
			if (m_mapIsAddBuff[uid])
			{
				RemoveBuff(uid, nCurBuffID, uid, uid);
				m_mapIsAddBuff[uid] = false;

#ifdef WAR_SAFETYZONE_PRINT
				char szLog[256] = { 0 };
				ssprintf_s(szLog, sizeof(szLog), "is not InNox, Remove buff=%d, uid=%d, NoxCenter=(%f,%f,%f)-%f,actorPos=(%f,%f,%f)", nCurBuffID, uid, newNoxZone.center.x, newNoxZone.center.y, newNoxZone.center.z,
					newNoxZone.radius, iter->second.x, iter->second.y, iter->second.z);
			WarningLn(szLog);
#endif
			}
		}
	}
}

bool CBattleWarSafetyZone::isNoxSpreading()
{
	return m_SpreadBeginTick > 0;
}

SWarSafetyZone * CBattleWarSafetyZone::getCurRoundSafetyZone()
{
	return g_EHelper.getWarSafetyZoneInfo(m_nCurRound);
}

Cicle CBattleWarSafetyZone::getCurNoxZone()
{
	Cicle newNoxZone = m_curNoxZone;
	if (isNoxSpreading())
	{
		// 如果是正在传播，则计算实时的毒气圈大小
		DWORD diffTick = gServerGlobal->getTimeSyncService()->GetTick() - m_SpreadBeginTick;

		newNoxZone = g_SZHelper.calcRealTimeNoxiousZone(m_curNoxZone, m_curSafetyZone, diffTick, m_SpreadTime);
	}

	return newNoxZone;
}
