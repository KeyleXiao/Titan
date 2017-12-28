/*******************************************************************
** 文件名:	SimpleTimerAxis.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	简单时间轴

			
********************************************************************/

#include "stdafx.h"
#include "SimpleTimerAxis.h"
#include "Framework.h"

bool SimpleTimerAxis::Create()
{
	m_pAxis = new TimerAxis(16,300,6000);
	return Framework::getInstance().get_service_timer()->SetTimer(0,32,this);
}

bool SimpleTimerAxis::Close()
{
	if ( m_pAxis ) 
	{
		delete m_pAxis;
		m_pAxis = 0;
	}

	return Framework::getInstance().get_service_timer()->KillTimer(0,this);
}



