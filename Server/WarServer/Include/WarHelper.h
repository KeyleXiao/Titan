/*******************************************************************
** 文件名:	WarHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	1/13/2015
** 版  本:	1.0
** 描  述:	

		战场辅助类
********************************************************************/


#pragma once

#include "IEntity.h"
#include "IServerGlobal.h"
#include "ServiceHelper.h"
#include "IEntityProperty.h"
#include "ISceneManager.h"
#include "ISceneLogicService.h"
#include "IGatewayAcceptorService.h"
#include "IWarManager.h"
#include "IWarService.h"

class WarHelper
{
public:
	SERVICE_PTR m_pContainer;
	IWarService * m_ptr;

    WarHelper(DWORD nWarID) : m_ptr(0)
    {
        IWarManager *pWarManager = gServerGlobal->getWarManager();
        if (pWarManager == NULL)
        {
            return;
        }

        SERVICE_PTR pWarContainer = pWarManager->GetWar(nWarID);
        if(pWarContainer == 0)
        {
            return;
        }

        m_ptr = (IWarService*)pWarContainer->query_interface();
    }


};