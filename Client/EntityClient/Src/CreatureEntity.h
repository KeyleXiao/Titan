/*******************************************************************
** 文件名:	CreatureEntity.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	7/8/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once
#include "IClientEntity.h"

// 生物对象
class CreatureEntity : public IClientEntity
{
public:
	virtual bool create( ACTOR_INFO * pInfo );

	virtual UID getID()
	{
		return m_nUID;
	}

	virtual ENTITY_ID getViewID()
	{
		return m_nViewID;
	};

	virtual bool onEvent( int32 nEventID,int32 param1,int32 param2 );

	virtual void handleMsg( int32 nMsgID,void * data,int32 len );

	virtual void release();

	CreatureEntity() : m_nUID(0),m_nViewID(0){}

protected:
	ENTITY_ID   m_nUID;
	ENTITY_ID   m_nViewID;
};
