/*******************************************************************
** 文件名:	ITankServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2008/10/15
** 版  本:	1.0
** 描  述:	载具系统服务器
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "DTankModule.h"

#ifndef __I_TankServer_H__
#define __I_TankServer_H__


struct __ITankBoxPart;
struct __IPersonTankPart;
struct ITank;
struct ITankServer;
struct STankSkillData;

//////////////////////////////////////////////////////////////////////////


// tolua_begin

////////////////////////////载具实体承载部件//////////////////////////////////
struct __ITankBoxPart : public __IEntityPart
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
	virtual const STankNodeData * GetListData(void) = NULL;

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

	/** 是否可在有效距离内载入乘客
	@param   __IPlayerRole * pPerson : 乘客
	@return  bool 是否成功
	*/
	virtual bool				Reachable(__IPlayerRole * pPerson) = NULL;

	/** 检查标识状态
	@param   DWORD dwFlagType:	标识类型参考 EMTankBoxFlag
	@return  bool: true：是，false:否
	*/
	virtual  bool				GetFlag(DWORD dwFlagType) = NULL;

	/** 设置标识状态
	@param   DWORD dwFlagType:	标识类型参考 EMTankBoxFlag
	@param   bool bFlag :要设置标识状态,true：是，false:否
	@return  bool: true：成功，false:失败
	*/
	virtual  bool				SetFlag(DWORD dwFlagType,bool bFlag) = NULL;

	/** 设置成员强退状态
	@param   DWORD dwIndex		: 序号(1~容量)
	@param   bool bFlag :要设置标识状态,true：是，false:否
	@return  bool: true：成功，false:失败
	*/
	virtual  bool				MemberForceExit(DWORD dwIndex,bool bFlag) = NULL;

	/** 取得标识状态数字
	@return  DWORD 标识状态数字
	*/
	virtual  DWORD				GetFlagValue(void) = NULL;

	/** 取得标识状态名
	@return  LPCSTR 标识状态名
	*/
	virtual  LPCSTR				GetFlagName(DWORD dwFlagType) = NULL;

	/** 加入乘客
	@param   __IPlayerRole * pPerson	: 乘客
	@param   DWORD dwIndex		: 序号(1~容量),为0表示自动分配序号
	@param   bMoveInstant		: 是否瞬移
	@return  DWORD 实际序号,为0表示失败
	*/
	virtual DWORD				AddPerson(__IEntity * pPerson, DWORD dwIndex = 0, bool bMoveInstant = false) = NULL;

	/** 删除乘客
	@param   DWORD dwIndex		: 序号(1~容量)
	@param   bMoveInstant		: 乘客出来时是否瞬移
	@param   ptNew				: 瞬移坐标
	@return  bool 是否成功
	*/
	virtual bool				Remove(DWORD dwIndex, bool bMoveInstant = false, POINT* ptNew = NULL) = NULL;

	/** 删除乘客
	@param   __IPlayerRole * pPerson : 乘客
	@return  bool 是否成功
	*/
	virtual bool				RemoveByPerson(__IEntity * pPerson) = NULL;

	/** 删除乘客
	@param   DWORD dwPlayerID		:玩家ID
	@return  bool 是否成功
	*/
	virtual bool				RemoveByDBID(DWORD dwPlayerID) = NULL;

	/** 取得乘客数据
	@param   DWORD dwIndex		: 序号(1~容量)
	@return  乘客数据
	*/
	virtual const STankNodeData * Get(DWORD dwIndex) = NULL;

	/** 取得乘客数据
	@param   __IPlayerRole * pPerson : 乘客
	@return  乘客数据
	*/
	virtual const STankNodeData * GetByPerson(__IEntity * pPerson) = NULL;


	/** 取得司机数据
	@return  乘客数据
	*/
	virtual const STankNodeData * GetDriver(void) = NULL;

	/** 取得指定位置的骨骼名
	@param   DWORD dwIndex		: 序号(1~容量)
	@return  骨骼名
	*/
	virtual const char *		GetBoneName(DWORD dwIndex) = NULL;

	/** 设定司机
	@param   DWORD dwIndex		: 序号(1~容量)
	@return  bool 是否成功
	*/
	virtual bool				SetDriver(DWORD dwIndex) = NULL;

	/** 交换位置
	@param   DWORD dwIndex1		: 序号1(1~容量)
	@param   DWORD dwIndex2		: 序号2(1~容量)
	@return  bool 是否成功
	*/
	virtual bool				ExchangeIndex(DWORD dwIndex1,DWORD dwIndex2) = NULL;

	/** 放出乘客
	@param   int nType: 放出方式 参考 EMTankPutOutMemberType
	@param   uidOperator ： 操作者（如杀死载具的凶手）
	@return  
	*/
	virtual bool				PutOutMember(int nType, UID uidOperator = INVALID_UID) = NULL;

	/** 发送载具数据到客户端
	@param  bool bCreate=false :是否是创建,是时客户端会拉入载具中
	@return  
	*/
	virtual void				SendDataToClient(__IPlayerRole * pPerson, bool bCreate=false) = NULL;

	/** 周围广播载具数据到客户端
	@param  bool bCreate=false :是否是创建,是时客户端会拉入载具中
	@return  
	*/
	virtual void				BroadcastToClient(POINT ptMapTile, bool bCreate=false) = NULL;

	/** 广播载具数据到所有乘客客户端
	@param  bool bCreate=false :是否是创建,是时客户端会拉入载具中
	@return  
	*/
	virtual void				BroadcastToMember(bool bCreate=false) = NULL;

	/** 载具死亡
	@param uidMurderer ： 凶手UID
	@return  
	*/
	virtual void				OnTankDie(UID uidMurderer) = NULL;

	/** 载具解散
	@return  
	*/
	virtual void				OnTankRemove(void) = NULL;

	/** 载具走完一段路经,用来巡逻
	@return  
	*/
	virtual void				OnTankDoneMove(void) = NULL;

	/** 设定主人角色ID,为0是无主人
	@return  
	*/
	virtual void				SetOwnerID(DWORD dwOwnerID, BYTE nOwnerType, DWORD dwWorldID=0, DWORD dwCountryID=0) = NULL;

	/** 取得主人角色ID,为0是无主人
	@return  
	*/
	virtual DWORD				GetOwnerID(BYTE &nOwnerType) = NULL;
	/** 取得主人类型 EMTankOwnerType
	@return  
	*/
	virtual BYTE				GetOwnerType(void) = NULL;
	
	/** 取得主人游戏世界ID
	@return  
	*/
	virtual DWORD				GetOwnerWorldID(void) = NULL;
	
	/** 取得主人国家ID
	@return  
	*/
	virtual DWORD				GetOwnerCountryID(void) = NULL;

	/** 设定载具存活时间(秒),为0时无限
	@return  
	*/
	virtual void				SetTankLifeTime(DWORD dwLifeTime) = NULL;

	/** 取得载具存活时间(秒),为0时无限
	@return  
	*/
	virtual DWORD				GetTankLifeTime(void) = NULL;

	/** 取得指定坐位乘客指定序号的技能权限
	@param   DWORD dwIndex		: 坐位序号(1~容量)
	@param   BYTE nSkillIndex	: 技能序号(1~5)
	@return  是否有权限
	*/
	virtual bool				CanUseSkill(DWORD dwIndex,BYTE nSkillIndex) = NULL;

	////////////// 巡逻AI接口 ////////////////////////////////////////////////////////////

	/** 启动巡逻 pPatrolInfo 为Null时,如原来有路线，就按原路线
	@param   STankPatrolSchemeInfo * pPatrolInfo :路线数据
	@param   int nDelayTime			: 发车延迟时间 毫秒
	@return  true:成功，false:失败
	*/
	virtual bool				DoPatrol(const STankPatrolSchemeInfo * pPatrolInfo,int nDelayTime=0) = NULL;

	/** 正在巡逻
	@return  true:巡逻，false:没巡逻
	*/
	virtual bool				IsBusying(void) = NULL;

	/** 取消巡逻
	@return  
	*/
	virtual bool				Cancel(void) = NULL;

	/** 切换载具控制模式 
	@return  
	*/
	virtual bool				SetControlMode(DWORD dwControlMode=0) = NULL;

	/** 取载具控制模式
	@return  控制模式
	*/
	virtual DWORD				GetControlMode(void) = NULL;

	/** 能够进入载具的人物列表
	@param   pArrayPDBID：能够进入载具的人物列表
	@param   nCount 　　：个数
	@return  
	*/
	virtual void				AddEnterPersonList(DWORD * pArrayPDBID, int nCount) = NULL;

	/** 取消进入载具的某些人物
	@param   
	@param   
	@return  
	*/
	virtual void				CancelEnterPersonList(DWORD * pArrayPDBID, int nCount) = NULL;

	/** 能否进入载具
	@param   
	@param   
	@return  
	*/
	virtual bool				CanEnter(__IPlayerRole * pPerson, bool bPersonInvited = false) = NULL;

	/** 传送载具
	@param   __IPlayerRole * pPerson :导致载具传送的人物,用来处理排除传载具上人物
	@param   SEventCreatureChangeLoc_S * pData :传送数据
	@return   bool :成功否
	*/
	virtual bool				OnChangeLoc(__IPlayerRole * pPerson, LPCSTR pEventContext, int nEventContextLen) = NULL;

	/** 更新载具技能使用时间
	*/
	virtual void				UpdateTankSkillInfo(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = NULL;

	/** 为载具技能添加冷却时间
	*/
	virtual void				AddSkillFreezeTime(__IPlayerRole *pPerson) = NULL;
};

////////////////////////////人物实体载具功能控制部件//////////////////////////////////
struct __IPersonTankPart : public __IEntityPart
{

	/** 进入载具事件,供载具承载部件调用,如已进入其它载具,强拉入
	@param	 ITank * pTank  :要进入的载具
	@return	  
	*/
	virtual void				OnEnterTank(ITank * pTank) = NULL;

	/** 从载具中出来事件,供载具承载部件调用
	@return	 
	*/
	virtual void				OnExitTank(void) = NULL;

	/** 驾驶事件,供载具承载部件调用
	@param   bool bFlag:true 取得驾驶，false:失去驾驶
	@return	
	*/
	virtual void				OnDriver(bool bFlag) = NULL;

	/** 位置改变事件,供载具承载部件调用
	@return	 
	*/
	virtual void				OnIndexChanged(void) = NULL;

	/** 确认请求
	@param	DWORD	dwSpare:备用数字
	@param   void * pData :附加数据
	@param   int nLen	:附加数据len
	@return	 
	*/
	virtual void				OnDriverConfirm(DWORD dwSpare,void * pData, int nLen) = NULL;

	/** 取得乘坐的载具实体,没有坐上为NULL
	@return	 ITank * pTank  :进入的载具
	*/
	virtual  ITank *			GetTank(void) = NULL;

	/** 是否开启乘坐载具功能
	@param   bool bFlag:true 开启，false:不开启
	@return	  bool 返回状态
	*/
	virtual bool				Enabled(bool bFlag) = NULL;

	/** 是否开启乘坐载具功能
	@return	  bool 返回状态
	*/
	virtual bool				IsEnabled(void) = NULL;

	/** 是否乘坐状态
	@param   bool bFastMode:true 用快速模式UID判断，false:用取载具实体判断
	@return	  bool 返回状态
	*/
	virtual bool				IsOnTank(bool bFastMode=false) = NULL;

	/** 是否司机状态
	@param   bool bFastMode:true 用快速模式UID判断，false:用取载具实体判断
	@return	  bool 返回状态
	*/
	virtual bool				IsOnDriver(bool bFastMode=false) = NULL;

	/** 检查是否司机状态,查载具司机是否是自己,慢
	@return	  bool 返回状态
	*/
	virtual bool				CheckOnDriver(void) = NULL;

	/** 取得人物在载具的技能ID
	@param   BYTE nSkillIndex	: 技能序号 1~5
	@return  long 技能ID
	*/
	virtual	long				GetTankSkill(BYTE nSkillIndex) = NULL;

	/** 取得定序号的技能权限
	@param   BYTE nSkillIndex	: 技能序号(1~5)
	@return  是否有权限
	*/
	virtual bool				CanUseSkill(BYTE nSkillIndex) = NULL;

	/** 从载具中拉下来,外部调用功能
	@return	 成功否
	*/
	virtual bool				ExitTank(void) = NULL;

	/** 强制从载具中拉下来,如死亡，下线处理等
	@return	 成功否 
	*/
	virtual bool				ForceExitTank(void) = NULL;

	/** 读条进入载具
	@param   ITank * pEnterTank		: 载具
	@param   DWORD dwIndex			: 座号
	@return  
	*/
	virtual bool				ProgressEnterTank(ITank * pEnterTank, DWORD dwIndex=0) = NULL;

	/**  发送消息到客户端
	@param  __IPlayerRole *pPerson :指定的客户端
	@param  bool bCreate=false :是否是创建,是时客户端会拉入载具中
	@return  
	*/
	virtual void				SendSelfData(__IEntity *pPerson,bool bCreate=false) = NULL;

	/**  载具上传送
	@param DWORD dwNewZoneID	: 场景ID，如果是怪物，则忽略
	@param POINT ptHere			: 停留的位置	
	@param int nTransportType	: 传送类型:tTransportType_Chunnel等
	@param bool bCoercive		: 是否强制传送
	@return  
	*/
	virtual void				OnTankTransport(DWORD dwNewZoneID,POINT ptHere,int nTransportType,bool bCoercive) = NULL;

	/** 是否正在载具上传送
	@return	 
	*/
	virtual bool				IsOnTankTransport(void) = NULL;

	/** 保存载具传送数据
	@param  bool bSave :true ,保存数据,false,清除数据
	@return	 
	*/
	virtual void				SaveTankTransportData(bool bSave) = NULL;

	/** 请求从载具中下来,外部调用功能
	@return
	*/
	virtual void				RequestExitTank(void) = NULL;

	/** 是否在载具上允许被传送，主要检测载具配置的可响应传送标识：TANK_OPINION_ENABLETRANS
	@return  
	*/
	virtual bool				CheckCanTransportOnTank(void) = NULL;

	/** 传送从载具中拉下来,外部调用功能
	@return	 成功否
	*/
	virtual bool				ExitTransportTank(void) = NULL;

	/** 快捷栏数据，设置PK载具ID
	@return	 
	*/
	virtual void				SetActionPKTankID(DWORD dwTankID) = NULL;	

	/** 更新快捷栏数据
	@return	 
	*/
	virtual void				SetActionData(DWORD dwIndex, DWORD dwSkillID) = NULL;

	/** 快捷栏存数据库
	@return	 
	*/
	virtual void				SaveActionData() = NULL;

	/** 申请数据库里面的快捷栏数据
	@return	 
	*/
	virtual void				RequestActionData() = NULL;

	/** 取内存中快捷键数据
	@return	 
	*/
	virtual void				OnReturnActionData(const STankSkillData* skillData) = NULL;

	/** 是否在pk载具内
	@return	 
	*/
	virtual bool				IsPKTank() = NULL;

};

// 载具管理
struct ITankManager
{
	/** 载具锁住怪
	@param   uidTank	：载具UID
	@param   uidMonster ：怪UID
	@return  
	*/
	virtual bool		LockMonster(LONGLONG uidTank, LONGLONG uidMonster) = NULL;

	/** 载具释放被锁住怪
	@param   uidTank	：载具UID
	@param   uidMonster ：怪UID，0.所有怪
	@return  
	*/
	virtual bool		UnLockMonster(LONGLONG uidTank, LONGLONG uidMonster=0) = NULL;

	/** 是否已锁怪
	@param   uidCreature	：生物UID
	@return  
	*/
	virtual bool		IsLockMonster(LONGLONG uidCreature) = NULL;
};

// 载具系统模块
struct ITankServer
{
	/** 执行Service的on_stop
	@return  
	*/
    virtual void                    onStop(void) = NULL;

	// 释放对象
	virtual void                    release(void) = NULL;

	// 创建一个载具系统部件
	virtual __ITankBoxPart* CreateTankBoxPart(void) = NULL;

	// 创建一个人物实体载具功能控制部件
	virtual __IPersonTankPart* CreatePersonTankPart(void) = NULL;

// tolua_end

	/**  巡逻信息列表指向列表最前端
	@return  
	*/
	virtual void						MoveHead(void) = NULL;

	/**  巡逻信息列表指向列表最后端
	@return  
	*/
	virtual void						MoveEnd(void) = NULL;

	/**  巡逻信息列表指向列表下一个
	@return  @return true: 没到结尾 false:到结尾
	*/
	virtual bool						MoveNext(void) = NULL;

	/**  取得巡逻信息列表当前指向的数据
	@return  STankPatrolSchemeInfo * :巡逻脚本定义数据
	*/
	virtual const STankPatrolSchemeInfo * GetListData(void) = NULL;


// tolua_begin


	/** 取得载具指定座位技能权限
	@param   BYTE nRightValue	: 技能权限
	@param   BYTE nSkillIndex	: 技能序号 1~5
	@return  bool
	*/
	virtual	bool CheckSkillRight(BYTE nRightValue,BYTE nSkillIndex) = NULL;


	/** 取指定pk载具的技能信息
	@param   lTankID: 载具id
	@return  载具技能信息
	*/
	virtual const SPKTankSkillSchemeInfo*	GetPKTankSkillSchemeInfo(long lTankID) = NULL;

	/** 取载具管理对象
	@return  
	*/
	virtual ITankManager *					GetTankManager(void) = NULL;
};

// tolua_end



//////////////////////////////////////////////////////////////////////////


//验证:Country 子系统
#if defined(_LIB) || defined(TANKSERVER_STATIC_LIB)/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(TankServer))
extern "C" ITankServer* CreateTankServer(void);
#	define	CreateTankServerProc	CreateTankServer
#else /// 动态库版本
typedef ITankServer* (RKT_STDCALL *procCreateTankServer)(void);
#	define	CreateTankServerProc	DllApi<procCreateTankServer>::load(MAKE_DLL_NAME(TankServer), "CreateTankServer")
#endif



#endif // __I_TankServer_H__