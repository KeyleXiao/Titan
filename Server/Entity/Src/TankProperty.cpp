/*******************************************************************
** �ļ���:	TankProperty.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	8/1/2014
** ��  ��:	1.0
** ��  ��:	

			�ؾ����Լ�����
********************************************************************/

#include "stdafx.h"
#include "TankProperty.h"

// ��ľ�̬��Ա��Ҫʵ������
DATA_CHUNK_DECLARE(TankProperty);

STATIC_INITIALIZE( EntityPart_TankProperty );

EntityPart_TankProperty::EntityPart_TankProperty()
{
	memcpy(&m_PropertyDeclare, &s_PropertyDeclare, sizeof(m_PropertyDeclare));
}


EntityPart_TankProperty::~EntityPart_TankProperty()
{

}
