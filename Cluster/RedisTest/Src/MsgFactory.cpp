#include "stdafx.h"
#include "MsgFactory.h"
#include "ViewMsgDef.h"


MsgFactory& gMsg = MsgFactory::getInstance();

MsgFactory::MsgFactory()
{
	m_vmHeader.SrcEndPoint = MSG_ENDPOINT_VIEW;
	m_vmHeader.DestEndPoint = MSG_ENDPOINT_VIEWMNG;
	m_vmHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_vmHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;

	m_vgHeader.SrcEndPoint = MSG_ENDPOINT_VIEW;
	m_vgHeader.DestEndPoint = MSG_ENDPOINT_VIEWGATE;
	m_vgHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_vgHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;

	m_vcHeader.SrcEndPoint = MSG_ENDPOINT_VIEW;
	m_vcHeader.DestEndPoint = MSG_ENDPOINT_CLIENT;
	m_vcHeader.byKeyModule = MSG_MODULEID_VIEW;
	m_vcHeader.byKeyAction = ENUM_MSG_VIEW_INVALID;
}

const SGameMsgHead&	MsgFactory::BuildHead_VM(EMMsgView emMsg)
{
	m_vmHeader.byKeyAction = emMsg;
	return m_vmHeader;
}

const SGameMsgHead&	MsgFactory::BuildHead_VG(EMMsgView emMsg)
{
	m_vgHeader.byKeyAction = emMsg;
	return m_vgHeader;
}

const SGameMsgHead&	MsgFactory::BuildHead_VC(EMMsgView emMsg)
{
	m_vcHeader.byKeyAction = emMsg;
	return m_vcHeader;
}
