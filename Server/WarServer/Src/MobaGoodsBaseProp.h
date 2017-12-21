/*******************************************************************
** �ļ���: e:\CoGame\SchemeEngine\include\MobaGoodsProp.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾(www.weimingtech.com)
** ������: ������
** ��  ��: 2015/3/11
** ��  ��: 1.0
** ��  ��: ս�����Թ�����Ϣ ��������
** Ӧ  ��: 
**************************** �޸ļ�¼ ******************************/
#pragma once

#include "stdafx.h"
#include "IEntity.h"
#include "MobaGoodsDef.h"
#include "MobaGoodsProp.h"


class CMobaGoodsBaseProp: public CMobaGoodsProp
{
public:
	CMobaGoodsBaseProp();

	~CMobaGoodsBaseProp();

	virtual bool				AddEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID);

	virtual bool				RemoveEffect(int nSlotID,int nEffectID);

	virtual void				RemoveAllEffect();
	
	virtual void				Release();

private:

	// ��λ ��Ӧ��Ч��ID
	typedef list<SPropInfo> T_LIST_BASE_EFFECT_ALL;

	T_LIST_BASE_EFFECT_ALL m_EffectAll;
};