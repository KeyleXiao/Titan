/*******************************************************************
** 文件名:	ProgressBarPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	2/26/2015
** 版  本:	1.0
** 描  述:	

			进度条部件
********************************************************************/

#pragma once

#include "IProgressBarPart.h"
#include "TimerHandler.h"


/**
	进度条部件 : 可用于
	1、技能读条
	2、开启物品读条
	3、上载具读条
	等等一切需要读条的场合
*/

class ProgressBarPart : public __IProgressBarPart,public TimerHandler,public IEventExecuteSink
{
    // 因为移动时使用时很容易被打断，技能使用前BREAK_DISTANCE_TIME不允许被打断
    #define BREAK_DISTANCE_TIME 100

private:
	__IEntity *	             m_pMaster;
	ProgressBarContext    *  m_pContext;
    // 进度条开始时间
    DWORD                    m_dwStartTime;
    // 读条时起点
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

    // 切换英雄
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