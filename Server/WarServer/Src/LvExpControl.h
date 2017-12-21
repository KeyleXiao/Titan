/*******************************************************************
** 文件名: LvExpControl.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/24
** 版  本: 1.0
** 描  述: 等级经验控制
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once

#include "ILvExpCtrlMgr.h"

class IWar;

class CLvExpControl : public ILvExpCtrlMgr
{
public:
	CLvExpControl(IWar* pWar);
	~CLvExpControl();

public:
	////////////////////////ILvExpCtrlMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 控制等级经验算法  野怪和小兵
	virtual void controlLvExp(EExpDropType expDropType, int nPlayerLv, int nCamp, Vector3 vLoc, int nDistance, int& nAdd, bool bIsUseShareCoeff = false);

	// 控制等级经验算法  英雄
	virtual void controlLvExp(int nPlayerLv, int nDiePlayerLv, int nAroundPlayerCount, int& nAdd);
	////////////////////////ILvExpCtrlMgr end/////////////////////////

private:
	// 获取分享者系数
	float getShareCoeff(int nMurderCamp, Vector3 vLoc, int nDistance);

	// 计算全场英雄平均等级  
	float getAllHeroMeanLv();

	// 计算某个阵营的平均等级nCamp 是枚举CAMP_TYPE
	float getCampHeroMeanLv(int nCamp);

	// 英雄和野怪的等级差控制系数
	float getHeroOrMonsterLevelGapControlCoeff(int nLevelGap, int nIncreaseCoeff, int nDecreaseCoeff);

	// 计算线上小兵的等级差控制系数
	float getSolderLevelGapControlCoeff(int nLevelGap, int nIncreaseCoeff, int nControlLevel);

private:

	// 战场
	IWar*	m_pWar;
};