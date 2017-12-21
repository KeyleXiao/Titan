/*******************************************************************
** 文件名:	EntityFactory.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体工厂
********************************************************************/

#pragma once

#include "IEntity.h"
#include "PlayerRole.h"
#include "buffer.h"
#include "ISkillPart.h"
#include "fast_id_map.h"


#define ENTITY_TYPE_MAX   (1<<6)

class EntityFactory : public IEntityFactory
{
	typedef  fast_id_map<int,__IEntity *>   ENTITY_LIST;

	ENTITY_LIST         m_entityList[ENTITY_TYPE_MAX];

public:
	// 创建实体
	// @param type   : 实体类型
	// @param context: 创建现场
	// @param len    : 长度
	// @param flag   : 现场序列化标识，比如从数据库现场创建，从服务器发给客户端的现场创建
	virtual __IEntity * createEntity( ENTITY_TYPE type,void * context,int len,int flag )
	{
		__IEntity * pEntity = 0;

		switch(type)
		{
		case TYPE_PLAYER_ROLE:
			pEntity = new PlayerRole();
		default:
			break;
		}

		if ( pEntity==0 )
		{
			ErrorLn("entity type is unrecognized!"<<type);
			return 0;
		}

		int index = m_entityList[type].allotID();
		m_entityList[type].setData(index,pEntity);
		++index;                                    // 这个索引不能从0开始，因为事件订阅时会用这个当作srcID,而srcID==0时为订阅所有对象事件

		UID_DATA uid;
		uid.type  = type;
		uid.serial= index;

		if ( !pEntity->create(*(UID*)&uid,context,len,flag) )
		{
			pEntity->release();
			ErrorLn("entity create error!");
			return 0;
		}

		return pEntity;
	}

	virtual __IEntity * getEntity( UID uid)
	{
		UID_DATA * data = (UID_DATA*)&uid;
		return m_entityList[data->type].getData(data->serial-1);// 这个索引不能从0开始，因为事件订阅时会用这个当作srcID,而srcID==0时为订阅所有对象事件
	}

	// 删除UID对应的实体
	virtual void revertUID( UID uid )
	{
		UID_DATA * data = (UID_DATA*)&uid;
		m_entityList[data->type].revertID(data->serial-1);  // 这个索引不能从0开始，因为事件订阅时会用这个当作srcID,而srcID==0时为订阅所有对象事件
	}
};
