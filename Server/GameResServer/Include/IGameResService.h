#pragma once
#include "EntityDef.h"

struct IGameResService
{
	// 为静态地图创建刷怪器
	virtual bool CreateMonsterGenerator(int nSceneID, int nMapID) = 0;

	// 销毁静态地图的刷怪器
	virtual void DestroyMonsterGenerator(int nSceneID) = 0;

	// 怪物死亡 (-1表示遍历找)
	virtual void OnMonsterDie(UID uidMonster, int nSceneID) = 0;
};