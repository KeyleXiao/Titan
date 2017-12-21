/*******************************************************************
** 文件名:	SpellServerDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/17/2015
** 版  本:	1.0
** 描  述:	技能服务器常用定义
********************************************************************/

#ifndef __SPELL_SERVER_DEF_H__
#define __SPELL_SERVER_DEF_H__

#pragma pack(1)

// 释放技能数据
struct SCastSpellData
{
	// 操作者
	UID			uidOperator;
	// 操作时间
	DWORD		dwTick;

	SCastSpellData()
	{
		memset(this, 0, sizeof(*this));
	}
};

#pragma pack()
#endif	// __SPELL_MESSAGE_DEF_H__