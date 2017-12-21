#include "stdafx.h"
#include "ClanService_Proxy.h"
#include "StubMacroDef.h"

void ClanService_Proxy::release()
{
	BUILD_MSG_CONTEXT_VOID( IClanService::release );
    m_pContainer->post_message( pMsg,nMsgLen,0);
}

//////////////////////////////////////////////////////////////////////////////////
void ClanService_Proxy::handleServerMsg( DWORD serverID,SNetMsgHead head,PACKAGE_PTR msg )
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( IClanService::handleServerMsg,DWORD ,serverID,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
};

//////////////////////////////////////////////////////////////////////////////////
void ClanService_Proxy::handleClientMsg( DWORD client,SNetMsgHead head,PACKAGE_PTR msg )
{
    PACKAGE_PTR::T_BAG bag(msg);
    BUILD_MSG_CONTEXT_3( IClanService::handleClientMsg,DWORD ,client,SNetMsgHead, head,PACKAGE_PTR::T_BAG, bag );

    m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK );
};

// ����ս���������ID
DWORD ClanService_Proxy::findKinClan(DWORD dwKinID)
{
	BUILD_MSG_CONTEXT_1( IClanService::findKinClan,DWORD, dwKinID);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(DWORD);
	return 0;
}

// �Ƴ����ڶ�ȡ��ս��
void ClanService_Proxy::removeReadingKin(DWORD dwKinID)
{
    BUILD_MSG_CONTEXT_1( IClanService::removeReadingKin,DWORD, dwKinID);
    m_pContainer->post_message( pMsg,nMsgLen,0, MSG_FLAG_NO_BLOCK);
}

// ��ȡ����Ա�б�
int ClanService_Proxy::getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen)
{
	BUILD_MSG_CONTEXT_3( IClanService::getClanKinList,DWORD, dwClanID, DWORD *,pKinList, int, nLen);

	rkt::obuf resultBuf;
	m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
	RETURN_FROM_MSG_BUF(int);
	return 0;
}

// ȡ�ð����ֵ
int ClanService_Proxy::getClanNumProp(DWORD dwClanID, int nNumIndex)
{
    BUILD_MSG_CONTEXT_2( IClanService::getClanNumProp,DWORD, dwClanID, int, nNumIndex);

    rkt::obuf resultBuf;
    m_pContainer->post_message(pMsg,nMsgLen,&resultBuf);
    RETURN_FROM_MSG_BUF(int);
    return 0;
}

// ��ȡ�������
string ClanService_Proxy::getClanName(DWORD dwClanID)
{
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

// ���˱�������
void ClanService_Proxy::notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName)
{
	if(pCupName == NULL) return;

	size_t nNameLen = strlen(pCupName) + 1;
	size_t nLen = sizeof(DWORD) + sizeof(LONGLONG) + sizeof(int) + sizeof(size_t)+ nNameLen;

	BUILD_MSG_BUFFER_LEN(IClanService::notifyCreateClanLegendCup, nLen);
	t_data << dwClanID << llLegendCupID << nMaxKinCount << nNameLen;
	t_data.push_back(pCupName, nNameLen);

	m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
}
