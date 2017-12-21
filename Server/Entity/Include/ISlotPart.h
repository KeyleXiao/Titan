/*******************************************************************
** 文件名:	ISlotPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	03/06/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IEntity.h"

// 实体状态部件接口
struct __ISlotPart : public __IEntityPart
{
	// 设置物品槽位列表
	virtual void		SetGoodsSlotList(LPCSTR pszData, int nDataLen) = NULL;

	// 获得相应槽位关技能ID
	virtual int			GetSlotSkillID(SLOT_TYPE SlotType, int nIndex) = NULL;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __ISlotPart * CreateSlotPart();
#	define	CreateSlotPartProc	CreateSlotPart
#else													/// 动态库版本
typedef __ISlotPart * (RKT_CDECL * procCreateSlotPart)();
#	define	CreateSlotPartProc	DllApi<procCreateSlotPart>::load(MAKE_DLL_NAME(ENTITY), "CreateSlotPart")
#endif
