/*******************************************************************
** 文件名:	PlayerRole.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家实体对象
********************************************************************/


#pragma once

#include "EntityBase.h"

class PlayerRole : public EntityBase
{
protected:
    bool                m_bHero;                            // 是否为当前玩家控制角色

public:
	PlayerRole() : m_bHero(false)
    {

    }

    virtual~ PlayerRole()
    {

    }

	/////////IEntity//////////////////////////////////////////////////////
	virtual bool isHero() const
    {
        return m_bHero;
    }

	virtual ENTITY_MASK getTypeMask() { return (ENTITY_MASK)(MASK_ACTOR | MASK_CREATURE); }

	/////////IClientEntity//////////////////////////////////////////////////////
	// 取得数值属性
	virtual int			getIntProperty(int nPropID);

	// 取得字符串属性
	virtual char*		getStringProperty(int nPropID);

	// 取得来源游戏世界ID
	virtual	int			getFromGameWorldID();
	/////////EntityBase//////////////////////////////////////////////////////
    bool create( UID uid,SEntiyExportShadowContext &shadowData,void * context,int len );

    virtual IEntityPart * createEntityPart( ENTITY_PART id );

    /** 获取实体显示层的基础信息(用于createView)
    @name				: 
    @param				: 
    @return
    */
    virtual bool getBasicViewInfo( EntityViewItem & item );

	/////////////PlayerRole/////////////////////////////////////////////////////////////
	const PersonSkinModelItem* getPersonSkinModelItemScheme(); 
};

