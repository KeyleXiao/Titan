/*******************************************************************
** 文件名:	ReplayMng.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/18
** 版  本:	1.0
** 描  述:	观战管理器
** 应  用:  这里记录观战逻辑的所有逻辑信息
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "ReplayNode.h"


enum EnumResultRequestReplay;

class ReplayMng : public Singleton<ReplayMng>
{
public:
	~ReplayMng();

	EnumResultRequestReplay	Find(ReplayID rID, ReplayNode*& pNode);

private:
	//************************************
	// Returns:   ViewID
	// Qualifier: 查找负载最轻的ViewServer，返回其ID；如果找不到，则返回0
	// Parameter: ViewID exceptID	要排除的ViewServerID
	//************************************
	ViewID	GetIdleView(ViewID exceptID = 0);

private:
	typedef std::map<ViewID, ReplayNode*>		OneReplayMap;	// <ViewServerID, 在该ViewServer观看该战报的节点信息>
	typedef	std::map<ReplayID, OneReplayMap*>	ReplayMap;		// <战报ID，观看这场战报的所有节点Map>
	typedef	std::map<ViewID, DWORD>				ViewMap;		// <ViewServerID, 该ViewServer的人数>

	ReplayMap	m_map;
	ViewMap		m_ViewMap;

	//MyRand		m_Rand;
};
extern ReplayMng& gReplayMng;