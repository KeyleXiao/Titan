/*******************************************************************
** 文件名:	EntityViewDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/21/2014
** 版  本:	1.0
** 描  述:

定义显示层和逻辑层通讯的实体相关命令
** 修  改:	定义命令相关的结构体, 减少与显示层的耦合		彭璐		11/13/2014
********************************************************************/


#pragma once

#include "GameDef.h"
#include "ManagedCode.h"

namespace Published
{
	ENUM PublishedID
	{
		/// <summary>
		/// 无效的渠道
		/// </summary>
		Published_Invalid = 0,

		/// <summary>
		/// 冰川网络
		/// </summary>
		Published_BingChuan = 1,

		/// <summary>
		/// steam
		/// </summary>
		Published_Steam = 2,


		/// <summary>
		/// MAX
		/// </summary>
		Published_MAX = 3,
	};
}
using namespace Published;
