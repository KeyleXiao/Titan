/*******************************************************************
** 文件名:	Monster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			怪物/NPC对象
********************************************************************/


#pragma once

#include "EntityBase.h"

class Monster : public EntityBase
{
public:
	Monster()
    {

    }

	virtual~ Monster()
    {

    }

	virtual bool isHero()
    {
        return false;
    }

	virtual ENTITY_MASK getTypeMask() 
    {
        return (ENTITY_MASK)(MASK_MONSTER | MASK_CREATURE);
    }

	/////////IClientEntity//////////////////////////////////////////////////////
	// 取得数值属性
	virtual int			getIntProperty(int nPropID);

	// 取得字符串属性
	virtual char*		getStringProperty(int nPropID);

	// 取得来源游戏世界ID
	virtual	int			getFromGameWorldID();

	/////////EntityBase//////////////////////////////////////////////////////
    virtual IEntityPart * createEntityPart( ENTITY_PART id );

    /** 获取实体显示层的基础信息(用于createView)
    @name				: 
    @param				: 
    @return
    */
    virtual bool getBasicViewInfo( EntityViewItem & item );

    const SMonsterScheme* getMonsterScheme();
};
