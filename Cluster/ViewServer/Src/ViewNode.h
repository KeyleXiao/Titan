#pragma once
#include "ViewMsgDef_Server.h"


struct ViewNode
{
	PlayerID		m_dwPlayerID;	// ���ΨһID
	ViewParam		m_sViewParam;

	bool	m_bRecv;		// �Ƿ��Ѿ��յ��ˡ�Ԥ����Ϣ��
	DWORD	currTime;		// �Ѿ����˵ĺ�����
	DWORD	m_dwIndex;		// �Ѿ����ŵĽڵ����һ���ڵ������

	ViewNode()
		: m_dwIndex(0)
		, m_bRecv(false)
		, currTime(0)
	{}
};