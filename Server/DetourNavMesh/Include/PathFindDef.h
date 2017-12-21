/*******************************************************************
** 文件名:	PathFindDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-05-20
** 版  本:	1.0
** 描  述:	寻路模块公共定义			
********************************************************************/


#pragma once

#include "Vector3.h"

#define PATH_FIND_DEBUG
#ifdef PATH_FIND_DEBUG
#	define PATH_TRACELN(x)		TraceLn(x)
#	define PATH_ERRORLN(x)		//ErrorLn(x)
#else
#	define PATH_TRACELN(x)
#	define PATH_ERRORLN(x)
#endif


#define PATHFIND_MSC_POLYREF_MAX			1000
#define PATHFIND_MSC_STRAIGHTPATH_MAX		1000
#define PathAgentNeighbourMaxCount			10


struct AgentNeighbour
{
	int		m_nIndex;		
	float	m_fDist;

	AgentNeighbour()
	{
		memset(this, 0, sizeof(*this));
	}
};