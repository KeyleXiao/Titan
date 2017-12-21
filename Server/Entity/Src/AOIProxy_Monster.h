/*******************************************************************
** 文件名:	AOIProxy_Monster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/24/2014
** 版  本:	1.0
** 描  述:	

			怪物/NPC的AOI Proxy
********************************************************************/

#pragma once

#include "AOIProxy_Base.h"

// 怪物视野
class AOIProxy_Monster : public AOIProxy_Base 
{
public:
	AOIProxy_Monster( __IEntity * pHost) : AOIProxy_Base(pHost)
	{
	}
};