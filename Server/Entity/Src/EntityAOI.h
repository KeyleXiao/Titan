/*******************************************************************
** 文件名:	EntityAOI.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/29/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IEntityAOI.h"
#include <list>
#include "IEventEngine.h"
#include "EventDef.h"
#include "IGameSceneService.h"

/**
	管理游戏中玩家的视野逻辑(Area Of Interest)
	具体规则：
	1.玩家上线，向周围视野内的所有玩家广播自己出现的消息(Appear)
	2.同时向该玩家发送周围所以玩家的信息(Nearest)
	3.玩家下线, 向周围视野内的所有玩家广播自己离开的消息(Disappear)
	4.玩家移动, 计算玩家之前所在位置的九宫格列表，再计算玩家新位置的九宫格列表
	  a).对于其中新增的单元格，广播玩家出现的消息(Appear),并把该单元格中的所有玩家列表发给此玩家(Nearest)
	  b).对于其中失效的单元格，广播玩家离开的消息(Disappear)，同时将该单元格内的所有玩家发给此玩家，告知离开
	5.动作广播: 玩家做出动作或者外观变化，则对周围视野内所有玩家进行广播
	6.属性广播: 角色属性发生变化时，需要如果是属于客户端看得见的属性
	则要下发给客户端，如果属于周围玩家看得见的属性，则要广播给周围角色
	7.视野变化：游戏中某些行为可导致视野变化，比如致盲，视野就会变成缩小或没有
	8.隐身功能：提取角色广播信息时返回false
	9.多视野功能: 比如WOW中的鹰眼,LOL中的插眼.查看队友的视野.(视野对象支持订阅功能)

	优化:
	1.属性广播和动作广播，需要根据距离的远近，调整广播频率
	2.视野变化：周围人数非常多时动态缩小视野以减少广播量
	*/

class EntityAOI : public __IEntityAOI, public IEventExecuteSink, public IEventVoteSink, public TimerHandler
{
public:
	////////////////////* IEntityPart */////////////////////////////////////////////
	virtual short getPartID();
	virtual bool onLoad( __IEntity* pEntity, int nLoginMode);
	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);
	virtual __IEntity * getHost();
	virtual void release();

	virtual bool onSerialize( rkt::ibuffer & in,int flag );
	virtual bool deSerialize( rkt::obuf & out,int flag );

    // 切换英雄
	virtual void onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore);
    virtual void restore();
	////////////////////* IEntityAOI */////////////////////////////////////////////
	virtual float getSight();
	virtual void setSight( float radii );
	virtual bool isHide();
	virtual void setHide( bool state );
	virtual void setEventFlag( int flag );

    // 设置隐形(对其他阵营隐形)
    virtual void setStealth( bool bSteal, bool bAlpha2SelfCamp, int nEffectID );

    // 反隐形
    virtual void antiStealth( bool bEnable );

    // 设置（取消）对某阵营一直可见
    virtual void keepInsight( int camp, bool bEnable );

    // 设置对有所阵营隐形（包括自己）
    virtual void setGhostState( bool bGhost = true);

    // 是否在某阵营的视野内
    virtual bool isInsight( int camp );

    // 获取AOI代理
    virtual AOI_PROXY_PTR getAOIProxy();

	//////////////////// IEventExecuteSink /////////////////////////////////////////////
	void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    virtual void OnTimer( unsigned long dwTimerID );

    virtual int broadcast(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg, const char * pFileInfo, int nFileLine);

public:
	EntityAOI();

protected:
	void onEvent_EntityCreate();
	void onEvent_EntityDestroy(event_entity_destroy & context);
    void onEvent_EntityMoveBehaviourChange(event_entity_move_behaviour_change* context);
	void onEvent_EntityTransport(event_entity_transport &event_msg);

	
	void onEvent_PropertyBroadcast(void* pData, int nLen );

    void onEvent_PropertyChange(void* pData, int nLen);

    void onEvent_PKTargetChange(void* pData, int nLen);

    void onEvent_PropLocationChange(event_entity_property_change* context);

    void onEvent_PropLevelChange(event_entity_property_change* context);

    void onEvent_PropHpChange(event_entity_property_change* context);

    void onEvent_PropMaxHpChange(event_entity_property_change* context);

    void onEvent_PropMpChange(event_entity_property_change* context);

    void onEvent_PropMaxMpChange(event_entity_property_change* context);

    void onEvent_PropStaminaChange(event_entity_property_change* context);

    void onEvent_PropMaskChange(event_entity_property_change* context);

    void onEvent_PropFromTypeChange(event_entity_property_change* context);

    void onEvent_PropObserverChange(event_entity_property_change* context);
    


	// 进出视野的几个通知
	void onMessage_InSight(int camp, UID uidTarget);
	void onMessage_OutOfSight(int camp, UID uidTarget);
	void onMessage_CreateNotify(UID uidPlayer,int camp);
	void onMessage_DestroyNotify(UID uidPlayer,int camp);
    void onStealthChange();

	bool addObserverLog(UID host);
	bool removeObserverLog(UID host);

	AOI_Proxy * CreateAOIProxy();

private:
	void scene_broadcast_all( BROADCAST_ID id,string & msg,int nCampMask=CAMP_MASK_ALL );

    void flushProperty2Client(const std::set<int>& refPropertys, bool isBroadcast =true);

    // 暴露自己给某阵营 
    void forestExpose( int nCamp,int nInterval );

    // 加载视野距离配置
    void loadSight();

    // 草丛事件(进入离开草丛)
    void forestEvent(int nForestID);

    // 通知客户端创建主角
    void updateSelfCreate();

protected:
	__IEntity *	    m_pMaster;
	float           m_fSightRadii;			// 视力半径
	bool            m_bHide;				// 是否隐身
    bool            m_isGodMode;            // 上帝模式
	int             m_nEventFlag;           // 事件标志
    
    bool            m_bStealthInitialValue;    // 是否隐形(在实体没有插入aoi管理器的时候设置，暂作缓存)
    bool            m_bAntiStealthInitialValue; // 是否反隐形(在实体没有插入aoi管理器的时候设置，暂作缓存)
    Vector3         m_vLookLoc;                 // 观察点

	ulong           m_dwBroadcastCD[2];			// 广播冷却，50毫秒周围范围广播一次位置，500毫秒全局视野广播一次

	AOI_PROXY_PTR   m_pAOIProxy;                // AOI代理器

private:
    typedef list<int>   STEALTH_LIST;
    STEALTH_LIST        m_StealthList;
};