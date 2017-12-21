/*******************************************************************
** 文件名: AIEcmCtrl.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/10/28
** 版  本: 1.0
** 描  述: AI玩家战场经济控制
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "GameDef.h"
#include "WarDef.h"
#include "SchemeDef.h"
#include "IAIEcmCtrlMgr.h"
#include <map>

class IWar;
struct IActorService;

class CAIEcmCtrl : public IAIEcmCtrlMgr
{
	// AI经济控制线
	enum EWAIEcmCtrlLine
	{
		EWAIEcmCtrlLine_1 = 0,
		EWAIEcmCtrlLine_2,
		EWAIEcmCtrlLine_3,
		EWAIEcmCtrlLine_MAX,
	};

	// AI经济差级别
	enum EWAIEcmDiff
	{
		EWAIEcmDiff_NONE = 0,	// 无阻力
		EWAIEcmDiff_SMALL,		// 小阻力
		EWAIEcmDiff_BIG,		// 大阻力
		EWAIEcmDiff_HUGE,		// 超大阻力
	};

public:
	CAIEcmCtrl(IWar* pWar);
	~CAIEcmCtrl();

public:
	////////////////////////IAIEcmCtrlMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 初始化平均匹配分
	virtual void initAvgMatchScore();

	// 计算经济控制系数
	virtual void calcAIEcmCtrlRatio();

	// 计算经济控制线
	virtual void calcAIEcmCtrlLine();

	// 计算经济控制后获得的天赋点
	virtual int controlTelentPoint(PDBID pdbID, int nAddValue, ETalentDropType nDropType);

	// 控制连杀经济
	virtual int controlContKillTelentPoint(IActorService* pDead, IActorService* pAttacker, int nAddValue, float fDropGainTalentRate, float fContKillEmcCtrlRatio, int nMaxMultiKill, int nContKillCount);
	////////////////////////IAIEcmCtrlMgr end/////////////////////////

private:
	// 获取经济控制差系数
	// param nType: 经济控制差系数类型，EWEcmCtrlRatioType
	float getAIEcmCtrlRatio(PDBID pdbID, EWEcmExpCtrlRatioType nRatioType);

	// 获取玩家的平均匹配分
	int getMatchScoreForCalc(PDBID pdbid);
private:
	// 战场逻辑
	IWar*										m_pWar;

	// 经济控制
	std::map<PDBID, int>						m_AIEcmCtrlList;
	// 经济控制线<阵营,AI经济控制线>
	std::map<int, map<int,int>>					m_AIEcmCtrlLine;

	// 是否初始化了经济控制线
	bool										m_bInitAIEcmCtrlLine;
	// 是否初始化了分数
	bool										m_bInitAvgMatchScore;

	int											m_nMatchType;

	// <阵营,敌方平均排位分>
	std::map<int, int>							m_mapMatchScore;
};