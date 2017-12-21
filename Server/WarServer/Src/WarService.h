/*******************************************************************
** 文件名:	WarService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-5
** 版  本:	1.0
** 描  述:	

			战场War服务
**************************** 修改记录 ******************************
** 修改人: ljw
** 日  期: 
** 描  述: 简化代码
********************************************************************/

#pragma once
#include "IWarService.h"
#include "EntityDef.h"
#include "IServiceContainer.h"

class IWar;
using namespace War;

// IWarService实现
class WarService : public IWarService
{
public:
    WarService();

    ~WarService();

    bool on_start(SERVICE_PTR pContainer, void* data, int len);

    bool on_stop();

    virtual void Start();

    // 释放
    virtual void release();

    // 把客户端添加到战场
    virtual bool PrepareEnterWar(SActorPreaperEnterWar sActorPreaperEnterWar);

    // 设置杯赛信息
    virtual void setLegendCupInfo(SMsgLegendCupRoomInfo sLegendCupInfo);

    // 玩家进入战场事件
    virtual void onActorEnterWar(SActorEnterWarBaseInfo sActorEnterWarBaseInfo);

    // 怪物死亡
    virtual void onEntityDie( sEntityDieInfo entityDieInfo);

	// 玩家死亡
    virtual void onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info);
	
	// 玩家复活
	virtual void onEntityRelive(UID uid);

	// 获取战场所有人物UID
	virtual int getAllPerson(UID *PersonArray, int nArraySize);

	// 获取场景ID
	virtual int getWarSceneID();

	// 获得战场类型id
	virtual int getWarTypeId();

    // client 的网络消息
    virtual void onWarClientMsg(UID uidActor, BYTE byKeyAction, PACKAGE_PTR msg);

    virtual void destroy();

    virtual void DragonBallEndWar(int nFailedCamp, bool bNormalEnd);

	// 场景已创建
	virtual void onSceneCreated(int nWarID);

	// 用于通知指定阵营野怪死亡状况
	virtual void msgToSpecCampWildInfo(UID uMonsterUid, int nCamp);

    virtual LONGLONG getWarDBKey();

    
    // 取得比赛类型
    virtual DWORD  getMatchTypeID();

    // 设置充能值
    virtual void setChargedValue(UID uidTarget, int nEffectID, int nValue);

    // 取得充能值
    virtual int getChargedValue(UID uidTarget, int nEffectID);

    // 取得充能值
    virtual void setWarEndReason(BYTE byEndReason);

    /** 获取战场指定类型UID
    @param bySelfCamp           : 自己的阵营
	@param byGetCampType		: 阵营类型 EWarCampType
    @param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual DWORD warSerchTypeUIDList(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize);

    /** 更新玩家BUFF信息列表
    @param data
	@param len
	*/
	virtual void updateActorBuffList(char* data, size_t len);

	/** 序列化战场内玩家的信息
	@param uid		: 玩家UID
	@param out		: 序列化的数据
	@param flag		: 序列化标识
	*/
	virtual void deSerialzed(UID uid, rkt::obuf & out, int flag);

	// 设置Ban选英雄
	virtual void setBanHeroList(char* data, size_t len);

	// 救治队友
	virtual void cureMember(UID uid);

	// 切换职业
	virtual void changeVocation(UID uid, int nSlotID);

	// 收集英雄基因
	virtual void collectHeroGenic(UID uid, int nAddHeroID, int nAddSkinID);

	// 假死者复活
	virtual void onFakeDeadRelive(UID uid) override;
private:
    IWar* m_pWar;
};

