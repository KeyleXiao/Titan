/*******************************************************************
** 文件名:	WarPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	03/06/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IWarPart.h"
#include "ActorBuffMgr.h"

class CBattleWarCheckInNox;

// 实体状态部件
class WarPart : public __IWarPart,public IEventExecuteSink
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
    virtual void            onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore);

    virtual void            restore();

    virtual int 			getWarID();

    virtual int             getMatchTypeID();

    virtual int getIndex();

    // 获得召唤师技能id
    virtual int             getSmSkillId(int nSlot);

    virtual int getWarTypeID();

    //////////////////// IEventExecuteSink /////////////////////////////////////////////
    void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	////////////////////* TestPart */////////////////////////////////////////////
public:
	WarPart();
	~WarPart();
private:
	bool Parse(string linkContent);
private:
	__IEntity *	    m_pMaster;

    int             m_nWarID;
    int             m_nIndex; // 战场里位置

    int             m_nMatchTypeID; // 匹配类型ID
    int             m_nWarTypeID; // 战场类型ID

    int             m_nSmSkillId[SUMMONER_SKILL_COUNT];     // 召唤师技能id
    int             m_nSmSkillIndex[SUMMONER_SKILL_COUNT];  // 召唤师技能id对应的槽位

    CActorBuffMgr   m_buffMgr;
};