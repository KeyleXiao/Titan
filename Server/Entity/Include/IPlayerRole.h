/*******************************************************************
** 文件名:	IPlayerRole.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家接口
********************************************************************/

#pragma once;

#include "IEntity.h"
#include "IMessageDispatch.h"
#include "NetMessageDef.h"
#include "AOI_Define.h"
#include "IGameSceneService.h"

struct SERVICE_ID;
/// 玩家扮演的角色 
struct __IPlayerRole : public __IEntity
{
	// 发送数据给客户端
	virtual int send2Client( PACKAGE_PTR package, int nFlag = 0) = 0;
	virtual int send2Client( void * data,size_t len) = 0;
    virtual void send2ClientAndOB( PACKAGE_PTR package ) = 0;
	virtual bool sendMessageToClient(BYTE byPartID, BYTE byAction, void * data,size_t len) = 0;
	virtual DWORD getClientID() = 0;
	virtual void setClientID(DWORD clientID) = 0;
	// 取得玩家SERVICE_ID
	virtual SERVICE_ID getServiceID() = 0;
	// 设置玩家SERVICE_ID
	virtual void setServiceID(SERVICE_ID serviceID) = 0;
    // 更改天赋点属性
    virtual void updateTalentPoint( int nPoint, int nCHangeTYpe, BYTE byAddType = 2) = 0;

    // 导出静态数据
    virtual bool deSerializeStaticData(rkt::obuf & out) = 0;

    // 检查是否有对应的权限
    virtual bool checkPrivilege(int nnPrivilege) = 0;

    // 增加权限
    virtual void addPrivilege(int nPrivilege) = 0;

    // 移除权限
    virtual void removePrivilege(int nPrivilege) = 0;

    // 设置游戏状态ACTOR_GAME_STATE
    virtual void setGameState(DWORD dwState) = 0;
    // 获取游戏状态 ACTOR_GAME_STATE
    virtual DWORD getGameState() = 0;

    // 获得玩家角色等级
    virtual int getPlayerLevel() = 0;

    // 获得玩家角色经验
    virtual int getPlayerExp() = 0;

	/** 增加/减少经验值
	@param   nExp:            经验值（负值减少，正值增加）
	@param   wApproach:       消耗原因,0.不需要记录日记
	@param   dwTargetID:      如果有目标者，这个就填入目标者ID
	@param   szLogMark:       传入交易日志，供查询  
	@param   nLogReserve:     日志的保留字段
	@return
	*/
    virtual bool addExp(int nExp, WORD wApproach, DWORD dwTargetID=0, const char * pszRemark=0, int nLogReserve=0) = 0;

    /** 是否可增加或扣除货币
    @param nTicket: 添加的点券，负数表示扣除点卷
    @param nGlod  : 添加的金币，负数表示扣除金币
    @return : 
    */
    virtual bool canAddMoney(int nTicket, int nGlod) = 0;

    /** 增加或扣除货币
    @param nTicket: 添加的点券，负数表示扣除点卷
    @param nGlod  : 添加的金币，负数表示扣除金币
    @param  : nReason 添加的原因（产生OSS_RESOURCE_ADD_RANGE/消耗的途径OSS_RESOURCE_OPT_DEL/转进OSS_RESOURCE_OPT_TRANSIN/转出OSS_RESOURCE_OPT_TRANSOUT）
    @param  : pszRemark 添加的点券日志的备注（记日志用）
    @param  : nSubclassID, 子类ID，购买物品则填物品ID，没有子类默认0
    @param  : nNum 个数，子类是物品ID则填物品个数
	@param  : bAddictLimit，是否考虑沉迷限制
    @ note  : 在添加点券操作提交数据库（包括其他模块提交的）还没有返回前，这时候你提交点券是会失败的
    */
    virtual bool addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark=0, int nSubclassID=0, short nNum=0, bool bAddictLimit=true) = 0;

    /** 切职业
    @param nNewVocID: 新的职业ID
    @param nNewVocID: 新的皮肤ID
    @return : 
    */
    virtual bool changeVoc(int nNewVocID, int nNewSkinID) = 0;
};
