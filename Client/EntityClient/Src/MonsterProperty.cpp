/*******************************************************************
** 文件名:	MonsterProperty.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			怪物、NPC属性集合
********************************************************************/

#include "stdafx.h"
#include "MonsterProperty.h"
#include "IClientEntity.h"
#include "ICamp.h"

#include "EventDef.h"

// 类的静态成员需要实例化下
DATA_CHUNK_DECLARE(MonsterProperty);

STATIC_INITIALIZE( EntityPart_MonsterProperty );

EntityPart_MonsterProperty::EntityPart_MonsterProperty()
{
	memcpy(&m_PropertyDeclare, &s_PropertyDeclare, sizeof(m_PropertyDeclare));

    // 最后一次HP更新到显示层Tick
    m_dwLastViewHPTick = 0;
}


EntityPart_MonsterProperty::~EntityPart_MonsterProperty()
{

}

void EntityPart_MonsterProperty::release()
{ 
	if(m_pEntity != NULL)
	{
		UID uid = m_pEntity->getUID();
		BYTE type = UID_2_TYPE(uid);
		gClientGlobal->getEventEngine()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE, type, uid);
	}


	delete this; 
}

bool EntityPart_MonsterProperty::onLoad(IEntity* pEntity)
{
	if(pEntity == NULL)
		return false;

	m_pEntity = pEntity;

	UID uid = m_pEntity->getUID();
	BYTE type = UID_2_TYPE(uid);
	gClientGlobal->getEventEngine()->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE, type, uid,__FILE__);
	return true;
}

///////////////////IEventExecuteSink////////////////////////////////////
// 订阅冷却改变事件，然后同步到显示层
void EntityPart_MonsterProperty::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch(wEventID)
	{
	case EVENT_ENTITY_CREATE:
		{
			changeFriendCampNPCMask();
		}
		break;

	default:break;
	}
}

bool EntityPart_MonsterProperty::changeFriendCampNPCMask()
{
	// 友方NPC需要加上MASKFLAG_NOT_ATTACK标识，技能才可以选中该NPC
	int subType = getProperty_Integer(PROPERTY_VOCATION);
	if (subType == MONSTER_SUB_TYPE_NPC && Camp::getCampFlag( m_pEntity, gClientGlobal->getHero()) == CampFlag_Friend)
	{
		int nOldMask = getProperty_Integer(PROPERTY_MASK);
		if ((nOldMask & MASKFLAG_NOT_ATTACK) == MASKFLAG_NOT_ATTACK)
		{
			return false;
		}

		int nNewMask = nOldMask | MASKFLAG_NOT_ATTACK;
		setProperty_Integer(PROPERTY_MASK, nNewMask);
	}

	return true;
}


// 更新显示层属性信息(参数为PROPERTY_MAX表示更新所有属性到显示层)
void EntityPart_MonsterProperty::updateView( int prop_id )
{
    if ( prop_id==PROPERTY_MAX )
    {
        for ( int i=0;i<PROPERTY_MAX;++i )
        {
            updateView( i);
        }
        return;
    }

    int viewID = ((IClientEntity*)m_pEntity)->getViewID();
    if ( viewID==INVALID_ENTITY_ID )
        return;
	// 设置数值属性
	entity_update_Numproperty updatePropData;
	updatePropData.nOtherData = 0;		// 状态数据
	updatePropData.uID = viewID;        //显示层的实体ID 
	updatePropData.isIntproperty = 0;
	updatePropData.isRaiseIprEvent = 0;

    // 通告显示层更新
    switch ( prop_id )
    {
    case PROPERTY_NAME:			// 名字
        {
            // 怪物直接从配置表中获取名字
            const char * name = getProperty_String(PROPERTY_NAME);
            if(strlen(name) < 2)
            {
                int nMonsterID = getProperty_Integer(PROPERTY_ID);
                SMonsterScheme* pMonsterScheme = gClientGlobal->getSchemeCenter()->getSchemeMonster()->getMonsterShemeInfo(nMonsterID);
                if(pMonsterScheme)
                {
                    setProperty_String(PROPERTY_NAME, pMonsterScheme->szName);
                }
                return;
            }
			m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SYNC_NAME,0,name,0 ,0 );
        }
        break;

	case PROPERTY_SKIN:			// 皮肤
		{
			updatePropData.nPropID = prop_id;	// 属性ID
			updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
			updatePropData.nOtherData = -1;//暂时写死-1，以后再定义 
			updatePropData.isIntproperty = 1;
			updatePropData.isRaiseIprEvent = 1;
		}
		break;
	case PROPERTY_HP:			// 血量
	case PROPERTY_MAX_HP:
	case PROPERTY_CAMP:			// 阵营
	case PROPERTY_SHIELD:		// 护盾（临时血量）
		{
			updatePropData.nPropID = prop_id;	// 属性ID
			updatePropData.nValue  =  getProperty_Integer(prop_id);	// 属性数值
			updatePropData.nOtherData = m_pEntity->isHero()? CampFlag_Self : Camp::getCampFlag( gClientGlobal->getHero(),m_pEntity );;	// 属性数值
			updatePropData.isIntproperty = 1;
			updatePropData.isRaiseIprEvent = 1;
		}
		break;

	case PROPERTY_MASK:		// 标志位
		{
			if(!changeFriendCampNPCMask())
				return;

			updatePropData.nPropID = prop_id;	// 属性ID
			updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
			updatePropData.isIntproperty= 1;
			updatePropData.isRaiseIprEvent = 1;
		}
		break;

	case PROPERTY_ID:			// 怪物ID
    case PROPERTY_SEX:
    case PROPERTY_VOCATION:
    case PROPERTY_SCENEID:
    case PROPERTY_MASTER_UID:
	case PROPERTY_SD:           // 速度
    case PROPERTY_VISIBLE_FLAG: // 实体对各个阵营的可见标志掩码
        {
            updatePropData.nPropID = prop_id;	// 属性ID
            updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
            updatePropData.isIntproperty= 1;
            updatePropData.isRaiseIprEvent = 1;
        }
        break;
    default:
        break;
    }

	// 数值属性更新
	if( updatePropData.isIntproperty )
	{
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_INTEGERPROPERTY,sizeof(updatePropData),0,&updatePropData,sizeof(updatePropData));  //更新实体的数值属性
	}
}

void EntityPart_MonsterProperty::onPropertyChange( int prop_id )
{
    if ( m_pEntity==NULL )
        return;

	//怪物的属性就别关心了吧，大量怪物时订阅抛出处理很耗时
	if (prop_id!=PROPERTY_LOCATION && prop_id!=PROPERTY_ANGLE)//高频设置的属性不抛出属性变化事件
	{
		event_entity_property_change context;
		context.nPropertyID = prop_id;
		m_pEntity->fireEvent( EVENT_ENTITY_PROPERTY_CHANGE,&context,sizeof(context) );
	}

    push_update_view(prop_id);
}


void EntityPart_MonsterProperty::push_update_view(int prop_id)
{
    // 针对血量进行控制频率，防止更新UI太猛
    bool need_update_view = true;
    do 
    {
        if(prop_id != PROPERTY_HP)
        {
            break;
        }

        // 没有血量时直接下发
        if(getProperty_Integer(PROPERTY_HP) == 0)
        {
            break;
        }

        // 塔类缩短为50ms
        //DWORD freezeTick = 50; // (getProperty_Integer(PROPERTY_SEX) == MONSTER_TYPE_TOWER) ? 200 : 500;
        DWORD freezeTick = IsInMasterEyeFocus(m_pEntity) ? 50 : 500;

        // 控制刷很多次，范围内的将不下发
        DWORD dwCurTick = getTickCount();
        if(dwCurTick > m_dwLastViewHPTick + freezeTick)
        {
            m_dwLastViewHPTick = dwCurTick;
            break;
        }

        // 不下发
        need_update_view = false;

    } while (false);

    // 伤害下发到显示层
    if(need_update_view)
    {
        updateView( prop_id );
    }
}