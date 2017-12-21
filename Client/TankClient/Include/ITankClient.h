/*******************************************************************
** 文件名:	e:\Rocket\Client\TankClient\Include\ITankClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2009/4/20
** 版  本:	1.0
** 描  述:	载具系统客户端
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "DTankModule.h"
#include "IEntity.h"
#include "IClientEntity.h"
#include "DSkillCommon.h"


#ifndef __I_TankClient_H__
#define __I_TankClient_H__


struct ITankBoxPart;
struct IPersonTankPart;
struct ITank;


// tolua_begin


////////////////////////////载具实体承载部件//////////////////////////////////
struct ITankBoxPart : public IEntityPart
{

	/**  指向列表最前端
	*/
	virtual void				MoveHead(void) = NULL;

	/**  指向列表最后端
	*/
	virtual void				MoveEnd(void) = NULL;

	/**  指向列表下一个
	@return  @return true: 没到结尾 false:到结尾
	*/
	virtual bool				MoveNext(void) = NULL;

	/**  取得列表当前指向的数据
	@return  乘客数据
	*/
	virtual const STankClientNodeData * GetListData(void) = NULL;

	/**  取得载具容量
	@return  int 容量大小
	*/
	virtual DWORD				GetMaxCount(void) = NULL;

	/**  取得载具当前人数
	@return  int 人数
	*/
	virtual DWORD				GetCount(void) = NULL;

	/** 是否为空
	@return	  bool 返回状态
	*/
	virtual bool				IsEmpty(void) = NULL;

	/** 是否满了
	@return	  bool 返回状态
	*/
	virtual bool				IsFull(void) = NULL;

	/** 检查标识状态
	@param   DWORD dwFlagType:	标识类型参考 EMTankBoxFlag
	@return  bool: true：是，false:否
	*/
	virtual  bool GetFlag(DWORD dwFlagType) = NULL;

	/** 取得标识状态名
	@return  LPCSTR 标识状态名
	*/
	virtual  LPCSTR GetFlagName(DWORD dwFlagType) = NULL;

	/** 取得乘客数据
	@param   DWORD dwIndex		: 序号(1~容量)
	@return  乘客数据
	*/
	virtual const STankClientNodeData * Get(DWORD dwIndex) = NULL;

	/** 取得乘客数据
	@param   IPerson * pPerson : 乘客
	@return  乘客数据
	*/
	virtual const STankClientNodeData * GetByPerson(IEntity * pPerson) = NULL;

	/** 取得司机数据
	@return  乘客数据
	*/
	virtual const STankClientNodeData * GetDriver(void) = NULL;

	/** 取得司机序号
	@return  司机序号
	*/
	virtual DWORD GetDriverIndex(void) = NULL;


	/** 司机移动
	@param  POINT ptTarget :目标点
	@param  POINT* ptPathList :路径
	@param  int nPathLen :路径长度
	@return  动作信息
	*/
	virtual void DriverMove(POINT ptTarget,POINT* ptPathList,int nPathLen) = NULL;


	/** 取载具控制模式
	@return  控制模式
	*/
	virtual DWORD				GetControlMode(void) = NULL;

	/** 取得主人角色ID,为0是无主人
	@return  
	*/
	virtual DWORD	GetOwnerID(BYTE &nOwnerType) = NULL;

	/** 能否进入载具
	@param   
	@param   
	@return  
	*/
	virtual bool				CanEnter(IEntity * pPerson) = NULL;

	/** 取得载具世界ID
	@return 载具世界ID
	*/
	virtual DWORD GetWorldID(void) = NULL;
};

////////////////////////////人物实体载具功能控制部件//////////////////////////////////
struct IPersonTankPart : public IEntityPart
{

	/** 取得乘坐的载具实体,没有坐上为NULL
	@return	 ITank * pTank  :进入的载具
	*/
	virtual  ITank *			GetTank(void) = NULL;

	/** 取得乘坐的载具实体承载部件,没有坐上为NULL
	@return	 ITankBoxPart * pPart  :承载部件
	*/
	virtual  ITankBoxPart *		GetTankBoxPart(void) = NULL;

	/** 取得乘坐的载具位置
	@return	 DWORD :坐在载具的位置
	*/
	virtual  DWORD				GetTankIndex(void) = NULL;

	/** 取得乘坐的载具UID
	@return	 LONGLONG :载具UID
	*/
	virtual  UID			GetTankUID(void) = NULL;

	/** 是否开启乘坐载具功能
	@return	  bool 返回状态
	*/
	virtual bool				IsEnabled(void) = NULL;

	/** 是否乘坐状态
	@return	  bool 返回状态
	*/
	virtual bool				IsOnTank(void) = NULL;

	/** 是否司机状态
	@return	  bool 返回状态
	*/
	virtual bool				IsOnDriver(void) = NULL;

	/** 请求进入指定载具
	@param   LONGLONG uidTank: 载具UID
	@param   DWORD	dwIndex :坐位序号,0为自动分号
	@return	  bool 成功否
	*/
	virtual bool				EnterTank(UID uidTank,DWORD dwIndex=0) = NULL;

	/** 请求退出当前载具
	@return	  bool 成功否
	*/
	virtual bool				ExitTank(void) = NULL;

	/** 退出当前载具
	@return	  bool 成功否
	*/
	virtual bool				PutDownTank(void) = NULL;

	/** 邀请玩家请进入当前载具
	@param   DWORD dwPlayerID :玩家角色ID
	@param   DWORD	dwIndex :坐位序号,0为自动分号
	@return	  bool 成功否
	*/
	virtual bool				InviteEnterTank(UID uidPlayerID,DWORD dwIndex=0) = NULL;

	/** 邀请玩家请退出当前载具
	@param   DWORD dwPlayerID :玩家角色ID
	@return	  bool 成功否
	*/
	virtual bool				InviteExitTank(DWORD dwPlayerID) = NULL;

	/** 交换位置,如是乘客自己 dwIndex1为自己的位置,dwIndex2为新的位置
	@param   DWORD dwIndex1		: 序号1(1~容量)
	@param   DWORD dwIndex2		: 序号2(1~容量)
	@return  bool 是否成功
	*/
	virtual bool				ExchangeIndex(DWORD dwIndex1,DWORD dwIndex2) = NULL;

	/** 司机确认
	@param   BYTE	nResult;		 结果	 0:取消,1:确定
	@param   BYTE	nType;			 类型
	@param   LONGLONG uidTank;		 载具uid
	@param   DWORD	dwPlayerID;		 操作人物
	@param   DWORD	dwSpare;		 数据
	@return	  bool 成功否
	*/
	virtual bool				DriverConfirm(BYTE nResult,BYTE nType,UID uidTank,UID uidPlayerID,UID uidSpare) = NULL;


	/**  放出所有乘客,自己必需为司机
	@param   int nType: 放出方式 参考 EMTankPutOutMemberType
	@return  bool 成功否
	*/
	virtual bool				PutOutMember(int nType=TANK_OUTTYPE_DRIVER) = NULL;

	/** 检查标识状态
	@param   DWORD dwFlagType:	标识类型参考 EMTankBoxFlag
	@return  bool: true：是，false:否
	*/
	virtual  bool				GetTankFlag(DWORD dwFlagType) = NULL;

	/** 设置标识状态,自己必需为司机
	@param   DWORD dwFlagType:	标识类型参考 EMTankBoxFlag
	@param   bool bFlag :要设置标识状态,true：是，false:否
	@return  bool: true：成功，false:失败
	*/
	virtual  bool				SetTankFlag(DWORD dwFlagType,bool bFlag) = NULL;

	/** 取得人物在载具的技能ID
	@param   BYTE nSkillIndex	: 技能序号 1~5
	@return  long 技能ID
	*/
	virtual	long				GetTankSkill(BYTE nSkillIndex) = NULL;


	/** 使用载具指定序号技能,并会修改当前选中技能序号
	@param   long x				：x
	@param   long y				：y
	@param   BYTE nSkillIndex	: 技能序号 1~5
	@param   UID TargetUID	：实体对象uid
	*/
	virtual	bool				UseTankIndexSkill(long x,long y,BYTE nSkillIndex,UID TargetUID) = NULL;

	/** 设定当前选中载具技能序号
	@param   BYTE nSkillIndex	: 技能序号(1~5)
	@return  
	*/
	virtual void				SetSelectSkillIndex(BYTE nSkillIndex) = NULL;

	/** 取得当前选中载具技能序号
	@param   
	@return  BYTE nSkillIndex	: 技能序号(1~5)
	*/
	virtual BYTE				GetSelectSkillIndex(void) = NULL;

	/** 切换载具控制模式 
	@return  
	*/
	virtual bool				SetControlMode(DWORD dwControlMode=0) = NULL;

// tolua_end

	/** 设定uid载具对象	
	@param	LONGLONG uidTank	: 载具UID
	@return	
	*/
	virtual void				SetTankEntity(UID uidTank) = NULL;

	/** 叫载具行走
	@param  POINT ptTarget :目标点
	@param  POINT* ptPathList :路径
	@param  int nPathLen :路径长度
	@return  
	*/
	virtual bool				DriverTankMove(POINT ptTarget,POINT* ptPathList,int nPathLen) = NULL;

// tolua_begin

	/** 请求部件数据信息
	@param
	*/
	virtual void				RequestData() = NULL;

	/** 请求部件数据信息
	@param
	*/
	virtual bool				IsPKTank() = NULL;

	/** 设置pk载具技能
	@param   
	@param   
	@return  
	*/
	virtual void				SetPKTankSkillMainID(int nPostion, DWORD nSkillID) = NULL;

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


	/** 设置pk载具快捷键信息为脏，需要更新
	@param   
	@param   
	@return  
	*/
	virtual void				SetPKTankActionDirty() = NULL;
};

// 载具系统客户端
struct ITankClient
{

// tolua_end
	/** 创建
	*/
	virtual bool create() = NULL;

	// 释放对象
	virtual void release(void) = NULL;

	// 创建一个载具系统部件
	virtual ITankBoxPart* CreateTankBoxPart(void) = NULL;

	// 创建一个人物实体载具功能控制部件
	virtual IPersonTankPart* CreatePersonTankPart(void) = NULL;

// tolua_begin

	/** 取得载具脚本定义数据
	@param   DWORD dwTankID	: 载具ID
	@param   
	@return  STankSchemeInfo * :载具脚本定义数据
	*/
	virtual	const STankSchemeInfo * GetTankSchemeInfo(DWORD dwTankID) = NULL;


	/**  指向列表最前端
	*/
	virtual void				MoveHead(void) = NULL;

	/**  指向列表最后端
	*/
	virtual void				MoveEnd(void) = NULL;

	/**  指向列表下一个
	@return  @return true: 没到结尾 false:到结尾
	*/
	virtual bool				MoveNext(void) = NULL;

	/**  取得列表个数
	@return  
	*/
	virtual int					GetListCount(void) = NULL;

	/**  取得列表当前指向的数据
	@return  
	*/
	virtual IClientEntity *			GetListData(void) = NULL;

};


// tolua_end



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(TANKCLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(TankClient))
extern "C" ITankClient * CreateTankClient(void);
#	define	CreateTankClientProc	CreateTankClient
#	define	CloseTankClient()
#elif !defined(TANKCLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(TankClient))
API_IMPORT ITankClient * CreateTankClient(void);
#	define	CreateTankClientProc	CreateTankClient
#	define	CloseTankClient()
#else												// 动态加载
typedef ITankClient * (RKT_CDECL *procTankClient)(void);
#	define	CreateTankClientProc	DllApi<procTankClient>::load(MAKE_DLL_NAME(TankClient), "CreateTankClient")
#	define	CloseTankClient()		DllApi<procTankClient>::freelib()
#endif


#endif	//__I_TankClient_H__
