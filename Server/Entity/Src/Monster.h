/*******************************************************************
** 文件名:	Monster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/23/2014
** 版  本:	1.0
** 描  述:	

			怪物/NPC实体对象（非玩家实体对象)
********************************************************************/

#pragma once

#include "Vector3.h"
#include "IEntity.h"
#include "EntityEvent.h"

#include "EntityStandState.h"
#include "EntityDieState.h"
#include "EntityFightState.h"
#include "MobaGoodsEffect.h"
#include "BelongEntityMgr.h"
#include "EntityUseFlagMgr.h"


using namespace rkt;
struct IEffect;

class Monster : public __IEntity
{
protected:
	__IEntityPart *		m_pPartList[ENTITY_PART_MAX];		// 记录实体Part指针
	EntityEvent   *		m_pEventEngine;
	int					m_nMapID;
    //std::set<unsigned int> m_setObsRef;                     // 与该实体关联的障碍多边形引用
    char                m_szName[ACTORNAME_MAXSIZE];

	// 当前控制状态
	BYTE				m_byCurControlState;
	// 当前状态
	CEntityState*		m_pCurrentState;
	// 站立状态
	CEntityStandState	m_EntityStandState;
	// 死亡状态
	CEntityDieState		m_EntityDieState;
	// 战斗死亡状态
	CEntityFightState	m_EntityFightState;


    CMobaGoodsEffect*   m_pMobaGoodsEffect;
    // 附属实体管理类（分身、宠物...）
    CBelongEntityMgr    m_belongEntityMgr;
    // 实体用途管理类
    CEntityUseFlagMgr   m_UseFlagMgr;


public:
	Monster();

	virtual bool create(UID uid,void * context,int len,int flag);

	virtual void release();

	// 销毁 : 因为智能指针会延迟销毁，所以有时候需要主动调用销毁函数
	virtual void destroy();

	virtual UID getUID() const;

	virtual ENTITY_TYPE getType();

	virtual char * getName();

	virtual int getSceneID() const;
	
	// 取得当前所在地图ID
	virtual int getMapID() const;

	virtual Vector3 getLocation() const;

	virtual void setLocation(Vector3 vLoc);

    virtual Vector3 getAngle() const;

	virtual __IEntityPart * getEntityPart(ENTITY_PART id);

	virtual __IEntityEvent * getEntityEvent() { return m_pEventEngine;}

	virtual void handleMessage( void * data,int32 len );

	// 改变当前状态 
	virtual bool changeState(int nState);

	// 取得当前状态 
	virtual int getState(void);

	// 取得数值属性
	virtual int getIntProperty(int nPropID);

    // 设置数值属性
    virtual bool setIntProperty(int nPropID, int nValue);

	// 取得字符串属性
	virtual std::string getStringProperty(int nPropID);

	// 取得来源游戏世界ID
	virtual	int	getFromGameWorldID();

	// 设置阵营
	virtual bool setCamp(BYTE byCamp);

	// 设置视野
	virtual bool setSight(float fEyeshot);

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect(int nEffectID, void * pAddContext, size_t len);

    // 移除效果
    virtual	void mobaGoodsRemoveEffect(int nID);

    // 移除效果
    virtual	void releaseMobaGoodsEffect();

    // 获取附属实体管理对象
    virtual IBelongEntityMgr* getBelongEntityMgr(void) { return &m_belongEntityMgr; }
    
    // 获取用途标识对象
    virtual IUseFlag* getUseFlag() { return &m_UseFlagMgr; }

	virtual bool onSerialize( rkt::ibuffer & in,int flag );

	virtual bool deSerialize( rkt::obuf & out,int flag );

public:
	// 设置地图ID
	void setMapID(int nMapID);
	// 用于通知指定阵营野怪死亡状况
	void msgToSpecCampWildInfo(UID uMonsterUid, int nCamp);
	// 强制自杀
	void forceKillSelf();

	// 设置气势系数 影响怪物属性
    bool setVigourRatio(int* pVigourRatio);
};
