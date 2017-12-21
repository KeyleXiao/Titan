/*******************************************************************
** 文件名: EconomyControl.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/14
** 版  本: 1.0
** 描  述: 真人玩家战场经济控制
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
#include "IEcmCtrlMgr.h"
#include <map>

class IWar;
struct IActorService;

class CEconomyControl : public IEcmCtrlMgr
{
	// 真实玩家经济控制线
	enum EWEcmCtrlLine
	{
		EWEcmCtrlLine_1 = 0,
		EWEcmCtrlLine_2,
		EWEcmCtrlLine_3,
		EWEcmCtrlLine_MAX,
	};

	// 真实玩家经济差级别
	enum EWEcmDiff
	{
		EWEcmDiff_NONE = 0,	// 无阻力
		EWEcmDiff_SMALL,	// 小阻力
		EWEcmDiff_BIG,		// 大阻力
		EWEcmDiff_HUGE,		// 超大阻力
	};

public:
	CEconomyControl(IWar* pWar);
	~CEconomyControl();

public:
	////////////////////////IEcmCtrlMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 计算经济控制系数
	virtual void calcEcmControlRatio();

	// 计算经济控制线
	virtual void calcEcmControlLine();


	// 计算经济控制后获得的天赋点
	virtual int controlTelentPoint(PDBID pdbID, int nAddValue, ETalentDropType nDropType);

	// 控制连杀经济
	virtual int controlContKillTelentPoint(IActorService* pDead, IActorService* pAttacker, int nAddValue, float fDropGainTalentRate, float fContKillEmcCtrlRatio, int nMaxMultiKill, int nContKillCount);

	////////////////////////IEcmCtrlMgr end/////////////////////////
private:
	// 获取经济控制差系数
	// param nType: 经济控制差系数类型，EWEcmCtrlRatioType
	float getEmcCtrlRatio(PDBID pdbID, EWEcmExpCtrlRatioType nRatioType);

private:
	// 战场逻辑
	IWar*										m_pWar;
	// 经济控制
	std::map<PDBID, int>						m_EcmCtrlList;
	// 经济控制线
	int											m_EcmCtrlLine[EWEcmCtrlLine_MAX];
	// 是否初始化了经济控制线
	bool										m_bInitEcmCtrlLine;
};