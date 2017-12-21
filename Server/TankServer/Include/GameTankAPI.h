/*******************************************************************
** 文件名:	GameTankAPI.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/04/22
** 版  本:	1.0
** 描  述:	载具系统API接口
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "DTankModule.h"
#include "ITankServer.h"

#ifndef __I_GameTankAPI_H__
#define __I_GameTankAPI_H__

#ifndef  TANK_API
#	ifdef TANKSEVER_EXPORTS
#		define  TANK_API  extern "C" __declspec(dllexport)
#	else
#		define  TANK_API  extern "C" __declspec(dllimport)
#		pragma comment(lib, MAKE_DLL_LIB_NAME(TankServer))
#	endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

// tolua_begin


/**  取得载具系统服务器
@param   
@return  载具系统服务器
*/
TANK_API ITankServer * GetTankServer(void);

/** 用载具ID取载具脚本配置参数信息
@param   DWORD dwTankID：载具ID
@param   
@return  载具脚本配置参数信息
*/
TANK_API const STankSchemeInfo*  GetTankSchemeInfo(DWORD dwTankID);

/** 用载具ID取载具脚本配置参数信息
@param   DWORD dwTankID：载具ID
@param   
@return  载具脚本配置参数信息
*/
TANK_API const STankSchemeInfo*  GetTankSchemeInfoByUID(UID uidTank);



// tolua_end

// tolua_begin


/** 用DBID取得玩家实体
@param   DWORD dwDBID   ：指定玩家DBID
@return __IPlayerRole *:玩家实体
*/
TANK_API __IPlayerRole * TankGetPersonByDBID(DWORD dwDBID);

/**
@name          : 用ID创建载具
@param DWORD dwZoneID	: 场景ID
@param int nLocx		: 位置x
@param int nLocy		: 位置y
@param int nTankID		: 载具ID
@param int nSeatCount	: 座位数，如果填-1,则表示用配置里坐位数
@param int nNation		: 所属国家
@param int nAngle		: 出生角度 315 面向玩家 
@param int nLeve		: 动态载具等级
@param int nCivilLevel	: 文明等级
@return        : 返回载具的UID,如果创建失败返回0
*/
TANK_API UID CreateTank(DWORD dwZoneID,int nLocx,int nLocy,int nTankID,int nSeatCount=-1,int nNation=0,int nAngle=315,int nLevel = 0, int nCivilLevel = 0);


/**
@name          : 在人物旁用ID创建载具
@param DWORD dwDBID		: 指定玩家DBID
@param int nLocx		: 位置x
@param int nLocy		: 位置y
@param int nTankID		: 载具ID
@param bool bPersonEnter: 是否加入人物
@param int nSeatCount	: 座位数，如果填-1,则表示用配置里坐位数
@param int nNation		: 所属国家
@param int nAngle		: 出生角度 315 面向玩家 
@return        : 返回载具的UID,如果创建失败返回0
*/
TANK_API UID CreateTankByPerson(DWORD dwDBID,int nLocx,int nLocy,int nTankID,bool bPersonEnter=false,int nSeatCount=-1,int nNation=0,int nAngle=315);


//////////////////////////////////////////////////////////////////////////
// 动态载具相关
/**
@name          : 用ID及等级创建动态载具
@param DWORD dwZoneID	: 场景ID
@param int nLocx		: 位置x
@param int nLocy		: 位置y
@param int nTankID		: 载具ID
@param int nLevel		: 动态等级
@param int nSeatCount	: 座位数，如果填-1,则表示用配置里坐位数
@param int nNation		: 所属国家
@param int nAngle		: 出生角度 315 面向玩家 
@return        : 返回载具的UID,如果创建失败返回0
*/
TANK_API UID CreateCustomTank(DWORD dwZoneID,int nLocx,int nLocy,int nTankID, int nLevel, int nSeatCount=-1,int nNation=0,int nAngle=315);

/**
@name          : 在人物旁用ID及等级创建动态载具
@param DWORD dwDBID		: 指定玩家DBID
@param int nLocx		: 位置x
@param int nLocy		: 位置y
@param int nTankID		: 载具ID
@param int nLevel		: 动态等级
@param bool bPersonEnter: 是否加入人物
@param int nSeatCount	: 座位数，如果填-1,则表示用配置里坐位数
@param int nNation		: 所属国家
@param int nAngle		: 出生角度 315 面向玩家 
@return        : 返回载具的UID,如果创建失败返回0
*/
TANK_API UID CreateCustomTankByPerson(DWORD dwDBID,int nLocx,int nLocy,int nTankID, int nLevel,bool bPersonEnter=false,int nSeatCount=-1,int nNation=0,int nAngle=315);

/**
@name          : 在人物旁用ID创建动态载具,并加入接龙跟随
@param DWORD dwDBID		: 指定玩家DBID
@param int nLocx		: 位置x
@param int nLocy		: 位置y
@param int nTankID		: 载具ID
@param int nLevel		: 动态等级
@param bool bPersonEnter: 是否加入人物
@param int nSeatCount	: 座位数，如果填-1,则表示用配置里坐位数
@param int nNation		: 所属国家
@param int nAngle		: 出生角度 315 面向玩家 
@return        : 返回载具的UID,如果创建失败返回0
*/
TANK_API UID CreateCustomFollowTankByPerson(DWORD dwDBID,int nLocx,int nLocy,int nTankID, int nLevel,bool bPersonEnter=false,int nSeatCount=-1,int nNation=0,int nAngle=315);


/**
@name          : 用ID创建动态载具,并启动巡逻
@param int nPathID		: 巡逻路线ID
@param int nLevel		: 动态等级
@param DWORD dwZoneID	: 场景ID,为0时用路线定义的地图ID
@param int nDelayTime	: 发车延迟时间 毫秒
@param int nLocx		: 位置x,为0时用路线定义的出生位置
@param int nLocy		: 位置y,为0时用路线定义的出生位置
@param int nTankID		: 载具ID,为0时用路线定义的巡逻载具ID
@param int nSeatCount	: 座位数，如果填-1,则表示用配置里坐位数
@param int nNation		: 所属国家
@param int nAngle		: 出生角度 315 面向玩家,为0时用路线定义的出生角度
@return        : 返回载具的UID,如果创建失败返回0
*/
TANK_API UID CreateCustomBusTank(int nPathID, int nLevel,DWORD dwZoneID=0,int nDelayTime=0,int nLocx=0,int nLocy=0,int nTankID=0,int nSeatCount=-1,int nNation=0,int nAngle=0);

//////////////////////////////////////////////////////////////////////////


/**  是否存在
@param  UID uidTank	: 载具UID
*/
TANK_API bool IsTankExist(UID uidTank);


/////////////// 载具实体承载部件 ITankBoxPart ///////////////////////////////////////////////////////////

// tolua_end

/**  用Uid取得载具接口
@param  UID uidTank	: 载具UID
*/
TANK_API ITank * GetTankByUID(UID uidTank);

/**  用Uid取得载具接口
@param  UID uidTank	: 载具UID
*/
TANK_API __ITankBoxPart *  GetTankBoxPart(UID uidTank);

// tolua_begin

/**  用Uid取得生物名称
@param  UID uidCreature	: 生物UID
*/
TANK_API const char *  GetCreatureName(UID uidCreature);


/**  指向列表最前端
@param  UID uidTank	: 载具UID
*/
TANK_API void				TankListMoveHead(UID uidTank);

/**  指向列表最后端
@param  UID uidTank	: 载具UID
*/
TANK_API void				TankListMoveEnd(UID uidTank);

/**  指向列表下一个
@param  UID uidTank	: 载具UID
@return  @return true: 没到结尾 false:到结尾
*/
TANK_API bool				TankListMoveNext(UID uidTank);

/**  取得列表当前指向的数据
@param  UID uidTank	: 载具UID
@return  乘客数据
*/
TANK_API const STankNodeData * GetTankListData(UID uidTank);

/**  取得载具容量
@param  UID uidTank	: 载具UID
@return  int 容量大小
*/
TANK_API DWORD				GetTankMaxCount(UID uidTank);

/**  取得载具当前人数
@param  UID uidTank	: 载具UID
@return  int 人数
*/
TANK_API DWORD				GetTankCount(UID uidTank);

/** 是否为空
@param  UID uidTank	: 载具UID
@return	  bool 返回状态
*/
TANK_API bool				IsTankEmpty(UID uidTank);

/** 是否满了
@param  UID uidTank	: 载具UID
@return	  bool 返回状态
*/
TANK_API bool				IsTankFull(UID uidTank);

/** 检查标识状态
@param  UID uidTank	: 载具UID
@param   DWORD dwFlagType:	标识类型参考 EMTankBoxFlag
@return  bool: true：是，false:否
*/
TANK_API  bool				GetTankFlag(UID uidTank,DWORD dwFlagType);

/** 设置标识状态
@param  UID uidTank	: 载具UID
@param   DWORD dwFlagType:	标识类型参考 EMTankBoxFlag
@param   bool bFlag :要设置标识状态,true：是，false:否
@return  bool: true：成功，false:失败
*/
TANK_API  bool				SetTankFlag(UID uidTank,DWORD dwFlagType,bool bFlag);

/** 加入乘客
@param  UID uidTank	: 载具UID
@param   DWORD dwPlayerID		:玩家ID
@param   DWORD dwIndex		: 序号(1~容量),为0表示自动分配序号
@return  DWORD 实际序号,为0表示失败
*/
TANK_API DWORD				TankAddPerson(UID uidTank,DWORD dwPlayerID,DWORD dwIndex = 0);

/** 删除乘客
@param  UID uidTank	: 载具UID
@param   DWORD dwIndex		: 序号(1~容量)
@param   bMoveInstant		: 乘客出来时是否瞬移 0标示 false
@param   nX,nY				: 瞬移坐标
@return  bool 是否成功
*/
TANK_API bool				TankRemovePerson(UID uidTank,DWORD dwIndex, bool bMoveInstant = false, LONG nX = 0, LONG nY = 0);


/** 删除乘客
@param  UID uidTank	: 载具UID
@param   DWORD dwPlayerID		:玩家ID
@return  bool 是否成功
*/
TANK_API bool				TankRemoveByDBID(UID uidTank,DWORD dwPlayerID);

/** 取得乘客数据
@param  UID uidTank	: 载具UID
@param   DWORD dwIndex		: 序号(1~容量)
@return  乘客数据
*/
TANK_API const STankNodeData * GetTankData(UID uidTank,DWORD dwIndex);

/** 取得司机数据
@param  UID uidTank	: 载具UID
@return  乘客数据
*/
TANK_API const STankNodeData * GetTankDriver(UID uidTank);

/** 放出乘客
@param  UID uidTank	: 载具UID
@param   int nType: 放出方式 参考 EMTankPutOutMemberType
@return  
*/
TANK_API bool				PutOutTankMember(UID uidTank,int nType);

/** 设定主人角色ID,为0是无主人
@param  UID uidTank	: 载具UID
@param  BYTE nOwnerType		: 主人类型
@return  
*/
TANK_API void				SetTankOwnerID(UID uidTank,DWORD dwOwnerID, BYTE nOwnerType, DWORD dwWorldID=0, DWORD dwCountryID=0);

/** 取得主人角色ID,为0是无主人
@param  UID uidTank	: 载具UID
@param  BYTE &nOwnerType	: 主人类型
@return  
*/
TANK_API DWORD				GetTankOwnerID(UID uidTank,BYTE &nOwnerType);

/** 设定载具存活时间(秒),为0时无限
@param  UID uidTank	: 载具UID
@return  
*/
TANK_API void				SetTankLifeTime(UID uidTank,DWORD dwLifeTime);

/** 取得载具存活时间(秒),为0时无限
@param  UID uidTank	: 载具UID
@return  
*/
TANK_API DWORD				GetTankLifeTime(UID uidTank);

/** 取得指定坐位乘客指定序号的技能权限
@param  UID uidTank	: 载具UID
@param   DWORD dwIndex		: 坐位序号(1~容量)
@param   BYTE nSkillIndex	: 技能序号(1~5)
@return  是否有权限
*/
TANK_API bool				CanUseTankSkill(UID uidTank,DWORD dwIndex,BYTE nSkillIndex);

/** 
@name                   : 让载具停止移动
@param uidTank              : 载具UID
*/
TANK_API void				TankStop( UID uidTank);

/** 
@name                   : 让载具移动
@param uidTank              : 载具UID
*/
TANK_API void				TankMove( UID uidTank,int x,int y );


///////////// 人物实体载具功能控制部件 __IPersonTankPart /////////////////////////////////////////////////////////////


///////////// 载具抓怪物管理 ITankCatchManager /////////////////////////////////////////////////////////////
/**  取得载具管理对象
@param   
@return  载具管理对象
*/
TANK_API ITankManager *		GetTankManager(void);

// tolua_end


#endif // __I_GameTankAPI_H__