/*******************************************************************
** 文件名:	e:\Rocket\Client\ClientModule\Src\ExternalHelper.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2016/3/9
** 版  本:	1.0
** 描  述:	辅助功能类
** 应  用:	
********************************************************************/

#include "stdafx.h"
#include "ExternalHelper.h"
#include "KinMsgDef.h"
#include "IClientGlobal.h"

CExternalHelper	g_EHelper;

/// purpose: 构造函数
CExternalHelper::CExternalHelper(void)
{

}

/// purpose: 析构函数
CExternalHelper::~CExternalHelper(void)
{

}

/// purpose: 发送消息
//@ param  : nSubModule 战队子模块消息码，参考emMsgKin_
//@ param  : dwKinID 战队ID，非emMsgKin_Kin的消息可以填0
//@ param  : nActionID 消息码
//@ param  : pData 消息内容
//@ param  : nLen 消息内容长度
void CExternalHelper::SendKinSocial(BYTE bySubModule,BYTE byActionID, DWORD dwKinID,  void* pData, int nLen)
{
	if (gClientGlobal->getEntityClient() == NULL)
	{
		return;
	}

	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
	gamemsghead.byKeyModule = MSG_MODULEID_KIN;
	gamemsghead.byKeyAction = bySubModule;

	SMsgKinDataSubMsg subMsg;
	subMsg.dwMsgCode = byActionID;
	subMsg.dwKinID =dwKinID;

	obuf1024 obuf;
	obuf.push_back(&gamemsghead, sizeof(SNetMsgHead));
	obuf.push_back(&subMsg, sizeof(SMsgKinDataSubMsg));

	if (pData != NULL && nLen > 0)
	{
		obuf.push_back(pData, nLen);
	}
    net_senddata( obuf.data(), obuf.size() );
}

/// purpose: 发送消息
void CExternalHelper::SendKinZone(BYTE byActionID, void* pData, int nLen)
{
	if (gClientGlobal->getEntityClient() == NULL)
	{
		return;
	}

	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SCENE;
	gamemsghead.byKeyModule = MSG_MODULEID_KIN;
	gamemsghead.byKeyAction = byActionID;

    obuf1024 obuf;
    obuf.push_back(&gamemsghead, sizeof(SNetMsgHead));

    if (pData != NULL && nLen > 0)
    {
        obuf.push_back(pData, nLen);
    }

    net_senddata( obuf.data(), obuf.size() );
}