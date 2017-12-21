
#include "stdafx.h"
#include "ServiceContainer.h"
#include "TimerHandlerProxy.h"
#include "Framework.h"

void TimerHandlerProxy::Do(SERVICE_MESSAGE * pMsg)
{
	// 已删除
	if ( m_pRealHandler==0 )
	{
		Release();
		return;
	}

	m_pRealHandler->OnTimer( m_dwTimerID );

	// 调用次数到了自删除
	if ( INFINITY_CALL!=m_dwCallTimes )
	{
		if ( --m_dwCallTimes==0 && m_pRealHandler!=0 )
		{
			Framework::getInstance().get_service_timer()->KillTimer( m_dwTimerID,m_pRealHandler );
		}
	}

    Release();
}