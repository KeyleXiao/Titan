/*******************************************************************
** 文件名:	SafetyZoneHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/11/2
** 版  本:	1.0
** 描  述:	大逃杀-安全圈辅助算法类
********************************************************************/

#pragma once

#include "MathHelper.h"

// 定义
class CSafetyZoneHelper
{
public:
	// 构造函数
	CSafetyZoneHelper(void);
	// 虚构函数
	~CSafetyZoneHelper(void);

public:
	// 计算安全圈圆心和半径
	// @param centerOfNoxZone: 病毒圈圆点
	// @param radiusOfNoxZone: 病毒圈半径
	// @param radiusOfSafetyZone: 安全区半径
	Vector3 getNewSafetyZoneCenter(const Cicle& curNoxZone, float radiusOfSafetyZone);
	//
	// 计算实时的毒气圈（圆心和半径）
	// 注意：在病毒圈外才算病毒区
	// 人位置、病毒圈初始圆心，病毒圈初始圆心，毒气扩散开始时间，安全区圆心，安全区半径
	Cicle calcRealTimeNoxiousZone(const Cicle& oldNoxZone, const Cicle& safetyZone, DWORD spreadBeginTick, DWORD spreadTime);
};

extern CSafetyZoneHelper				g_SZHelper;