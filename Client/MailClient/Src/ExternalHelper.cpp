#include "stdafx.h"
#include "ExternalHelper.h"
#include "IClientGlobal.h"
#include "MailMsgDef.h"
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
void CExternalHelper::sendMailSocial(BYTE byActionID,  void* pData, int nLen)
{
	if (gClientGlobal->getHero() == NULL)
	{
		return;
	}


	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
	gamemsghead.byKeyModule = MSG_MODULEID_MAIL;
	gamemsghead.byKeyAction = byActionID;

	obuf1024 obuf;
	obuf.push_back(&gamemsghead, sizeof(SNetMsgHead));

	if (pData != NULL && nLen > 0)
	{
		obuf.push_back(pData, nLen);
	}
    net_senddata( obuf.data(), obuf.size() );
}

// 发送消息
void CExternalHelper::sendMailZone(BYTE byActionID, void* pData, int nLen)
{
	if (gClientGlobal->getHero() == NULL)
	{
		return;
	}


	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SCENE;
	gamemsghead.byKeyModule = MSG_MODULEID_MAIL;
	gamemsghead.byKeyAction = byActionID;

    obuf1024 obuf;
    obuf.push_back(&gamemsghead, sizeof(SNetMsgHead));

    if (pData != NULL && nLen > 0)
    {
        obuf.push_back(pData, nLen);
    }
    net_senddata( obuf.data(), obuf.size() );
}