/*******************************************************************
** 文件名:	TimerTrigger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	定时器触发器
********************************************************************/

#ifndef __TIMER_TRIGGER_H__
#define __TIMER_TRIGGER_H__

#include "ITrigger.h"
#include <string>

/**
@name : 定时触发器
@brief: 
*/
class TimerTrigger : public ITrigger, public TimerHandler
{
public:
	virtual bool Setup();

	virtual void Release();

	virtual void SetID(int nID);

	virtual int GetID() const;

	virtual int GetTriggerType(void);

	virtual std::string GetTriggerCallBack(void);

	virtual int GetSceneID();

    //////////////////////////////////////////////////////////////////////////
    virtual void OnTimer( unsigned long dwTimerID );

    //////////////////////////////////////////////////////////////////////////
	TimerTrigger(int nSceneID, const char * szCallFunc, int nInterval, int nFireTimes);

    ~TimerTrigger();

private:
    int            m_nSceneID;
	std::string    m_szCallFunc;
	int            m_nInterval;
	int            m_nFireTimes;
	int            m_nTriggerID;
};

#endif//__TIMER_TRIGGER_H__