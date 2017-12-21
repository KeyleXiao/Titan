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

	// 该观战节点是否人满
	bool	IsFull();
	// 能承载的剩余人数
	DWORD	RemainCount();
	// 通知该ViewServer，做好播放某战报的准备
	void	NotifyView();
private:
	typedef	std::set<DWORD>	DBIDSet;

	const ViewID	m_ViewID;	// ViewServerID
	const ReplayID	m_ReplayID;	// 战报ID
	DBIDSet			m_SetDBID;	// 在这里观战战报的玩家DBID集合
};