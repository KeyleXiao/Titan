#pragma once
#include "IEntity.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"

class PathHelper
{
public:
	PathHelper(){}
	Vector3 GetMonsterPos(int uid) {
		IEntityFactory* pFactory = gClientGlobal->getEntityFactory();
		IClientEntity* entity = pFactory->getEntity(uid);
		return ((IEntity*)entity)->getLocation();
	}

	Vector3 GetMonsterAngle(int uid) {
		IEntityFactory* pFactory = gClientGlobal->getEntityFactory();
		IClientEntity* entity = pFactory->getEntity(uid);
		return ((IEntity*)entity)->getAngle();
	}
};