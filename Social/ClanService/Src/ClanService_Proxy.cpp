#include "stdafx.h"
#include "ClanService_Proxy.h"
#include "StubMacroDef.h"

void ClanService_Proxy::release()
{
	if (m_pContainer == 0)
	{
		return;
	}
	BUILD_MSG_CONTEXT_VOID( IClanService::release );
    m_pContainer->post_message( pMsg,nMsgLen,0);
}

//////////////////////////////////////////////////////////////////////////////////
void ClanService_Proxy::handleServerMsg( DWORD serverID,SNetMsgHead head,void *data, size_t len )
{
	if (m_pContainer == 0)
	{
		return;
	}
    obuf256 t_data;
    t_data << serverID << head << len;
    t_data.push_back(data, len);

    BUILD_MSG_BUFFER(IClanService::handleServerMsg, t_data);

    m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
};

//////////////////////////////////////////////////////////////////////////////////
void ClanService_Proxy::handleClientMsg( DWORD client,SNetMsgHead head,void *data, size_t len)
{
	if (m_pContainer == 0)
	{
		return;
	}
    obuf256 t_data;
    t_data << client << head << len;
    t_data.push_back(data, len);

    BUILD_MSG_BUFFER(IClanService::handleClientMsg, t_data);

    m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
};

// 查找战队所属帮会ID
DWORD ClanService_Proxy::findKinClan(DWORD dwKinID)
{
	if (m_pContainer == 0)
	{
		return 0;
	}
	BUILD_MSG_CONTEXT_1( IClanService::findKinClan,DWORD, dwKinID);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(DWORD);
	return 0;
}

// 移除正在读取的战队
void ClanService_Proxy::removeReadingKin(DWORD dwKinID)
{
	if (m_pContainer == 0)
	{
		return;
	}
    BUILD_MSG_CONTEXT_1( IClanService::removeReadingKin,DWORD, dwKinID);
    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}

// 获取帮会成员列表
int ClanService_Proxy::getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen)
{
	if (m_pContainer == 0)
	{
		return 0;
	}
	BUILD_MSG_CONTEXT_3( IClanService::getClanKinList,DWORD, dwClanID, DWORD *,pKinList, int, nLen);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(int);
	return 0;
}

// 取得帮会数值
int ClanService_Proxy::getClanNumProp(DWORD dwClanID, int nNumIndex)
{
	if (m_pContainer == 0)
	{
		return 0;
	}
    BUILD_MSG_CONTEXT_2( IClanService::getClanNumProp,DWORD, dwClanID, int, nNumIndex);

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(int);
    return 0;
}

// 获取帮会名称
string ClanService_Proxy::getClanName(DWORD dwClanID)
{
	if (m_pContainer == 0)
	{
		return "";
	}
	BUILD_MSG_CONTEXT_1( IClanService::getClanName,DWORD, dwClanID);

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

// 联盟杯赛创建
void ClanService_Proxy::notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName)
{
	if (m_pContainer == 0)
	{
		return;
	}
	if(pCupName == NULL) return;

	size_t nNameLen = strlen(pCupName) + 1;
	size_t nLen = sizeof(DWORD) + sizeof(LONGLONG) + sizeof(int) + sizeof(size_t)+ nNameLen;

	BUILD_MSG_BUFFER_LEN(IClanService::notifyCreateClanLegendCup, nLen);
	t_data << dwClanID << llLegendCupID << nMaxKinCount << nNameLen;
	t_data.push_back(pCupName, nNameLen);

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}

