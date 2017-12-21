/*******************************************************************
** 文件名: e:\CoGame\SchemeEngine\include\MobaLeechdom.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 秦其勇
** 日  期: 2015/3/11
** 版  本: 1.0
** 描  述: 战场管理信息
** 应  用: 
**************************** 修改记录 ******************************/
#pragma once

#include "stdafx.h"
#include "MobaGoodsDef.h"
#include "MobaGoodsSchemeDef.h"
#include "IEntity.h"
#include "MobaGoods.h"

class CMobaLeechdom: public CMobaGoods
{
public:
	CMobaLeechdom();

	~CMobaLeechdom();

	virtual void				Initial(__IEntity * pPerson, int nSlotIndex, int nGoodID);

	virtual void				AddGoodsEffect();

	virtual void				RemoveGoodsEffect();

	/** 重置使用可以使用次数
	@param   
	@param   
	@return  重置消耗次数物品能使用的次数
	*/
	virtual bool				ResetCanUseCount();

	/** 移除
	@param   
	@param   
	*/
	virtual bool				Remove();


	/** 能否使用
	@param   
	@param   
	@return  
	*/
	virtual bool				CanUse();

	/** 能否使用
	@param   
	@param   
	@return  
	*/
	virtual bool				Use();

	virtual void				Release();

private:
};