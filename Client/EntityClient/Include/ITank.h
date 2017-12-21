/*******************************************************************
** 文件名:	Tank.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			载具对象
********************************************************************/


#pragma once

#include "IClientEntity.h"
#include "Vector3.h"
#include "DTankModule.h"

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

struct ITank : public IEntity
{
	/**
	@param
	@param
	@return
	*/
	virtual LPCSTR				GetName(void) = NULL;

	/** 取得载具配置数据
	@param   
	@param   
	@return  
	*/
	virtual STankSchemeInfo *	GetTankSchemeInfo(void) = NULL;

	/** 取最大技能索引
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetMaxSkillIndex(void) = NULL;

	/** 取技能ID
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetSkillIDByIndex(BYTE nSkillIndex) = NULL;

	/** 是否是PK载具
	@param   
	@param   
	@return  
	*/
	virtual bool				IsPKTank() = NULL;

	/** 设置pk载具技能
	@param   
	@param   
	@return  
	*/
	virtual void				SetPKTankSkill(int nPostion, long nSkillID) = NULL;

	/** 设置pk载具技能mainID
	@param   
	@param   
	@return  
	*/
	virtual void				SetPKTankSkillMainID(int nPostion, ushort nSkillMainID) = NULL;

	/** 检测是否存在PK技能
	@param   
	@param   
	@return  
	*/
	virtual bool				HavePKTankSkill(long nSkillID) = NULL;


	/** 取指定主位置的subID
	@param   
	@param   
	@return  
	*/
	virtual int				GetPKTankSkillByPostion(int nPostion) = NULL;

	/** 清除mianid列表
	@param   
	@param   
	@return  
	*/
	virtual void				ClearPKTankMainID() = NULL;


	/** 取PK载具技能个数
	@return  
	*/
	virtual int					GetPKSkillNum() = NULL;

	/** 判断是否能随时跳出载具
	@name				: 
	@param				: 
	@return	
	*/
	virtual bool				CanJumpOut() = NULL;

	/** 队友在此载具中时, 左键头像点击直接选中载具
	@name				: 
	@param				: 
	@return
	*/
	virtual bool				IsSelectByFace() = NULL;
};
