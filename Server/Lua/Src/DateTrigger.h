/*******************************************************************
** 文件名:	DateTrigger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/28/2016
** 版  本:	1.0
** 描  述:	日期触发器
********************************************************************/

#ifndef __DATE_TRIGGER_H__
#define __DATE_TRIGGER_H__

#include "ITrigger.h"
#include <string>

/**
@name : 日期触发器
@brief: 
*/
class DateTrigger : public ITrigger, public TimerHandler
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
    void OnTimer( unsigned long dwTimerID );

    //////////////////////////////////////////////////////////////////////////
    DateTrigger(int nSceneID, const char * szCallFunc, int nYear, int nMonth, int nDay, int nHour, int nMinute,int nWDay);

    ~DateTrigger();

private:
    bool CheckTime(const time_t &t);

private:
    int               m_nSceneID;
    std::string       m_szCallFunc;
    int               m_nYear;
    int               m_nMonth;
    int               m_nDay;
    int               m_nHour;
    int               m_nMinute;
    int               m_nWDay;
    int               m_nTriggerID;
    DWORD             m_dwLastCheck;
    time_t            m_nCheckTimer;
};

#endif//__DATE_TRIGGER_H__