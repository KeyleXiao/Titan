#include "stdafx.h"
#include "ClanSceneService_Proxy.h"
#include "ClanDef.h"

// 获取帮会名称
string ClanSceneService_Proxy::getClanName(DWORD nClanID)
{
    if (m_pContainer == 0)
        return string();

	BUILD_MSG_CONTEXT_1( IClanSceneService::getClanName,DWORD, nClanID);

	rkt::obuf resultBuf;
	m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_READ_ONLY);
	if (resultBuf.good() && resultBuf.size() > 0)
	{
		ibuffer t_in(resultBuf.data(), resultBuf.size());
		string vReturn;
		t_in >> vReturn;
		if (t_in.good()) return vReturn;
	}

	return string();
}

void ClanSceneService_Proxy::handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len)
{
    if (m_pContainer == 0)
        return ;

    obuf256 t_data;
    t_data << serverID << head << len;
    t_data.push_back(data, len);
    
    BUILD_MSG_BUFFER(IClanSceneService::handleServerMsg, t_data);
    
    m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
}

//void ClanSceneService_Proxy::handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len)
//{
//    obuf256 t_data;
//    t_data << client << head << len;
//    t_data.push_back(data, len);
//
//    BUILD_MSG_BUFFER(IClanSceneService::handleClientMsg, t_data);
//
//    m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
//}

int ClanSceneService_Proxy::getNumProp(DWORD dwClaneID, DWORD dwProp)
{
    if (m_pContainer == 0)
        return int();

    BUILD_MSG_CONTEXT_2( IClanSceneService::getNumProp,DWORD, dwClaneID, DWORD, dwProp);

    rkt::obuf resultBuf;
    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
    if (resultBuf.good() && resultBuf.size() > 0)
    {
        ibuffer t_in(resultBuf.data(), resultBuf.size());
        int vReturn;
        t_in >> vReturn;
        if (t_in.good()) return vReturn;
    }

    return int();
}

bool ClanSceneService_Proxy::setNumProp(DWORD dwClanID, DWORD dwProp, int nValue)
{
    if (m_pContainer == 0)
        return bool();

	BUILD_MSG_CONTEXT_3( IClanSceneService::setNumProp,DWORD, dwClanID,DWORD, dwProp,int, nValue );

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(bool);
	return 0;
}

bool ClanSceneService_Proxy::checkCreateClanLegendCup(DWORD dwClanID,int LegendCupConfigID)
{
    if (m_pContainer == 0)
        return bool();

	BUILD_MSG_CONTEXT_2( IClanSceneService::checkCreateClanLegendCup,DWORD, dwClanID,int, LegendCupConfigID);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(bool);
	return 0;
}
