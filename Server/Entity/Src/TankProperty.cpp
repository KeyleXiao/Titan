/*******************************************************************
** 文件名:	TankProperty.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			载具属性集对象
********************************************************************/

#include "stdafx.h"
#include "TankProperty.h"

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
