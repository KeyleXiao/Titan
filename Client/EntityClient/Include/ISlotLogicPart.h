/*******************************************************************
** 文件名:	ISlotLogicPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇 
** 日  期:	2015/1/30  21:15
** 版  本:	1.0
** 描  述:	槽位控制逻辑层
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __COMMON_I_SlotLogicCLIENT_H__
#define __COMMON_I_SlotLogicCLIENT_H__


#include "IEntity.h"
#include "EntityViewDef.h"

struct SMiscBuildContext;

// 冷却部件
struct ISlotLogicPart : public IEntityPart
{

	/** 更新
	@param   
	@param   
	@return  
	*/
	virtual void			UpdateSlot(cmd_creature_set_slot *SlotData) = NULL;

	// 获得相应槽位关技能ID
	virtual int				GetSlotSkillID(SLOT_TYPE SlotType, int nIndex) = NULL;

	// 获得技能ID相应槽位相关信息
	virtual bool			GetSkillIDInfo(int SkillID, SLOT_TYPE &SlotType, int &nIndex) = NULL;

    // 进入别人视野发送装备列表
    virtual void			EnterViewSendGoodsRecord() = NULL;

};


#endif	// __COMMON_I_SlotLogicCLIENT_H__