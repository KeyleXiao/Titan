/*******************************************************************
** �ļ���:	TankProperty.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	12/23/2014
** ��  ��:	1.0
** ��  ��:	

			���NPC���Լ���
********************************************************************/

#pragma once

#include "EntityProperty.h"
#include "Vector3.h"

#pragma pack(1)


// �������Լ�����
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