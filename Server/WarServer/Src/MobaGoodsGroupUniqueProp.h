/*******************************************************************
** �ļ���: e:\CoGame\SchemeEngine\include\MobaGoodsProp.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾(www.weimingtech.com)
** ������: ������
** ��  ��: 2015/3/11
** ��  ��: 1.0
** ��  ��: ս�����Թ�����Ϣ ��Ψһ����
** Ӧ  ��: 
**************************** �޸ļ�¼ ******************************/
#pragma once

#include "stdafx.h"
#include "IEntity.h"
#include "MobaGoodsDef.h"
#include "MobaGoodsProp.h"

class CMobaGoodsGroupUniqueProp: public CMobaGoodsProp
{
public:
	CMobaGoodsGroupUniqueProp();

	~CMobaGoodsGroupUniqueProp();

	virtual bool				AddEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID);

	virtual bool				RemoveEffect(int nSlotID, int nEffectID);

	virtual void				RemoveAllEffect();

	virtual void				Release();

private:

	// ��ID ��Ӧ��Ч���ṹ
	typedef map<int,SPropInfo> T_MAP_GROUP_EFFECT_ALL;

	T_MAP_GROUP_EFFECT_ALL m_EffectAll;
};