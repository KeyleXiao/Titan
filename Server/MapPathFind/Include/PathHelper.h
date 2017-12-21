#pragma once
#include "EntityHelper.h"
class PathHelper
{
public:
	PathHelper(){}
	Vector3 GetMonsterPos(int uid) {
		MonsterHelper monsterCreated(uid);
		return monsterCreated.m_ptr->getLocation(uid);
	}

	Vector3 GetMonsterAngle(int uid) {
		MonsterHelper monsterCreated(uid);
		return monsterCreated.m_ptr->getAngle(uid);
	}
};