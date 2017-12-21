/*******************************************************************
** �ļ���: e:\CoGame\SchemeEngine\include\MobaGoodsProp.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾(www.weimingtech.com)
** ������: ������
** ��  ��: 2015/3/11
** ��  ��: 1.0
** ��  ��: ս�����Թ�����Ϣ
** Ӧ  ��: 
**************************** �޸ļ�¼ ******************************/
#pragma once

#include "stdafx.h"
#include "IEntity.h"
#include "MobaGoodsDef.h"
#include "MobaGoodsSchemeDef.h"

class CMobaGoodsProp: public IMobaGoodsProp
{
public:
	CMobaGoodsProp();

	~CMobaGoodsProp();

    int                         AddEntityGoodsEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID);

    void                        removeEntityGoodsEffect(UID TargetUID, int nEntityRecEffectIndex);

	virtual bool				AddEffect(int nSlotID, int nEffectID, UID OperatorUID, UID TargetUID);
	virtual bool				RemoveEffect(int nSlotID,int nEffectID);
	virtual void				RemoveAllEffect();

	/** ȡ������������Ϣ
	@param   
	@param   
	@return  
	*/
	virtual SSchemeGoodEffectGroupType*	GetSchemeInfo(int nEffectID);

	virtual void				Release();

private:
};