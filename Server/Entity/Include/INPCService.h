/*******************************************************************
** 文件名:	INPCService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/24/2014
** 版  本:	1.0
** 描  述:	

			NPC服务，管理所有非玩家对象，包括怪物和NPC(一个协程)
********************************************************************/

#pragma once

struct EFFECT_CONTEXT;
struct CreateNpcContext;
struct BuffTypeList;
struct EntityBelongContext;

#include "PathFindDef.h"
struct __IEntity;

typedef share_ptr<__IEntity, true>     ENTITY_PTR,NPC_PTR;

/**
	职责: 
	1.管理某个场景的所有怪物，NPC的创建销毁
	2.管理某个场景的所有怪物AI
	3.管理某个场景的所有NPC对话
*/
struct INPCService
{
	// 手动创建NPC
	virtual UID createNPC( int id,Vector3 loc, CreateNpcContext context) = 0;

	// 手动销毁NPC
	virtual bool destroyNPC( UID uid ) = 0;

	// 获取NPC
	virtual NPC_PTR findNPC( UID uid ) = 0;

	//////////////////////////////// IEntity ///////////////////////////////////////////////
	// 取得实体名称
	virtual string getName(UID uid)  = 0;

	// 取得当前所在场景ID
	virtual int getSceneID() = 0;

	// 取得实体位置
	virtual Vector3 getLocation(UID uid) =0;

	// 设置实体位置
	virtual void setLocation(UID uid,Vector3 vLoc) =0;

	// 取得实体角度
	virtual Vector3 getAngle(UID uid) = 0;

	// 收到消息
	virtual void handleMessage( UID uid,void *pData,int nLen ) = 0;

    // 群体怪物收到消息,nDataLen=nOneDataLen为全体公用一个消息，否则dwArrayCount=nDataLen/nOneDataLen
    virtual void batchHandleMessage(UID* uidArray, DWORD dwArrayCount, BYTE byKeyModule, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData=0, int nDataLen=0, int nOneDataLen = 0) = 0;

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect(UID uid, int nEffectID, void * pAddContext, size_t len) = 0;

    // 移除效果
    virtual	void mobaGoodsRemoveEffect(UID uid, int nID) = 0;

    // 实体通知主人
    virtual void entityNotifyMaster(UID uid, EntityBelongContext context) = 0;

	/////////////////////// IEntityProperty ///////////////////////////////////////////////
	// 取得某个属性的类型
	virtual string getPropertyType( UID uid,int prop_id ) = 0;

	// 取得某个属性的标志数据(通常是序列化标志)
	virtual int getPropertyFlag( UID uid,int prop_id ) = 0;

	// 获取实体属性(布尔类型）
	virtual bool getProperty_Bool( UID uid,int prop_id) = 0;

	// 设置实体属性(布尔类型）
	virtual bool setProperty_Bool( UID uid,int prop_id,bool value) = 0;

	// 获取实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual int getProperty_Integer( UID uid,int prop_id) = 0;

	// 设置实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual bool setProperty_Integer( UID uid,int prop_id,int value) = 0;

	// 增加属性数值(注意内部需要控制上下限)
	virtual bool addProperty_Integer( UID uid,int prop_id,int delta) = 0;

	// 获取实体属性(字符串)
	virtual string getProperty_String( UID uid,int prop_id ) = 0;

	// 设置实体属性(字符串)
	virtual bool setProperty_String( UID uid,int prop_id,string value) = 0;

	// 获取实体属性(自定义结构,如果属性存在返回)
	virtual string getProperty_custom( UID uid,int prop_id) = 0;

	// 设置实体属性(自定义结构)
	virtual bool setProperty_custom( UID uid,int prop_id,string value) = 0;

	/////////////////////// IEntityAOI ///////////////////////////////////////////////
	// 取得视野半径
	virtual float getSight(UID uid) = 0;

	// 调整视野半径
	virtual void setSight( UID uid, float radii ) = 0;

	// 是否隐身
	virtual bool isHide(UID uid) = 0;

	// 设置隐身状态
	virtual void setHide( UID uid,bool state ) = 0;

	virtual void release() = 0;

	/////////////////////// IEnmityPart ///////////////////////////////////////////////
	// 添加仇恨数据
	virtual void AddEnmityValue(UID uidMonster, UID uidDes, int nVal) = 0;

	virtual void AddBroadEnmityValue(UID uidMonster, UID uidDes, int nVal) = 0;

	virtual void SetEnmityValue(UID uidMonster, UID uidDes, int nVal) = 0;

	/////////////////////// IBuffPart ///////////////////////////////////////////////
    // 给群体怪物带检测指定BUFFID和BUFF类型来增加BUFF
    virtual bool BatchCheckAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwCheckBuff, DWORD dwCheckLevel, DWORD dwCheckBuffType, DWORD dwTime, void *pBuffContext, int nContextLen, bool bExistAdd) = 0;
    // 给群体怪物带检测指定BUFFID和BUFF类型来移除BUFF
    virtual bool BatchCheckRemoveBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, UID uidAdd, UID uidRemove, DWORD dwCheckBuff, DWORD dwCheckLevel, DWORD dwCheckBuffType, bool bExistRemove) = 0;

    // 给群体怪物增加BUFF
    virtual bool BatchAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime, void *pBuffContext, int nContextLen) = 0;

    // 给群体怪物移除BUFF
    virtual bool BatchRemoveBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, UID uidAdd, UID uidRemove) = 0;

    /** 给实体添加buff */
	virtual bool AddBuff(UID uidMonster, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime, void *pBuffContext, int nContextLen) = 0;
	/** 给实体移除buff */
	virtual bool RemoveBuff(UID uidMonster, DWORD dwBuffID, UID uidAdd, UID uidRemove) = 0;
	/** 某个BUFF是否存在 */
	virtual bool IsExistBuff(UID uidMonster, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd) = 0;
    /** 某个BUFF类型是否存在 */
    virtual bool IsExistBuffType(UID uidMonster, DWORD dwBuffType) = 0;
    /** 取玩家某个BUFF的等级 */
	virtual DWORD GetBuffLevel(UID uidMonster, DWORD dwBuffID, UID uidAdd) = 0;
	/** 取玩家某个BUFF剩余时间 */
	virtual int GetLeftTime(UID uidMonster, DWORD dwBuffID, UID uidAdd) = 0;
	/** 设置累积BUFF */
	virtual void SetAccumulateBuff(UID uidMonster, DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime, void *pBuffContext, int nContextLen) = 0;
	// 通过索引删除buff
	virtual bool RemoveBuffByIndex(UID uidMonster,DWORD dwIndex)=0;
	/////////////////////// IStatePart ///////////////////////////////////////////////
	virtual void changeState(UID uidMonster, int nState) = 0;
	// 得到当前状态 
	virtual int getState(UID uidMonster) = 0;
	
	/////////////////////// IMovePart ///////////////////////////////////////////////
	// 传送
	virtual bool Transport(UID uidMonster, Vector3 newLoc, Vector3 rotation, bool bTransport, int nSceneID, int nReason = TRANSPORT_REASON_FLASH) = 0;

	virtual bool MoveTo(UID uidMonster, Vector3 &loc) = 0;

	// 移除所有NPC
	virtual void destroyAll() = 0;

	// 通知指定阵营当前野怪点野怪的生死状况,仅用于隐形实体
	virtual void msgToSpecCampWildInfo(UID uidMonster, int nCamp) = 0;

    // uid是否对camp可见
    virtual bool isInsight(UID uid, int camp) = 0;

    // 获取战场ID
    virtual int getWarID(UID uid) = 0;

    // 给movepart发送动态障碍数据
    virtual void sendDynamicObstacleToMovePart(UID uid, void * pObstacleInfo, size_t nLen) =0 ;

    virtual BuffTypeList getBuff(UID uid, DWORD dwBuffType) = 0;

    /** 取得怪物数组
    @param nMonsterID   要找的怪物ID,0为所有怪物ID
    @param nMonsterCamp 要找的怪物阵营,0为所有怪物阵营
    @param pArray       找到怪物UID数组
    @param nArraySize   怪物UID数组数量,防止怪物UID数组越界
    @return 返回找到怪物数量
    */
    virtual int getMonsterArray(int nMonsterID, int nMonsterCamp, UID *pArray, int nArraySize) = 0;

    virtual void handleReplaceableMessage(UID uid, unsigned long ulKey,void * pMsg,int nLen) = 0;

    // 选择目标
    virtual bool chooseTarget(UID uid, int nTargetFilter, UID uidTarget) = 0;
    // 取得PK类型
    virtual int getPKType(UID uid) = 0;
    // 取得PK状态
    virtual int getPKState(UID uid) = 0;

    // 取得人物所在地图ID
    virtual int getMapID(UID uid)=0;

    // 冲锋
    virtual void shift(UID uid, void *pData, int nDataLen) = 0;

    //强制结束PK状态
    virtual void  forceEndPK(UID uid) = 0;

	// 获取PK阵营
	virtual int getPKCamp(UID uid) = 0;
};