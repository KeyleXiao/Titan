/*******************************************************************
** 文件名: WarAddBuff.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/6/7
** 版  本: 1.0
** 描  述: 战场添加Buff
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once

#include "IWarAddBuffMgr.h"

class IWar;

// 自动加Buff定时器
enum 
{
    ETimerEventID_AutoAddBuff,					// 添加Buff
};

class CWarAddBuff : public IWarAddBuffMgr, public TimerHandler
{

public:
	CWarAddBuff(IWar* pWar);
	~CWarAddBuff();

public:
	//////////////////////TimerHandler///////////////////
	virtual void OnTimer(unsigned long dwTimerID);

public:
	////////////////////////IWarAddBuffMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();
	////////////////////////IWarAddBuffMgr end/////////////////////////

private:
	// 战场
	IWar*	m_pWar;

	// <定时器ID,添加的Buff信息>
	map<int, SWarAddBuff> m_BuffInfoList;

	int m_nMatchTypeID;
};