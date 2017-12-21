/*******************************************************************
** 文件名:	MonsterProperty.h
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
#include "IEventEngine.h"


#include "IEventEngine.h"

#pragma pack(1)


// 怪物属性集部件
class EntityPart_MonsterProperty : public EntityProperty<MonsterProperty>, public IEventExecuteSink
{
public:
    static void init()
    {
        MonsterProperty::serialize_init( s_PropertyDeclare,_countof(s_PropertyDeclare) );
    }

	EntityPart_MonsterProperty();
	~EntityPart_MonsterProperty();

    // 更新显示层属性信息(参数为PROPERTY_MAX表示更新所有属性到显示层)
    virtual void updateView( int prop_id );

    virtual void onPropertyChange( int prop_id );

	virtual void release();

	virtual bool onLoad(IEntity* pEntity);

	///////////////////IEventExecuteSink////////////////////////////////////
	// 订阅冷却改变事件，然后同步到显示层
	virtual void			OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);


private:
    void push_update_view(int prop_id );

	bool changeFriendCampNPCMask();

private:
    // 最后一次HP更新到显示层Tick
    ulong       m_dwLastViewHPTick;
};

#pragma pack()