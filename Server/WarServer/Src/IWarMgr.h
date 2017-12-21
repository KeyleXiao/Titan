/*******************************************************************
** 文件名: CWarBase.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/5
** 版  本: 1.0
** 描  述: 战场子功能接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

// 战场子管理
enum WarMgrID
{
	Mgr_Drop = 0,			// 掉落
	Mgr_XPSKill,			// XP技能
	Mgr_AIEcmCtrl,			// AI经济控制
	Mgr_AIExpCtrl,			// AI经验控制
	Mgr_EcmCtrl,			// 真人经济控制
	Mgr_SafetyZone,			// 安全区玩法
	Mgr_ForceEnd,			// 强制结束
	Mgr_LvExpCtrl,			// 经济经验控制
	Mgr_SoliderVigour,		// 士气
	Mgr_Support,			// 点赞
	Mgr_AddBuff,			// 战场加Buff
	Mgr_CheckBadBehavior,	// 检测恶劣行为
	Mgr_LogRecord,			// 战场数据库记录
	Mgr_RandRefreshMons,	// 战场随机刷新怪物
	Mgr_MonsterDiedExec,	// 战场怪物死亡处理
	Mgr_Monster,			// 战场怪物管理
	Mgr_DevilKing,			// 战场魔王玩法
	Mgr_HeroGenic,			// 战场英雄基因玩法

	Mgr_Max,
};

class __IWarMgr
{
public:
	// 初始化
	virtual bool Init(int nMatchType) = 0;

	// 释放
	virtual void Release() = 0;

	// 战场正式开始
	virtual void onWarRealStart() = 0;

	// 战场结束
	virtual void onWarEnd() = 0;
};