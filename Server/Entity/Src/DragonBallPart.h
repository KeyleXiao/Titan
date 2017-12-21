/*******************************************************************
** 文件名:	TestPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	03/06/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IDragonBallPart.h"
#define  DRAGONBALLMAXNUM 10

// 实体状态部件
class DragonBallPart : public __IDragonBallPart
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

    ////////////////////* TestPart */////////////////////////////////////////////
public:
    DragonBallPart();
    ~DragonBallPart();
private:
    bool Parse(string linkContent);
private:
    __IEntity *	    m_pMaster;
};