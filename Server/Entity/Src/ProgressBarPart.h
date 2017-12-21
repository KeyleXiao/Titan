/*******************************************************************
** �ļ���:	ProgressBarPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	2/26/2015
** ��  ��:	1.0
** ��  ��:	

			����������
********************************************************************/

#pragma once

#include "IProgressBarPart.h"
#include "TimerHandler.h"


/**
	���������� : ������
	1�����ܶ���
	2��������Ʒ����
	3�����ؾ߶���
	�ȵ�һ����Ҫ�����ĳ���
*/

class ProgressBarPart : public __IProgressBarPart,public TimerHandler,public IEventExecuteSink
{
    // ��Ϊ�ƶ�ʱʹ��ʱ�����ױ���ϣ�����ʹ��ǰBREAK_DISTANCE_TIME���������
    #define BREAK_DISTANCE_TIME 100

private:
	__IEntity *	             m_pMaster;
	ProgressBarContext    *  m_pContext;
    // ��������ʼʱ��
    DWORD                    m_dwStartTime;
    // ����ʱ���
    Vector3                  m_vStartPoint;

public:
	ProgressBarPart() : m_pMaster(0),m_pContext(0),m_dwStartTime(0),m_vStartPoint(0,0,0)
    {
    }

	virtual ~ProgressBarPart(){}

	virtual short getPartID() { return PART_PROGRESS_BAR;}

    virtual void  restore();

	virtual bool onLoad( __IEntity* pEntity, int nLoginMode);

	virtual __IEntity * getHost() { return m_pMaster; }

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	virtual void release();

    // �л�Ӣ��
    virtual void  onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

	virtual bool StartProgressBar( ProgressBarContext * context );

	virtual void CloseProgressBar(IEventExecuteSink * pSink,bool bBreak);

	virtual ProgressBarContext * GetCurrentContext();

	virtual bool onSerialize( rkt::ibuffer & in,int flag ){ return true;}

	virtual bool deSerialize( rkt::obuf & out,int flag ){ return true;}

	virtual void OnTimer( unsigned long dwTimerID );

	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	bool SubscibeEvents();

	bool UnSubscibeEvents();
};