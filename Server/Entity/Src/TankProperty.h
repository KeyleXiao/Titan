/*******************************************************************
** 文件名:	TankProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/23/2014
** 版  本:	1.0
** 描  述:	

			怪物、NPC属性集合
********************************************************************/

#pragma once

#include "EntityProperty.h"
#include "Vector3.h"

#pragma pack(1)


// 怪物属性集部件
class EntityPart_TankProperty : public EntityProperty<TankProperty>
{
public:
	static void init()
	{
       TankProperty::serialize_init( s_PropertyDeclare,_countof(s_PropertyDeclare) );
	}

	EntityPart_TankProperty();
	~EntityPart_TankProperty();
};

#pragma pack()