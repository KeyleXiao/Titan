/*******************************************************************
** 文件名:	MobaGoodsPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/6/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "MobaGoodsPart.h"
#include "EntityHelper.h"
#include "IServiceContainer.h"
#include "IServerGlobal.h"
#include "ISceneManager.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "EntityDef.h"
#include "ServiceHelper.h"
#include <math.h>
#include "WarMessageDef.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "IPlayerRole.h"
#include "MobaGoods.h"
#include "MobaGoodsBaseProp.h"
#include "MobaGoodsGroupUniqueProp.h"
#include "MobaGoodsIDUniqueProp.h"
#include "MobaEquipment.h"
#include "MobaLeechdom.h"
#include "ISlotPart.h"
#include "IWarPart.h"
#include "ChatHelper.h"
#include "IChatSceneService.h"
#include "IBankPart.h"
#include "TaskStorageDef.h"
#include "EquipSchemeMgr.h"
#include "IEventEngine.h"

//#include "LuaHelper.h"
#define  NOTLUA_CALL_LINE
////////////////////* MobaGoodsPart */////////////////////////////////////////////
MobaGoodsPart::MobaGoodsPart()
	:m_pMaster(NULL)
	,m_bCanBuyOrSell(false)
	,m_bCanCancelOperation(false)
	,m_pEquipSchemeMgr(NULL)
	,m_nMatchTypeID(0)
{
    m_WarDBKey = 0;
	memset(m_MobaSlotGoods, 0 , sizeof(m_MobaSlotGoods));
}

MobaGoodsPart::~MobaGoodsPart()
{
    
}

////////////////////* IEntityPart */////////////////////////////////////////////
short MobaGoodsPart::getPartID()
{
	return PART_MOBAGOODS;
}


bool MobaGoodsPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	if (!pEntity)
	{
		return false;
	}

	m_pMaster = pEntity;

    m_nLoginMode = nLoginMode;

    m_mapReplicationMonster.clear();

    if(m_pMaster->getType() != TYPE_PLAYER_ROLE)
    {
        return true;
    }
	__IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return false;
	}

	// 注册施法技能
	pEntityEvent->Subscibe(static_cast<IEventExecuteSink*> (this), EVENT_ENTITY_CAST_SPELL, __FUNCTION__);
	pEntityEvent->Subscibe(static_cast<IEventVoteSink*> (this), EVENT_ENTITY_CAST_SPELL, __FUNCTION__);
    pEntityEvent->Subscibe(static_cast<IEventExecuteSink*> (this), EVENT_ENTITY_CREATE, __FUNCTION__);

	//// 注册怪物死亡事件
    //pEventEngine->Subscibe(static_cast<IEventExecuteSink*> (this), EVENT_GAME_ENTITY_REPLICATION_MONSTER, TYPE_MONSTER, 0, __FUNCTION__);

    // 启动定时器
    gServerGlobal->getTimerService()->SetTimer(ETimerEventID_SaveEquipLog, WarBuyEquipLog, this);

    // 设置warkey
    setWarInfo();

    gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	// 真人才开启装备管理功能
	PDBID pdbid = m_pMaster->getIntProperty(PROPERTY_ID);
	if (IsComputerPDBID(pdbid) == false)
	{
		m_pEquipSchemeMgr = new CEquipSchemeMgr();
		if (!m_pEquipSchemeMgr->init(m_pMaster, m_WarDBKey, m_nMatchTypeID))
		{
			WarningLn(__FUNCTION__ ": Initial equip scheme failed.");
			m_pEquipSchemeMgr->release();
			m_pEquipSchemeMgr = NULL;
		}
	}


    m_mapCDOverLayData.clear();

	return true;
}

void MobaGoodsPart::release()
{
    batchSaveBuyEquipLog();

	if (m_pMaster != NULL)
	{
		// 保存常用装备，玩家在战场结束后释放的时候执行
		__IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
		if (pWarPart != NULL && pWarPart->getWarID() != INVALID_WAR_ID)
		{
			if (m_pEquipSchemeMgr != NULL)
				m_pEquipSchemeMgr->onWarEnd();
		}

		__IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
		if (pEntityEvent != NULL)
		{
			// 注销施法技能
			pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CAST_SPELL);
			pEntityEvent->UnSubscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CAST_SPELL);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE);
            
		}
	}

    gServerGlobal->getTimerService()->KillTimer(ETimerEventID_SaveEquipLog, this);

	for (int i = 0; i< MAX_GOODS_SLOT_COUNT; ++i)
	{
		if (m_MobaSlotGoods[i] != NULL )
		{
			// 移除相关属性、技能、buff
			m_MobaSlotGoods[i]->Remove();
			m_MobaSlotGoods[i]->Release();
			m_MobaSlotGoods[i] = NULL;
		}
	}

    // 释放申请的内存空间
    T_MAP_PROP_MANAGER::iterator iter = m_MobaGoodsPropManager.begin();
    for (;iter != m_MobaGoodsPropManager.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            iter->second->Release();
        }
        
    }
    m_MobaGoodsPropManager.clear();

    map<UID, T_MAP_PROP_MANAGER *>::iterator iterAllMonster = m_mapReplicationMonster.begin();
    for (;iterAllMonster != m_mapReplicationMonster.end(); ++iterAllMonster)
    {
        if (iterAllMonster->second == NULL)
        {
            continue;
        }
        T_MAP_PROP_MANAGER *pPropMgr = iterAllMonster->second;
        for (T_MAP_PROP_MANAGER::iterator iter = pPropMgr->begin(); iter != pPropMgr->end(); ++iter)
        {
            iter->second->Release();
            iter->second = NULL;
        }
        pPropMgr->clear();
        delete pPropMgr;
        pPropMgr = NULL;
    }
    m_mapReplicationMonster.clear();

	m_pMaster = 0;

    gServerGlobal->getDBEngineService()->unregisterHandler(this);

	if(m_pEquipSchemeMgr != NULL)
		m_pEquipSchemeMgr->release();

    m_mapCDOverLayData.clear();

	delete this;
}

bool MobaGoodsPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch(msgId)
	{
	case CS_MSG_WAR_BUY_GOOD:
		{
			if (len < sizeof(msg_entity_fight_buy_good))
			{
				return false;
			}
			msg_entity_fight_buy_good * pData = (msg_entity_fight_buy_good*)data;
			if(!OnClientBuyGood(pData->nGoodID))
            {
                return false;
            }
		}
		break;
	case CS_MSG_WAR_SELL_GOOD:
		{
			if (len < sizeof(msg_entity_fight_sell_good))
			{
				return false;
			}
			msg_entity_fight_sell_good * pData = (msg_entity_fight_sell_good*)data;
			OnClientSellGood(pData->nSlotIndex, pData->nGoodID);
		}
		break;
	case CS_MSG_WAR_EXCHANGE_GOOD:
		{
			if (len < sizeof(msg_entity_fight_exchange_good))
			{
				return false;
			}
			msg_entity_fight_exchange_good * pData = (msg_entity_fight_exchange_good*)data;
			OnClientExChangeGood(pData->nSrcSlot, pData->nTargetSlot);
		}
		break;
	case CS_MSG_WAR_USE_GOOD:
		{
			if (len < sizeof(msg_entity_fight_use_good))
			{
				return false;
			}
			msg_entity_fight_use_good * pData = (msg_entity_fight_use_good*)data;
			OnClientUseGood(pData->nSlotIndex);
		}
		break;
	case CS_MSG_WAR_REVOCATION_OPERATION:
		{
			if (len < sizeof(msg_entity_revocation_operation))
			{
				return false;
			}
			msg_entity_revocation_operation * pData = (msg_entity_revocation_operation*)data;
			OnClientRevocationOperation(pData->uUID);
		}
		break;
	case CS_MSG_WAR_ADD_EQUIPSCHEME:
	case CS_MSG_WAR_DEL_EQUIPSCHEME:
	case CS_MSG_WAR_MODIFY_EQUIPSCHEME:
	case CS_MSG_WAR_READ_EQUIPSCHEME:
		{
			if(m_pEquipSchemeMgr != NULL)
				m_pEquipSchemeMgr->onMessage(msgId, data, len);
		}
		break;
	default:
		break;
	}
	return true;
}

__IEntity * MobaGoodsPart::getHost()
{
	return m_pMaster;
}



bool MobaGoodsPart::onSerialize( rkt::ibuffer & in,int flag )
{
    if(flag & TO_OTHER_SEVER)
    {
        // 是否能出售购买
        in >>m_bCanBuyOrSell>>m_bCanCancelOperation>>m_WarDBKey>>m_nMatchTypeID;

        // 先创建物品出来 还不能初始化 实体onload之后才能初始化
        for (int i = 0; i < MAX_GOODS_SLOT_COUNT; ++i)
        {
            int nOutSlotIndex = 0;
            bool bHaveGoods = false;
            int eGoodType = MGT_NoneType; 
            //压入槽位ID,标示和类型
            in>>nOutSlotIndex >>bHaveGoods >>eGoodType;

            // 槽位对应的物品或者装备
            if (bHaveGoods)
            {
                // 分配新物品内存
                switch(eGoodType)
                {
                case MGT_EquipmentType:
                    {
                        m_MobaSlotGoods[i] = new CMobaEquipment;
                    }
                    break;
                case MGT_LeechdomType:
                    {
                        m_MobaSlotGoods[i] = new CMobaLeechdom;
                    }
                    break;
                default:
                    {
                        ErrorLn(__FUNCTION__" eGoodType = "<<eGoodType<<" index = "<<i);
                    }
                    break;
                }
                m_MobaSlotGoods[i]->onSerialize(in, flag);
                
            }
        }
    }

	return true;
}

bool MobaGoodsPart::deSerialize( rkt::obuf & out,int flag )
{        
    if (flag & TO_CLIENT_SELF)
    {
        out <<m_bCanBuyOrSell;
    }
    else if(flag & TO_OTHER_SEVER)
    {

	    // 是否能出售购买
	    out <<m_bCanBuyOrSell<<m_bCanCancelOperation<<m_WarDBKey<<m_nMatchTypeID;

        for (int i = 0; i < MAX_GOODS_SLOT_COUNT; ++i)
        {
            bool haveGoods = false;
            // 槽位对应的物品或者装备
            if (m_MobaSlotGoods[i] != NULL)
            {
                haveGoods = true;
                int GoodsType = m_MobaSlotGoods[i]->GetGoodsType();
                //压入槽位ID,标示和类型
                out<<i <<haveGoods <<GoodsType;
                m_MobaSlotGoods[i]->deSerialize(out, flag);
            }
            else
            {
                haveGoods = false;
                int GoodsType = MGT_NoneType;
                out<<i<<haveGoods<<GoodsType;
            }
        }

    }


	return true;
}

bool MobaGoodsPart::Parse(string linkContent)
{
	return true;
}

// 显示飘字
void MobaGoodsPart::showSystemMessage(__IEntity *pEntity, EMChatTipID nTipID, LPCSTR szParam)
{
	if (pEntity == NULL)
	{
		return;
	}

	if (pEntity->getType() != TYPE_PLAYER_ROLE)
	{
		return;
	}

    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService == NULL)
    {
        return;
    }

	pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, pEntity->getIntProperty(PROPERTY_ID), nTipID, szParam);
}

bool MobaGoodsPart::OnClientBuyGood(int nGoodID, int nGoodsCost)
{
	if (m_pMaster == nullptr)
	{
		return false;
	}

    __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent == NULL)
    {
        return false;
    }

	int nCost = nGoodsCost;
	bool bIsSpecialSlotGood = false;
	emGoodAutoUseType eAutoUseType = GAT_NoneType;
	emMobaGoodType nGoodsType = MGT_NoneType;
	T_MAP_COMPOSE_SLOT_CUR_RECORD mapComposeSlotCurRecord;
	// 购买物品流程
    EWarCheckBuyGoodErrorCode buyErroCode = CanBuyGood(nGoodID, nGoodsType, nCost, eAutoUseType, bIsSpecialSlotGood, mapComposeSlotCurRecord);

    event_entity_can_buy_good eventData;
    eventData.nGoodID = nGoodID;
    if (!pEntityEvent->FireVote(EVENT_ENTITY_CAN_BUY_GOOD, (char *)&eventData, sizeof(eventData)))
    {
        buyErroCode = (EWarCheckBuyGoodErrorCode)eventData.ErrorCode;
    }

    switch(buyErroCode)
    {

    case EBEC_CheckCantBuyState:
        {
            showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_CANNOTBUY_STATE);
        }
        break;

    case EBEC_CheckConfigError:
        {
            showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_NOSUCHGOODS);
        }
        break;

    case EBEC_CheckHoldMaxCount:
        {
            showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_MAXHOLD);
        }
        break;

    case EBEC_CheckIsSlotFull:
        {
            showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_SLOT_ISFULL);
        }
        break;

    case EBEC_CheckNotEnoughMoney:
        {
            showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_MONEYNOTENOUGH);
        }
        break;
    case EBEC_CheckGoodBuyLimit:
        {
            showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_BUY_LIMIT);
        }
        break;
    case EBEC_CheckGoodBuyFreeze:
        {
            showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_BUY_FREEZE);
        }
        break;
    
    default:
        break;
    }

	if (buyErroCode != EBEC_CheckNoneError)
	{
		return false;
	}

    fillBuyEquipLog((int)MGBT_BUY,nGoodID);

	return BuyGood(nGoodID, nGoodsType, nCost, eAutoUseType, bIsSpecialSlotGood, mapComposeSlotCurRecord);

}


EWarCheckBuyGoodErrorCode MobaGoodsPart::CanBuyGood(int nGoodID, emMobaGoodType &nGoodType, int &nCost,emGoodAutoUseType &eAutoUseType, bool &bIsSpecialSlotGood, T_MAP_COMPOSE_SLOT_CUR_RECORD &mapComposeSlotCurRecord)
{
    if(m_pMaster == NULL || m_pMaster->getType() != TYPE_PLAYER_ROLE)
    {
        return EBEC_CheckEntityEerror;
    }

    // 电脑不用考虑购物buff。。  AI条件里已经检测了与出生点距离。
    // 这儿经常会挡住电脑买东西。
    __IEntityProperty* pProperty = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
    if (pProperty == NULL)
    {
        ErrorLn(__FUNCTION__": pProperty == NULL");
        return EBEC_CheckEntityEerror;
    }

    if(pProperty->getProperty_Integer(PROPERTY_AI_SCHEME_ID) <= 0 && !m_bCanBuyOrSell)
    {
        return EBEC_CheckCantBuyState;
    }

	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
        ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return EBEC_CheckConfigError;
	}

	SSchemeMobaGoods *pSchemeMobaGoods = pSchemeCenter->getSchemeMobaGoods()->getSchemeMobaGoodsInfo(nGoodID);

	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeMobaGoods == NULL nGoodID ="<<nGoodID);
		return EBEC_CheckConfigError;
	}
	bIsSpecialSlotGood = pSchemeMobaGoods->bIsSpecialSlotGood;
	eAutoUseType = pSchemeMobaGoods->eAutoUseType;
	//最大拥有数量检查
	int CurrentCount = 0;
	for(int n = 0; n < MAX_GOODS_SLOT_COUNT ; ++n)
	{
		if (m_MobaSlotGoods[n] != NULL &&m_MobaSlotGoods[n]->GetGoodID() == nGoodID)
		{
			CurrentCount += m_MobaSlotGoods[n]->GetOverLayCount();
		}
	}

	if (CurrentCount >= pSchemeMobaGoods->nMaxCount)
	{
		return EBEC_CheckHoldMaxCount;
	}


	// 检测在本战场是否可以购买此物品
	if (CheckGoodsIsAtThisWar(pSchemeMobaGoods) == false )
	{
		return EBEC_CheckThisWarCantBuy;
	}

    // 当前购买物品需要的价格
    if (nCost < 0)
    {
        nCost = pSchemeMobaGoods->nGoodCost;
    }

    int needCost = nCost;
	nGoodType = pSchemeMobaGoods->nTypeID;

	//合成价格清算
	ComposeGoodCheck(nGoodID, nCost, mapComposeSlotCurRecord);

    // 自动使用&满物品判定
    bool bIsSlotFull = true;
    //能够被当前拥有合成
    if(needCost != nCost)
    {
        bIsSlotFull = false;
    }
    else
    {
        for(int n = 0; n < MAX_GOODS_SLOT_COUNT ; ++n)
        {
            //特殊槽位物品不用判定
            if (pSchemeMobaGoods->bIsSpecialSlotGood)
            {
                bIsSlotFull = false;
                break;
            }

            // 强制使用和自动使用不用判定
            if (pSchemeMobaGoods->eAutoUseType > GAT_NoneType && pSchemeMobaGoods->eAutoUseType < GAT_MAX)
            {
                bIsSlotFull = false;
                break;
            }

            //有空槽位
            if (m_MobaSlotGoods[n] == NULL && n != SPECIAL_SLOT_INDEX)
            {
                bIsSlotFull = false;
                break;
            }
        }
    }

    if (bIsSlotFull)
    {
        // 物品满
        return EBEC_CheckIsSlotFull;
    }

	//是否够条件买
	if (pProperty->getProperty_Integer(PROPERTY_TALENT_POINT) < nCost)
	{
		return EBEC_CheckNotEnoughMoney;
	}

    return EBEC_CheckNoneError;
}

bool MobaGoodsPart::CheckGoodsIsAtThisWar( SSchemeMobaGoods* pSchemeMobaGoods)
{

	__IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
	if(pWarPart == NULL)
	{
		return false;
	}
	int nWarID = pWarPart->getWarID();

	SERVICE_PTR pWarContainer = gServerGlobal->getWarManager()->GetWar(nWarID);
	if(pWarContainer == 0)
	{
		return false;
	}
	IWarService* pWarService = (IWarService*)pWarContainer->query_interface();
	if(pWarService == NULL)
	{
		return false;
	}

	int nWarConfigId = pWarService->getWarTypeId();

	for ( int n = 0; n < WAR_LIMIT_MAX; ++n)
	{
		int nWarLimit = pSchemeMobaGoods->WarLimit[n];
		if ( nWarLimit == 0 && nWarLimit != nWarConfigId )
		{
			showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_NOSUCHGOODS);
			return false;
		} else if ( nWarLimit != 0 && nWarLimit == nWarConfigId )
		{
			break;
		}
	}
	return true;
}


bool MobaGoodsPart::ComposeGoodCheck(int nGoodID, int &nCost, T_MAP_COMPOSE_SLOT_CUR_RECORD &mapComposeSlotCurRecord)
{
	int NeedComposeList[COMPOSE_MAX_COUNT] = {0};
	int nCount = 0;

	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return false;
	}

	SSchemeMobaGoods *pSchemeMobaGoods = pSchemeCenter->getSchemeMobaGoods()->getSchemeMobaGoodsInfo(nGoodID);
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeMobaGoods == NULL nGoodID ="<<nGoodID);
		return false;
	}

	// 优先合成判定 获得初级合成材料
	pSchemeCenter->getSchemeMobaGoods()->getSchemeNeedComposeList(nGoodID, NeedComposeList, nCount);

	//合成价格清算
	if (nCount > 0)
	{
		for (int i = 0; i < nCount; ++ i)
		{
			bool bHaveInsertSlotCurRecord = false;
			for(int n = 0; n < MAX_GOODS_SLOT_COUNT ; ++n)
			{
				if (m_MobaSlotGoods[n] != NULL && NeedComposeList[i] == m_MobaSlotGoods[n]->GetGoodID())
				{
					// 是否锁定了
					map<int,bool>::iterator iter = mapComposeSlotCurRecord.find(n);
					if (iter != mapComposeSlotCurRecord.end())
					{
						continue;
					}

					// 添加到临时锁定列表
					mapComposeSlotCurRecord.insert(pair<int,bool>(n, true));

					//找到并设置为已经锁定
					bHaveInsertSlotCurRecord = true;

					// 减去所需合成物品价格
					nCost -= m_MobaSlotGoods[n]->GetSchemeInfo()->nGoodCost;
					break;
				}
			}

			//没找到的继续拆分遍历
			if (bHaveInsertSlotCurRecord == false)
			{
				ComposeGoodCheck(NeedComposeList[i], nCost, mapComposeSlotCurRecord);
			}
		}

	}

	return true;
}

bool MobaGoodsPart::BuyGood(int nGoodID, emMobaGoodType nGoodType,int nCost, emGoodAutoUseType eAutoUseType, bool bIsSpecialSlotGood, T_MAP_COMPOSE_SLOT_CUR_RECORD &mapComposeSlotCurRecord,bool bSpecialEffectAdd)
{
	if (nGoodID<=0 || nGoodType<=MGT_NoneType || nGoodType>=MGT_MAX || nCost < 0 )
	{
        WarningLn(__FUNCTION__":nGoodID="<<nGoodID<<", GoodType="<<nGoodType <<",nCost="<<nCost);
		return false;
	}

	__IEntityProperty * pProperty = (__IEntityProperty*)(m_pMaster->getEntityPart(PART_PROPERTY));
	if (pProperty == NULL)
	{
		ErrorLn("MobaGoodsPart::BuyGood() -- pProperty == NULL");
		return false;
	}

	bool bHaveAddSuccess = false;

	//记录到撤销管理器
	SRevocationInfo tmpRevocationInfo;
	tmpRevocationInfo.RevocationType = EWRT_Buy;
	tmpRevocationInfo.nTalentPointChange = nCost;

    bool isAutoUsed = false;
    // 先判定是否是强制使用的物品
    if (eAutoUseType == GAT_ForceAutoUse)
    {
        IMobaGoods* pMobaGood = createMobaGoods(nGoodType);
        if(pMobaGood)
        {
            pMobaGood->Initial(m_pMaster, GOODS_AUTOUSE_SLOT, nGoodID);
            if (pMobaGood->CanUse())
            {
                pMobaGood->Use();
                pMobaGood->Release();
                m_MobaSlotGoods[GOODS_AUTOUSE_SLOT] = NULL;
                bHaveAddSuccess = true;
                isAutoUsed = true;
            }
        }
    }

    int nAddSlotIndex = -1;
    int nAddSlotItemCount = -1;
    // 能否叠加
    if (!bHaveAddSuccess)
    {
        // 查找槽位是否有同样ID物品能叠加
        for (int n = 0; n < MAX_GOODS_SLOT_COUNT; ++n)
        {
            IMobaGoods* pMobaGoods = m_MobaSlotGoods[n];
            if (!pMobaGoods || pMobaGoods->GetGoodID() != nGoodID )
            {
                continue;
            }
            //判定能否叠加
            if (!pMobaGoods->CanAddOverlayCount())
            {
                continue;
            }

            SSlotRevocationInfo tmpSlotRevocation;
            tmpSlotRevocation.nSlotIndex= n;
            tmpSlotRevocation.nGoodID   = pMobaGoods->GetGoodID();
            tmpSlotRevocation.nCount    = pMobaGoods->GetOverLayCount();
            tmpSlotRevocation.GoodType  = pMobaGoods->GetGoodsType();

            tmpRevocationInfo.bisAddOverlayCount = true;
            tmpRevocationInfo.addGoodsList.push_back(tmpSlotRevocation);        		
            // 增加槽上物品数量
            pMobaGoods->AddOverlayCount();
            bHaveAddSuccess = true;	
            nAddSlotIndex = n;
            nAddSlotItemCount = pMobaGoods->GetOverLayCount();
        }
    }

	// 不能叠加则找空闲的槽位
	if (!bHaveAddSuccess)
	{
        //消耗并移除合成物品
        T_MAP_COMPOSE_SLOT_CUR_RECORD::iterator iter= mapComposeSlotCurRecord.begin();
        for (;iter != mapComposeSlotCurRecord.end(); ++iter)
        {
            const int & nCurIndex = iter->first;
            IMobaGoods* &pMobaGoods = m_MobaSlotGoods[nCurIndex];
            if(!pMobaGoods || !iter->second)
                continue;

            // 移除槽位物品
            SSlotRevocationInfo tmpSlotRevocation;
            tmpSlotRevocation.nSlotIndex= nCurIndex;
            tmpSlotRevocation.nGoodID   = pMobaGoods->GetGoodID();
            tmpSlotRevocation.nCount    = pMobaGoods->GetOverLayCount();
            tmpSlotRevocation.GoodType  = pMobaGoods->GetGoodsType();
            // 添加进撤销管理器
            tmpRevocationInfo.removeGoodsList.push_back(tmpSlotRevocation);

            // 移除相关属性、技能、buff
            pMobaGoods->Remove();
            pMobaGoods->Release();
            pMobaGoods = NULL;

            RecomputeAllGoodsEffect();
        } 

		//判定是否特殊槽位物品
		if (bIsSpecialSlotGood == true)
		{
            IMobaGoods* &pMobaGoods = m_MobaSlotGoods[SPECIAL_SLOT_INDEX];
			// 存在的话就要先移除
			if (pMobaGoods != NULL)
			{
				SSlotRevocationInfo tmpSlotRevocation;
                // todo
				tmpSlotRevocation.nSlotIndex= SPECIAL_SLOT_INDEX;
				tmpSlotRevocation.nGoodID   = pMobaGoods->GetGoodID();
				tmpSlotRevocation.nCount    = pMobaGoods->GetOverLayCount();
				tmpSlotRevocation.GoodType  = pMobaGoods->GetGoodsType();
				// 添加进撤销管理器
				tmpRevocationInfo.removeGoodsList.push_back(tmpSlotRevocation);

				pMobaGoods->Remove();
				pMobaGoods->Release();
				pMobaGoods = NULL;

				RecomputeAllGoodsEffect();
			}
            

            pMobaGoods = createMobaGoods(nGoodType);

            if ( pMobaGoods != NULL )
            {
                pMobaGoods->Initial(m_pMaster, SPECIAL_SLOT_INDEX, nGoodID);

                //添加到撤销管理器
                SSlotRevocationInfo tmpSlotRevocation;
                tmpSlotRevocation.nSlotIndex= SPECIAL_SLOT_INDEX;
                tmpSlotRevocation.nGoodID   = pMobaGoods->GetGoodID();
                tmpSlotRevocation.nCount    = pMobaGoods->GetOverLayCount();
                tmpSlotRevocation.GoodType  = pMobaGoods->GetGoodsType();

                tmpRevocationInfo.addGoodsList.push_back(tmpSlotRevocation);
                bHaveAddSuccess = true;
            }
		}
		else
		{
			//新槽位添加新物品
			for (int n = 0; n < MAX_GOODS_SLOT_COUNT; ++n)
			{
                if (SPECIAL_SLOT_INDEX == n)
                {
                    continue;
                }

                IMobaGoods* &pMobaGoods = m_MobaSlotGoods[n];
				if (pMobaGoods != NULL)
				{
                    continue;
                }


                pMobaGoods = createMobaGoods(nGoodType);
				if(!pMobaGoods)
					continue;
				pMobaGoods->Initial(m_pMaster, n, nGoodID);

				//添加到撤销管理器
				SSlotRevocationInfo tmpSlotRevocation;
				tmpSlotRevocation.nSlotIndex= n;
				tmpSlotRevocation.nGoodID   = pMobaGoods->GetGoodID();
				tmpSlotRevocation.nCount    = pMobaGoods->GetOverLayCount();
				tmpSlotRevocation.GoodType  = pMobaGoods->GetGoodsType();

				tmpRevocationInfo.addGoodsList.push_back(tmpSlotRevocation);

                bHaveAddSuccess = true;

                nAddSlotIndex = n;
                nAddSlotItemCount = 1;

				break;
			}

			// 如果槽位不够则判定是否为自动使用物品(不能撤销)
			if (bHaveAddSuccess == false && eAutoUseType == GAT_SlotNotEnoughAutoUse)
			{
				IMobaGoods* pMobaGood = createMobaGoods(nGoodType);
				if(pMobaGood)
				{
					pMobaGood->Initial(m_pMaster, GOODS_AUTOUSE_SLOT, nGoodID);
					if (pMobaGood->CanUse())
					{
						pMobaGood->Use();
						pMobaGood->Release();
                        m_MobaSlotGoods[GOODS_AUTOUSE_SLOT] = NULL;
						bHaveAddSuccess = true;
                        isAutoUsed = true;
					}
				}
			}
		}
	} 

    // 添加成功
	if (bHaveAddSuccess == true)
	{
		//消耗天赋点
        if (!bSpecialEffectAdd)
        {
            ((__IPlayerRole*)m_pMaster)->updateTalentPoint(-nCost, ENUM_TALENT_BUY);
        }

        if (!isAutoUsed && !bSpecialEffectAdd)
        {
            //添加到撤销管理器
            m_MobaGoodsRevocationManager.push(tmpRevocationInfo);
            SetCanCancelOperation(true);
        }
        else
        {
            // 使用物品则清空撤销管理器
            while (!m_MobaGoodsRevocationManager.empty())
            {
                m_MobaGoodsRevocationManager.pop();
            }
            SetCanCancelOperation(false);
        }

		BroadCastGoodsList();

        // 发出事件
        event_buy_item event;
        event.nItemID = nGoodID;
        event.nItemType = nGoodType;
        m_pMaster->getEntityEvent()->FireExecute(EVENT_BUY_ITEM, (LPCSTR)&event, sizeof(event));
	
		// 购买装备
		if(m_pEquipSchemeMgr != NULL && m_pEquipSchemeMgr->isSaveOftenUseEquip())
			m_pEquipSchemeMgr->addEquip(nGoodID);
        
        if(nAddSlotIndex > 0)
        {
            event_item_in_slot_change event;
            event.nSlotIndex = nAddSlotIndex;
            event.nItemID = nGoodID;
            event.nItemCount = nAddSlotItemCount;

            m_pMaster->getEntityEvent()->FireExecute(EVENT_ITEM_IN_SLOT_CHANGE, (LPCSTR)&event, sizeof(event));
        }
    }
	
	return bHaveAddSuccess;
}

bool MobaGoodsPart::CanUseGood(int nSlotIndex)
{
	// 槽位判定
	if (nSlotIndex<0 || nSlotIndex >= MAX_GOODS_SLOT_COUNT)
	{
		return false;
	}
	
	// 物品是否存在
	if (m_MobaSlotGoods[nSlotIndex] == NULL)
	{
		return false;
	}
	
	// 人物状态
	if (m_pMaster == NULL || m_pMaster->getState() == ENTITY_STATE_DIE)
	{
		return false;
	}

	//物品自身使用逻辑判定
	return m_MobaSlotGoods[nSlotIndex]->CanUse();
}

bool MobaGoodsPart::OnClientUseGood(int nSoltIndex)
{
	bool bNeedDelete = false;
	if (!CanUseGood(nSoltIndex))
	{
		return false;
	}
	else
	{
		// 找到槽位物品并使用物品
		if (m_MobaSlotGoods[nSoltIndex] != NULL)
		{
            int nItemID = m_MobaSlotGoods[nSoltIndex]->GetGoodID(); 
			bNeedDelete =  m_MobaSlotGoods[nSoltIndex]->Use();
            int nLeftCount = m_MobaSlotGoods[nSoltIndex]->GetOverLayCount(); 

            // 发出物品栏改变事件
            event_item_in_slot_change eventSlotChange;
            eventSlotChange.nSlotIndex = nSoltIndex;
            eventSlotChange.nItemID = nItemID;
            eventSlotChange.nItemCount = nLeftCount; 
            m_pMaster->getEntityEvent()->FireExecute(EVENT_ITEM_IN_SLOT_CHANGE, (LPCSTR)&eventSlotChange, sizeof(eventSlotChange));

            // 发出使用物品事件
            event_use_item eventUseItem;
            eventUseItem.nItemID = nItemID;
            m_pMaster->getEntityEvent()->FireExecute(EVENT_USE_ITEM, (LPCSTR)&eventUseItem, sizeof(eventUseItem));
		}

		if (bNeedDelete == true)
		{
			m_MobaSlotGoods[nSoltIndex]->Release();
			m_MobaSlotGoods[nSoltIndex] = NULL;
		}

		// 使用物品则清空撤销管理器
		while (!m_MobaGoodsRevocationManager.empty())
		{
			m_MobaGoodsRevocationManager.pop();
		}
		SetCanCancelOperation(false);
		BroadCastGoodsList();
	}
	return true;
}


bool MobaGoodsPart::AddEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType)
{
	bool bAddSucesse = true;
    UID MasterUID = m_pMaster->getUID();
	T_MAP_PROP_MANAGER::iterator iter = m_MobaGoodsPropManager.find(nEffectType);
	if (iter == m_MobaGoodsPropManager.end())
	{
		IMobaGoodsProp *pMobaGoodsProp = NULL;
		//新创建各种属性类管理器 1：基础 2、ID唯一 3、组唯一
		switch (nEffectType)
		{
		case MGET_BaseEffect:
			{
				pMobaGoodsProp = new CMobaGoodsBaseProp;
			}
			break;
		case MGET_IDUniqueEffect:
			{
				pMobaGoodsProp = new CMobaGoodsIDUniqueProp;
			}
			break;
		case MGET_GroupUniqueEffect:
			{
				pMobaGoodsProp = new CMobaGoodsGroupUniqueProp;
			}
			break;
		default:
			break;
		}

		if ( pMobaGoodsProp == NULL)
		{
			return false;
		}
		// 通过属性集合添加效果
		m_MobaGoodsPropManager[nEffectType] = pMobaGoodsProp;
        
		bAddSucesse = pMobaGoodsProp->AddEffect(nSlotID, nEffectID, MasterUID,MasterUID);
	}
	else
	{
		bAddSucesse = iter->second->AddEffect(nSlotID,nEffectID, MasterUID, MasterUID);
	}
	return bAddSucesse;
}

bool MobaGoodsPart::RemoveEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType)
{

	T_MAP_PROP_MANAGER::iterator iter = m_MobaGoodsPropManager.find(nEffectType);
	if (iter == m_MobaGoodsPropManager.end())
	{
		ErrorLn(__FUNCTION__": -- nSlotID ="<<nSlotID<<" nEffectID ="<<nEffectID<<" nEffectType" << nEffectType);
		return false;
	}

	return iter->second->RemoveEffect(nSlotID, nEffectID);
}


bool MobaGoodsPart::AddMonsterInheritEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType)
{
    bool bAddSucesse = true;
    UID OperatorUID = m_pMaster->getUID();
    map<UID, T_MAP_PROP_MANAGER *>::iterator iterAllMonster = m_mapReplicationMonster.begin();
    for (;iterAllMonster != m_mapReplicationMonster.end(); ++iterAllMonster)
    {
        T_MAP_PROP_MANAGER* pMonsterPropMgr = iterAllMonster->second;
        if (pMonsterPropMgr != NULL)
        {
           
            T_MAP_PROP_MANAGER::iterator iter = pMonsterPropMgr->find(nEffectType);
            if (iter == pMonsterPropMgr->end())
            {
                IMobaGoodsProp *pMobaGoodsProp = NULL;
                //新创建各种属性类管理器 1：基础 2、ID唯一 3、组唯一
                switch (nEffectType)
                {
                case MGET_BaseEffect:
                    {
                        pMobaGoodsProp = new CMobaGoodsBaseProp;
                    }
                    break;
                case MGET_IDUniqueEffect:
                    {
                        pMobaGoodsProp = new CMobaGoodsIDUniqueProp;
                    }
                    break;
                case MGET_GroupUniqueEffect:
                    {
                        pMobaGoodsProp = new CMobaGoodsGroupUniqueProp;
                    }
                    break;
                default:
                    break;
                }

                if ( pMobaGoodsProp == NULL)
                {
                    ErrorLn(__FUNCTION__": -- pMobaGoodsProp == NULL emMobaGoodEffectType ="<<nEffectType<<" nEffectID="<<nEffectID)
                    return false;
                }

                // 通过属性集合添加效果
                pMonsterPropMgr->insert(pair<emMobaGoodEffectType, IMobaGoodsProp*>(nEffectType, pMobaGoodsProp));
                bAddSucesse = pMobaGoodsProp->AddEffect(nSlotID, nEffectID, OperatorUID, iterAllMonster->first);
            }
            else
            {
                bAddSucesse = iter->second->AddEffect(nSlotID, nEffectID, OperatorUID, iterAllMonster->first);
            }
        }
    }

    if (!bAddSucesse)
    {
        WarningLn(__FUNCTION__": add effect failed! slotid ="<< nSlotID<<"nEffectID ="<<nEffectID);
    }
    return bAddSucesse;
}

bool MobaGoodsPart::RemoveMonsterInheritEffect(int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType)
{
    map<UID, T_MAP_PROP_MANAGER *>::iterator iterAllMonster = m_mapReplicationMonster.begin();
    for (;iterAllMonster != m_mapReplicationMonster.end(); ++iterAllMonster)
    {
        T_MAP_PROP_MANAGER* pPropMgr = iterAllMonster->second;
        T_MAP_PROP_MANAGER::iterator iter = pPropMgr->find(nEffectType);
        if (iter == pPropMgr->end())
        {
            ErrorLn(__FUNCTION__": -- nSlotID ="<<nSlotID<<" nEffectID ="<<nEffectID<<" nEffectType" << nEffectType);
            return false;
        }

        return iter->second->RemoveEffect(nSlotID, nEffectID);
    }

    return false;
}

bool MobaGoodsPart::OnClientExChangeGood(int nSrcSlot, int nTargetSolt)
{
	IMobaGoods* pMobaGoods =NULL;

	if (nSrcSlot == SPECIAL_SLOT_INDEX || nTargetSolt == SPECIAL_SLOT_INDEX)
	{
		return false;
	}

	if (nSrcSlot < 0 || nSrcSlot >= MAX_GOODS_SLOT_COUNT)
	{
		return false;
	}

	if (nTargetSolt < 0 || nTargetSolt >= MAX_GOODS_SLOT_COUNT)
	{
		return false;
	}

	//交换两个槽位的物品
	pMobaGoods = m_MobaSlotGoods[nSrcSlot];
	m_MobaSlotGoods[nSrcSlot] = m_MobaSlotGoods[nTargetSolt];
	m_MobaSlotGoods[nTargetSolt] = pMobaGoods;

    //记录到撤销管理器
    SRevocationInfo tmpRevocationInfo;
    tmpRevocationInfo.RevocationType = EWDT_ExChange;

    // 添加进撤销管理器
    tmpRevocationInfo.ExchangeInfo.srcIndex = nSrcSlot;
    tmpRevocationInfo.ExchangeInfo.destIndex = nTargetSolt;

    //添加到撤销管理器
    m_MobaGoodsRevocationManager.push(tmpRevocationInfo);

	
	//下发装备列表
	BroadCastGoodsList();
	return true;
}

bool MobaGoodsPart::broadCastDataToHaveSightPlayer(Vector3 vec, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
	if (m_pMaster == NULL )
	{
		return false;
	}
	if (pszData == NULL || nDataLen == 0)
	{
		return false;
	}


	SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(m_pMaster->getSceneID());
	if (pContainer == NULL)
	{
		return false;
	}
	obuf o;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule    = MSG_MODULEID_WAR_GOOD;
	head.byKeyAction    = byKeyAction;


	o << head;
	o.push_back(pszData, nDataLen);

	IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
	if (pScene != NULL)
	{
		pScene->broadcast( vec, 0, BROADCAST_SCENE, std::string(o.data(),o.size()), AOI_PROXY_PTR(0), false);
	}

	return true;
}

bool MobaGoodsPart::BroadCastGoodsList()
{

	__ISlotPart* pSlotPart = (__ISlotPart*)m_pMaster->getEntityPart(PART_SLOTLOGIC);
	if(pSlotPart == NULL)
	{
		return false;
	}

	msg_entity_fight_good_list tmpGoodList;
	tmpGoodList.nCount = 0;
	tmpGoodList.idActor =  m_pMaster->getIntProperty(PROPERTY_ID);

	//所有装备信息
    obuf256 oAllGoods;
	for (int i = 0; i< MAX_GOODS_SLOT_COUNT; ++i)
	{
		msg_entity_fight_good tmpGood;
		tmpGood.uUID = m_pMaster->getUID();
		tmpGood.nSlotIndex = i;
		tmpGood.nSlotType =SLOT_TYPE::SLOT_TYPE_GOODS;
		if (m_MobaSlotGoods[i] != NULL)
		{
            SSchemeMobaGoods* pScheme =  m_MobaSlotGoods[i]->GetSchemeInfo();
            if(pScheme == NULL)
            {
                ErrorLn("SSchemeMobaGoods == NULL, actor pdbid= " << tmpGoodList.idActor << ", goodid=" << m_MobaSlotGoods[i]->GetGoodID());
                continue;
            }

			tmpGood.nGoodID =m_MobaSlotGoods[i]->GetGoodID();
			tmpGood.nCanUseTimes = m_MobaSlotGoods[i]->GetCanUseTimes();
			tmpGood.bCanUse = pScheme->bCanUse;
			// buff或者技能冷却ID
			tmpGood.nFreezeID = pScheme->nUseBuffFreezeID;
			tmpGood.nBuffID = pScheme->nUseBuffID;
			tmpGood.nSkillID = pScheme->nUseSkillID;
			tmpGood.bIsNewAdd = m_MobaSlotGoods[i]->GetIsNewAddGoods();

			m_MobaSlotGoods[i]->SetIsNewAddGoods(false);
		}
		oAllGoods<<tmpGood;
        tmpGoodList.nCount++;
	}

    obuf sendBuf;
    sendBuf<<tmpGoodList;
    sendBuf.push_back(oAllGoods.data(),oAllGoods.size());
	// 设置到槽位数据
	pSlotPart->SetGoodsSlotList(sendBuf.data(),sendBuf.size());

	// 下发给有视野的人
	broadCastDataToHaveSightPlayer(m_pMaster->getLocation(), SC_MSG_WAR_GOODS_LIST,sendBuf.data(),sendBuf.size());

    SendGoodListToWar();

	return true;
}

bool MobaGoodsPart::OnClientSellGood(int nSlotIndex, int nGoodID)
{
	if (!m_bCanBuyOrSell)
	{
		showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_CANNOTSELL_STATE);
		return false;
	}
	//出售物品 校验
	if (m_MobaSlotGoods[nSlotIndex] == NULL)
	{
		return false;
	}

	if (m_MobaSlotGoods[nSlotIndex]->GetGoodID() != nGoodID)
	{
		return false;
	}

	// 获取出售价格
	int nSoldOutCost = m_MobaSlotGoods[nSlotIndex]->GetSchemeInfo()->nSoldOutCost;

	//是否有叠加数量
	if (m_MobaSlotGoods[nSlotIndex]->GetOverLayCount() > 1)
	{
		nSoldOutCost = m_MobaSlotGoods[nSlotIndex]->GetOverLayCount()*nSoldOutCost;
	}

	//记录到撤销管理器
	SRevocationInfo tmpRevocationInfo;
	tmpRevocationInfo.RevocationType = EWDT_Sell;
	tmpRevocationInfo.nTalentPointChange = nSoldOutCost;


	SSlotRevocationInfo tmpSlotRevocation;
	tmpSlotRevocation.nSlotIndex = nSlotIndex;
	tmpSlotRevocation.nGoodID = m_MobaSlotGoods[nSlotIndex]->GetGoodID();
	tmpSlotRevocation.nCount = m_MobaSlotGoods[nSlotIndex]->GetOverLayCount();
	tmpSlotRevocation.GoodType = m_MobaSlotGoods[nSlotIndex]->GetGoodsType();
	
	// 添加进撤销管理器
    tmpRevocationInfo.removeGoodsList.push_back(tmpSlotRevocation);

	//添加到撤销管理器
	m_MobaGoodsRevocationManager.push(tmpRevocationInfo);
	SetCanCancelOperation(true);

	// 移除相关属性、技能、buff
	m_MobaSlotGoods[nSlotIndex]->Remove();

	//出售
	m_MobaSlotGoods[nSlotIndex]->Release();
	m_MobaSlotGoods[nSlotIndex] = NULL;

	//重算属性
	RecomputeAllGoodsEffect();

	//给自己添加金币
	if (nSoldOutCost > 0)
	{
        ((__IPlayerRole*)m_pMaster)->updateTalentPoint(nSoldOutCost, ENUM_TALENT_SELL);
        //setAddTalentToMap(nSoldOutCost);
	}

	//下发装备列表
	BroadCastGoodsList();

    // 填充购买物品的日志信息
    fillBuyEquipLog((int)MGBT_SELL,nGoodID);

    // 物品栏改变事件(其实电脑不会卖物品)
    event_item_in_slot_change event;
    event.nSlotIndex = nSlotIndex;
    event.nItemID = nGoodID;
    event.nItemCount = 0; 
    m_pMaster->getEntityEvent()->FireExecute(EVENT_ITEM_IN_SLOT_CHANGE, (LPCSTR)&event, sizeof(event));

	return true;
}

void MobaGoodsPart::sellGoodsOriginalPrice()
{
    for (int nSlotIndex =0; nSlotIndex < MAX_GOODS_SLOT_COUNT; ++nSlotIndex)
    {
        //出售物品 校验
        if (m_MobaSlotGoods[nSlotIndex] == NULL)
        {
            continue;
        }

        // 获取出售价格
        int nGoodID = m_MobaSlotGoods[nSlotIndex]->GetSchemeInfo()->nGoodsID;
        int nSoldOutCost = m_MobaSlotGoods[nSlotIndex]->GetSchemeInfo()->nGoodCost;

        //是否有叠加数量
        if (m_MobaSlotGoods[nSlotIndex]->GetOverLayCount() > 1)
        {
            nSoldOutCost = m_MobaSlotGoods[nSlotIndex]->GetOverLayCount()*nSoldOutCost;
        }

        // 移除相关属性、技能、buff
        m_MobaSlotGoods[nSlotIndex]->Remove();

        //出售
        m_MobaSlotGoods[nSlotIndex]->Release();
        m_MobaSlotGoods[nSlotIndex] = NULL;

        //给自己添加金币
        if (nSoldOutCost > 0)
        {
            ((__IPlayerRole*)m_pMaster)->updateTalentPoint(nSoldOutCost, ENUM_TALENT_SELL);
        }

        // 填充购买物品的日志信息
        //fillBuyEquipLog((int)MGBT_SELL,nGoodID);

        // 物品栏改变事件(其实电脑不会卖物品)
        event_item_in_slot_change event;
        event.nSlotIndex = nSlotIndex;
        event.nItemID = nGoodID;
        event.nItemCount = 0; 
        m_pMaster->getEntityEvent()->FireExecute(EVENT_ITEM_IN_SLOT_CHANGE, (LPCSTR)&event, sizeof(event));
    }

    //下发装备列表
    BroadCastGoodsList();

    //重算属性
    RecomputeAllGoodsEffect();
}


bool MobaGoodsPart::HaveOtherSameGoods(int nGoodID, int nSlotIndex)
{
	if (nGoodID <= 0)
	{
		return false;
	}

	for (int i =0; i < MAX_GOODS_SLOT_COUNT; ++i)
	{
		if (m_MobaSlotGoods[i] != NULL && i != nSlotIndex && m_MobaSlotGoods[i]->GetGoodID() == nGoodID)
		{
			return true;
		}
	}

	return false;
}

void MobaGoodsPart::RecomputeAllGoodsEffect()
{
	for (int i =0; i < MAX_GOODS_SLOT_COUNT; ++i)
	{
		if (m_MobaSlotGoods[i] != NULL)
		{
			m_MobaSlotGoods[i]->RecomputeGoodsEffect();
		}
	}
}

void MobaGoodsPart::OnTimer( unsigned long dwTimerID )
{
    // 发送信息到客户端
    switch( dwTimerID )
    {
    case ETimerEventID_SaveEquipLog:
        {
            batchSaveBuyEquipLog();
        }
        break;

    }
}

bool MobaGoodsPart::OnClientRevocationOperation(UID uUID)
{
	if (uUID == INVALID_UID || m_pMaster->getUID() != uUID)
	{
		return false;
	}

	if (m_MobaGoodsRevocationManager.empty() == true)
	{
		SetCanCancelOperation(false);
		return false;
	}

    SRevocationInfo& revocationInfo = m_MobaGoodsRevocationManager.top();

	switch(revocationInfo.RevocationType)
	{
	case EWRT_Buy:
		{
			// 撤销购买物品
			RevocationBuyOperation(revocationInfo);
		}
		break;
	case EWDT_Sell:
		{
			// 撤销出售物品
			RevocationSellOperation(revocationInfo);
		}
		break;
    case EWDT_ExChange:
        {
            // 撤销出售物品
            RevocationExchangeOperation(revocationInfo);
        }
        break;

	default:
        // todo 如果不是买和卖，逻辑应当不用往下走了吧？
		break;
	}
    // 重算下没有启用的效果
    RecomputeAllGoodsEffect();

	//if (revocationInfo.pListAddGoodsList != NULL)
	//{
	//	delete revocationInfo.pListAddGoodsList;
	//	revocationInfo.pListAddGoodsList = NULL;
	//}

	//if (revocationInfo.pListRemoveGoodsList != NULL)
	//{
	//	delete revocationInfo.pListRemoveGoodsList;
	//	revocationInfo.pListRemoveGoodsList = NULL;
	//}

	//弹出
    //if(revocationInfo.pListRemoveGoodsList) { delete revocationInfo.pListRemoveGoodsList; revocationInfo.pListRemoveGoodsList = NULL;}
    //if(revocationInfo.pListAddGoodsList) { delete revocationInfo.pListAddGoodsList; revocationInfo.pListAddGoodsList = NULL;}
	m_MobaGoodsRevocationManager.pop();

    if (m_MobaGoodsRevocationManager.empty())
    {
        SetCanCancelOperation(false);
    }
	
	return true;
}

bool MobaGoodsPart::RevocationBuyOperation(SRevocationInfo &RevocationInfo)
{
	//如果是叠加增加方式 则还原到叠加数量之前
	if (RevocationInfo.bisAddOverlayCount == true)
	{
		if (!RevocationInfo.addGoodsList.empty())
		{
			list<SSlotRevocationInfo>::iterator iter =  RevocationInfo.addGoodsList.begin();
			for (;iter!= RevocationInfo.addGoodsList.end(); ++iter)
			{
				// 对应完成之后能撤销
				if (m_MobaSlotGoods[iter->nSlotIndex] != NULL && m_MobaSlotGoods[iter->nSlotIndex]->GetGoodID() == iter->nGoodID)
				{
					m_MobaSlotGoods[iter->nSlotIndex]->SetOverLayCount(iter->nCount);
				}

                fillBuyEquipLog((int)MGBT_REVOCATION_BUY,iter->nGoodID);
			}
		}
	}
	else
	{
		// 先移除购买过程中添加的物品
		if (!RevocationInfo.addGoodsList.empty())
		{
			list<SSlotRevocationInfo>::iterator iterAddGoods =  RevocationInfo.addGoodsList.begin();
			for (;iterAddGoods!= RevocationInfo.addGoodsList.end(); ++iterAddGoods)
			{
                IMobaGoods* & pMobaGoods = m_MobaSlotGoods[iterAddGoods->nSlotIndex];
				if (pMobaGoods != NULL )
				{
					// 移除相关属性、技能、buff
					pMobaGoods->Remove();
					pMobaGoods->Release();
					pMobaGoods = NULL;
				}
			}
		}
		

		if (!RevocationInfo.removeGoodsList.empty())
		{
			// 添加购买过程中移除的物品
			list<SSlotRevocationInfo>::iterator iterRomve = RevocationInfo.removeGoodsList.begin();
			for (;iterRomve != RevocationInfo.removeGoodsList.end(); ++iterRomve)
			{
                IMobaGoods* & pMobaGoods = m_MobaSlotGoods[iterRomve->nSlotIndex];
				if (pMobaGoods != NULL )
				{
					ErrorLn(__FUNCTION__":pMobaGoods != NULL, mapRemoveGoodsList slot have Goods");
					return false;
				}

                pMobaGoods = createMobaGoods(iterRomve->GoodType);
                if (!pMobaGoods)
                {
                    continue;
                }
				//初始化被移除的物品
				pMobaGoods->Initial(m_pMaster, iterRomve->nSlotIndex, iterRomve->nGoodID);
				pMobaGoods->SetOverLayCount(iterRomve->nCount);
			}
		}
		
		//重算属性
		RecomputeAllGoodsEffect();
	}

	// 将消耗天赋点返还
    ((__IPlayerRole*)m_pMaster)->updateTalentPoint(RevocationInfo.nTalentPointChange, ENUM_TALENT_REVOKE);

	BroadCastGoodsList();

	// 撤销装备

	if (m_pEquipSchemeMgr != NULL && m_pEquipSchemeMgr->isSaveOftenUseEquip())
			m_pEquipSchemeMgr->revocationEquip();

	return true;	
}

bool MobaGoodsPart::RevocationSellOperation(SRevocationInfo &RevocationInfo)
{

	// 添加出售过程中移除的物品
	if (!RevocationInfo.removeGoodsList.empty())
	{
		list<SSlotRevocationInfo>::iterator iter =  RevocationInfo.removeGoodsList.begin();
		for (;iter!= RevocationInfo.removeGoodsList.end(); ++iter)
		{
			if (m_MobaSlotGoods[iter->nSlotIndex] != NULL )
			{
				ErrorLn("MobaGoodsPart::RevocationBuyOperation() -- mapRemoveGoodsList slot have Goods");
				return false;
			}

            m_MobaSlotGoods[iter->nSlotIndex] = createMobaGoods(iter->GoodType);
            if (m_MobaSlotGoods[iter->nSlotIndex] == NULL )
            {
                continue;
            }
			m_MobaSlotGoods[iter->nSlotIndex]->Initial(m_pMaster, iter->nSlotIndex, iter->nGoodID);
			m_MobaSlotGoods[iter->nSlotIndex]->SetOverLayCount(iter->nCount);

            fillBuyEquipLog((int)MGBT_REVOCATION_SELL,iter->nGoodID);
		}
	}
	

	//重算属性
	RecomputeAllGoodsEffect();

	// 扣除天赋点
    ((__IPlayerRole*)m_pMaster)->updateTalentPoint(-(RevocationInfo.nTalentPointChange), ENUM_TALENT_SELL_REVOKE);

	BroadCastGoodsList();
	return true;
}

bool MobaGoodsPart::RevocationExchangeOperation(SRevocationInfo &RevocationInfo)
{
    if(RevocationInfo.ExchangeInfo.destIndex < 0 || RevocationInfo.ExchangeInfo.destIndex >= MAX_GOODS_SLOT_COUNT )
    {
        return false;
    }

    if(RevocationInfo.ExchangeInfo.srcIndex < 0 || RevocationInfo.ExchangeInfo.srcIndex  >= MAX_GOODS_SLOT_COUNT )
    {
        return false;
    }
    // 还原物品的位置
    IMobaGoods* pMobaGoods = m_MobaSlotGoods[RevocationInfo.ExchangeInfo.destIndex];
    m_MobaSlotGoods[RevocationInfo.ExchangeInfo.destIndex] = m_MobaSlotGoods[RevocationInfo.ExchangeInfo.srcIndex];
    m_MobaSlotGoods[RevocationInfo.ExchangeInfo.srcIndex] = pMobaGoods;

    BroadCastGoodsList();
    return true;
}



void MobaGoodsPart::SendGoodsListToSelf(BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
	SendEntityMessage(m_pMaster->getUID(), byKeyAction, getPartID(),pszData, nDataLen);
}

void MobaGoodsPart::SendEntityMessage(UID uid, BYTE byKeyAction, BYTE byPartID, LPCSTR pszData, int nDataLen)
{
	obuf obufData;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule	= MSG_MODULEID_ENTITY;
	head.byKeyAction	= byKeyAction;


	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = uid;
	entityHead.byPartID = byPartID;

	obufData << head << entityHead;
	obufData.push_back(pszData,nDataLen);

	((__IPlayerRole*)m_pMaster)->send2Client(obufData.data(), obufData.size());
}

bool MobaGoodsPart::CanUseSkill(SSlotSkillInfo *SlotSkillInfo)
{
	if (SlotSkillInfo == NULL)
	{
		return false;
	}

	if (SlotSkillInfo->nSlotIndex >= MAX_GOODS_SLOT_COUNT || SlotSkillInfo->nSlotIndex < 0)
	{
		return false;
	}

	if (SlotSkillInfo->nSkillID <= 0 )
	{
		return false;
	}

	if (m_MobaSlotGoods[SlotSkillInfo->nSlotIndex] == NULL &&m_MobaSlotGoods[SlotSkillInfo->nSlotIndex]->GetSkillID() !=  SlotSkillInfo->nSkillID)
	{
		return false;
	}


	if (!m_MobaSlotGoods[SlotSkillInfo->nSlotIndex]->CanUse())
	{
		return false;
	}

	return true;
}

bool MobaGoodsPart::UseSkill(SSlotSkillInfo *SlotSkillInfo)
{
	// 目前适用于
	if (SlotSkillInfo == NULL)
	{
		return false;
	}

	if (SlotSkillInfo->nSlotIndex >= MAX_GOODS_SLOT_COUNT || SlotSkillInfo->nSlotIndex < 0)
	{
		return false;
	}

	if (SlotSkillInfo->nSkillID <= 0 )
	{
		return false;
	}

	if (m_MobaSlotGoods[SlotSkillInfo->nSlotIndex] == NULL &&m_MobaSlotGoods[SlotSkillInfo->nSlotIndex]->GetSkillID() !=  SlotSkillInfo->nSkillID)
	{
		return false;
	}

	bool bNeedDelete = false;
	// 找到槽位物品并使用物品
	bNeedDelete =  m_MobaSlotGoods[SlotSkillInfo->nSlotIndex]->Use();

	if (bNeedDelete == true)
	{
		m_MobaSlotGoods[SlotSkillInfo->nSlotIndex]->Release();
		m_MobaSlotGoods[SlotSkillInfo->nSlotIndex] = NULL;
	}

	// 使用物品则清空撤销管理器
	while (!m_MobaGoodsRevocationManager.empty())
	{
		m_MobaGoodsRevocationManager.pop();
	}
	SetCanCancelOperation(false);
	BroadCastGoodsList();
	return true;
}

/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  如果返回false : 则中断执行，否则继续向下执行
	@note     
	@warning 
	@retval buffer 
	*/
bool MobaGoodsPart::OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_CAST_SPELL:
		{
			if (pszContext == NULL || nLen != sizeof(event_cast_spell))
			{
				return false;
			}

			event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;
			if (pCastSpell->nSlotType == SLOT_TYPE::SLOT_TYPE_GOODS)
			{
                if (pCastSpell->nSlotIndex >= MAX_GOODS_SLOT_COUNT || pCastSpell->nSlotIndex < 0)
                {
                    WarningLn(__FUNCTION__": -- spellID="<<pCastSpell->nID<<" slotType="<<pCastSpell->nSlotType<<" slotindex="<<pCastSpell->nSlotIndex);
                    return false;
                }

                IMobaGoods* pMobaGoods = m_MobaSlotGoods[pCastSpell->nSlotIndex];
                if ( pMobaGoods == NULL)
                {
                    WarningLn(__FUNCTION__": -- spellID="<<pCastSpell->nID<<" slotType="<<pCastSpell->nSlotType<<" slotindex="<<pCastSpell->nSlotIndex);
                    return false;
                }

                bool bCanUse = pMobaGoods->CanUse();
                if(!bCanUse)
                {
                    //todo 无法使用的原因
					if(pMobaGoods->bIsMultipleCDGoods())
							pCastSpell->nFailResult = (int)CHAT_TIP_WAR_GOODS_FREEZE;
					else
							pCastSpell->nFailResult = (int)CHAT_TIP_ENTITY_SPELL_CAST_FAIL_CHARGED_NOTENOUGH;
							
                }
                return bCanUse;
			}
		}
		break;
	default:
		break;
	}

	return true;
}

void MobaGoodsPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_CAST_SPELL:		// 施法技能事件
		{
			if (m_pMaster == NULL)
			{
				return;
			}

			if (pszContext == NULL || nLen != sizeof(event_cast_spell))
			{
				return;
			}

			event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

			if (pCastSpell->nSlotType == SLOT_TYPE::SLOT_TYPE_GOODS)
			{
				if (m_MobaSlotGoods[pCastSpell->nSlotIndex] != NULL)
				{
					m_MobaSlotGoods[pCastSpell->nSlotIndex]->Use();
				}
			}
			else
			{
				RecordUseXp(pCastSpell->nSpellEffectType);
			}
		}
		break;

    case EVENT_ENTITY_CREATE:
        {
            // 物品在新场景重新启用相关处理
            for (int i = 0; i < MAX_GOODS_SLOT_COUNT; ++i)
            {
                if (m_MobaSlotGoods[i] != NULL )
                {
                    // 移除相关属性、技能、buff
                    m_MobaSlotGoods[i]->onLoad(m_pMaster, m_nLoginMode);
                }
            }
        }
        break;
   //  case EVENT_GAME_ENTITY_REPLICATION_MONSTER:		// 分身怪物事件
   //     {
   //         // 注册实体创建成功事件
   //         if (m_pMaster == NULL)
   //         {
   //             return;
   //         }

   //         if (pszContext == NULL || nLen != sizeof(event_entity_replication_monster))
   //         {
   //             return;
   //         }

   //         event_entity_replication_monster *pEventData = (event_entity_replication_monster *)pszContext;

   //         if(pEventData->uidMaster != m_pMaster->getUID())
   //         {
   //             // 不是创建者
   //             return;
   //         }

   //         if (pEventData->bAdd)
   //         {
   //             AddInheritMonster(pEventData->uidMonster);
   //         }
   //     }
   //     break;

   //    case EVENT_GAME_ENTITY_DIE:		// 分身怪物事件
   //     {
   //         // 注册实体创建成功事件
   //         if (m_pMaster == NULL)
   //         {
   //             return;
   //         }

   //         if (pszContext == NULL || nLen != sizeof(event_entity_die))
			//{
			//	return;
			//}

			//event_entity_die *pDie = (event_entity_die *)pszContext;
   //         RemoveInheritMonster(pDie->uidTarget);
   //     }
   //     break;
        

	default:
		break;
	}
}


void MobaGoodsPart::RemoveInheritMonster(UID uid)
{
    //DWORD dwSceneID = UID_2_SCENE(uid);
    //SceneLogicEventHelper eventHelper(dwSceneID);
    //IEventEngine *pGlobalEvent = eventHelper.m_ptr;
    //if (pGlobalEvent != NULL)
    //{
    //    pGlobalEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, UID_2_TYPE(uid), uid);
    //}

    map<UID, T_MAP_PROP_MANAGER *>::iterator iter = m_mapReplicationMonster.find(uid);
    if (iter == m_mapReplicationMonster.end())
    {
        return;
    }

    if (iter->second != NULL)
    {
        T_MAP_PROP_MANAGER *pMonsterPropMgr = iter->second;
        T_MAP_PROP_MANAGER::iterator iterPropMgr = pMonsterPropMgr->begin();
        for (;iterPropMgr != pMonsterPropMgr->end(); ++iterPropMgr)
        {
            if (iterPropMgr->second != NULL)
            {
                iterPropMgr->second->Release();
                iterPropMgr->second = NULL;
            }

        }
        pMonsterPropMgr->clear();
        delete pMonsterPropMgr;
        pMonsterPropMgr = NULL;
    }

    m_mapReplicationMonster.erase(uid);
}

void MobaGoodsPart::AddInheritMonster(UID uid)
{
    if(uid == INVALID_UID)
    {
        return;
    }

    if (m_mapReplicationMonster.find(uid) != m_mapReplicationMonster.end())
    {
        WarningLn(__FUNCTION__": uid have exist! uid ="<<uid);
        return;
    }

    T_MAP_PROP_MANAGER *pPropManager = new T_MAP_PROP_MANAGER();
    m_mapReplicationMonster.insert(pair<UID, T_MAP_PROP_MANAGER *>(uid, pPropManager));

    for (int i = 0; i< MAX_GOODS_SLOT_COUNT; ++i)
    {
        if (m_MobaSlotGoods[i] != NULL)
        {
            m_MobaSlotGoods[i]->addCreateMonsterInheritEffect(uid);
        }
    }

    //DWORD dwSceneID = UID_2_SCENE(uid);
    //SceneLogicEventHelper eventHelper(dwSceneID);
    //IEventEngine *pGlobalEvent = eventHelper.m_ptr;
    //if (pGlobalEvent != NULL)
    //{
    //    // 注册拖动者死亡事件
    //    pGlobalEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, UID_2_TYPE(uid), uid, __FUNCTION__);
    //}
}


bool MobaGoodsPart::AddCreateMonsterInheritEffect(UID uid, int nSlotID, int nEffectID, emMobaGoodEffectType nEffectType)
{
    bool bAddSucesse = false;
    UID OperatorUID = m_pMaster->getUID();
    map<UID, T_MAP_PROP_MANAGER *>::iterator iterFindMonster = m_mapReplicationMonster.find(uid);
    if (iterFindMonster == m_mapReplicationMonster.end())
    {
        ErrorLn(__FUNCTION__": cant find uid ="<<uid);
        return false;
    }
    
    T_MAP_PROP_MANAGER* pMonsterPropMgr = iterFindMonster->second;
    if (pMonsterPropMgr != NULL)
    {

        T_MAP_PROP_MANAGER::iterator iter = pMonsterPropMgr->find(nEffectType);
        if (iter == pMonsterPropMgr->end())
        {
            IMobaGoodsProp *pMobaGoodsProp = NULL;
            //新创建各种属性类管理器 1：基础 2、ID唯一 3、组唯一
            switch (nEffectType)
            {
            case MGET_BaseEffect:
                {
                    pMobaGoodsProp = new CMobaGoodsBaseProp;
                }
                break;
            case MGET_IDUniqueEffect:
                {
                    pMobaGoodsProp = new CMobaGoodsIDUniqueProp;
                }
                break;
            case MGET_GroupUniqueEffect:
                {
                    pMobaGoodsProp = new CMobaGoodsGroupUniqueProp;
                }
                break;
            default:
                break;
            }

            if ( pMobaGoodsProp == NULL)
            {
                ErrorLn(__FUNCTION__": -- pMobaGoodsProp == NULL emMobaGoodEffectType ="<<nEffectType<<" nEffectID="<<nEffectID)
                return false;
            }

            // 通过属性集合添加效果
            pMonsterPropMgr->insert(pair<emMobaGoodEffectType, IMobaGoodsProp*>(nEffectType, pMobaGoodsProp));

            bAddSucesse = pMobaGoodsProp->AddEffect(nSlotID, nEffectID, OperatorUID, uid);
        }
        else
        {
            bAddSucesse = iter->second->AddEffect(nSlotID, nEffectID, OperatorUID, uid);
        }
    }

    if (!bAddSucesse)
    {
        WarningLn(__FUNCTION__": add effect failed! uid ="<< uid<<"slotid ="<< nSlotID<<"nEffectID ="<<nEffectID);
    }

    return bAddSucesse;
}


void MobaGoodsPart::SetCanBuyOrSell(bool isCanBuyOrSell)
{
    if (m_pMaster == NULL)
    {
        ErrorLn(__FUNCTION__": failed m_pMaster == NULL current m_bCanBuyOrSell ="<<m_bCanBuyOrSell<<" isCanBuyOrSell ="<<isCanBuyOrSell);
        return;
    }
	if (m_bCanBuyOrSell != isCanBuyOrSell)
	{
		m_bCanBuyOrSell = isCanBuyOrSell;

        // 状态切换将不能撤销物品，将撤销管理器情况
        if ( isCanBuyOrSell == false )
        {
            while (!m_MobaGoodsRevocationManager.empty())
            {
                m_MobaGoodsRevocationManager.pop();
            }
            SetCanCancelOperation(false);
        }

		// 设置能否购买状态
		msg_entity_can_buy_or_sell sMsgContext;
		sMsgContext.bCanOrSell = m_bCanBuyOrSell;

		//下发到客户端
		SendEntityMessage(m_pMaster->getUID(), SC_MSG_WAR_CAN_BUY_OR_SELL, getPartID(),(char *)&sMsgContext, sizeof(sMsgContext));
	}
}

void MobaGoodsPart::SetCanCancelOperation(bool isCanCancelOperation)
{
	if (m_bCanCancelOperation != isCanCancelOperation)
	{
		m_bCanCancelOperation = isCanCancelOperation;

		// 设置能否购买状态
		msg_entity_can_cancel_opration sMsgContext;
		sMsgContext.bCanCancelOpration = isCanCancelOperation;

		//下发到客户端
		SendEntityMessage(m_pMaster->getUID(), SC_MSG_WAR_CAN_CANCEL_OPREATION, getPartID(),(char *)&sMsgContext, sizeof(sMsgContext));
	}
}

void MobaGoodsPart::ResetCanUseCount()
{
	//数据改变标志位
	bool nCountIsChange=false;
	for (int i = 0; i< MAX_GOODS_SLOT_COUNT; ++i)
	{
		if (m_MobaSlotGoods[i] != NULL)
		{
			if(m_MobaSlotGoods[i]->ResetCanUseCount())
			{
				nCountIsChange=true;
			}
		}
	}
	// 数据没有改变直接返回，不下发客户端
	if(!nCountIsChange)
	{
		return;
	}
	BroadCastGoodsList();
}
IMobaGoods* MobaGoodsPart::getMobaGoods(SLOT_TYPE nSlotType,int nSlotIndex)
{
	if(nSlotType!=SLOT_TYPE_GOODS||nSlotIndex < 0||nSlotIndex >= MAX_GOODS_SLOT_COUNT)
	{
		return NULL;
	}
	if(m_MobaSlotGoods[nSlotIndex]==NULL)
	{
		ErrorLn("MobaGoodsPart::getMobaGoods() -- IMobaGoods == NULL");
		return NULL;
	}
	return m_MobaSlotGoods[nSlotIndex];
}


void MobaGoodsPart::UpdateGoodSlotInfo(SLOT_TYPE nSlotType, int nSlotIndex)
{
    if (nSlotIndex >= MAX_GOODS_SLOT_COUNT || nSlotIndex < 0)
    {
        return;
    }

    if (m_MobaSlotGoods[nSlotIndex] == NULL)
    {
        return;
    }
    __ISlotPart* pSlotPart = (__ISlotPart*)m_pMaster->getEntityPart(PART_SLOTLOGIC);
    if(pSlotPart == NULL)
    {
        return;
    }

    //更新设置信息
    msg_entity_fight_good tmpGood;
    tmpGood.uUID = m_pMaster->getUID();
    tmpGood.nSlotIndex = nSlotIndex;
    tmpGood.nSlotType =nSlotType;

    SSchemeMobaGoods* pSchemeInfo =  m_MobaSlotGoods[nSlotIndex]->GetSchemeInfo();
    tmpGood.nGoodID =m_MobaSlotGoods[nSlotIndex]->GetGoodID();
    tmpGood.nCanUseTimes = m_MobaSlotGoods[nSlotIndex]->GetCanUseTimes();
    tmpGood.bIsNewAdd = m_MobaSlotGoods[nSlotIndex]->GetIsNewAddGoods();
    tmpGood.bCanUse = pSchemeInfo->bCanUse;
    // buff或者技能冷却ID
    tmpGood.nFreezeID = pSchemeInfo->nUseBuffFreezeID;
    tmpGood.nBuffID = pSchemeInfo->nUseBuffID;
    tmpGood.nSkillID = pSchemeInfo->nUseSkillID;

    obuf256 oAllGoods;
    msg_entity_fight_good_list tmpGoodList;
    tmpGoodList.nCount = 1;
    tmpGoodList.idActor =  m_pMaster->getIntProperty(PROPERTY_ID);
    oAllGoods<<tmpGoodList<<tmpGood;

    // 设置到槽位数据
    pSlotPart->SetGoodsSlotList(oAllGoods.data(),oAllGoods.size());
}

void MobaGoodsPart::fillBuyEquipLog( int nBuyType, int nGoodsId)
{
    DBREQ_PARAM_WAR_BUYEQUIP_DETAIL buyDetail;
    buyDetail.byBuyType = nBuyType;
    buyDetail.dwEquipID = nGoodsId;
    buyDetail.dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
    buyDetail.dwOperateTime = time(NULL);
    m_vecBugEquipLog.push_back(buyDetail);
}

void MobaGoodsPart::setWarInfo()
{
    __IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
    if(pWarPart == NULL)
    {
        return;
    }

    int nWarID = pWarPart->getWarID();

    SERVICE_PTR pWarContainer = gServerGlobal->getWarManager()->GetWar(nWarID);
    if(pWarContainer == 0)
    {
        return;
    }
    IWarService* pWarService = (IWarService*)pWarContainer->query_interface();
    if(pWarService == NULL)
    {
        return;
    }

    m_WarDBKey = pWarService->getWarDBKey();
	m_nMatchTypeID = pWarService->getMatchTypeID();
}

bool MobaGoodsPart::batchSaveBuyEquipLog()
{
    if (m_pMaster==NULL || m_WarDBKey == 0)
    {
        return false;
    }

    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn("BatchSavePirateLog() buy equip log failed pDBEngine == NULL");
        return false;
    }

    int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

    obuf128 dbbuf;
    obuf dbdata;
    DBREQ_PARAM_WAR_BUYEQUIP_SAVE log;
    log.nWarID = m_WarDBKey;
    log.nCount = 0;

    size_t node_len = sizeof( DBREQ_PARAM_WAR_BUYEQUIP_DETAIL );

    vector<DBREQ_PARAM_WAR_BUYEQUIP_DETAIL>::iterator itEnd = m_vecBugEquipLog.end();
    for (vector<DBREQ_PARAM_WAR_BUYEQUIP_DETAIL>::iterator iter = m_vecBugEquipLog.begin(); iter != itEnd; iter++)
    {

        if ((log.nCount + 1) * node_len >  7000)
        {
            dbbuf.push_back( &log, sizeof(log));
            DWORD dwBytes = log.nCount * node_len;
            dbbuf.push_back(&dwBytes, sizeof(dwBytes));
            dbbuf.push_back(dbdata.data(), dbdata.size());

            if (pDBEngine->executeSP(GAMEDB_REQUEST_WAR_SAVE_BUYEQUIP, nPDBID, dbbuf.data(), dbbuf.size(), static_cast<IDBRetSink *>(this)) == false)
            {
                ErrorLn("MobaGoodsPart::batchSaveBuyEquipLog failed");
            }
            log.nCount= 0; 
            dbdata.clear();
            dbbuf.clear();
        }

        DBREQ_PARAM_WAR_BUYEQUIP_DETAIL detail;
        detail = *iter;
        log.nCount++;
        dbdata.push_back(&detail, sizeof(DBREQ_PARAM_WAR_BUYEQUIP_DETAIL));
    }
    if(log.nCount > 0)
    {
        dbbuf.push_back(&log, sizeof(log));
        DWORD dwBytes = log.nCount* node_len;
        dbbuf.push_back(&dwBytes, sizeof(dwBytes));
        dbbuf.push_back(dbdata.data(), dbdata.size());
        if (pDBEngine->executeSP(GAMEDB_REQUEST_WAR_SAVE_BUYEQUIP, nPDBID, dbbuf.data(), dbbuf.size(), static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn("MobaGoodsPart::batchSaveBuyEquipLog failed");
            return false;
        }
    }
    m_vecBugEquipLog.clear();
    return true;
}


IMobaGoods*  MobaGoodsPart::createMobaGoods(BYTE nGoodType)
{
    if(nGoodType == MGT_EquipmentType)
        return new CMobaEquipment;
    else if(nGoodType == MGT_LeechdomType)
        return new CMobaLeechdom;
    else
        return NULL;
}

//void MobaGoodsPart::setAddTalentToMap(int nAdd)
//{
//    __IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
//    if(pWarPart == NULL)
//    {
//        return;
//    }
//
//    int nWarID = pWarPart->getWarID();
//
//    SERVICE_PTR pWarContainer = gServerGlobal->getWarManager()->GetWar(nWarID);
//    if(pWarContainer == 0)
//    {
//        return;
//    }
//    IWarService* pWarService = (IWarService*)pWarContainer->query_interface();
//    if(pWarService == NULL)
//    {
//        return;
//    }
//    PDBID pDBID = m_pMaster->getIntProperty(PROPERTY_ID);
//    pWarService->setAddTalentToMap(pDBID, nAdd, ETALENTDROPTYPE_SELLEQUIP);
//}

void MobaGoodsPart::effectAddGood(int nGoodID, int nCount)
{
    // 强制添加物品
    if (m_pMaster == nullptr)
    {
        return;
    }

    int nCost = 0;
    bool bIsSpecialSlotGood = false;
    emGoodAutoUseType eAutoUseType = GAT_NoneType;
    emMobaGoodType nGoodsType = MGT_NoneType;
    T_MAP_COMPOSE_SLOT_CUR_RECORD mapComposeSlotCurRecord;
    // 购买物品流程
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        ErrorLn(__FUNCTION__": -- pSchemeCenter == NULL");
        return;
    }

    SSchemeMobaGoods *pSchemeMobaGoods = pSchemeCenter->getSchemeMobaGoods()->getSchemeMobaGoodsInfo(nGoodID);
    if (pSchemeMobaGoods == NULL)
    {
        ErrorLn(__FUNCTION__": -- pSchemeMobaGoods == NULL");
        return ;
    }
    bIsSpecialSlotGood = pSchemeMobaGoods->bIsSpecialSlotGood;
    eAutoUseType = pSchemeMobaGoods->eAutoUseType;
    nGoodsType = pSchemeMobaGoods->nTypeID;
    //最大拥有数量检查
    int CurrentCount = 0;
    for(int n = 0; n < MAX_GOODS_SLOT_COUNT ; ++n)
    {
        if (m_MobaSlotGoods[n] != NULL &&m_MobaSlotGoods[n]->GetGoodID() == nGoodID)
        {
            CurrentCount += m_MobaSlotGoods[n]->GetOverLayCount();
        }
    }

    if (CurrentCount + nCount >= pSchemeMobaGoods->nMaxCount)
    {
        showSystemMessage(m_pMaster, CHAT_TIP_WAR_GOODS_MAXHOLD);
        return;
    }

    // 检测在本战场是否可以购买此物品
    if (CheckGoodsIsAtThisWar(pSchemeMobaGoods) == false )
    {
        ErrorLn(__FUNCTION__": can't use in this war! nGoodID ="<<nGoodID);
        return;
    }

    for (int i = 0; i < nCount; ++i)
    {
        if(!BuyGood(nGoodID, nGoodsType, nCost, eAutoUseType, bIsSpecialSlotGood, mapComposeSlotCurRecord,true))
        { 
            ErrorLn(__FUNCTION__": can't add Good! nGoodID ="<<nGoodID);
            return;
        }
    }
}

void MobaGoodsPart::effectRemoveGood(int nGoodID,int nCount)
{
    if (nGoodID == INVALID_MOBA_GOOD_ID)
    {
        WarningLn(__FUNCTION__": nGoodID == INVALID_MOBA_GOOD_ID ");
        return;
    }

    if (nCount <= 0)
    {
        WarningLn(__FUNCTION__": nCount ="<<nCount);
        return;
    }
    // 强制移除
    int nRemoveCount = nCount;
    for (int i = 0; i< MAX_GOODS_SLOT_COUNT; ++i)
    {
        IMobaGoods* pSlotGood  = m_MobaSlotGoods[i];
        if (pSlotGood != NULL )
        {
            if (pSlotGood->GetGoodID() == nGoodID)
            {
                // 移除相关属性、技能、buff
                int nOverLayCount = pSlotGood->GetOverLayCount();
                if (nOverLayCount > nRemoveCount)
                {
                    // 物品累积数量够
                    pSlotGood->SetOverLayCount(nOverLayCount - nRemoveCount);
                    nRemoveCount = 0;
                }
                else
                {
                    if (nRemoveCount > 0)
                    {
                        nRemoveCount -= nOverLayCount;
                        pSlotGood->Remove();
                        pSlotGood->Release();
                        pSlotGood = NULL;
                    }
                }

                if (nRemoveCount <= 0)
                {
                    break;
                }
            }
        }
    }
    BroadCastGoodsList();
}

void MobaGoodsPart::SendGoodListToWar()
{
    if (m_pMaster == NULL)
    {
        return;
    }
    event_entity_war_good_list event;
    event.ActorID = m_pMaster->getIntProperty(PROPERTY_ID);
    event.nCamp = m_pMaster->getIntProperty(PROPERTY_CAMP);
    for (int i = 0; i < MAX_GOODS_SLOT_COUNT; ++i )
    {
        if (m_MobaSlotGoods[i] == NULL )
        {
            event.nGoodList[i] = 0;
        }else
        {
            event.nGoodList[i] = m_MobaSlotGoods[i]->GetGoodID();
        }
    }

    m_pMaster->getEntityEvent()->FireExecute(EVENT_GAME_ACTOR_WAR_GOOD_LIST, (LPCSTR)&event, sizeof(event));

}

// 记录XP使用
void MobaGoodsPart::RecordUseXp(int nSkillEffectType)
{
	if (m_pMaster == NULL)
	{
		return;
	}

	if ((nSkillEffectType&SpellEffectType_XP) != SpellEffectType_XP)
	{
		return;
	}

	__IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
	if(pBankPart == NULL)
	{
		return;
	}

	int nUseFlag = pBankPart->getTaskDataInt(TASKKEY_ACTOR_WAR_USEXP);
	if (nUseFlag == TASKDATA_DEFAULT_VALUE)
	{
		pBankPart->addTaskDataInt(TASKKEY_ACTOR_WAR_USEXP, 1);
	}
}

void MobaGoodsPart::restore()
{

}

SEffectCDOverlay MobaGoodsPart::getCDOverlayInfo(int nFreezeID)
{
    SEffectCDOverlay data;
    map<int, SEffectCDOverlay>::iterator iter = m_mapCDOverLayData.find(nFreezeID);
    if (iter == m_mapCDOverLayData.end())
    {
        return data;
    }
    return iter->second;
}

void MobaGoodsPart::setCDOverlayInfo(int nFreezeID, SEffectCDOverlay data)
{
    m_mapCDOverLayData[nFreezeID] = data;
}

void MobaGoodsPart::removeCDOverlayInfo(int nFreezeID)
{
    m_mapCDOverLayData.erase(nFreezeID);
}

bool MobaGoodsPart::luaBuyGoods(int nGoodID, int nCost)
{
    return OnClientBuyGood(nGoodID, nCost);
}

void MobaGoodsPart::luaSellGoods(int nSlotIndex, int nGoodID)
{
    OnClientSellGood(nSlotIndex, nGoodID);
}

void MobaGoodsPart::onChangeHero( int nOldHeroID, int nNewHeroID, bool isBefore )
{
    if(isBefore)
    {
        // 
    }
    else
    {
        RecomputeAllGoodsEffect();
    }
}

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __IMobaGoodsPart * CreateMobaGoodsPart()
{
	return new MobaGoodsPart;
}

