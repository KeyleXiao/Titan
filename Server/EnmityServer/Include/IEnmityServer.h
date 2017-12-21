/*******************************************************************
** 文件名:	e:\Rocket\Server\EnmityServer\Include\IEnmityServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/6/1 17:43
** 版  本:	1.0
** 描  述:	仇恨服务器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IEntity.h"
#include "DEnmityServer.h"

////////////////////////////// 仇恨部件 ////////////////////////////
struct __IEnmityPart : public __IEntityPart
{
	/** 取得团队ID
	@param   
	@param   
	@return  
	*/
	virtual DWORD				GetEnmityTeamID(void) = NULL;

	/** 设置团队ID
	@param   
	@param   
	@return  
	*/
	virtual void				SetEnmityTeamID(DWORD dwTeamID) = NULL;

	/** 取得最高仇恨的敌人
	@param   
	@param   
	@return  
	*/
	virtual UID 				GetTopEnemy(void) = NULL;

	/** 取得友人
	@param   
	@param   
	@return  
	*/
	virtual UID			GetOneFriend(void) = NULL;

	/** 取得某生物的仇恨值，无法取到友好度
	@param   
	@param   
	@return  如果没有，返回0
	*/
	virtual int					GetEnmityValue(UID uidEnmity) = NULL;

	/** 九宫里的玩家或添或减
	@param   
	@param   
	@return  
	*/
	virtual void				OnAdd9GridPersonQty(int nCurQty, int nPreQty) = NULL;

	///////////////////////////////////////////////////////////////////
	/** 增加仇恨值
	@param   uidEnemy：敌人
	@param   nValue  ：大于0表示增加仇恨值，小于0表示减少仇恨值
	@param   
	@return  
	*/
	virtual bool				AddEnmityValue(UID uidEnemy, int nValue, bool bNotifyAI = true) = NULL;

	// 增加广播仇恨值
	virtual bool				AddBroadEnmityValue(UID uidEnemy, int nValue) = NULL;

	/** 增加友好度
	@param   uidFriend：友人
	@param   nValue   ：大于0表示增加友好度，小于0表示减少友好度
	@return  
	*/
	virtual bool				AddFriendValue(UID uidFriend, int nValue) = NULL;

	/** 移除敌人
	@param   
	@param   
	@return  
	*/
	virtual bool				RemoveOneEnemy(UID uidEnemy) = NULL;	

	/** 移除友人
	@param   
	@param   
	@return  
	*/
	virtual bool				RemoveOneFriend(UID uidFriend) = NULL;

	///////////////////////////////////////////////////////////////////
	/** 按点数增加/减少仇恨列表中所有的仇恨值
	@param   nEnmity ：大于0表示增加，小于0表示减少
	@param   
	@return  
	*/
	virtual bool				BatchAddEnmity(int nEnmity) = NULL;

	/** 直接将仇恨列表中所有仇恨值设置为某个点数
	@param   nEnmity ：只能大于0
	@param   
	@return  
	*/
	virtual bool				BatchSetEnmity(int nEnmity) = NULL;

	/** 直接将仇恨列表中所有仇恨值设为当前百分多少
	@param   nPercent： 大于0表示增加，小于0表示减少
	@param   
	@return  
	*/
	virtual bool				BatchPercentSet(int nPercent) = NULL;

	/** 设置仇恨值， 如果超过当前最高仇恨比，则直接攻击，不受OP规则
	@param   uidEnemy：施加者
	@param   nValue  ：必须大于0
	@return  
	*/
	virtual bool				SetEnmityValue(UID uidEnemy, int nEnmity) = NULL;

	/** 从pSrcCreature生物复制整个仇恨列表插入到自已的仇恨列表，同理友好列表也会复制
	@param   
	@param   
	@return  
	*/
	virtual bool				FromCopy(UID uidSrcCreature) = NULL;

	/** 从pSrcCreature生物复制整个仇恨列表插入到自已的仇恨列表，但不复制相应仇恨值
	@param 对大于0的以100处理，小于0的以-100处理，等于0的按0处理
	@param   
	@return  
	*/
	virtual bool				FromCopyToEx(UID uidSrcCreature) = NULL;

	/** 从pSrcCreature生物复制最高仇恨项插入到自已的仇恨列表中
	@param   bForceTop ：是否强制设为最高仇恨
	@param   
	@return  
	*/
	virtual bool				FromCopyTopEnemy(UID uidSrcCreature, bool bForceTop) = NULL;

	/** 清空某生物仇恨列表，同理友好列表也会清空
	@param   
	@param   
	@return  
	*/
	virtual bool				CleanEnemyList(bool bNotifyAI = true) = NULL;

	///////////////////////////////////////////////////////////////////
	/** 启用/取消某生物的仇恨仇恨传播效果
	@param   bRadiate ：是否允许传播
	@param   
	@return  
	*/
	virtual void				OpenCampRadiate(bool bRadiate) = NULL;

	/** 启用/取消某生物的团队仇恨传播效果
	@param   bRadiate ：是否允许传播
	@param   
	@return  
	*/
	virtual void				OpenTeamRadiate(bool bRadiate) = NULL;

	/** 是否打开仇恨列表管理
	@param   
	@param   
	@return  
	*/
	virtual void				OpenEnemyList(bool bOpenFlag) = NULL;

	///////////////////////////////////////////////////////////////////
	/** 能否攻击，＊＊＊＊只适用于怪物攻击，人物攻击取PK部件判断
	@param   
	@param   
	@return  
	*/
	virtual bool				CanAttack(/*ICreature*/ __IEntity * pTargetCreature) = NULL;

	/** 能否治疗，＊＊＊＊只适用于怪物攻击，人物攻击取PK部件判断
	@param   
	@param   
	@return  
	*/
	virtual bool				CanCure(/*ICreature*/ __IEntity * pTargetCreature) = NULL;

	///////////////////////////////////////////////////////////////////
	/** 获得当前状态
	@param   
	@param   
	@return  
	*/
	virtual int					GetCurEnmityState(void) = NULL;

	/** 切换仇恨状态
	@param   
	@param   
	@return  
	*/
	virtual void				GotoEnmityState(int nState, LPCSTR pContext, int nLen) = NULL;

	/** 取得搜敌人模式
	@param   
	@param   
	@return  
	*/
	virtual int					GetCurCreepMode(void) = NULL;

	/** 切换搜敌人模式
	@param   
	@param   
	@return  
	*/
	virtual void				GotoCreepMode(int nMode) = NULL;

	///////////////////////////////////////////////////////////////////
	/** 动态增加阵营策略
	@param   
	@param   
	@return  
	*/
	virtual void				AddCampStrategyFlag(DWORD dwStrategyFlag) = NULL;

	/** 动态移除阵营策略
	@param   
	@param   
	@return  
	*/
	virtual void				RemoveCampStrategyFlag(DWORD dwStrategyFlag) = NULL;

	/** 取得所有敌人
	@param   pEnemyArray ： 用来返回所有敌人的列表，如果填空，返回敌人个数
	@param   nCount：IN:数组大小；OUT：敌人个数
	@return  成功返回true,失败返回：false，原因一般是数组容易不够
	*/
	virtual bool				GetAllEnemy(SOutputEnemy * pEnemyArray, int &nCount) = NULL;

	/** 获得是否有某阵营策略
	@param   
	@param   
	@return  
	*/
	virtual bool				IsCampStrategy(int nStrategyFlag) = NULL;

    virtual void                restore(){};
};

//////////////////////////////仇恨服务器////////////////////////////
struct IEnmityServer
{
	/** 执行Service的on_stop
	@return  
	*/
    virtual void                onStop(void) = NULL;

	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void				release(void) = NULL;

	/** 给实体创建仇恨部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual __IEnmityPart* CreatePart(void) = NULL;
};
/** @} */

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENMITYSERVER_STATIC_LIB)		/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(EnmityServer))
	extern "C" IEnmityServer * CreateEnmityServer(void);
	#	define	CreateEnmityServerProc	CreateEnmityServer
#else														/// 动态库版本
	typedef IEnmityServer * (RKT_CDECL *procCreateEnmityServer)(void);
	#	define	CreateEnmityServerProc	DllApi<procCreateEnmityServer>::load(MAKE_DLL_NAME(EnmityServer), "CreateEnmityServer")
#endif
