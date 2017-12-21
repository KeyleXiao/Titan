/*******************************************************************
** 文件名:	BuffPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/16  11:01
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
#include "BuffClient.h"
#include "Buff.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "SpellDef.h"
#include "EffectDef.h"
#include "Stage_ManagedDef.h"
#include "Entity_inl.h"
#include "ISpell.h"
using namespace Stage_ManagerDef;
using namespace EFFECT_CLIENT;

#define  BUFF_DO_INNER_EFFECT( typeName,obj,context)  gClientGlobal->getEffectFactory()->DoInnerEffect( #typeName,&obj,sizeof(obj),context)

/////////////////////////////////////////ISerializable/////////////////////////////////////////
bool CBuffPart::onSerialize(UID uid, rkt::ibuffer &in, int flag)
{
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


bool CBuffPart::deSerialize(rkt::obuf &out, int flag)
{
	return true;
}


/////////////////////IEntityPart////////////////////////////////////
short CBuffPart::getPartID()
{
	return PART_BUFF;
}


bool CBuffPart::onLoad(IEntity* pEntity)
{
	if (pEntity == NULL)
	{
		return false;
	}
	m_pMaster = pEntity;
	
	// 注册事件
	subscibeEvents();
	
	return true;
}


// 消息通讯
bool CBuffPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch (msgId)
	{
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
	case BUFF_MSG_CHARGE_COUNT_CHANGE:
		{ 
			//服务器消息改变buff充能层数
			OnServerMessageChangeChargeCount(data, len);
		}
	default:
		break;
	}
	return true;
}


// 显示层的命令(true表示此部件处理,其他不处理,false交由其他部件继续处理)
bool CBuffPart::onCommand(int nCmdID, void *pData, size_t nLen)
{
	return false;
}


IEntity *CBuffPart::getHost()
{
	return m_pMaster;
}


void CBuffPart::release()
{
	TLIST_BUFF::iterator it = m_listBuff.begin();
	for( ; it != m_listBuff.end(); )
	{
		IBuff *pBuff = *it;
		if (pBuff == NULL)
		{
			++it;
			continue;
		}

        DWORD dwFlashID = pBuff->GetFlashID();
        if(dwFlashID > 0)
        {
            // 停止预施法光效
            EffectClient_StopLighting StopLighting;
            ssprintf_s(StopLighting.strName, sizeof(StopLighting.strName), "%d", dwFlashID);
            BUFF_DO_INNER_EFFECT(EffectClient_StopLighting, StopLighting, pBuff->GetEffectContext());
        }

		SAFE_RELEASE(pBuff);

		// 删除节点
		it = m_listBuff.erase(it);
	}
	m_listBuff.clear();

	// 注销事件
	unSubscibeEvents();

	m_pMaster = NULL;

	delete this;
}


/////////////////////////////////////////IBuffPart/////////////////////////////////////////
/** 通过buff index取得buff
@param   
@param   
@return  
*/
IBuff *CBuffPart::GetBuff(DWORD dwBuffIndex)
{
	TLIST_BUFF::iterator it = m_listBuff.begin();
	for( ; it != m_listBuff.end(); ++it)
	{
		if((*it)->GetIndex() == dwBuffIndex)
		{
			return (IBuff *)(*it);
		}		
	}

	return NULL;
}


/** 某个BUFF是否存在
@param   
@param  
@return  
*/
bool CBuffPart::IsExist(DWORD dwBuffID)
{
	bool bAccQuery = false;
	SBuffSchemeInfo *pSchemeInfo = g_pBuffClient->GetBuffSchemeInfo(dwBuffID);
	if (pSchemeInfo != NULL)
	{
		for (list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchemeInfo->listLevelInfo.begin(); it != pSchemeInfo->listLevelInfo.end(); ++it)
		{
			if (it->nAccQueryIndex >= 0)
			{
				bAccQuery = true;
				if (m_AccQueryMask.test(it->nAccQueryIndex))
				{
					return true;
				}
			}
			else
			{
				EmphasisLn("query buff =" << dwBuffID << ", was not thread safe, please config tBuff_TypeFlag_QueryAccelerate flag");
			}
		}
	}

	if (bAccQuery)
		return false;

	TLIST_BUFF::iterator it = m_listBuff.begin();
	for( ; it != m_listBuff.end(); ++it)
	{
		if((*it)->GetBuffID() == dwBuffID)
		{
			return true;
		}		
	}

	return false;
}


/** 获得所有BUFF的index
@param   pBuffIndex ：buff index的数组
@param   nCount ：IN = 数组大小，OUT = buff数
@return  成功：true, 失败：false，表示数组过小
*/
bool CBuffPart::GetAllBuff(DWORD * pBuffIndex, int &nCount)
{
	int nTotalCount = nCount; nCount = 0;
	TLIST_BUFF::iterator it = m_listBuff.begin();
	for( ; it != m_listBuff.end(); ++it)
	{
		if(nCount >= nTotalCount)
		{
			return false;
		}

		pBuffIndex[nCount] = (*it)->GetIndex();nCount++;
	}

	return true;
}


/** 右键取消某个buff
@param   
@param   
@return  
*/
bool CBuffPart::RightClickRemoveBuff(int nBuffIndex)
{
	return true;
}


/** 通过buff id取得buff
@param   
@param   
@return  
*/
IBuff* CBuffPart::GetBuffByID(DWORD dwBuffID)
{
	TLIST_BUFF::iterator it = m_listBuff.begin();
	for( ; it != m_listBuff.end(); ++it)
	{
		if((*it)->GetBuffID() == dwBuffID)
		{
			return (*it);
		}		
	}

	return NULL;
}


/** 某个effectID是否存在
@param   
@param  
@return  
*/
bool CBuffPart::IsEffectIDExist(int nEffectID)
{
	TLIST_BUFF::iterator it = m_listBuff.begin();
	for( ; it != m_listBuff.end(); ++it)
	{
		if((*it)->IsEffectIDExist(nEffectID))
		{
			return true;
		}		
	}

	return false;
}


/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
// 订阅属性改变事件，然后同步到显示层
void CBuffPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    if (m_pMaster == NULL)
        return;

    switch(wEventID)
	{
		// 增加buff和移除buff向显示层发同一个事件，显示层跟进包数据nType来区分，省一条命令
	case EVENT_ENTITY_ADDBUFF:
	case EVENT_ENTITY_REMOVEBUFF:
		{
			/// 取得主角对象
			IClientEntity *pClientEntity = gClientGlobal->getHero();
			if (pClientEntity == NULL)
				break;

			// 不是自己
			if (pClientEntity->getUID() != m_pMaster->getUID())
				break;

			if (pszContext == NULL || nLen != sizeof(event_entity_buff_client))
				break;

			event_entity_buff_client *pBuffClient = (event_entity_buff_client *)pszContext;

			// 图标为0不显示
			if (pBuffClient->nIconID == 0)
			{
				return;
			}

			cmd_creature_setBuff setBuff;
			setBuff.nType = pBuffClient->nType;
			setBuff.dwIndex = pBuffClient->dwIndex;
			setBuff.nIconID = pBuffClient->nIconID;
			setBuff.dwTime = pBuffClient->dwTime;
			sstrcpyn(setBuff.szName, pBuffClient->szName, sizeof(setBuff.szName));
			sstrcpyn(setBuff.szTips, pBuffClient->szTips, sizeof(setBuff.szTips));
			setBuff.nLevel=pBuffClient->nLevel;
			setBuff.nIsShow=(pBuffClient->nIsShow ? 1 : 0);

			// 设置buff
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_BUFF, sizeof(cmd_creature_setBuff), 0, &setBuff, sizeof(setBuff));
		}
		break;

    case EVENT_ENTITY_INPUT_MANAGER:
        {
            // 取得当前时间
            DWORD dwTick = getTickCountEx();
            if (dwTick < m_dwLastInputTime + INPUT_SPACE_TIME)
            {
                break;
            }

            if (pszContext == NULL || nLen != sizeof(event_input_manager))
            {
                break;
            }
            event_input_manager *pManager = (event_input_manager *)pszContext;

            vector<DWORD> vBuff;
            // 鼠标按下状态
            if (pManager->nKeyState == EVKS_Down)
            {
                // ESC和鼠标右键取消BUFF
                if (pManager->nKeyCode == EVKC_Escape || pManager->nKeyCode == EVKC_Mouse1)
                {
                    TLIST_BUFF::iterator it = m_listBuff.begin();
                    for( ; it != m_listBuff.end(); ++it)
                    {
                        IBuff *pBuff = *it;
                        if (pBuff == NULL)
                        {
                            continue;
                        }

                        SBuffSchemeInfo *pSchemeInfo = g_pBuffClient->GetBuffSchemeInfo(pBuff->GetBuffID());
                        if(pSchemeInfo == NULL)
                        {
                            continue;
                        }

                        // 结束条件
                        list<DWORD>::iterator itEnd = pSchemeInfo->listEndFlag.begin();
                        for( ; itEnd != pSchemeInfo->listEndFlag.end(); ++itEnd)
                        {
                            if(*itEnd == BUFF_END_CLIENT_REMOVE)
                            {
                                vBuff.push_back(pBuff->GetIndex());
                                break;
                            }
                        }
                    }
                }

                int nCount = vBuff.size();
                if (nCount <= 0)
                {
                    break;
                }

                SBuffMsgRemoveIndex_CS remove;
                remove.nCount = nCount;

                obuf data;
                data.push_back(&remove, sizeof(remove));
                data.push_back(&vBuff[0], sizeof(DWORD)*nCount);
                SendMessageToEntityScene(m_pMaster->getUID(), PART_BUFF, BUFF_MSG_REMOVE_INDEX, data.data(), data.size()); 

                // 设置上次时间
                m_dwLastInputTime = getTickCountEx();
            }
        }

	case EVENT_ENTITY_ENTER_VIEWPORT: // 进入视野
		{
			if (m_SerializeBuff.empty())
			{
				break;
			}

			if (pszContext == NULL || nLen != sizeof(event_entity_enter_viewport))
			{
				break;
			}

			event_entity_enter_viewport *pEnterViewport = (event_entity_enter_viewport *)pszContext;
			
			SERIALIZEBUFF::iterator Iter = m_SerializeBuff.begin();
			while (Iter != m_SerializeBuff.end())
			{
				SBuffMsgAddBuff_SC addBuff = *Iter;

				// 是否可以增加该Buff
				if (canAddBuff(&addBuff))
				{
					// 增加BUFF
					OnServerMessageAddBuff(&addBuff, sizeof(addBuff), true);
				}

				// 移除列表
				Iter = m_SerializeBuff.erase(Iter);
			}
		}
		break;

	case EVENT_ENTITY_LEAVE_VIEWPORT:   // 离开视野
		{
			IClientEntity *pClientEntity = gClientGlobal->getHero();
			if (pClientEntity == NULL)
			{
				break;
			}

			if (m_listBuff.empty())
			{
				break;
			}

			if (pszContext == NULL || nLen != sizeof(event_entity_leave_viewport))
			{
				break;
			}

			event_entity_leave_viewport *pLevelViewport = (event_entity_leave_viewport *)pszContext;

			TLIST_BUFF::iterator Iter = m_listBuff.begin();
			while (Iter != m_listBuff.end())
			{
				IBuff *pBuff = *Iter;
				if (pBuff == NULL)
				{
					// 删除节点
					Iter = m_listBuff.erase(Iter);
					continue;
				}

				// 是自己才发移除BUFF事件
				if (m_pMaster->getUID() == pClientEntity->getUID())
				{
					event_entity_buff_client buffClient;
					buffClient.nType = BUFF_CLIENT_TYPE_REMOVE;
					buffClient.dwIndex = pBuff->GetIndex();
					buffClient.nIconID = pBuff->GetIconID();
					buffClient.nLevel=pBuff->GetLevel();
					// 删除buff事件
					m_pMaster->fireEvent(EVENT_ENTITY_REMOVEBUFF, &buffClient, sizeof(buffClient));
				}

				DWORD dwFlashID = pBuff->GetFlashID();
				if(dwFlashID > 0)
				{
					// 停止预施法光效
					EffectClient_StopLighting StopLighting;
					ssprintf_s(StopLighting.strName, sizeof(StopLighting.strName), "%d", dwFlashID);
					BUFF_DO_INNER_EFFECT(EffectClient_StopLighting, StopLighting, pBuff->GetEffectContext());
				}

                // 删除BUFF
				SAFE_RELEASE(pBuff);

				// 删除节点
				Iter = m_listBuff.erase(Iter);
			}
            m_listBuff.clear();
		}
		break;
        
    case EVENT_ACTOR_RELIVE:    // 玩家复活
        {
            if (pszContext == NULL || nLen != sizeof(event_actor_relive))
            {
                break;
            }            
            onEventReliev();
        }
        break;

    case EVENT_ENTITY_DIE:       // 人物死亡
        {
            if (pszContext == NULL || nLen != sizeof(event_entity_die))
            {
                break;
            }
            onEventDie();
        }
        break;
	default:
		break;
	}
}

// 复活事件处理
void CBuffPart::onEventReliev()
{
    if (m_listBuff.empty())
    {
        return;
    }

    DWORD dwFlag = 0;
    DWORD dwFlashID = 0;
    
    for ( TLIST_BUFF::iterator iter = m_listBuff.begin(); iter != m_listBuff.end(); ++iter)
    {
        IBuff *pBuff = (*iter);
        if(!pBuff)
        {
            continue;
        }

        // 取得BUFF标识
        dwFlag = pBuff->GetFlag();
        if( (dwFlag & tBuff_TypeFlag_DieRemoveReliveAdd)!= tBuff_TypeFlag_DieRemoveReliveAdd /* 死亡关闭复活重加光 */ )
        {
            continue;
        }

        // 要重加的光效
        dwFlashID = pBuff->GetFlashID();
        if(dwFlashID == 0)
        {
            continue;
        }

        // 播放光效
        EffectClient_AddLighting Lighting;
        ssprintf_s(Lighting.strName, sizeof(Lighting.strName), "%d", dwFlashID);
        BUFF_DO_INNER_EFFECT(EffectClient_AddLighting, Lighting, pBuff->GetEffectContext());

        //EmphasisLn(__FUNCTION__": add light id="<< dwFlashID);
    }
}

// 死亡事件处理
void CBuffPart::onEventDie()
{
    if (m_listBuff.empty())
    {
        return;
    }

    DWORD dwFlag = 0;
    DWORD dwFlashID = 0;

    // 死亡时将对“死亡关闭光效”和“死亡关闭复活重加光效”进行移除操作
    for ( TLIST_BUFF::iterator iter = m_listBuff.begin(); iter != m_listBuff.end(); ++iter)
    {
        IBuff *pBuff = (*iter);
        if(!pBuff)
        {
            continue;
        }

        // 取得BUFF标识
        dwFlag = pBuff->GetFlag();
        if( (dwFlag & tBuff_TypeFlag_DieRemove) != tBuff_TypeFlag_DieRemove /*死亡关闭光效*/ && (dwFlag & tBuff_TypeFlag_DieRemoveReliveAdd)!= tBuff_TypeFlag_DieRemoveReliveAdd /* 死亡关闭复活重加光 */ )
        {
            continue;
        }

        // 要移除的光效ID
        dwFlashID = pBuff->GetFlashID();
        if(dwFlashID == 0)
        {
            continue;
        }

        // 关闭光效
        EffectClient_StopLighting data;
        ssprintf_s(data.strName, sizeof(data.strName), "%d", dwFlashID);;
        BUFF_DO_INNER_EFFECT(EffectClient_StopLighting, data, pBuff->GetEffectContext());
    }
}

bool CBuffPart::canAddBuff(SBuffMsgAddBuff_SC * pBuffData)
{
	if (m_pMaster == NULL)
		return false;

	SBuffSchemeInfo *pSchemeInfo = g_pBuffClient->GetBuffSchemeInfo(pBuffData->dwBuffID);
	if (pSchemeInfo == NULL)
		return false;

	//判断是否加给主人的Buff
	if ((pSchemeInfo->dwFlag & tBuff_TypeFlag_BcastOnlyMaster) == tBuff_TypeFlag_BcastOnlyMaster)
	{

		// 怪物UID
		UID addUID = pBuffData->uidAdd;
		if (addUID == 0 || addUID == INVALID_UID)
			return false;

		// 怪物实体
		IClientEntity * pEntity = gClientGlobal->getEntityFactory()->getEntity(addUID);
		if (pEntity == NULL)
			return false;

		UID masterUID = pEntity->getIntProperty(PROPERTY_MASTER_UID);
		if (masterUID == 0 || masterUID == INVALID_UID)
			return false;

		// 不是怪物的主人,则本客户则不添加该Buff
		IClientEntity* pHero = gClientGlobal->getHero();
		if (pHero == NULL)
			return false;

		// 怪物的主人不是本地客户端主角
		if (masterUID != pHero->getUID())
			return false;
	}

	return true;
}

/////////////////////////////////////////CBuffPart/////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
CBuffPart::CBuffPart(void)
{
	m_pMaster = NULL;

	m_listBuff.clear();

	m_SerializeBuff.clear();

    m_dwLastInputTime = 0;
}


/** 
@param   
@param   
@return  
*/
CBuffPart::~CBuffPart(void)
{
    m_pMaster = NULL;
}


/** 
@param   
@param   
@return  
*/
bool CBuffPart::AddBuff(IBuff *pBuff)
{
	if(pBuff == NULL || m_pMaster == NULL)
	{
		return false;
	}

	// 启动BUFF
	pBuff->Start();

	// 压入列表中
	m_listBuff.push_back(pBuff);

	DWORD dwFlashID = pBuff->GetFlashID();
	if(dwFlashID > 0)
	{
		// 播放光效
		EffectClient_AddLighting Lighting;
		ssprintf_s(Lighting.strName, sizeof(Lighting.strName), "%d", dwFlashID);
		BUFF_DO_INNER_EFFECT(EffectClient_AddLighting, Lighting, pBuff->GetEffectContext());
	}

	return true;
}

/** 
@param   
@param   
@return  
*/
bool CBuffPart::RemoveBuff(IBuff *pBuff)
{
	if (pBuff == NULL || m_pMaster == NULL)
	{
		return false;
	}
	IClientEntity *pClientEntity = gClientGlobal->getHero();

	DWORD dwBuffID = pBuff->GetBuffID();
	DWORD dwBuffLevel = pBuff->GetLevel();

	TLIST_BUFF::iterator it = m_listBuff.begin();
	for( ; it != m_listBuff.end(); ++it)
	{
		if((*it) != pBuff)
		{
			continue;
		}

		// 是自己才发事件
		if (pClientEntity != NULL && pClientEntity->getUID() == m_pMaster->getUID())
		{
			event_entity_buff_client buffClient;
			buffClient.nType = BUFF_CLIENT_TYPE_REMOVE;
			buffClient.dwIndex = pBuff->GetIndex();
			buffClient.nIconID = pBuff->GetIconID();
			buffClient.nLevel=pBuff->GetLevel();
			// 删除buff事件
			m_pMaster->fireEvent(EVENT_ENTITY_REMOVEBUFF, &buffClient, sizeof(buffClient));
		}

		DWORD dwFlashID = pBuff->GetFlashID();
		if(dwFlashID > 0)
		{
			// 停止预施法光效
			EffectClient_StopLighting StopLighting;
			ssprintf_s(StopLighting.strName, sizeof(StopLighting.strName), "%d", dwFlashID);
			BUFF_DO_INNER_EFFECT(EffectClient_StopLighting, StopLighting, pBuff->GetEffectContext());
		}

		// 删除BUFF
		SAFE_RELEASE(pBuff);

		// 删除节点
		m_listBuff.erase(it);

		// 更新快速查询掩码
		SBuffSchemeInfo *pSchemeInfo = g_pBuffClient->GetBuffSchemeInfo(dwBuffID);
		if (pSchemeInfo != NULL)
		{
			for (list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchemeInfo->listLevelInfo.begin(); it != pSchemeInfo->listLevelInfo.end(); ++it)
			{
				if (it->dwBuffLevel == dwBuffLevel)
				{
					m_AccQueryMask.clear(it->nAccQueryIndex);
					break;
				}
			}
		}

		return true;
	}

	return false;
}


// 注册事件
void CBuffPart::subscibeEvents(void)
{
	if (m_pMaster == NULL)
	{
		return;
	}

	UID uid = m_pMaster->getUID();
	BYTE type  = UID_2_TYPE(uid);

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
	{
		return;
	}
	// 是自己
	if (m_pMaster->isHero())
	{
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ADDBUFF, type, uid, __FILE__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_REMOVEBUFF, type, uid, __FILE__);
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_INPUT_MANAGER, type, uid, __FILE__);
	}

    if(type==TYPE_PLAYER_ROLE)
    {
        // 注册人物复活事件
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_RELIVE, type, uid, __FILE__);
        // 注册人物死亡事件
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DIE, type, uid, __FILE__);
    }

	pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ENTER_VIEWPORT, type, uid, __FILE__);
	pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_LEAVE_VIEWPORT, type, uid, __FILE__);
}


// 注销事件
void CBuffPart::unSubscibeEvents(void)
{
	if (m_pMaster == NULL)
	{
		return;
	}

	UID uid = m_pMaster->getUID();
	BYTE type  = UID_2_TYPE(uid);

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
	{
		return;
	}
	// 是自己
	if (m_pMaster->isHero())
	{
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ADDBUFF, type, uid);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_REMOVEBUFF, type, uid);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_INPUT_MANAGER, type, uid);
	}
    if(type==TYPE_PLAYER_ROLE)
    {
        // 取消注册人物复活事件
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_RELIVE, type, uid);
        // 取消注册人物死亡事件
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DIE, type, uid);
    }

	pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ENTER_VIEWPORT, type, uid);
	pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_LEAVE_VIEWPORT, type, uid);
}


// 服务器消息增加buff
void CBuffPart::OnServerMessageAddBuff(void *pData, int nLen, bool bAppear)
{
	if (m_pMaster == NULL)
	{
		return;
	}
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

	if(pData == NULL || nLen != sizeof(SBuffMsgAddBuff_SC))
	{
		return;
	}
	SBuffMsgAddBuff_SC *pMsgAddBuff = (SBuffMsgAddBuff_SC *)pData;

	// 通过buff index取得buff,是否有重复
	IBuff *pOldBuff = GetBuff(pMsgAddBuff->dwIndex);
	if(pOldBuff != NULL)
	{
		return;
	}


	EFFECT_CONTEXT context;
	context.pEntity = m_pMaster;
	context.uidOperator = pMsgAddBuff->uidAdd;
	context.uidTarget = m_pMaster->getUID();
	context.nID = pMsgAddBuff->dwBuffID;
	context.ptTargetTile = pMsgAddBuff->targetTile;
    context.bOnAppear = bAppear;

	// 获取技能距离
	if(pMsgAddBuff->nSpellID > 0)
	{
		ISpellFactory* pSpellFactory = gClientGlobal->getSpellFactory();
		SPELL_DATA* pSpellData = pSpellFactory->getSpellData(pMsgAddBuff->nSpellID);
		if (pSpellData != NULL)
		{
			context.fDistance = (float)pSpellData->getInt(SPDATA_DISTANCE) / SPELL_FLOAT_VALUE;
		}
	}

	// 创建buff
	IBuff *pNewBuff = new CBuff();
	if(!pNewBuff->Create(this, pMsgAddBuff->dwIndex, pMsgAddBuff->dwBuffID, pMsgAddBuff->dwBuffLevel, pMsgAddBuff->dwTime, &context))
	{
		SAFE_RELEASE(pNewBuff);
		return;
	}

	// 增加Buff
	AddBuff(pNewBuff);

	SBuffSchemeInfo *pSchemeInfo = g_pBuffClient->GetBuffSchemeInfo(pMsgAddBuff->dwBuffID);
	if (pSchemeInfo == NULL)
		return;

	// 更新快速查询掩码
	DWORD dwQueryMask = 0;
	for (list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchemeInfo->listLevelInfo.begin(); it != pSchemeInfo->listLevelInfo.end(); ++it)
	{
		if (it->dwBuffLevel == pMsgAddBuff->dwBuffLevel)
		{
			m_AccQueryMask.set(it->nAccQueryIndex);
			break;
		}
	}

	// 是自己才发事件
	if (pClientEntity->getUID() == m_pMaster->getUID())
	{
        int dwBuffFlag = pSchemeInfo->dwFlag;

		event_entity_buff_client buffClient;
		buffClient.nType = BUFF_CLIENT_TYPE_ADD;
		buffClient.dwIndex = pNewBuff->GetIndex();
		buffClient.nIconID = pNewBuff->GetIconID();
		buffClient.dwTime = pNewBuff->GetLeftTime();
		sstrcpyn(buffClient.szName, pNewBuff->GetBuffName(), sizeof(buffClient.szName));
		sstrcpyn(buffClient.szTips, pNewBuff->GetDesc(), sizeof(buffClient.szTips));
		buffClient.nLevel=pNewBuff->GetLevel();
		if((dwBuffFlag&tBuff_TypeFlag_IsShow) == tBuff_TypeFlag_IsShow)
		{
		   buffClient.nIsShow=true;
		}
		else
		{
		   buffClient.nIsShow=false;
		}
		// 增加buff事件
		m_pMaster->fireEvent(EVENT_ENTITY_ADDBUFF, &buffClient, sizeof(buffClient));
	}
}


// 服务器消息移除buff
void CBuffPart::OnServerMessageRemoveBuff(void *pData, int nLen)
{
	if (m_pMaster == NULL)
	{
		return;
	}
	if(pData == NULL || nLen != sizeof(SBuffMsgRemoveBuff_SC))
	{
		return;
	}
	SBuffMsgRemoveBuff_SC *pMsgRemoveBuff = (SBuffMsgRemoveBuff_SC *)(pData);

	// 通过buff index取得buff
	IBuff *pOldBuff = GetBuff(pMsgRemoveBuff->dwIndex);
	if(pOldBuff == NULL)
	{
		return;
	}

	// 移除
	RemoveBuff(pOldBuff);
}
void  CBuffPart::OnServerMessageChangeChargeCount(void *pData, int nLen)
{
	if (m_pMaster == NULL)
	{
		return;
	}
	if(pData == NULL || nLen != sizeof(SBuffMsgBuffChargeCountChange_SC))
	{
		return;
	}
    SBuffMsgBuffChargeCountChange_SC *pChangeChargeCount = (SBuffMsgBuffChargeCountChange_SC *)pData;

	IBuff *pInBuff = GetBuff(pChangeChargeCount->dwIndex);
	if (pInBuff==NULL)
	{
		return;
	}

	cmd_buff_charge_count_change ChargeCount;
	ChargeCount.chargeCount = pChangeChargeCount->nChargeCount;
	ChargeCount.nBuffID = pInBuff->GetBuffID();
	ChargeCount.nChargeValue = pChangeChargeCount->nChargeValue;
	// 发送消息
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CHARGE_COUNT_CHANGE, sizeof(cmd_buff_charge_count_change), 0, &ChargeCount, sizeof(ChargeCount));
}