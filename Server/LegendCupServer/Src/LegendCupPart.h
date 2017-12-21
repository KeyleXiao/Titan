/*******************************************************************
** �ļ���:	LegendCupPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�

********************************************************************/

#pragma once
#include "ILegendCupPart.h"
#include "LegendCupMsgDef.h"

// ʵ��״̬����
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

    // �л�Ӣ��
    virtual void  onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual void  restore();

private:
	__IEntity* m_pMaster;

};