#include "stdafx.h"
#include "MsgFactory.h"
#include "ViewMsgDef.h"


MsgFactory& gMsg = MsgFactory::getInstance();

MsgFactory::MsgFactory()
{
	m_gmHeader.SrcEndPoint = MSG_ENDPOINT_VIEWGATE;
	m_gmHeader.DestEndPoint = MSG_ENDPOINT_VIEWMNG;
	m_gmHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_gmHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;

	m_gvHeader.SrcEndPoint = MSG_ENDPOINT_VIEWGATE;
	m_gvHeader.DestEndPoint = MSG_ENDPOINT_VIEW;
	m_gvHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_gvHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;

	m_gcHeader.SrcEndPoint = MSG_ENDPOINT_VIEWGATE;
	m_gcHeader.DestEndPoint = MSG_ENDPOINT_CLIENT;
	m_gcHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_gcHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;
}

const SGameMsgHead&	MsgFactory::BuildHead_GM(EMMsgView emMsg)
{
	m_gmHeader.byKeyAction = emMsg;
	return m_gmHeader;
}

const SGameMsgHead&	MsgFactory::BuildHead_GV(EMMsgView emMsg)
{
	m_gvHeader.byKeyAction = emMsg;
	return m_gvHeader;
}

const SGameMsgHead&	MsgFactory::BuildHead_GC(EMMsgView emMsg)
{
	m_gcHeader.byKeyAction = emMsg;
	return m_gcHeader;
}
