#include "stdafx.h"
#include "LegendCupSceneService_Proxy.h"
#include "LegendCupDef.h"

#if LegendCupScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

void LegendCupSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( ILegendCupSceneService::handleServerMsg,DWORD ,serverID,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

void LegendCupSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( ILegendCupSceneService::handleClientMsg,DWORD ,client,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}
