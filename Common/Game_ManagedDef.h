/*******************************************************************
** 文件名:	Game_ManagedDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017-11-3
** 版  本:	1.0
** 描  述:	游戏公共定义托管公共定义


********************************************************************/

#pragma once

#include "ManagedCode.h"


namespace Game_ManagedDef
{

	/// <summary>
	/// 游戏模式的类型
	/// </summary>
	ENUM EGameMode
	{
		EGM_Moba = 0,	                // Moba类游戏
		EGM_RPG,		                // RPG类游戏
		EGM_MCAMP,		                // 多阵营类游戏
		EGM_MAX,
	};

}

using namespace Game_ManagedDef;