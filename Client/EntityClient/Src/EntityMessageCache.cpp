/*******************************************************************
** 文件名:	EntityMessageCache.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/10/13
** 版  本:	1.0
** 描  述:	实体消息缓存，用于客户端还没创建好实体，但是服务器已经下发了实体消息时，需要缓存消息，等实体创建好后重新执行消息

********************************************************************/

#include "stdafx.h"
#include "EntityMessageCache.h"
#include "Event_ManagedDef.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"

//#define ENTITY_MSG_CACHE_PRINT

CEntityMessageCache::CEntityMessageCache()
{
}

CEntityMessageCache::~CEntityMessageCache()
{
	ENTITYMSGCACHEMAP_ITER iterMap = m_EntityMsgCacheList.begin();
	for (; iterMap != m_EntityMsgCacheList.end(); ++iterMap)
	{
		SEntityMsgCacheInfo* pInfo = iterMap->second;
		safeRelease(pInfo);
	}
	m_EntityMsgCacheList.clear();

}

bool CEntityMessageCache::Init()
{
	gClientGlobal->getTimerAxis()->SetTimer(TIMERID_CLEAR_CACHE_MSG, TIMER_INTERNAL_CLEAR_CACHE_MSG, static_cast<ITimerHandler*>(this));

	return true;
}

void CEntityMessageCache::UnInit()
{
	gClientGlobal->getTimerAxis()->KillTimer(TIMERID_CLEAR_CACHE_MSG, static_cast<ITimerHandler*>(this));
}

void CEntityMessageCache::AddEntityMessage(SNetMsgHead * head, SMsgEntityHead * entityHead, char * data, size_t len)
{
	UID uid = entityHead->uidMaster;

	if (uid == INVALID_UID || uid == 0)
		return;

	SEntityMsgCache* pCacheMsg = new SEntityMsgCache();
	pCacheMsg->head = *head;
	pCacheMsg->entityHead = *entityHead;
	pCacheMsg->data = new char[len];
	pCacheMsg->len = len;

	memcpy(pCacheMsg->data, data, len);

	if (m_EntityMsgCacheList.find(uid) == m_EntityMsgCacheList.end())
	{
		SEntityMsgCacheInfo* pInfo = new SEntityMsgCacheInfo();
		pInfo->cacheTime = time(NULL);
		m_EntityMsgCacheList[uid] = pInfo;
	}
	m_EntityMsgCacheList[uid]->msgList.push_back(pCacheMsg);

#ifdef  ENTITY_MSG_CACHE_PRINT
	if (entityHead->byPartID == PART_BUFF)
	{
		if (head->byKeyAction == 0)
		{
			struct SBuffMsgAddBuff_SC
			{
				DWORD		dwIndex;		// 序号
				DWORD		dwBuffID;		// BUFF ID
				WORD		dwBuffLevel;	// BUFF 等级
				DWORD		dwTime;			// 时间长
				UID			uidAdd;			// 增加buff者
				Vector3		targetTile;		// 加buff时候鼠标位置
				int			nSpellID;		// 技能ID

				SBuffMsgAddBuff_SC(void)
				{
					memset(this, 0, sizeof(*this));
				}
			};

			SBuffMsgAddBuff_SC *pMsgAddBuff = (SBuffMsgAddBuff_SC *)data;
			if (pMsgAddBuff->dwBuffID == 2476)
			{
				WarningLn("EntityMsgCache,Add,add buff, buffid=" << pMsgAddBuff->dwBuffID);
			}
		}
	}
	else
	{
		WarningLn("EntityMsgCache,Add, uid=" << uid << ",partId=" << entityHead->byPartID << ",moduleId=" << head->byKeyModule << ",msgId=" << head->byKeyAction);
	}
#endif //  ENTITY_MSG_CACHE_PRINT
}

void CEntityMessageCache::RunMessage(UID uid)
{
	if (uid == INVALID_UID || uid == 0)
		return;

	ENTITYMSGCACHEMAP_ITER iterMap = m_EntityMsgCacheList.find(uid);
	if (iterMap == m_EntityMsgCacheList.end())
		return;

	if (iterMap->second == NULL)
		return;

	list<SEntityMsgCache*>::iterator iterList = iterMap->second->msgList.begin();
	for (; iterList != iterMap->second->msgList.end(); ++iterList)
	{
		if (*iterList != NULL)
		{
			IEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(uid);
			if (pEntity != NULL)
			{
				SEntityMsgCache* pMsg = *iterList;
				pEntity->handMessage(&pMsg->head, &pMsg->entityHead, pMsg->entityHead.byPartID, pMsg->head.byKeyAction, pMsg->data, pMsg->len);

#ifdef  ENTITY_MSG_CACHE_PRINT
				WarningLn("EntityMsgCache,Run,uid=" << uid << ",partId=" << pMsg->entityHead.byPartID << ",moduleId=" <<
					pMsg->head.byKeyModule << ",msgId=" << pMsg->head.byKeyAction);
#endif
			}
			safeRelease(*iterList);
		}
	}
	safeRelease(iterMap->second);

	// 删除该实体的所有缓存消息
	m_EntityMsgCacheList.erase(iterMap);
}

void CEntityMessageCache::OnTimer(unsigned long dwTimerID)
{
	if (dwTimerID == TIMERID_CLEAR_CACHE_MSG)
	{
		if (m_EntityMsgCacheList.empty())
			return;

		DWORD now = time(NULL);

		ENTITYMSGCACHEMAP_ITER iterMap = m_EntityMsgCacheList.begin();
		for (; iterMap != m_EntityMsgCacheList.end();)
		{
			if (iterMap->second == NULL || now - iterMap->second->cacheTime >= LIFETIME_CACHE_MSG)
			{
				// 超时则释放
				safeRelease(iterMap->second);
				iterMap = m_EntityMsgCacheList.erase(iterMap);
			}
			else
			{
				++iterMap;
			}
		}
	}
}