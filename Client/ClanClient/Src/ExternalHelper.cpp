#include "stdafx.h"
#include "ExternalHelper.h"
#include "IClientGlobal.h"
#include "ClanMessageDef.h"
#include "ISchemeCenter.h"

CExternalHelper	g_EHelper;

// 构造函数
CExternalHelper::CExternalHelper(void)
{

}

// 析构函数
CExternalHelper::~CExternalHelper(void)
{

}

// 发送消息
void CExternalHelper::sendClanSocial(BYTE bySubModule,BYTE byActionID, DWORD dwClanID,  void* pData, int nLen)
{
	if (gClientGlobal->getHero() == NULL)
	{
		return;
	}


	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
	gamemsghead.byKeyModule = MSG_MODULEID_CLAN;
	gamemsghead.byKeyAction = bySubModule;

	msg_clan_sub_head subMsg;
	subMsg.dwMsgCode = byActionID;
	subMsg.dwClanID =dwClanID;

	obuf1024 obuf;
	obuf.push_back(&gamemsghead, sizeof(SNetMsgHead));
	obuf.push_back(&subMsg, sizeof(msg_clan_sub_head));

	if (pData != NULL && nLen > 0)
	{
		obuf.push_back(pData, nLen);
	}
    net_senddata( obuf.data(), obuf.size() );
}

// 发送消息
void CExternalHelper::sendClanZone(BYTE byActionID, void* pData, int nLen)
{
	if (gClientGlobal->getHero() == NULL)
	{
		return;
	}


	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SCENE;
	gamemsghead.byKeyModule = MSG_MODULEID_CLAN;
	gamemsghead.byKeyAction = byActionID;

    obuf1024 obuf;
    obuf.push_back(&gamemsghead, sizeof(SNetMsgHead));

    if (pData != NULL && nLen > 0)
    {
        obuf.push_back(pData, nLen);
    }
    net_senddata( obuf.data(), obuf.size() );
}


// 取得帮会每级信息
SClanLevel* CExternalHelper::getLevelInfo(int nClanLevel)
{
    ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return NULL;
    }

    ISchemeClanDataCenter *pSchemeClanDataCenter = pSchemeCenter->getSchemeClanDataCenter();
    if (pSchemeClanDataCenter == NULL)
    {
        return NULL;
    }

    return pSchemeClanDataCenter->getLevelInfo(nClanLevel);
}

// 取得帮会常量信息
SClanConfig* CExternalHelper::getConfig()
{
    ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return NULL;
    }

    ISchemeClanDataCenter *pSchemeClanDataCenter = pSchemeCenter->getSchemeClanDataCenter();
    if (pSchemeClanDataCenter == NULL)
    {
        return NULL;
    }

    return pSchemeClanDataCenter->getConfig();
}