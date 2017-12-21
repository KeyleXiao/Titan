/*******************************************************************
** 文件名: WarMonsterMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/16
** 版  本: 1.0
** 描  述: 战场随机刷怪接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMonsterMgr.h"
#include "WarDef_Internal.h"

class IWar;

class CWarMonsterMgr : public IWarMonsterMgr, public TimerHandler
{
	enum ETimerID
	{
		ETimerID_RefreshMonster = 0,	// 刷怪检测定时器
	};

	enum ETimerInternal
	{
		ETimerInternal_RefreshMonster = 1000,    // 刷怪检测间隔
	};

public:
	CWarMonsterMgr(IWar* pWar);
	~CWarMonsterMgr();

	////////////////////////////////////////IWarMonsterMgr Start////////////////////////////
public:
	// 初始化
	virtual bool Init(int nMatchType);

	// 释放
	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 加载刷怪配置脚本
	virtual bool LoadMonsterScheme();

	/** 获取战场指定类型的怪物UID
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual DWORD getUIDListByMonsterType(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize);

	virtual IMonsterExec* getWarMonsterExecByType(EWarMonsterExec WarMonsterExec);

	// 获得本战场内怪物索引类型
	virtual EWarMonsterExec	getWarMonsterType(int nIndex);

	// 获取怪物类型
	virtual EWarMonsterExec getWarMonsterTypeByUID(UID uid);

	// 获取已创建的怪物信息
	virtual CreateMonsterInfo* getWarMonsterInfoByUID(UID uid);

	// 获取已创建的怪物列表
	virtual vector<UID> getWarMonsterList();

	virtual int getMaxMonsterCountLimit() { return m_nWarMaxMonsterCount; }

	// 停止战场怪物AI
	virtual void stopMonsterAI();

	// 释放战场怪物资源
	virtual void releaseMonsterRes();

	// 玩家断线重连
	virtual void onActorContinuePlay(int clientID);

	// 创建NPC
	virtual UID createNpc(int nMonsterID, Vector3 vLoc, EWarMonsterExec eType, CreateNpcContext* pNpcContext, int nRefreshIndex, int nLocalIndex, INPCService*pNpcService = NULL, bool bRandomRefresh = false);

	// 怪物死亡
	virtual void onMonsterDie(sEntityDieInfo*  pEntityDieInfo);

	// NPC销毁
	virtual void onNpcDestroy(UID uid);
	////////////////////////////////////////IWarMonsterMgr End////////////////////////////

	///////////////////////////////////////////TimeHandler Start/////////////////////////
	virtual void OnTimer(unsigned long dwTimerID);

	///////////////////////////////////////////TimeHandler End/////////////////////////

private:
	void timeExec();

private:
	IWar* m_pWar;

	int m_nWarType;

	MonsterExecMap          m_mapAllMonstExec;      // 战场怪物的处理类型。 包括怪物创建，状态和死亡处理。
	CreateMonsterMap        m_mapCreateMonster;     // 创建的怪物列表
	bool                    m_bReleaseAllMonster;   // 是否释放所有怪物
	int						m_nWarMaxMonsterCount;  // 怪物最大数
};