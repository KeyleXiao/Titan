/*******************************************************************
** 文件名:	IWingPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	翅膀部件接口
********************************************************************/

#pragma once

#include "IEntity.h"

struct SWing;

struct IWingPart : public IEntityPart
{
    virtual SWing* getWing() = 0;
};