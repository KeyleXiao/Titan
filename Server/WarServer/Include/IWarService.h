/*******************************************************************
** 文件名:	IWarService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-5
** 版  本:	1.0
** 描  述:	本文件为根据IWarService接口定义，自动生成的WarService接口
            代理文件，该文件是为符合Framework对接口进行的包装

**************************** 修改记录 ******************************
** 修改人: ljw
** 日  期: 
** 描  述: 代码简化			
********************************************************************/

#pragma once

#include "ILuaEngine.h"
#include "WarDef.h"
#include "EntityDef.h"
#include "EventDef.h"
#include "PackagePtr.h"

// IWarService接口
struct IWarService
{
public:
    // 开启战场逻辑
    virtual void Start() = 0;

    // 把客户端添加到战场
    virtual bool PrepareEnterWar(SActorPreaperEnterWar sActorPreaperEnterWar) = 0;

    // 玩家进入战场
    virtual void onActorEnterWar(SActorEnterWarBaseInfo sActorEnterWarBaseInfo) = 0;

    // 怪物死亡
    virtual void onEntityDie( sEntityDieInfo entityDieInfo) = 0;
	// 玩家死亡
    virtual void onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info) = 0;
	// 玩家复活
	virtual void onEntityRelive(UID uid) = 0;

	// 获取战场所有人物UID
	virtual int getAllPerson(UID *PersonArray, int nArraySize)=0;

	// client 的网络消息
	virtual void onWarClientMsg(UID uidActor, BYTE byKeyAction, const char* pData, size_t nLen) = 0;

    // 战场进入停止流程
    virtual void destroy() = 0;

	// 获取场景ID
	virtual int getWarSceneID()= 0;

	// 获得战场类型Id
	virtual int getWarTypeId() = 0;

    // 战场结束。(七龙珠功能)
    virtual void DragonBallEndWar(int nFailedCamp, bool bNormalEnd) = 0;
	
	// 场景已创建
	virtual void onSceneCreated(int nWarID) = 0;

	// 用于通知指定阵营野怪死亡状况
	virtual void msgToSpecCampWildInfo(UID uMonsterUid, int nCamp) = 0;

    // 获得战场唯一id
    virtual LONGLONG getWarDBKey() = 0;


    // 取得比赛类型
    virtual DWORD  getMatchTypeID() = 0;

    // 设置充能值
    virtual void setChargedValue(UID uidTarget, int nEffectID, int nValue) = 0;

    // 取得充能值
    virtual int getChargedValue(UID uidTarget, int nEffectID) = 0;

    // 设置结束战场原因
    virtual void setWarEndReason(BYTE byEndReason) = 0;

     /** 获取战场指定类型UID
    @param bySelfCamp           : 自己的阵营
	@param byGetCampType		: 阵营类型 EWarCampType
    @param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual DWORD warSerchTypeUIDList(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize) = 0;


	/** 指定类型buff处理
	@param bySelfCamp           : 自己的阵营
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual void warAddSerchTypeBuff(SWarEffectAddBuff sData) = NULL;

    /** 更新玩家BUFF信息列表
    @param data
	@param len
	*/
	virtual void updateActorBuffList(char* data, size_t len) = 0;

	/** 序列化战场内玩家的信息
	@param uid		: 玩家UID
	@param out		: 序列化的数据
	@param flag		: 序列化标识
	*/
	virtual void deSerialzed(UID uid, rkt::obuf & out, int flag) = 0;

    // 设置杯赛信息
    virtual void setLegendCupInfo(SMsgLegendCupRoomInfo sLegendCupInfo) = 0;

	// 设置Ban选英雄
	virtual void setBanHeroList(char* data, size_t len) = 0;

	// 救治队友
	virtual void cureMember(UID uid) = 0;

	// 切换职业
	virtual void changeVocation(UID uid, int nSlotID) = 0;

	// 收集英雄基因
	virtual void collectHeroGenic(UID uid, int nAddHeroID, int nAddSkinID) = 0;

	// 假死者复活
	virtual void onFakeDeadRelive(UID uid) = 0;
};