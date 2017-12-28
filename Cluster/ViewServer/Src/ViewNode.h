#pragma once
#include "ViewMsgDef_Server.h"


struct ViewNode
{
	PlayerID		m_dwPlayerID;	// 玩家唯一ID
	ViewParam		m_sViewParam;

	bool	m_bRecv;		// 是否已经收到了“预备信息”
	DWORD	currTime;		// 已经看了的毫秒数
	DWORD	m_dwIndex;		// 已经播放的节点的下一个节点的索引

	ViewNode()
		: m_dwIndex(0)
		, m_bRecv(false)
		, currTime(0)
	{}
};