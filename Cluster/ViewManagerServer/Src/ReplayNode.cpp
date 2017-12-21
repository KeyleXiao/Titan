#include "stdafx.h"
#include "ReplayNode.h"
#include "ViewList.h"
#include "ViewUser.h"
#include "ViewMsgDef_Server.h"


bool ReplayNode::IsFull()
{
	// TODO ���㸺���Ƿ�����
	ViewUser* pView = gViewList.Get(this->m_ViewID);
	if (pView)
		return pView->GetCurrCount() + 0 >= pView->GetMaxCount();// TODO ��Ҫ��0���滻ΪԤ���������
	else
		return true;
}

DWORD ReplayNode::RemainCount()
{
	ViewUser* pView = gViewList.Get(this->m_ViewID);
	if (pView)
		return pView->GetMaxCount() - pView->GetCurrCount();	// TODO ��Ҫ��ȥԤ���������
	else
		return 0;
}

void ReplayNode::NotifyView()
{
	ViewUser* pView = gViewList.Get(m_ViewID);
	if (pView==nullptr)
	{
		ErrorLn(_GT("������ս�ڵ㣬ViewServerID=") << m_ViewID << _GT("����������ȴ������!"));
		return;
	}

	// ֪ͨ��ViewServer������׼������ʼ��ȡս��
	SMsgView_MV_StartView msg;
	msg.rID = m_ReplayID;
	const SGameMsgHead& header = gMsg.BuildHead_MV(ENUM_MSG_VIEW_START_VIEW);
	TSendMsg(pView, header, msg);
}
