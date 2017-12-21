
#include "stdafx.h"
#include "ServiceContainer.h"
#include "DateTriggerHandlerProxy.h"
#include "Framework.h"

void DateHandlerProxy::Do(SERVICE_MESSAGE * pMsg)
{
	// 已删除
	if ( m_pRealHandler==0 )
	{
		Release();
		return;
	}

	m_pRealHandler->DateCome( m_dwTriggerID);

    // 移除判定
    if (m_bRepeatFlag)
    {
        DecRef();
    }
    else
    {
        Framework::getInstance().get_service_datetrigger()->UnRegisterDateTrigger( m_dwTriggerID,m_pRealHandler );

        Release();
    }
}