/*******************************************************************
** 文件名:	DiePart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	实体死亡部件

********************************************************************/

#pragma once

#include "IDiePart.h"
#include "IEntityEvent.h"
#include "ISchemeCenter.h"
#include "TimerHandler.h"
#include "PathFindDef.h"
enum emExpType
{
    ADD_TO_MURDER,			// 凶手
    ADD_TO_AROUND,			// 自己周围的人
    ADD_TO_MURDERTEMP,		// 凶手所在队伍
};
enum
{
    ETIMER_RELIVE,			// 复活TIMER
};
struct event_entity_die;


// 实体死亡部件
class DiePart : public __IDiePart, public IEventExecuteSink, public TimerHandler
{

public:
    ////////////////////* IEntityPart */////////////////////////////////////////////
    /** 取得部件ID
    @return  
    */
    virtual short			getPartID();
    virtual bool			onLoad( __IEntity* pEntity, int nLoginMode);
    virtual bool			onMessage(void * pEntityHead, int msgId, void * data, size_t len);
    virtual __IEntity *		getHost();
    virtual void			release();
    virtual bool			onSerialize( rkt::ibuffer & in,int flag );
    virtual bool			deSerialize( rkt::obuf & out,int flag );
    // 切换英雄
    virtual void            onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }
    virtual void            restore();

    ////////////////////* IEventExecuteSink */////////////////////////////////////////////
    virtual void			OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////* __IDiePart */////////////////////////////////////////////
    /** 实体复活
    @return
    */
    virtual bool			relive();

	/** 取得复活信息
	@return  
	*/
    virtual bool            getReliveInfo(int &nSceneID, float &fLocX, float &fLocY, float &fLocZ, float &fAngleY);

	virtual void setIntPropertyOnRelive(int nProperty, int nValue);

    ////////////////////* TimerHandler */////////////////////////////////////////////
    virtual void			OnTimer( unsigned long dwTimerID );


public:
    DiePart();
    ~DiePart();
     void receiveDynamicObstacleInfo(void * pObstacleInfo, size_t nLen);
private:
    void					onDie(UID uidMurder, int nDeadType, int nOldPKType);
    // 处理最终伤害事件
    void					onEventDamageResult(BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);
    /** 广播死亡复活信息
    @return  
    */
    bool					broadcast(BYTE byAction, LPCSTR pszContext, size_t nLen);

    Vector3					getReliveLoc();

private:
    __IEntity *			m_pMaster;
    SEntityReliveInfo	m_ReliveInfo;
    SObstacleNode		m_SObstacleNode;

    BYTE                m_nDeadType;
	

	struct PropToSet
	{
		int nPropID;
		int nValue;

		PropToSet()
		{
			memset(this, 0, sizeof(*this));
		}
	};
	std::vector<PropToSet> m_vecPropToSet;
};