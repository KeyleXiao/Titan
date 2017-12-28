#include "stdafx.h"
#include "LegendCupSceneService_Proxy.h"
#include "LegendCupDef.h"

#if LegendCupScene_DisableShowDebug
#undef TraceLn
#define TraceLn(x)
#endif

void LegendCupSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen)
{
	obuf256 t_data;
	t_data << serverID << head << nLen;
	t_data.push_back(pData, nLen);

	BUILD_MSG_BUFFER(ILegendCupSceneService::handleServerMsg, t_data);

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
}

void LegendCupSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen)
{
	obuf256 t_data;
	t_data << client << head << nLen;
	t_data.push_back(pData, nLen);
	BUILD_MSG_BUFFER(ILegendCupSceneService::handleClientMsg, t_data);

	m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
}
