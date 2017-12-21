/*******************************************************************
** 文件名: IAIExpCtrlMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/14
** 版  本: 1.0
** 描  述: 战场AI经验控制接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"
#include "IActorService.h"

class IAIExpCtrlMgr : public __IWarMgr
{
public:
	// 初始化平均匹配分
	virtual void initAvgMatchScore() = 0;

	// 计算经济控制系数
	virtual void calcAIExpCtrlRatio() = 0;

	// 计算经济控制线
	virtual void calcAIExpCtrlLine() = 0;

	// 计算经济控制后获得的天赋点
	virtual int controlExp(PDBID pdbID, int nAddValue, EExpDropType nDropType) = 0;

	// 控制连杀经济
	virtual int controlContKillExp(IActorService* pDead, IActorService* pAttacker, int nAddValue, float fDropGainTalentRate, float fContKillEmcCtrlRatio, int nMaxMultiKill, int nContKillCount) = 0;
};