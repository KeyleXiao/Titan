/*******************************************************************
** 文件名:	IEntity.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体接口
********************************************************************/


#pragma once

#include "ISerializable.h"
#include "GameDef.h"

#include "EntityDef.h"

class Vector3;
struct __IEntity;

//////////////////////// 载具承载类型 /////////////////////////////
enum
{
	TANK_CARRYTYPE_UNKNOWN = 0,			// 未知
	TANK_CARRYTYPE_BUS = 1,				// 公共汽车（只可运载）
	TANK_CARRYTYPE_CHARIOT,				// 战车（可运载，可攻击）
	TANK_CARRYTYPE_BATTLEPLANE,			// 战斗机（可运载，可攻击，可跨越占位块）
	TANK_CARRYTYPE_BMW,					// 双人坐骑
	TANK_CARRYTYPE_TRUCK,				// 骠车(可运载，可攻击)
	TANK_CARRYTYPE_PK,					// pk载具
	TANK_CARRYTYPE_DYNAMICRELIVE,		// 复活型载具
	TANK_CARRYTYPE_LOCKBOSS,			// 锁怪型载具
	TANK_CARRYTYPE_SHIP,				// 船
};

////////////////////////////载具对像////////////////////////////////////
struct ITank : public __IEntity
{
	/** 取得称号
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR				GetTitle(void) = NULL;

	/** 设置称号
	@param   
	@param   
	@return  
	*/
	virtual void				SetTitle(LPCSTR pszTitle) = NULL;

	/** 取得本配置信息
	@param   
	@param   
	@return  
	*/
	virtual STankSchemeInfo *	GetSchemeInfo(void) = NULL;

	/** 解散自已
	@param   
	@param   
	@return  
	*/
	//virtual void				KillSelf(void) = NULL;

	/** 测试上次移除的tile位置
	@param   
	@param   
	@return  
	*/
	virtual void				DebugLastRemoveTile(POINT ptLastTile) = NULL;

	/** 同场景服切换地图
	@param   
	@param   
	@return  
	*/
	//virtual bool				ChangeZone(DWORD dwNewZoneID, POINT ptNewTile) = NULL;

	/** 不同场景服切换地图
	@param   
	@param   
	@return  
	*/
	//virtual bool				ChangeServer(DWORD dwNewZoneID, POINT ptNewTile, bool bCoercive, DWORD serveridTarget) = NULL;

	/** 导出数据库属性现场
	@param   
	@param   
	@return  
	*/
	virtual bool				ExportMapContext(LPSTR pszContext, int &nLen) = NULL;

	/** 取得扩展数据
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR				GetExtendData(void) = NULL;

	/** 设置扩展数据
	@param szExtendData 扩展数据
	@param nLen			扩展数据长度  
	@return  
	*/
	//virtual bool				SetExtendData(LPCSTR szExtendData, int nLen) = NULL;

	/** 是否PK载具
	@return  
	*/
	virtual bool				IsPKTank() = NULL;

	/** PK载具是否有当前技能
	@return  
	*/
	virtual bool				IfHavePKTankSkill(int nSpellID) = NULL;

	/** 读配置文件，添加buf
	@param   
	@param   
	@return  
	*/
	virtual bool				AddBuffFromScheme(void) = NULL;

	/** 判断是否能随时跳出载具
	@name				: 
	@param				: 
	@return	
	*/
	virtual bool				CanJumpOut(void) = NULL;

	/** 判断是否禁用操作提示
	@name				: 
	@param				: 
	@return
	*/
	virtual bool				IsForbidTip(void) = NULL;

	/** 是否移动时打断进度条
	@name				: 
	@param				: 
	@returns
	*/
	virtual bool				IsMoveBreakProgBar(void) = NULL;
	/* 处理 属性事件
	@parm:	bBefore为 true时表示 属性写入前调用此接口  
					  false 表示 属性写入后调用此接口
	@parm:  data 写入前为 相应要修改的属性的值的地址写入后没有使用
	*/
	virtual void OnPropertyChange(bool bBefore,int prop_id, void * data,int32 len) = NULL;
};
