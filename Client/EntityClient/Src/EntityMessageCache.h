/*******************************************************************
** 文件名:	EntityMessageCache.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/10/13
** 版  本:	1.0
** 描  述:	实体消息缓存，用于客户端还没创建好实体，但是服务器已经下发了实体消息时，需要缓存消息，等实体创建好后重新执行消息

********************************************************************/

#pragma once

#include "IEventEngine.h"
#include "EntityDef.h"
#include "NetMessageDef.h"
#include "IEventEngine.h"
using namespace rkt;

#include <map>

// 清除缓存消息定时器ID
#define TIMERID_CLEAR_CACHE_MSG  0
// 清除缓存消息定时器间隔
#define TIMER_INTERNAL_CLEAR_CACHE_MSG  1000

// 缓存消息最大存在时间
#define LIFETIME_CACHE_MSG	5000

// 实体消息缓存结构
struct SEntityMsgCache
{
	SNetMsgHead head;
	SMsgEntityHead entityHead;
	char* data;
	size_t len;

	SEntityMsgCache()
	{
		memset(this, 0, sizeof(*this));
	}

	void release()
	{
		safeDelete(data);

		delete this;
	}
};

struct SEntityMsgCacheInfo
{
	DWORD cacheTime;
	list<SEntityMsgCache*> msgList;

	void release()
	{
		list<SEntityMsgCache*>::iterator iterList = msgList.begin();
		for (; iterList != msgList.end(); ++iterList)
		{
			if (*iterList != NULL)
			{
				(*iterList)->release();
			}
		}
		msgList.clear();

		delete this;
	}
};

// 实体消息缓存
class CEntityMessageCache : public rkt::TimerHandler, public Singleton<CEntityMessageCache>
{
	typedef std::map<UID, SEntityMsgCacheInfo*> ENTITYMSGCACHEMAP;
	typedef ENTITYMSGCACHEMAP::iterator ENTITYMSGCACHEMAP_ITER;

public:
	CEntityMessageCache();
	~CEntityMessageCache();
	
public:
	/////////////////////////TimerHandler///////////////////////////////////////////////////////
	virtual void OnTimer(unsigned long dwTimerID) override;

public:
	// 初始化
	bool Init();

	// 释放
	void UnInit();

	// 添加一个消息
	void AddEntityMessage(SNetMsgHead* head, SMsgEntityHead* entityHead, char* data, size_t len);

	// 执行一个实体的所有消息
	void RunMessage(UID uid);
private:

	ENTITYMSGCACHEMAP m_EntityMsgCacheList;
};
