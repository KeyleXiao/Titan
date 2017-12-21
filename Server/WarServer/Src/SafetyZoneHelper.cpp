/*******************************************************************
** 文件名:	SafetyZoneHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/2
** 版  本:	1.0
** 描  述:	大逃杀-安全圈辅助算法类
********************************************************************/

#include "stdafx.h"
#include "SafetyZoneHelper.h"
#include <math.h>

#define PI 3.1415926  

CSafetyZoneHelper g_SZHelper;

CSafetyZoneHelper::CSafetyZoneHelper(void)
{
}

CSafetyZoneHelper::~CSafetyZoneHelper(void)
{
}

Vector3 CSafetyZoneHelper::getNewSafetyZoneCenter(const Cicle& curNoxZone, float radiusOfSafetyZone)
{
	return CMathHelper::getRandomCicleOfCicle(curNoxZone, radiusOfSafetyZone);
}

Cicle CSafetyZoneHelper::calcRealTimeNoxiousZone(const Cicle & oldNoxZone, const Cicle & safetyZone, DWORD diffTick, DWORD spreadTime)
{
	// 计算病毒圈当前位置和距离
	// 病毒圈半径的缩小速度vR
	// 病毒圈圆心到安全圈圆心之间缩小的速度vL
	// 根据速度和消逝的时间得到新的病毒圈圆心位置cN和半径rN

	float vR = (oldNoxZone.radius - safetyZone.radius) / spreadTime;

	Vector3  vL;
	vL.x = (oldNoxZone.center.x - safetyZone.center.x) / spreadTime;
	vL.y = (oldNoxZone.center.y - safetyZone.center.y) / spreadTime;
	vL.z = (oldNoxZone.center.z - safetyZone.center.z) / spreadTime;

	float rN = oldNoxZone.radius - (vR * diffTick);

	Vector3 cN;

	cN.x = oldNoxZone.center.x - diffTick * vL.x;
	cN.y = oldNoxZone.center.y - diffTick * vL.y;
	cN.z = oldNoxZone.center.z - diffTick * vL.z;

	return Cicle(cN, rN);
}
