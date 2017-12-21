/*******************************************************************
** 文件名:	IPlayerKillPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/04/2016
** 版  本:	1.0
** 描  述:	PK部件接口
********************************************************************/

#pragma once

#include "IEntity.h"

struct IPlayerKillPart : public IEntityPart
{
    // 是否和此目标进行PK
    virtual bool isWithPK(IEntity * pEntity) = 0;
};