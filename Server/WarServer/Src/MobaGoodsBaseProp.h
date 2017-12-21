/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\MobaGoodsProp.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 秦其勇
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场属性管理信息 基础属性
** 应  用: 
**************************** 修改记录 ******************************/
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

	// 槽位 对应的效果ID
	typedef list<SPropInfo> T_LIST_BASE_EFFECT_ALL;

	T_LIST_BASE_EFFECT_ALL m_EffectAll;
};