/*******************************************************************
** 文件名:	TankProperty.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			载具属性集合
********************************************************************/

#include "stdafx.h"
#include "TankProperty.h"
#include "IClientEntity.h"

// 类的静态成员需要实例化下
DATA_CHUNK_DECLARE(TankProperty);

STATIC_INITIALIZE( EntityPart_TankProperty );

EntityPart_TankProperty::EntityPart_TankProperty()
{
	memcpy(&m_PropertyDeclare, &s_PropertyDeclare, sizeof(m_PropertyDeclare));
}


EntityPart_TankProperty::~EntityPart_TankProperty()
{

}

// 更新显示层属性信息(参数为PROPERTY_MAX表示更新所有属性到显示层)
void EntityPart_TankProperty::updateView( int prop_id )
{
    if ( prop_id==PROPERTY_MAX )
    {
        for ( int i=0;i<PROPERTY_MAX;++i )
        {
            updateView( i );
        }
        return;
    }

    int viewID = ((IClientEntity*)m_pEntity)->getViewID();
    if ( viewID==INVALID_ENTITY_ID )
        return;
	// 设置数值属性
	entity_update_Numproperty updatePropData;
	updatePropData.nOtherData = 0;		// 状态数据
	updatePropData.uID = 0;        //显示层的实体ID 
	updatePropData.isIntproperty = 0;
	updatePropData.isRaiseIprEvent = 0;

    // 通告显示层更新
    switch ( prop_id )
    {
    case PROPERTY_NAME:			// 名字
        {
            const char * name = getProperty_String(PROPERTY_NAME);
			m_pEntity->sendCommandToEntityView( ENTITY_TOVIEW_SYNC_NAME,0,name,0 ,0 );
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
			updatePropData.nOtherData = getProperty_Integer(PROPERTY_CAMP);	// 阵营 与 人物不同 统一存放
			updatePropData.isIntproperty = 1;
			updatePropData.isRaiseIprEvent = 1;
		}
		break;
	case PROPERTY_SD:           // 速度
		{
			updatePropData.nPropID = prop_id;	// 属性ID
			updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
		//	updatePropData.isIntproperty= true;
		//	updatePropData.isRaiseIprEvent = true;
			m_pEntity->sendCommandToEntityView( ENTITY_TOVIEW_SET_SPEED,getProperty_Integer(PROPERTY_SD),0,0,0 );
		}
		break;
	case PROPERTY_LEVEL:		// 等级
		{
		}
		break;
    default:
        break;
    }
}

void EntityPart_TankProperty::onPropertyChange( int prop_id )
{
    if ( m_pEntity==NULL )
        return;

    event_entity_property_change context;
    context.nPropertyID = prop_id;
    m_pEntity->fireEvent( EVENT_ENTITY_PROPERTY_CHANGE,&context,sizeof(context) );

    updateView( prop_id );
}
