#include "stdafx.h"
#include "ReplayNode.h"
#include "ViewList.h"
#include "ViewUser.h"
#include "ViewMsgDef_Server.h"


bool ReplayNode::IsFull()
{
	// TODO 计算负载是否已满
	ViewUser* pView = gViewList.Get(this->m_ViewID);
	if (pView)
		return pView->GetCurrCount() + 0 >= pView->GetMaxCount();// TODO 需要将0，替换为预分配的人数
	else
		return true;
}

DWORD ReplayNode::RemainCount()
{
	ViewUser* pView = gViewList.Get(this->m_ViewID);
	if (pView)
		return pView->GetMaxCount() - pView->GetCurrCount();	// TODO 还要减去预分配的人数
	else
		return 0;
}

void ReplayNode::NotifyView()
{
	ViewUser* pView = gViewList.Get(m_ViewID);
	if (pView==nullptr)
	{
		ErrorLn(_GT("创建观战节点，ViewServerID=") << m_ViewID << _GT("，而其连接却不存在!"));
		return;
	}

	// 通知该ViewServer，做好准备，开始读取战报
	SMsgView_MV_StartView msg;
	msg.rID = m_ReplayID;
	const SGameMsgHead& header = gMsg.BuildHead_MV(ENUM_MSG_VIEW_START_VIEW);
	TSendMsg(pView, header, msg);
}
