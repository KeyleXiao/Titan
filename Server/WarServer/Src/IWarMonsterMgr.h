/*******************************************************************
** 文件名: IWarMonsterMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/15
** 版  本: 1.0
** 描  述: 战场怪物管理接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"

class IMonsterExec;
struct CreateMonsterInfo;
struct CreateNpcContext;
struct INPCService;

class IWarMonsterMgr : public __IWarMgr
{
public:
	// 加载刷怪配置脚本
	virtual bool LoadMonsterScheme() = 0;

	/** 获取战场指定类型的怪物UID
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual DWORD getUIDListByMonsterType(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize) = 0;

	virtual IMonsterExec* getWarMonsterExecByType(EWarMonsterExec WarMonsterExec) = 0;

	// 获得本战场内怪物索引类型
	virtual EWarMonsterExec	getWarMonsterType(int nIndex) = 0;

	// 获取怪物类型
	virtual EWarMonsterExec getWarMonsterTypeByUID(UID uid) = 0;

	// 获取已创建的怪物信息
	virtual CreateMonsterInfo* getWarMonsterInfoByUID(UID uid) = 0;

	// 获取已创建的怪物列表
	virtual vector<UID> getWarMonsterList() = 0;

	// 获取怪物最大数
	virtual int getMaxMonsterCountLimit() = 0;

	// 停止战场怪物AI
	virtual void stopMonsterAI() = 0;

	// 释放战场怪物资源
	virtual void releaseMonsterRes() = 0;

	// 玩家断线重连
	virtual void onActorContinuePlay(int clientID) = 0;

	// 创建NPC
	virtual UID createNpc(int nMonsterID, Vector3 vLoc, EWarMonsterExec eType, CreateNpcContext* pNpcContext, int nRefreshIndex, int nLocalIndex, INPCService*pNpcService = NULL, bool bRandomRefresh = false) = 0;

	// 怪物死亡
	virtual void onMonsterDie(sEntityDieInfo*  pEntityDieInfo) = 0;

	// NPC销毁
	virtual void onNpcDestroy(UID uid) = 0;
};