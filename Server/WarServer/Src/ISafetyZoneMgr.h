/*******************************************************************
** 文件名: ISafetyZoneMgr.h
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

#include "IWarMgr.h"
#include "Vector3.h"
#include "SafetyZoneHelper.h"

class ISafetyZoneMgr : public __IWarMgr
{
public:
	// 获取毒圈内随机坐标
	virtual Vector3 getRandomPosOfNoxZone() = 0;

	// 获取安全圈内随机坐标
	// @fDistance: 离圆边的距离，默认为0则是整个安全圈大小内随机一个点
	virtual Vector3 getRandomPosOfSafetyZone(float fDistance = 0.f) = 0;

	// 获取安全圈内随机坐标
	virtual Cicle getSafetyZone() = 0;
};