#include "stdafx.h"
#include "MsgFactory.h"
#include "ViewMsgDef.h"


MsgFactory& gMsg = MsgFactory::getInstance();

MsgFactory::MsgFactory()
{
	m_mvHeader.SrcEndPoint = MSG_ENDPOINT_VIEWMNG;
	m_mvHeader.DestEndPoint = MSG_ENDPOINT_VIEW;
	m_mvHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_mvHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;

	m_mgHeader.SrcEndPoint = MSG_ENDPOINT_VIEWMNG;
	m_mgHeader.DestEndPoint = MSG_ENDPOINT_VIEWGATE;
	m_mgHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_mgHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;

	m_mcHeader.SrcEndPoint = MSG_ENDPOINT_VIEWMNG;
	m_mcHeader.DestEndPoint = MSG_ENDPOINT_CLIENT;
	m_mcHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_mcHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;

	m_msHeader.SrcEndPoint = MSG_ENDPOINT_VIEWMNG;
	m_msHeader.DestEndPoint = MSG_ENDPOINT_UNKNOW;	// 观战管理服根本不关心对方是什么类型的游戏服...因此只能填这个类型
	m_msHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_msHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;
}

const SGameMsgHead&	MsgFactory::BuildHead_MV(EMMsgView emMsg)
{
	m_mvHeader.byKeyAction = emMsg;
	return m_mvHeader;
}

const SGameMsgHead&	MsgFactory::BuildHead_MG(EMMsgView emMsg)
{
	m_mgHeader.byKeyAction = emMsg;
	return m_mgHeader;
}

const SGameMsgHead&	MsgFactory::BuildHead_MC(EMMsgView emMsg)
{
	m_mcHeader.byKeyAction = emMsg;
	return m_mcHeader;
}

const SGameMsgHead&	MsgFactory::BuildHead_MS(EMMsgView emMsg)
{
	m_msHeader.byKeyAction = emMsg;
	return m_msHeader;
}