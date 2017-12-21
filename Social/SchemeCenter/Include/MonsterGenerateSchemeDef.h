#pragma once
#include "Vector3.h"

#pragma pack(push, 1)

struct GenMonsterStrategy
{
	enum
	{
		REBORN_MODE_FIX_INTERVAL,  // 固定间隔重刷
		REBORN_MODE_AFTER_DIE_INTERVAL, // 死亡后nRebornInterval重刷
	};

	struct MonsterGenInfo
	{
		int nMonsterID;  // 怪物ID
		int nGroup;		 // 阵营
		int nAIMode;	 // AI模式
		int nRadii;		 // 视野
		int nBornTime;	 // 刷怪器启动后nBornTime秒刷出第一波
		Vector3 ptBorn;	 // 出生地
		float fAngle;    // 朝向
		int nNumPerBatch; // 一波怪物数量
		int nRebornMode;  // 0--固定间隔nRebornInterval重刷一波 1--死后间隔nRebornInterval重刷一波
		int nRebornInterval; // 重刷间隔
		int nNumUperLimit;	// 数量上限
		std::vector<Vector3> vecWayPoint;	// 巡逻路径

		MonsterGenInfo() : nMonsterID(0), nGroup(0), nAIMode(0), nRadii(0), nBornTime(0), 
			nNumPerBatch(0), nRebornMode(0), nRebornInterval(0), nNumUperLimit(0)
		{
			vecWayPoint.clear();
		}
	};

	std::vector<MonsterGenInfo> vecInfo;

	GenMonsterStrategy()
	{
		vecInfo.clear();
	}
};

#pragma pack(pop)