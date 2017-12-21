/*******************************************************************
** 文件名:	LegendCupPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜

********************************************************************/

#pragma once
#include "ILegendCupPart.h"
#include "LegendCupMsgDef.h"

// 实体状态部件
class LegendCupPart:public __ILegendCupPart
{
public:
	LegendCupPart();
	~LegendCupPart();
public:
	// IEntityPart
	//////////////////////////////////////////////////////////////////////////
	virtual short getPartID();
	virtual bool onLoad( __IEntity* pEntity, int nLoginMode);
	virtual __IEntity * getHost();
	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);
	virtual void release();

	virtual bool onSerialize( rkt::ibuffer & in,int flag );

	virtual bool deSerialize( rkt::obuf & out,int flag );

    // 切换英雄
    virtual void  onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual void  restore();

private:
	__IEntity* m_pMaster;

};