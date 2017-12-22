#pragma once



class ReplayNode
{
public:
	ReplayNode(ViewID dwViewID, ReplayID replayID)
		: m_ViewID(dwViewID)
		, m_ReplayID(replayID)
	{
	}

	const ViewID&	GetViewID() { return m_ViewID; }

	// �ù�ս�ڵ��Ƿ�����
	bool	IsFull();
	// �ܳ��ص�ʣ������
	DWORD	RemainCount();
	// ֪ͨ��ViewServer�����ò���ĳս����׼��
	void	NotifyView();

private:
	typedef	std::set<PlayerID>	PlayerIDSet;

	const ViewID	m_ViewID;		// ViewServerID
	const ReplayID	m_ReplayID;		// ս��ID
	PlayerIDSet		m_SetPlayerID;	// �������սս�������ID����
};