/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 秦其勇
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场信息
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IMonsterExec.h"
#include "War.h"


// 专职刷新Crystal的信息，对塔的处理（建筑）
class CMonsCrystalExec : public IMonsterExec, IEventExecuteSink
{
public:
	// 初始化
	// nWarID: 战场ID
	// eType: 怪物处理类型
	virtual bool Init(CWar *pCWar);
	// 时间处理
	virtual void timeExec();
	// 更新怪物配置，隐藏的
	// nCamp: 阵营
	// nMonsterIndex: 怪物配置表中的怪物
	virtual void UpdateMonsterInfo(int nIndex, int nPam, bool bStatus);
	// 怪物死亡事件处理
	virtual void MonsterDeadExec(UID uID, UID uMurderID, int nMonsterID,int nCamp, int nMurderCamp,Vector3 vec);
	// 外部调用内部影响事件处理
	virtual void WarEventDeal(EWarInfoEvent WarInfoEvent,int nIndex, int nPam, bool bStatus);
	// 静态创建
	static CMonsCrystalExec * CreateMonsterExec(CWar *pCWar);

	//*******************************IEventExecuteSink***********************
	void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);
private:
	// 设置保护塔
	void SetTowerProtect(int nIndex, int nPam, bool bAddOrRemove = false);

	// 添加超级兵
	void SetSoliderRefresh(int nIndex, int nPam, bool bStatus);

	// 设置保护基地
	void SetBaseProtect(int nIndex, int nPam, bool bAddOrRemove = false);

	// 处理互斥保护基地
	void HandleMutexBaseProtect();

	// 战斗受到的伤害
	void onGetDamage(event_entity_damage damage, DWORD dwSrcUID);
private:
	// 塔的怪物信息
	MonsterRefreshVect m_MonsterRefreshVect;
	// 战场信息
	CWar *m_CWar;
	// 类型
	EWarMonsterExec m_ExexType;

	struct SProtectInfo
	{
		bool bHaveExist;
		int nEffectIndex;
		int nParam;
		SProtectInfo()
		{
			bHaveExist = false;
			nEffectIndex = 0;
			nParam = 0;
		}
	};

	// 索引怪物累积刷新次数
	RefreshCumulantList refreshCumulantList;

	// 记录互斥基地保护信息 索引 值
	typedef map<int,SProtectInfo> MAP_RecBaseProtect;
	// 阵营 互斥保护基地数量记录
	map<int,MAP_RecBaseProtect> m_CampProtectBaseCount;

	// 记录最后小地图战斗信息时间
	map<UID,DWORD> m_UIDMiniMapInfoRecord;
};