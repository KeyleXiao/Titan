/*******************************************************************
** 文件名: AIEcmExpCtrl.h
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
#include "IAIExpCtrlMgr.h"
#include <map>

class IWar;
struct IActorService;

class CAIExpCtrl : public IAIExpCtrlMgr
{
	// AI经验控制线
	enum EWAIExpCtrlLine
	{
		EWAIExpCtrlLine_1 = 0,
		EWAIExpCtrlLine_2,
		EWAIExpCtrlLine_3,
		EWAIExpCtrlLine_MAX,
	};

	// AI经验差级别
	enum EWAIExpDiff
	{
		EWAIExpDiff_NONE = 0,	// 无阻力
		EWAIExpDiff_SMALL,		// 小阻力
		EWAIExpDiff_BIG,		// 大阻力
		EWAIExpDiff_HUGE,		// 超大阻力
	};

public:
	CAIExpCtrl(IWar* pWar);
	~CAIExpCtrl();

public:
	////////////////////////IAIExpCtrlMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 初始化平均匹配分
	virtual void initAvgMatchScore();

	// 计算经济控制系数
	virtual void calcAIExpCtrlRatio();

	// 计算经济控制线
	virtual void calcAIExpCtrlLine();

	// 计算经济控制后获得的天赋点
	virtual int controlExp(PDBID pdbID, int nAddValue, EExpDropType nDropType);

	// 控制连杀经济
	virtual int controlContKillExp(IActorService* pDead, IActorService* pAttacker, int nAddValue, float fDropGainTalentRate, float fContKillEmcCtrlRatio, int nMaxMultiKill, int nContKillCount);
	////////////////////////IAIExpCtrlMgr end/////////////////////////

private:
	// 获取经验控制差系数
	// param nType: 经验控制差系数类型，EWExpCtrlRatioType
	float getAIExpCtrlRatio(PDBID pdbID, EWEcmExpCtrlRatioType nRatioType);

	// 获取玩家的平均匹配分
	int getMatchScoreForCalc(PDBID pdbid);
private:
	// 战场逻辑
	IWar*										m_pWar;

	// 经验控制
	std::map<PDBID, int>						m_AIExpCtrlList;
	// 经验控制线<阵营,AI经验控制线>
	std::map<int, map<int,int>>					m_AIExpCtrlLine;
	// 是否初始化了经验控制线
	bool										m_bInitAIExpCtrlLine;
	// 是否初始化了分数
	bool										m_bInitAvgMatchScore;

	int											m_nMatchType;

	// <阵营,敌方平均排位分>
	std::map<int, int>							m_mapMatchScore;
};