/*******************************************************************
** 文件名:	BuffPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/13  17:05
** 版  本:	1.0
** 描  述:	BUFF部件
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "BuffPart.h"
#include "ExternalFacade.h"
#include "SchemeBuff.h"
#include "BuffServer.h"
#include "DGlobalMessage.h"
#include <time.h>
#include "EntityDef.h"
#include "EventDef.h"
#include "IServerGlobal.h"
#include "IEntityEvent.h"
#include "EntityHelper.h"

/** 
@param   
@param   
@return  
*/
CBuffPart::CBuffPart(void)
{
	// 所属实体
	m_pMaster = NULL;

	// 实体的所有buff
	m_BuffMap.clear();

	// BUFF index
	m_dwBuffIndex = 0;

	// 是否需要备份buff
	m_bBackInitBuff = false;

	// 在创建过程中加的buff
	m_listInitBuff.clear();

	// 是否正在清除BUFF过程中
	m_bCleaningFlag = false;

	// 是否启动了清除无效BUFF定时器
	m_bSetClanInvalidTimer = false;

	// 不能添加BUFF的原因ID
	m_nErrorID = EBuffAdd_Failure;

	// 序列化时Buff表清空
	m_SerializeBuff.clear();
	// 反序列化时Buff表清空
	m_deSerializeBuff.clear();

	// 延迟删除BUFF表清空
	m_DelayRemoveBuffMap.clear();

	// 未启用延迟删除buff定时器
	m_bDelayRemoveBuffTimer = false;

    // 所有BUFF的FLAG标识
    m_dwBitFlag = 0;

	// 是否序列化buff
	m_bDeSerializeBuff = false;
}

/** 
@param   
@param   
@return  
*/
CBuffPart::~CBuffPart(void)
{
}

///////////////////ISerializable////////////////////////////////////
/** 序列化接口
@return  
*/
bool CBuffPart::onSerialize( rkt::ibuffer & in,int flag )
{
    if(flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {
        return true;
    }

	m_SerializeBuff.clear();
	// 取得Buff数量
	int nBuffCount = 0;
	in >> nBuffCount;
	for (int nIndex=0; nIndex<nBuffCount; ++nIndex)
	{
		SBuffMsgAddBuff_SC addBuff;
		in >> addBuff;

		m_SerializeBuff.push_back(addBuff);
	}

	return true;
}

/** 反序列化接口
@return  
*/
bool CBuffPart::deSerialize( rkt::obuf & out,int flag )
{
    if(flag&ACTOR_TO_HERO || flag&HERO_TO_ACTOR)
    {
        return true;
    }

	obuf obBuffData;
	// Buff数量
	int nBuffCount = 0;

	if (flag & TO_OTHER_SEVER)
	{
		SERIALIZEBUFF::iterator it = m_deSerializeBuff.begin();
		for (; it != m_deSerializeBuff.end(); ++it)
		{
			if (it->dwTime < 0/*时间已越过*/)
			{
				continue;
			}

			// 小于1秒的不转过去
			if (it->dwTime > 0 && it->dwTime < 1000)
			{
				continue;
			}

			++nBuffCount;
			obBuffData << *it;
		}
	}
	else
	{
		for (MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
		{
			TLIST_BUFF &listBuff = IterMap->second;
			for (TLIST_BUFF::iterator Iter = listBuff.begin(); Iter != listBuff.end(); ++Iter)
			{
				CBuff *pBuff = *Iter;
				if (pBuff == NULL || !pBuff->IsValid())
				{
					continue;
				}

				int nLeftTime = pBuff->GetLeftTime();
				if (nLeftTime < 0/*时间已越过*/)
				{
					continue;
				}

				// 小于1秒的不转过去
				if (nLeftTime > 0 && nLeftTime < 1000)
				{
					continue;
				}

				// 序列化给别人时
				if ((flag&TO_CLIENT_OTHER) == TO_CLIENT_OTHER)
				{
					DWORD dwBuffFlag = pBuff->GetFlag();
					bool bBcastFlag = false;
					if ((dwBuffFlag&tBuff_TypeFlag_BcastOnlyMaster) == tBuff_TypeFlag_BcastOnlyMaster)
					{
						// 在主人的客户端才生效，这里都发下去，然后客户端判断是否主人的客户端
						bBcastFlag = true;
					}
					else if ((dwBuffFlag&tBuff_TypeFlag_Bcast) == tBuff_TypeFlag_Bcast)
					{
						bBcastFlag = true;
					}
					else if ((dwBuffFlag&tBuff_TypeFlag_Client) == tBuff_TypeFlag_Client)
					{
						bBcastFlag = false;
					}
					else
					{
						continue;
					}

					// 序列化给别人时,不广播BUFF不带下去
					if (!bBcastFlag)
					{
						continue;
					}
				}

				SBuffMsgAddBuff_SC addBuff;
				addBuff.dwIndex = pBuff->GetIndex();
				addBuff.dwBuffID = pBuff->GetBuffID();
				addBuff.dwBuffLevel = (WORD)pBuff->GetBuffLevel();
				addBuff.dwTime = pBuff->GetLeftTime();
				addBuff.uidAdd = pBuff->GetAdd();
				addBuff.targetTile = pBuff->GetEffectTargetTile();

				++nBuffCount;
				obBuffData << addBuff;
			}
		}
	}
	// 压入Buff数量
	out << nBuffCount;
	out.push_back(obBuffData.data(), obBuffData.size());
	
	return true;
}

/** 取是部件ID
*/
short CBuffPart::getPartID(void)
{
	return PART_BUFF;
}


///////////////////__IEntityPart////////////////////////////////////
/** 创建，重新启用，也会调此接口
*/
bool CBuffPart::onLoad(__IEntity *pMaster, int nLoginMode)
{
	if(pMaster == NULL)
	{
		return false;
	}
	__IEntityEvent *pEntityEvent = pMaster->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return false;
	}
	m_pMaster = pMaster;


	// 因为创建BUFF的时候，很多部件都没有创建，但此时他们都需要添加buff，而很多部件
	// 都没有准备好，比如仇恨部件，当某个添加到实体身上，然后触发仇恨，此时仇恨部件
	// 还没有准备就绪，就很容易异常，在创建的过程添加的buff保存起来，然后等所有部件
	// 创建好了，再添加进来
	m_bBackInitBuff = true; 

	// 创建序列化过来的buff
	SERIALIZEBUFF::iterator iter = m_SerializeBuff.begin();
	for (; iter != m_SerializeBuff.end(); ++iter)
	{
		SBuffContext context;
		context.nID = iter->nSpellID;
		context.ptTargetTile = iter->targetTile;

		Add(iter->dwBuffID, iter->dwBuffLevel, iter->uidAdd, iter->dwTime, &context, sizeof(context));
	}

	// 注册实体创建事件
	pEntityEvent->Subscibe(this, EVENT_ENTITY_CREATE, "CBuffPart");
	pEntityEvent->Subscibe(this, EVENT_ACTOR_PRE_EXPORT_DATA, __FUNCTION__);

	return true;
}


/** 取是主人
*/
__IEntity * CBuffPart::getHost(void)
{
	return m_pMaster;
}


bool CBuffPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch (msgId)
	{
		// 增加Buff	
	case BUFF_MSG_ADD_BUFF:
		{
			// 服务器消息增加buff
			OnServerMessageAddBuff(data, len);
		}
		break;
	case BUFF_MSG_REMOVE_BUFF:
		{
			// 服务器消息移除buff
			OnServerMessageRemoveBuff(data, len);
		}
		break;
    case BUFF_MSG_REMOVE_INDEX:
        {
            // 客户端消息移除buff
            OnClientMessageRemoveBuff(data, len);
        }
        break;
	default:
		break;
	}
	return true;
}


/** 释放,会释放内存
*/
void CBuffPart::release(void)
{
	restore();

	if (m_pMaster != NULL)
	{
		// 退定实体创建事件
		__IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
		if (pEntityEvent != NULL)
		{
			pEntityEvent->UnSubscibe(this, EVENT_ENTITY_CREATE);
			pEntityEvent->UnSubscibe(this, EVENT_ACTOR_PRE_EXPORT_DATA);
		}

		m_pMaster  = NULL;
	}

	delete this;
}


// 切换英雄
void CBuffPart::onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore)
{
    if (isBefore)
    {
        restore();
    }
}


///////////////////IBuffPart//////////////////////////////////////
/** 给实体添加buff前，需调用此方法检测
@param   dwBuffID ：buffID
@param   dwBuffLevel ：buff等级
@param   uidAdd ：添加者
@return  
*/
bool CBuffPart::CanAdd(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
{
	m_nErrorID = EBuffAdd_Failure;

	// 此处曾经出现一个非法：当CBuffPart::Clean调用BUFF::Stop时，然后会释放效果，
	// 释放效果会触发某些事件，导致触发某个事件，又新添了BUFF进来，这样就出现了
	// for( ; it != listTempBuff.end(); ++it)此循环中无法找到新添的BUFF，然后
	// m_BuffMap.clear()清掉以后，没有管理，当部件释放以后，CBuff::m_pBuffPart
	// 数据被清空了，就非法了
	SBuffSchemeInfo * pSchemeInfo = g_pBuffServer->GetBuffSchemeInfo(dwBuffID);
	if(pSchemeInfo == NULL || m_bCleaningFlag)
	{
		return false;
	}

	if (m_pMaster == NULL)
	{
		return false;
	}

	__IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
	if(pEventEngine == NULL)
	{
		return false;
	}

	/////////////////////////////////////////////////////////////////////

	// 先发一投票事件，是否有免疫
	event_entity_add_buff addBuff;
	addBuff.dwBuffID = dwBuffID;
	addBuff.dwBuffLevel = dwBuffLevel;
	addBuff.uidEffect = m_pMaster->getUID();
	addBuff.uidAdd = uidAdd;
	addBuff.dwBuffFlag = pSchemeInfo->dwFlag;	
	addBuff.nErrorID = EBuffAdd_Success;
	
	if(!pEventEngine->FireVote(EVENT_ENTITY_ADDBUFF, (LPCSTR)&addBuff, sizeof(addBuff)))
	{
		if (addBuff.nErrorID == EBuffAdd_Success)
		{
			addBuff.nErrorID = EBuffAdd_Failure;
		}
		m_nErrorID = addBuff.nErrorID;
		
		return false;
	}	

	///////////////////////////////////////////////////////////////////
	// 判断叠加规则
	if(!g_pBuffServer->CanAdd(m_pMaster,pSchemeInfo->dwReplaceRule, dwBuffID, dwBuffLevel, uidAdd))
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////
	// 应用替换规则：如果有同一个添加者，并且新添加的级别比原级别低，则不能添加
	CBuff * pBuff = GetBuff(dwBuffID, uidAdd);
	if(pBuff != NULL && pBuff->GetBuffLevel() > dwBuffLevel)
	{
		return false;
	}

	///////////////////////////////////////////////////////////////////
	// BUFF条件 （加载时检查），非BUFF条件 （加载时检查）
	list<SBuffSchemeInfo::SLevelInfo>::iterator itLevel = pSchemeInfo->listLevelInfo.begin();
	for( ; itLevel != pSchemeInfo->listLevelInfo.end(); ++itLevel)
	{
		SBuffSchemeInfo::SLevelInfo * pLevelInfo = &(*itLevel);
		if(pLevelInfo->dwBuffLevel == dwBuffLevel)
		{
			if(pLevelInfo->dwHasCheck == 0 /*加载时检查*/ && !pLevelInfo->listHasBuff.empty())
			{
				bool bLoadFlag = false;
				list<DWORD>::iterator itB = pLevelInfo->listHasBuff.begin();
				for( ; itB != pLevelInfo->listHasBuff.end(); ++itB)
				{
					CBuff * outbuff[256]; int nOutQty = 256;
					GetBuff((*itB), outbuff, nOutQty);
					if(nOutQty > 0)
					{
						bLoadFlag = true;
					}
				}

				if(!bLoadFlag)
				{
					return false;
				}
			}

			if(pLevelInfo->dwNoCheck == 0/*加载时检查*/ && !pLevelInfo->listNoBuff.empty())
			{				
				list<DWORD>::iterator itB = pLevelInfo->listNoBuff.begin();
				for( ; itB != pLevelInfo->listNoBuff.end(); ++itB)
				{
					CBuff * outbuff[256]; int nOutQty = 256;
					GetBuff((*itB), outbuff, nOutQty);
					if(nOutQty > 0)
					{
						return false;
					}
				}
			}

			break;
		}
	}

	m_nErrorID = EBuffAdd_Success;
	return true;
}

/** 给实体添加buff
@param   dwBuffID ：buffID
@param   dwBuffLevel ：buff等级
@param   uidAdd ：添加者 
@param   dwTime ：BUFF持续时间，填0表示取配置时间
@return  
*/
bool CBuffPart::Add(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime, void *pBuffContext, int nContextLen)
{
	// 此处曾经出现一个非法：当CBuffPart::Clean调用BUFF::Stop时，然后会释放效果，
	// 释放效果会触发某些事件，导致触发某个事件，又新添了BUFF进来，这样就出现了
	// for( ; it != listTempBuff.end(); ++it)此循环中无法找到新添的BUFF，然后
	// m_BuffMap.clear()清掉以后，没有管理，当部件释放以后，CBuff::m_pBuffPart
	// 数据被清空了，就非法了
	SBuffSchemeInfo * pSchemeInfo = g_pBuffServer->GetBuffSchemeInfo(dwBuffID);
	if(pSchemeInfo == NULL || m_bCleaningFlag)
	{
		return false;
	}

	if (m_pMaster == NULL)
	{
		return false;
	}

	SBuffContext BuffContext;
	if (nContextLen > 0)
	{
		if (pBuffContext != NULL && nContextLen == sizeof(SBuffContext))
		{
			SBuffContext *pData = (SBuffContext *)pBuffContext;
			// 复制数据
			memcpy(&BuffContext, pData, sizeof(SBuffContext));
		}
		else
		{
			ErrorLn("CBuffPart::Add nContextLen="<<nContextLen<<",sizeof(SBuffContext)="<<sizeof(SBuffContext)<<",or pBuffContext == NULL");
			return false;
		}
	}

	__IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
	if(pEventEngine == NULL)
	{
		return false;
	}

	// 因为创建BUFF的时候，很多部件都没有创建，但此时他们都需要添加buff，而很多部件
	// 都没有准备好，比如仇恨部件，当某个添加到实体身上，然后触发仇恨，此时仇恨部件
	// 还没有准备就绪，就很容易异常，在创建的过程添加的buff保存起来，然后等所有部件
	// 创建好了，再添加进来
	if(m_bBackInitBuff)
	{
		SInitBuff initbuff;
		initbuff.dwBuffID = dwBuffID;
		initbuff.dwBuffLevel  = dwBuffLevel;
		initbuff.uidAdd   = uidAdd;
		initbuff.dwTime   = dwTime;
		initbuff.BuffContext = BuffContext;
		initbuff.nContextLen = nContextLen;

		m_listInitBuff.push_back(initbuff);

		return true;
	}

    if (!CanAdd(dwBuffID, dwBuffLevel, uidAdd))
	{
		return false;
	}

	UID uid = m_pMaster->getUID();

	///////////////////////////////////////////////////////////////////
	// 应用替换规则：如果有同一个添加者，并且新添加的级别大于等于旧等级，则替换
	CBuff * pOldBuff = GetBuff(dwBuffID, uidAdd);
	if(pOldBuff != NULL)
	{
		DWORD	dwOldLevel = pOldBuff->GetBuffLevel();

		if (dwOldLevel > dwBuffLevel)
		{
			return false;
		}

		// 同等级且配了标识的buff，则不remove，而是只重置时间
		if (((pOldBuff->GetFlag() & tBuff_TypeFlag_AddBuffReplaceTime) == tBuff_TypeFlag_AddBuffReplaceTime) && dwOldLevel == dwBuffLevel)
		{
			//WarningLn(__FUNCTION__":buffID=" << dwBuffID << ",buffLevel=" << dwBuffLevel << ",dwTime=" << dwTime);

			// 只替换已有buff的持续时间，其他不变
			return pOldBuff->AddBuffOnlyReplaceTime(dwBuffLevel, dwTime);
		}
		else
		{
			// 移除掉原有的
			Remove(pOldBuff, uid, false);
		}
	}

	///////////////////////////////////////////////////////////////////
	// 如果填0，表示无限叠加，否则超过此叠加数量，则随便抽取一个替换掉
	if(pSchemeInfo->nPileQty > 0)
	{
		CBuff * outbuff[256]; int nOutQty = 256;
		GetBuff(dwBuffID, outbuff, nOutQty);
		if(nOutQty >= pSchemeInfo->nPileQty)
		{
			CBuff * pOldBuff = outbuff[0];
			if(pOldBuff != NULL)
			{
				DWORD	dwOldLevel = pOldBuff->GetBuffLevel();
				bool	bRemove = true;
				if (dwOldLevel > dwBuffLevel)
				{
					bRemove = false;
				}
				if (bRemove)
				{
					// 移除掉原有的
					Remove(pOldBuff, pOldBuff->GetAdd(), false);
				}
				else
				{
					return false;
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	m_dwBuffIndex++;

	// 创建BUFF
	CBuff * pNewBuff = new CBuff();//g_pBuffServer->PopFromRecycle();

    Vector3 vLoc = BuffContext.ptTargetTile;
    if (equalto0(vLoc.getLength()))
    {
        vLoc = getLocation(uid);
    }
	// 效果现场
	EFFECT_CONTEXT context;
	context.pEntity = m_pMaster;
	context.uidOperator = uidAdd;
	context.uidTarget = uid;
	context.nID = BuffContext.nID;
	context.dwIndex = m_dwBuffIndex;
	context.dwFlag = pSchemeInfo->dwFlag;
	context.nAttackCount = BuffContext.nAttackCount;
	context.ptTargetTile = vLoc;
    context.fFocoEnergiaPCT = BuffContext.fFocoEnergiaPCT;

	if(!pNewBuff->Create(m_dwBuffIndex, this, uidAdd, dwBuffID, dwBuffLevel, dwTime, &context))
	{
		SAFE_RELEASE(pNewBuff);
		return false;
	}

	///////////////////////////////////////////////////////////////////
	// 是否有驱散BUFF
	list<SBuffSchemeInfo::SLevelInfo>::iterator itLevel = pSchemeInfo->listLevelInfo.begin();
	for( ; itLevel != pSchemeInfo->listLevelInfo.end(); ++itLevel)
	{
		SBuffSchemeInfo::SLevelInfo * pLevelInfo = &(*itLevel);
		if(pLevelInfo->dwBuffLevel == dwBuffLevel)
		{
			if(!pLevelInfo->listDispel.empty())
			{
				DWORD dwDispelRand = 0; rktRangeRand(dwDispelRand, 0, (10000/*概率比率*/ - 1));
				if(dwDispelRand <= pLevelInfo->dwDispelPer)
				{
					list<DWORD>::iterator itDis = pLevelInfo->listDispel.begin();
					for( ; itDis != pLevelInfo->listDispel.end(); ++itDis)
					{
						CBuff * outbuff[256]; int nOutQty = 256;
						GetBuff((*itDis), outbuff, nOutQty);

						// 移除掉
						for(int nQ = 0; nQ < nOutQty; nQ++)
						{
							Remove(outbuff[nQ], m_pMaster->getUID(), false);
						}
					}
				}
			}

			break;
		}
	}

	///////////////////////////////////////////////////////////////////
	// 添加到列表并且启动BUFF
    MAP_BUFF::iterator IterMap = m_BuffMap.find(dwBuffID);
    if (IterMap == m_BuffMap.end())
    {
        TLIST_BUFF listBuff;
        listBuff.push_back(pNewBuff);
        // 插入表中
        m_BuffMap.insert(MAP_BUFF::value_type(dwBuffID, listBuff));
    }
    else
    {
        TLIST_BUFF &listBuff = IterMap->second;
	    listBuff.push_back(pNewBuff);
    }
	
    // 更新FLAG
    m_dwBitFlag |= pNewBuff->GetFlag();

	// 更新快速查询mask
	int nAccQueryIndex = g_pBuffServer->getAccQueryIndex(dwBuffID, dwBuffLevel);
	m_AccQueryMask.set(nAccQueryIndex);

	bool bStartResult = pNewBuff->Start();

	/////////////////////////////////////////////////////////////////////
	// 实体增加buff
	event_entity_add_buff addBuff;
	addBuff.dwBuffID = dwBuffID;
	addBuff.dwBuffLevel = dwBuffLevel;
	addBuff.uidEffect = uid;
	addBuff.uidAdd = uidAdd;
	addBuff.dwBuffFlag = pSchemeInfo->dwFlag;
	pEventEngine->FireExecute(EVENT_ENTITY_ADDBUFF, (LPCSTR)&addBuff, sizeof(addBuff));

	return bStartResult;
}

/** 
@param   
@param   
@return  
*/
void CBuffPart::OnStartBuff(CBuff * pBuff)
{
	if(pBuff == NULL || pBuff->GetLeftTime() < 0)
	{
		return;
	}

	if (m_pMaster == NULL)
	{
		return;
	}

	DWORD dwBuffFlag = pBuff->GetFlag();
	bool bBcastFlag = false;
	bool bBcastOnlyMasterFlag = false;
	if ((dwBuffFlag&tBuff_TypeFlag_BcastOnlyMaster) == tBuff_TypeFlag_BcastOnlyMaster)
	{
		bBcastOnlyMasterFlag = true;
	}
	else if((dwBuffFlag&tBuff_TypeFlag_Bcast) == tBuff_TypeFlag_Bcast)
	{
		bBcastFlag = true;
	}
	else if((dwBuffFlag&tBuff_TypeFlag_Client) == tBuff_TypeFlag_Client)
	{
		bBcastFlag = false;
	}
	else
	{
		return;
	}

	SBuffMsgAddBuff_SC addBuff;
	addBuff.dwIndex = pBuff->GetIndex();
	addBuff.dwBuffID = pBuff->GetBuffID();
	addBuff.dwBuffLevel = (WORD)pBuff->GetBuffLevel();
	addBuff.dwTime = pBuff->GetLeftTime();
	addBuff.uidAdd = pBuff->GetAdd();
	addBuff.targetTile = pBuff->GetEffectTargetTile();
	addBuff.nSpellID = pBuff->GetSpellID();


	if (bBcastOnlyMasterFlag)
	{
		g_ExternalFacade.playerSend2OtherClient(m_pMaster->getUID(), m_pMaster->getIntProperty(PROPERTY_MASTER_UID), PART_BUFF, BUFF_MSG_ADD_BUFF, (char*)&addBuff, sizeof(addBuff));
	}
    else if (bBcastFlag)
    {
        g_ExternalFacade.broadCastInAOI(m_pMaster, m_pMaster->getLocation(), PART_BUFF, BUFF_MSG_ADD_BUFF, (char*)&addBuff, sizeof(addBuff));
    }
    else
    {
        g_ExternalFacade.playerSend2Client(m_pMaster, PART_BUFF, BUFF_MSG_ADD_BUFF, (char*)&addBuff, sizeof(addBuff));
    }
}

/** 给实体移除buff
@param   dwBuffID ：buffID
@param   uidAdd ：添加者，不能把别人添加的移除掉;为INVALID_UID时，移除所有dwBuffID
@param   uidRemove ：移除者
@return  
*/
bool CBuffPart::Remove(DWORD dwBuffID, UID uidAdd, UID uidRemove)
{
    if (m_pMaster == NULL)
    {
        return false;
    }

	// 因为创建BUFF的时候，很多部件都没有创建，但此时他们都需要添加buff，而很多部件
	// 都没有准备好，比如仇恨部件，当某个添加到实体身上，然后触发仇恨，此时仇恨部件
	// 还没有准备就绪，就很容易异常，在创建的过程添加的buff保存起来，然后等所有部件
	// 创建好了，再添加进来
	if(m_bBackInitBuff)
	{
		if (uidAdd == INVALID_UID)
		{
			TLIST_INITBUFF::iterator it = m_listInitBuff.begin();
			for( ; it != m_listInitBuff.end(); )
			{
				if((*it).dwBuffID == dwBuffID)
				{
					it = m_listInitBuff.erase(it); 
				}
				else
				{
					++it;
				}
			}
		}
		else
		{
			TLIST_INITBUFF::iterator it = m_listInitBuff.begin();
			for( ; it != m_listInitBuff.end(); ++it)
			{
				if((*it).dwBuffID == dwBuffID && (*it).uidAdd == uidAdd)
				{
					m_listInitBuff.erase(it); 
					break;
				}
			}
		}

		return true;
	}

	if (uidAdd == INVALID_UID)
	{
        MAP_BUFF::iterator IterMap = m_BuffMap.find(dwBuffID);
        if (IterMap == m_BuffMap.end())
        {
            return true;
        }
        TLIST_BUFF &listBuff = IterMap->second;
		for(TLIST_BUFF::iterator itL = listBuff.begin(); itL != listBuff.end(); ++itL)
		{
			CBuff * pBuff = (*itL);
			if(pBuff == NULL || !pBuff->IsValid())
			{
				continue;
			}

			Remove(pBuff, uidRemove, false);
		}
	}
	else
	{
		CBuff * pBuff = GetBuff(dwBuffID, uidAdd);
		if(pBuff != NULL)
		{
			Remove(pBuff, uidRemove, false);
		}
	}

	return true;
}

/** 给实体移除同类型的buff
@param  dwBuffFlag ：buff类型 
@param  bAllFlag ：是否移除所有的同类型的
@param  uidRemove ：移除者
@return  
*/
bool CBuffPart::Remove(DWORD dwBuffType, bool bAllFlag, UID uidRemove)
{
    if (m_pMaster == NULL)
    {
        return false;
    }

	// 因为创建BUFF的时候，很多部件都没有创建，但此时他们都需要添加buff，而很多部件
	// 都没有准备好，比如仇恨部件，当某个添加到实体身上，然后触发仇恨，此时仇恨部件
	// 还没有准备就绪，就很容易异常，在创建的过程添加的buff保存起来，然后等所有部件
	// 创建好了，再添加进来
	if(m_bBackInitBuff)
	{			
		for(TLIST_INITBUFF::iterator it = m_listInitBuff.begin(); it != m_listInitBuff.end(); )
		{
			SBuffSchemeInfo * pSchemeInfo = g_pBuffServer->GetBuffSchemeInfo((*it).dwBuffID);
			if(pSchemeInfo == NULL)
			{
				++it; 
				
				continue;
			}

			if((pSchemeInfo->dwFlag & dwBuffType) != 0)
			{
				it = m_listInitBuff.erase(it);

				if(bAllFlag == false)
				{
					return true;
				}
			}
			else
			{
				++it;
			}			
		}

		return true;		
	}

	bool bRemoveFlag = true;
    for(MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
        for(TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); ++it)
        {
            CBuff * pBuff = (*it);
            if(pBuff == NULL || !pBuff->IsValid())
            {
                continue;
            }

            if((pBuff->GetFlag() & dwBuffType) != 0 && bRemoveFlag)
            {
                Remove(pBuff, uidRemove, false);

                if(bAllFlag == false) bRemoveFlag = false;
            }
        }
    }

	return true;
}

/** 延迟移除buff
@param   dwBuffIndex  Buff索引
@param   dwTime		  延迟删除时间
*/
void CBuffPart::DelayRemove(DWORD dwBuffIndex, DWORD dwTime)
{
	if (m_pMaster == NULL || dwBuffIndex == 0)
	{
		return;
	}

	DWORD dwDelayTime = getTickCountEx() + dwTime;
	// 延迟删除BUFF表插入，插失败无需处理
	m_DelayRemoveBuffMap.insert(DELAY_REMOVE_BUFF_MAP::value_type(dwBuffIndex, dwDelayTime));

	if (m_DelayRemoveBuffMap.size() > 0 && !m_bDelayRemoveBuffTimer)
	{
		g_ExternalFacade.SetTimer(ETimerEventID_DelayRemoveBuffTimer, 1000, this, "CBuffPart::DelayRemove");

		// 启用延迟删除buff定时器
		m_bDelayRemoveBuffTimer = true;
	}
}

/** 通过索引移除buff
@param   dwBuffIndex Buff索引
@return  
*/
bool CBuffPart::Remove(DWORD dwBuffIndex)
{
	if (m_pMaster == NULL)
	{
		return false;
	}

    for(MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
	    for(TLIST_BUFF::iterator itL = listBuff.begin(); itL != listBuff.end(); ++itL)
	    {
		    if((*itL) == NULL || !(*itL)->IsValid() || (*itL)->GetIndex() != dwBuffIndex)
		    {
			    continue;
		    }

		    Remove((*itL), m_pMaster->getUID(), false);

		    return true;
	    }
    }

	return true;
}

 bool CBuffPart:: RemoveBuffByIndex(DWORD dwIndex)
{
	return this->Remove(dwIndex);
}
/** 移除某个BUFF
@param   
@param   
@return  
*/
bool CBuffPart::Remove(CBuff *pInBuff, UID uidRemove, bool bCreateEndArose)
{
	// 此处曾经出现一个非法：当CBuffPart::Clean调用BUFF::Stop时，然后会释放效果，
	// 释放效果会触发某些事件，导到其他模块又来释放此BUFF，这样就出现重复释放！	
	if(pInBuff == NULL || !pInBuff->IsValid())
	{
		return false;
	}

	if (m_pMaster == NULL)
	{
		return false;
	}

	__IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
	if(pEventEngine == NULL)
	{
		return false;
	}

	UID uid = m_pMaster->getUID();

	event_entity_remove_buff eventRemoveBuff;
	eventRemoveBuff.dwBuffID = pInBuff->GetBuffID();
	eventRemoveBuff.dwBuffLevel = pInBuff->GetBuffLevel();
	eventRemoveBuff.uidEffect = uid;
	eventRemoveBuff.uidAdd = pInBuff->GetAdd();
	eventRemoveBuff.dwBuffFlag = pInBuff->GetFlag();
	eventRemoveBuff.uidRemove = uidRemove;
	if(!pEventEngine->FireVote(EVENT_ENTITY_REMOVEBUFF, (LPCSTR)&eventRemoveBuff, sizeof(eventRemoveBuff)))
	{
		return false;
	}

	// 如果没有创建效果，则不用通知客户端释放，因为也没有通知客户端创建
	if(pInBuff->GetBuildEffectFlag())
	{
        DWORD dwFlag = pInBuff->GetFlag();
        bool bBcastFlag = false;
        bool bBcastOnlyMasterFlag = false;
		if ((dwFlag & tBuff_TypeFlag_BcastOnlyMaster) == tBuff_TypeFlag_BcastOnlyMaster)
		{
			bBcastOnlyMasterFlag = true;
		}
        else if((dwFlag & tBuff_TypeFlag_Bcast) == tBuff_TypeFlag_Bcast)
        {
            bBcastFlag = true;
        }
        else if((dwFlag & tBuff_TypeFlag_Client) == tBuff_TypeFlag_Client)
        {
            bBcastFlag = false;
        }

        SBuffMsgRemoveBuff_SC removeBuff;
        removeBuff.dwIndex = pInBuff->GetIndex();
		if (bBcastOnlyMasterFlag)
		{
			g_ExternalFacade.playerSend2OtherClient(uid,m_pMaster->getIntProperty(PROPERTY_MASTER_UID), PART_BUFF, BUFF_MSG_REMOVE_BUFF, (char*)&removeBuff, sizeof(removeBuff));
		}
        else if (bBcastFlag)
        {
            g_ExternalFacade.broadCastInAOI(m_pMaster, m_pMaster->getLocation(), PART_BUFF, BUFF_MSG_REMOVE_BUFF, (char*)&removeBuff, sizeof(removeBuff));
        }
        else
        {
            g_ExternalFacade.playerSend2Client(m_pMaster, PART_BUFF, BUFF_MSG_REMOVE_BUFF, (char*)&removeBuff, sizeof(removeBuff));
        }
	}

	// 发移除buff执行事件
	pEventEngine->FireExecute(EVENT_ENTITY_REMOVEBUFF, (LPCSTR)&eventRemoveBuff, sizeof(eventRemoveBuff));

	// 清掉这个buff的mask
	int nAccQueryIndex = g_pBuffServer->getAccQueryIndex(eventRemoveBuff.dwBuffID, eventRemoveBuff.dwBuffLevel);
	m_AccQueryMask.clear(nAccQueryIndex);

	// 此处曾经出现一个非法：当CBuffPart::Clean调用BUFF::Stop时，然后会释放效果，
	// 释放效果会触发某些事件，导到其他模块又来释放此BUFF，这样就出现重复释放！
	if(pInBuff->IsValid())
	{		
		pInBuff->Restore(bCreateEndArose);
	}

    // 重算Flag
    recountFlag();

	// 是否启动了清除无效BUFF定时器
	if(!m_bSetClanInvalidTimer)
	{
		g_ExternalFacade.SetTimer(ETimerEventID_CleanInvalidBuff, 1000, this, "CBuffPart::Remove");

		m_bSetClanInvalidTimer = true;
	}

	return true;
}

// 重算Flag
void CBuffPart::recountFlag()
{
    DWORD  dwFlag = 0;

    for (MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
        for (TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); ++it)
        {
            CBuff * pBuff = (*it);
            if (pBuff == NULL || !pBuff->IsValid())
            {
                continue;
            }

            dwFlag |= pBuff->GetFlag();
        }
    }
    // 更新FLAG
    m_dwBitFlag = dwFlag;
}

/** 通过索引获取buff
@param   dwBuffIndex Buff索引
@return  
*/
CBuff *CBuffPart::getBuff(DWORD dwBuffIndex)
{
    if (m_pMaster == NULL)
    {
        return NULL;
    }

    for(MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
        for(TLIST_BUFF::iterator itL = listBuff.begin(); itL != listBuff.end(); ++itL)
        {
            CBuff *pBuff = *itL;
            if(pBuff == NULL || !pBuff->IsValid() || pBuff->GetIndex() != dwBuffIndex)
            {
                continue;
            }

            return pBuff;
        }
    }

    return NULL;
}

/** 某个BUFF是否存在
@param   
@param   uidAdd ：如果不管添加者，则填INVALID_UID
@return  
*/
bool CBuffPart::IsExist(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
{
    if (m_pMaster == NULL)
    {
        return false;
    }

	if(uidAdd == INVALID_UID)
	{
		// 配了查询加速 快速查询
		int nAccQueryIndex = -1;
		if ((nAccQueryIndex = g_pBuffServer->getAccQueryIndex(dwBuffID, dwBuffLevel)) >= 0)
		{
			return m_AccQueryMask.test(nAccQueryIndex);
		}

		// 为0的时候表示所有等级
		if (dwBuffLevel == 0)
		{
            MAP_BUFF::iterator IterMap = m_BuffMap.find(dwBuffID);
            if (IterMap == m_BuffMap.end())
            {
                return false;
            }
            TLIST_BUFF &listBuff = IterMap->second;
            return (listBuff.size() > 0);
		}
		else
		{
            MAP_BUFF::iterator IterMap = m_BuffMap.find(dwBuffID);
            if (IterMap == m_BuffMap.end())
            {
                return false;
            }

            TLIST_BUFF &listBuff = IterMap->second;
			for(TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); ++it)
			{
				CBuff * pBuff = (*it);
				if(pBuff == NULL || !pBuff->IsValid())
				{
					continue;
				}

				if(pBuff->GetBuffLevel() == dwBuffLevel)
				{
					return true;
				}
			}
		}
	}
	else
	{
		// 为0的时候表示所有等级
		if (dwBuffLevel == 0)
		{
			if(GetBuff(dwBuffID, uidAdd) != NULL)
			{
				return true;			
			}
		}
		else
		{
            MAP_BUFF::iterator IterMap = m_BuffMap.find(dwBuffID);
            if (IterMap == m_BuffMap.end())
            {
                return false;
            }

            TLIST_BUFF &listBuff = IterMap->second;
			for(TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); ++it)
			{
				CBuff * pBuff = (*it);
				if(pBuff == NULL || !pBuff->IsValid())
				{
					continue;
				}

				if(pBuff->GetBuffLevel() == dwBuffLevel && pBuff->GetAdd() == uidAdd)
				{
					return true;
				}
			}
		}
	}

	return false;
}

/** 某种BUFF是否存在
@param   
@param   
@return  
*/
bool CBuffPart::IsExist(DWORD dwBuffType)
{
    if (m_pMaster == NULL)
    {
        return false;
    }

    return ((m_dwBitFlag & dwBuffType) != 0);
}

/** 清除所有BUFF
@param   
@param   
@return  
*/
bool CBuffPart::Clean(void)
{
    if (m_pMaster == NULL)
    {
        return false;
    }

	// 此处曾经出现一个非法：当CBuffPart::Clean调用BUFF::Stop时，然后会释放效果，
	// 释放效果会触发某些事件，导致触发某个事件，又新添了BUFF进来，这样就出现了
	// for( ; it != listTempBuff.end(); ++it)此循环中无法找到新添的BUFF，然后
	// listBuff.clear()清掉以后，没有管理，当部件释放以后，CBuff::m_pBuffPart
	// 数据被清空了，就非法了
	m_bCleaningFlag = true;

    UID uid = m_pMaster->getUID();
    for(MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
        for(TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); ++it)
        {
            CBuff * pBuff = (*it);
            if(pBuff == NULL)
            {
                continue;
            }

			// 有该标记位则保存需要打包的buff
			if (m_bDeSerializeBuff && (pBuff->GetFlag() & tBuff_TypeFlag_Save))
			{
				SBuffMsgAddBuff_SC addBuff;
				addBuff.dwIndex = pBuff->GetIndex();
				addBuff.dwBuffID = pBuff->GetBuffID();
				addBuff.dwBuffLevel = (WORD)pBuff->GetBuffLevel();
				addBuff.dwTime = pBuff->GetLeftTime();
				addBuff.uidAdd = pBuff->GetAdd();
				addBuff.targetTile = pBuff->GetEffectTargetTile();

				m_deSerializeBuff.push_back(addBuff);
			}

            // 需要告诉客户端移除
            Remove(pBuff, uid, false);

            SAFERELEASE(pBuff);
        }
    }

	m_BuffMap.clear();

	m_bCleaningFlag = false;

	return true;
}

/** 右键取消某个buff
@param   
@param   
@return  
*/
bool CBuffPart::RightClickRemoveBuff(DWORD dwBuffIndex)
{
	if (m_pMaster == NULL)
	{
		return false;
	}

    for(MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
        for(TLIST_BUFF::iterator itL = listBuff.begin(); itL != listBuff.end(); ++itL)
        {
            if((*itL) == NULL || !(*itL)->IsValid() || (*itL)->GetIndex() != dwBuffIndex)
            {
                continue;
            }

            Remove((*itL), m_pMaster->getUID(), false);

            return true;
        }
    }

	return true;
}

/**	
@param   
@param   
@return  
*/
CBuff *	CBuffPart::GetBuff(DWORD dwBuffID, UID uidAdd)
{
    if (m_pMaster == NULL)
    {
        return NULL;
    }

    MAP_BUFF::iterator IterMap = m_BuffMap.find(dwBuffID);
    if (IterMap == m_BuffMap.end())
    {
        return NULL;
    }

    TLIST_BUFF &listBuff = IterMap->second;
	for(TLIST_BUFF::iterator itL = listBuff.begin(); itL != listBuff.end(); ++itL)
	{
		CBuff * pBuff = (*itL);
		if(pBuff == NULL || !pBuff->IsValid())
		{
			continue;
		}

		if(pBuff->GetAdd() == uidAdd)
		{
			return pBuff;
		}
	}

	return NULL;
}

/** 
@param   pOutBuff ：输出buff 
@param   nQty ：数量
@return  
*/
void CBuffPart::GetBuff(DWORD dwBuffID, CBuff ** ppOutBuff, int &nQty)
{
    if (m_pMaster == NULL)
    {
        return;
    }

	int nInQty = nQty; nQty = 0;	
    MAP_BUFF::iterator IterMap = m_BuffMap.find(dwBuffID);
    if (IterMap == m_BuffMap.end())
    {
        return;
    }
    TLIST_BUFF &listBuff = IterMap->second;
	for(TLIST_BUFF::iterator itL = listBuff.begin(); itL != listBuff.end(); ++itL)
	{
		CBuff * pBuff = (*itL);
		if(pBuff == NULL || !pBuff->IsValid())
		{
			continue;
		}

        if(nQty >= nInQty)
        {
            return;
        }

        ppOutBuff[nQty] = pBuff; nQty++;
	}
}

/** 创建初始化的BUFF，总是在创建部件时，创建BUFF，但此时很多部件没有创建，很容易非法
@param   
@param   
@return  
*/
bool CBuffPart::BuildInitBuff()
{
	if(m_pMaster == NULL)
	{
		return true;
	}

	// 因为创建BUFF的时候，很多部件都没有创建，但此时他们都需要添加buff，而很多部件
	// 都没有准备好，比如仇恨部件，当某个添加到实体身上，然后触发仇恨，此时仇恨部件
	// 还没有准备就绪，就很容易异常，在创建的过程添加的buff保存起来，然后等所有部件
	// 创建好了，再添加进来
	m_bBackInitBuff = false; //一定要先将此标志设为false,否则Add又加进去了,就循环了
	TLIST_INITBUFF::iterator it = m_listInitBuff.begin();
	for( ; it != m_listInitBuff.end(); ++it)
	{
		SInitBuff *pInitBuff = &(*it);
		Add(pInitBuff->dwBuffID, pInitBuff->dwBuffLevel, pInitBuff->uidAdd, pInitBuff->dwTime, (void *)&pInitBuff->BuffContext, pInitBuff->nContextLen);
	}
	m_listInitBuff.clear();

	return true;
}

/** 移除对方添加的某种类型的Buff
@param   dwBuffType ：Buff的类型，填0表示移除所有类型
@param   uidAdd		：添加者，填0表示移除所有生物添加的
@param   uidRemove	：移除者
@return  
*/
void CBuffPart::CleanBuff(DWORD dwBuffType, UID uidAdd, UID uidRemove)
{
    if (m_pMaster == NULL)
    {
        return;
    }

	// 因为创建BUFF的时候，很多部件都没有创建，但此时他们都需要添加buff，而很多部件
	// 都没有准备好，比如仇恨部件，当某个添加到实体身上，然后触发仇恨，此时仇恨部件
	// 还没有准备就绪，就很容易异常，在创建的过程添加的buff保存起来，然后等所有部件
	// 创建好了，再添加进来
	if(m_bBackInitBuff)
	{			
		for(TLIST_INITBUFF::iterator it = m_listInitBuff.begin(); it != m_listInitBuff.end(); )
		{
			SBuffSchemeInfo * pSchemeInfo = g_pBuffServer->GetBuffSchemeInfo((*it).dwBuffID);
			if(pSchemeInfo == NULL)
			{
				++it; 
				continue;
			}

			if ((dwBuffType == 0 || (pSchemeInfo->dwFlag & dwBuffType) != 0) && (uidAdd == 0 || (*it).uidAdd == uidAdd))
			{
				it = m_listInitBuff.erase(it);
			}
			else
			{
				++it;
			}			
		}

		return;		
	}
	
    for (MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
        for (TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); ++it)
        {
            CBuff * pBuff = (*it);
            if(pBuff == NULL || !pBuff->IsValid())
            {
                continue;
            }

            if ((dwBuffType == 0 || (pBuff->GetFlag() & dwBuffType) != 0) && (uidAdd == 0 || pBuff->GetAdd() == uidAdd))
            {
                Remove(pBuff, uidRemove, false);
            }
        }
    }

	return;
}

/** 取玩家某个BUFF的等级(如果有多个相同ID的BUFF取等级最高的)
@param   dwBuffID：buff的ID
@param   uidAdd ：如果不管添加者，则填INVALID_UID   
@return  
*/
DWORD CBuffPart::GetBuffLevel(DWORD dwBuffID, UID uidAdd) 
{
    if (m_pMaster == NULL)
    {
        return 0;
    }

	DWORD dwBuffLevel = 0;
	if(uidAdd == INVALID_UID)
	{
		CBuff * outbuff[256]; int nOutQty = 256;
		GetBuff(dwBuffID, outbuff, nOutQty);
		DWORD dwTempLevel = 0;
		for (int i=0; i<nOutQty && i<256; i++)
		{
			if (outbuff[i]!=NULL)
			{
				dwTempLevel = outbuff[i]->GetBuffLevel();
				if (dwTempLevel>dwBuffLevel)
				{
					dwBuffLevel = dwTempLevel;
				}
			}
		}
	}
	else
	{
		CBuff* pBuff = GetBuff(dwBuffID, uidAdd);
		if(pBuff != NULL)
		{
			dwBuffLevel = pBuff->GetBuffLevel();			
		}
	}

	return dwBuffLevel;
}

/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CBuffPart::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case ETimerEventID_CleanInvalidBuff:
		{
			g_ExternalFacade.KillTimer(ETimerEventID_CleanInvalidBuff, this);

			// 是否启动了清除无效BUFF定时器
			m_bSetClanInvalidTimer = false;

			// 将无效的BUFF删除
            for (MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
            {
                TLIST_BUFF &listBuff = IterMap->second;
			    for(TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); )
			    {
				    CBuff * pBuff = (*it);
				    if(pBuff == NULL)
				    {
					    it = listBuff.erase(it);
					    continue;
				    }

				    if(!pBuff->IsValid())
				    {
					    SAFERELEASE(pBuff);

					    it = listBuff.erase(it);
				    }
				    else
				    {
					    ++it;
				    }		
			    }
            }
			break;
		}
	case ETimerEventID_DelayRemoveBuffTimer:
		{
			// 取得当前时间
			DWORD dwTick = getTickCountEx();
			DELAY_REMOVE_BUFF_MAP::iterator Iter = m_DelayRemoveBuffMap.begin();
			while (Iter != m_DelayRemoveBuffMap.end())
			{
				if (dwTick >= Iter->second)
				{
					// 移除buff
					Remove(Iter->first);
					Iter = m_DelayRemoveBuffMap.erase(Iter);
				}
				else
				{
					++Iter;
				}
			}

			if (m_DelayRemoveBuffMap.empty())
			{
				g_ExternalFacade.KillTimer(ETimerEventID_DelayRemoveBuffTimer, this);
				// 启用延迟删除buff定时器
				m_bDelayRemoveBuffTimer = false;
			}
		}
		break;
	default:
		break;
	}
}


/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
void CBuffPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_CREATE:
		{
			// 创建初始化的BUFF
			BuildInitBuff();
		}
		break;
	case EVENT_ACTOR_PRE_EXPORT_DATA:
		{
			if (nLen != sizeof(event_actor_pre_export_data))
				break;

			event_actor_pre_export_data* pContext = (event_actor_pre_export_data*)pszContext;
			if (pContext->nFlag & TO_OTHER_SEVER)
			{
				m_bDeSerializeBuff = true;
				restore();
			}
		}
		break;
	default:
		break;
	}
}

/** 取不能添加BUFF的原因（CanAdd后，采用这个函数可以取到具体原因）
@param   
@param   
@return  
*/
int CBuffPart::GetErrorID()
{
	return m_nErrorID;
}

/** 剩余时间
@param   dwBuffID：buff的ID
@param   uidAdd ：如果不管添加者，则填INVALID_UID  
@return －1：表示时间已越过，不用通知客户端创建。0：表示永久时间。正数表示剩余时间 
*/
int CBuffPart::GetLeftTime(DWORD dwBuffID, UID uidAdd)
{
    if (m_pMaster == NULL)
    {
        return -1;
    }

	int nBuffLeftTime = -1;
	if(uidAdd == INVALID_UID)
	{
		DWORD dwBuffLevel = 0;
		CBuff * outbuff[256]; int nOutQty = 256;
		GetBuff(dwBuffID, outbuff, nOutQty);
		DWORD dwTempLevel = 0;
		for (int i=0; i<nOutQty && i<256; i++)
		{
			if (outbuff[i]!=NULL)
			{
				dwTempLevel = outbuff[i]->GetBuffLevel();
				if (dwTempLevel>dwBuffLevel)
				{
					dwBuffLevel = dwTempLevel;
					nBuffLeftTime = outbuff[i]->GetLeftTime();
				}
			}
		}
	}
	else
	{
		CBuff* pBuff = GetBuff(dwBuffID, uidAdd);
		if(pBuff != NULL)
		{
			nBuffLeftTime = pBuff->GetLeftTime();			
		}
	}

	return nBuffLeftTime;
}

/** 某个effectID是否存在
@param   dwEffectID:效果ID
@param  lIgnoreUid:忽略的添加者 
@return  
*/
bool CBuffPart::IsEffectIDExist(DWORD dwEffectID, UID lIgnoreUid)
{
    if (m_pMaster == NULL)
    {
        return false;
    }

    for (MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
        for(TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); ++it)
        {
            if((*it)->IsEffectIDExist(dwEffectID, lIgnoreUid))
            {
                return true;
            }		
        }
    }

	return false;
}

/** 设置累积BUFF
@param  dwBuffID:buffID
@param  uidAdd ：如果不管添加者，则填INVALID_UID
@param  bOverlay:true递增BUFF等级，直到buff等级上限，false递减buff等级，直到0
*/
void CBuffPart::SetAccumulateBuff(DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime, void *pBuffContext, int nContextLen)
{
    if (m_pMaster == NULL)
    {
        return;
    }

	// 取得当前身上buff等级
	DWORD dwBuffLevel = GetBuffLevel(dwBuffID, uidAdd);

	// 判断是否需要移除
	CBuff * pOldBuff = GetBuff(dwBuffID, uidAdd);
	bool bRemove = false;
	if (pOldBuff != NULL)
	{
		bRemove = (pOldBuff->GetFlag() & tBuff_TypeFlag_AddBuffReplaceTime) != tBuff_TypeFlag_AddBuffReplaceTime;
	}
	if (dwBuffLevel > 0 && bRemove)
	{
		// 先移除旧BUFF
		Remove(dwBuffID, uidAdd, uidAdd);
	}

	if (bOverlay)
	{
		DWORD dwAddBuffLevel = 0;
		do 
		{
			SBuffSchemeInfo *pSchemeInfo = g_pBuffServer->GetBuffSchemeInfo(dwBuffID);
			if(pSchemeInfo == NULL)
			{
				break;
			}

			list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchemeInfo->listLevelInfo.begin();
			for( ; it != pSchemeInfo->listLevelInfo.end(); ++it)
			{
				if((*it).dwBuffLevel == dwBuffLevel + 1)
				{
					dwAddBuffLevel = (*it).dwBuffLevel;
					break;
				}		
			}
			// 没有后续BUFF等级，就加当前BUFF等级
			if (dwAddBuffLevel <= 0)
			{
				dwAddBuffLevel = dwBuffLevel;
			}
		} while (false);
		
		if (dwAddBuffLevel > 0)
		{
			// 增加BUFF
			Add(dwBuffID, dwAddBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen);
		}
	}
	else
	{
		DWORD dwAddBuffLevel = 0;
		do 
		{
			if (dwBuffLevel == 0)
			{
				break;
			}

			SBuffSchemeInfo *pSchemeInfo = g_pBuffServer->GetBuffSchemeInfo(dwBuffID);
			if(pSchemeInfo == NULL)
			{
				break;
			}

			SBuffSchemeInfo::SLevelInfo * pLevelInfo = NULL;
			list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchemeInfo->listLevelInfo.begin();
			for( ; it != pSchemeInfo->listLevelInfo.end(); ++it)
			{
				if((*it).dwBuffLevel == dwBuffLevel - 1)
				{
					dwAddBuffLevel = (*it).dwBuffLevel;
					break;
				}		
			}
		} while (false);

		if (dwAddBuffLevel > 0)
		{
			// 增加BUFF
			Add(dwBuffID, dwAddBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen);
		}
	}
}


/** 取得类型buff列表
*/
void CBuffPart::GetBuff(DWORD dwBuffType, DWORD *pBuffID, DWORD *pBuffLevel, int *pBuffTime, int &nOutCount)
{
    if (pBuffID == NULL || pBuffLevel == NULL || pBuffTime == NULL || m_pMaster == NULL)
    {
        return;
    }

    int nCount = nOutCount;
    nOutCount = 0;
    for (MAP_BUFF::iterator IterMap = m_BuffMap.begin(); IterMap != m_BuffMap.end(); ++IterMap)
    {
        TLIST_BUFF &listBuff = IterMap->second;
        for (TLIST_BUFF::iterator it = listBuff.begin(); it != listBuff.end(); ++it)
        {
            CBuff * pBuff = (*it);
            if(pBuff == NULL || !pBuff->IsValid())
            {
                continue;
            }

            // 超过上限了
            if (nOutCount >= nCount)
            {
                return;
            }

            if ((dwBuffType == 0 || (pBuff->GetFlag() & dwBuffType) != 0))
            {
                pBuffID[nOutCount] = pBuff->GetBuffID();
                pBuffLevel[nOutCount] = pBuff->GetBuffLevel();
                pBuffTime[nOutCount] = pBuff->GetLeftTime();
                ++nOutCount;
            }
        }
    }
}


// 服务器消息增加buff
void CBuffPart::OnServerMessageAddBuff(void *pData, size_t nLen)
{
    if (m_pMaster == NULL)
    {
        return;
    }

	// 判断参数
	if (pData == NULL || nLen != sizeof(SBuffMsgAddBuff_SS))
	{
		return;
	}

	SBuffMsgAddBuff_SS *pAddBuff = (SBuffMsgAddBuff_SS *)pData;

	// 给实体添加buff
	Add(pAddBuff->dwBuffID, pAddBuff->dwBuffLevel, pAddBuff->uidAddBuff);
}


// 服务器消息移除buff
void CBuffPart::OnServerMessageRemoveBuff(void *pData, size_t nLen)
{
    if (m_pMaster == NULL)
    {
        return;
    }

	// 判断参数
	if (pData == NULL || nLen != sizeof(SBuffMsgRemoveBuff_SS))
	{
		return;
	}

	SBuffMsgRemoveBuff_SS *pRemoveBuff = (SBuffMsgRemoveBuff_SS *)pData;

	// 给实体移除buff
	Remove(pRemoveBuff->dwIndex);
}

// 客户端消息移除buff
void CBuffPart::OnClientMessageRemoveBuff(void *pData, size_t nLen)
{
    if (m_pMaster == NULL)
    {
        return;
    }

    size_t nRemoveIndexLen = sizeof(SBuffMsgRemoveIndex_CS);
    // 判断参数
    if (pData == NULL || nLen < nRemoveIndexLen)
    {
        return;
    }
    SBuffMsgRemoveIndex_CS *pRemoveBuff = (SBuffMsgRemoveIndex_CS *)pData;

    // 判断数据长度
    if (nLen != nRemoveIndexLen + pRemoveBuff->nCount*sizeof(DWORD))
    {
        return;
    }

    if (m_pMaster == NULL)
    {
        return;
    }

    __IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
    if(pEventEngine == NULL)
    {
        return;
    }

    // 实体增加buff
    event_entity_client_remove_buff removeBuff;
    removeBuff.uidOperator = m_pMaster->getUID();
    pEventEngine->FireExecute(EVENT_ENTITY_CLIENT_REMOVE_BUFF, (LPCSTR)&removeBuff, sizeof(removeBuff));
}

/** 还原
*/
void CBuffPart::restore()
{
    Clean();

    // BUFF index
    m_dwBuffIndex = 0;

    // 是否需要备份buff
    m_bBackInitBuff = false;

    // 在创建过程中加的buff
    m_listInitBuff.clear();

    // 是否正在清除BUFF过程中
    m_bCleaningFlag = false;

    // 是否启动了清除无效BUFF定时器
    m_bSetClanInvalidTimer = false;
    // 杀掉清除无效BUFF
    g_ExternalFacade.KillTimer(ETimerEventID_CleanInvalidBuff, this);

    // 启用延迟删除buff定时器
    m_bDelayRemoveBuffTimer = false;
    // 杀掉延迟删除buff定时器
    g_ExternalFacade.KillTimer(ETimerEventID_DelayRemoveBuffTimer, this);
    // 清除延迟删除BUFF表
    m_DelayRemoveBuffMap.clear();

    // 不能添加BUFF的原因ID
    m_nErrorID = EBuffAdd_Failure;

	// 是否序列化Buff
	m_bDeSerializeBuff = false;
}
