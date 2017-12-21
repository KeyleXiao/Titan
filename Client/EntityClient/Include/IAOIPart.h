/*******************************************************************
** 文件名:	IAOIPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	12/11/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once
/**
	AOI部件: 控制角色视野和属性更新
*/

#include "IEntity.h"

struct IAOIPart : public IEntityPart
{
    // 获取视野半径
    virtual int getSight() = 0;

    // 是否在主角焦点范围内
    virtual bool isInMasterEyeFocus(float x, float y, float z) = 0;
};
