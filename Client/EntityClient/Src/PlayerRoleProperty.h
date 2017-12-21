/*******************************************************************
** 文件名:	PlayerRoleProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家属性集对象
********************************************************************/

#pragma once

#include "EntityProperty.h"
#include "IEventEngine.h"
#include "Vector3.h"
#include "SpellDef.h"
using namespace SPELL;

#pragma pack(1)


// 玩家属性集部件
class EntityPart_PlayerRoleProperty : public EntityProperty<PlayerRoleProperty>
{
public:
	static void init()
	{
        PlayerRoleProperty::serialize_init( s_PropertyDeclare,_countof(s_PropertyDeclare) );
	}

	EntityPart_PlayerRoleProperty();
	~EntityPart_PlayerRoleProperty();

    // 更新显示层属性信息(参数为PROPERTY_MAX表示更新所有属性到显示层)
    virtual void updateView( int prop_id );

    virtual void onPropertyChange( int prop_id );

	virtual bool onCommand( int cmdid,void * data,size_t len );

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

private:
    void push_update_view(int prop_id );

    // 同步属性到显示层
    void onUpdateViewPropertyValue(int prop_id);

	void OnMessageEntityPropertyReturn(void *pData, size_t nLen);

private:
    // 最后一次HP更新到显示层Tick
    ulong       m_dwLastViewHPTick;
};

#pragma pack()

