/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\MobaGoodsProp.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 秦其勇
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场属性管理信息
** 应  用: 
**************************** 修改记录 ******************************/
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

	/** 取得属性配置信息
	@param   
	@param   
	@return  
	*/
	virtual SSchemeGoodEffectGroupType*	GetSchemeInfo(int nEffectID);

	virtual void				Release();

private:
};