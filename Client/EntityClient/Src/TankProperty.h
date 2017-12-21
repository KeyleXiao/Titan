/*******************************************************************
** 文件名:	TankProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	12/23/2014
** 版  本:	1.0
** 描  述:	

			载具属性集合
********************************************************************/

#pragma once

#include "EntityProperty.h"
#include "Vector3.h"
#include "IEventEngine.h"

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

    // 更新显示层属性信息(参数为PROPERTY_MAX表示更新所有属性到显示层)
    virtual void updateView( int prop_id );

    virtual void onPropertyChange( int prop_id );
};

#pragma pack()