/*******************************************************************
** 文件名:	QueryFilter.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/12/2015	16:26
** 版  本:	1.0
** 描  述:	
            寻路服务
            继承自RecastDetour的dtQueryFilter,定义多边形过滤以及通过某些导航格子所需要花费的消耗
********************************************************************/

#pragma once

#include "DetourNavMeshQuery.h"
#include "PathFindDef.h"

enum CheckType
{
	CHECK_PASS_ENTITY = 0,
	CHECK_PASS_SKILL = 1
};

class QueryFilter :public dtQueryFilter
{
public:
	virtual bool passFilter(const dtPolyRef ref,
		const dtMeshTile* tile,
		const dtPoly* poly) const;
	inline void setDiameter(float fDiameter) { m_fDiameter = fDiameter; }	
	inline void setCamp(int camp) { m_Camp = camp; }
	inline void setFlagList(unsigned int* flags) { m_flags = flags; }
public:
	QueryFilter();
	bool checkPass(int nCampFlag, int obCamp, int selfCamp, CheckType type)const;
private:
	float				m_fDiameter;
	int					m_Camp;		//寻路目标本身的阵营ID
	unsigned int*				m_flags;
};