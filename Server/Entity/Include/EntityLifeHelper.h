/*******************************************************************
** 文件名:	EntityLifeHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-05
** 版  本:	1.0
** 描  述:	实体生命周期辅助类

********************************************************************/


#pragma once

#include "IEntity.h"


struct IEntityLife
{
	virtual void release() { }
};

// 人物
class ActorLife: public IEntityLife
{

};

// 怪物
class MonsterLife : public IEntityLife
{
public:
	MonsterLife(UID uid) : m_uid(uid), m_npc(0),m_bDieDestroy(false)
	{
        MonsterHelper helper(uid);
		INPCService * pNPCService = helper.m_ptr;
		if (pNPCService)
		{
			m_npc = pNPCService ->findNPC(uid);
		}
	}
	~MonsterLife()
	{
		//TraceLn("MonsterLife::~MonsterLife(), m_uid="<< m_uid << ", bDieDestroy="<< (m_bDieDestroy?"True":"False") );
		m_npc = NPC_PTR(0);
		m_uid = 0;
		m_bDieDestroy = false;
	}

	// 设置死亡要执行销毁功能
	void setDieDestroy(bool bDestroy)
	{
		m_bDieDestroy = bDestroy;
	}

	// 生命周期到后回调
	static void destoryCallBack(void * pLife)
	{
		if (pLife)
		{
			((MonsterLife*)pLife)->ExecuteDestroyNPC();
		}
	}

	void ExecuteDestroyNPC()
	{
		if (m_bDieDestroy)
		{
			MonsterHelper helper(m_uid);
			INPCService * pNpcService = helper.m_ptr;
			if (pNpcService != NULL)
			{
				pNpcService->destroyNPC(m_uid);
			}
		}
	}
	
	virtual void release()
	{
		delete this;
	}

private:
	NPC_PTR		m_npc;
	UID			m_uid;
	bool		m_bDieDestroy;
};

// 载具
class TankLife : public IEntityLife
{

};

