/*******************************************************************
** 文件名:	e:\Rocket\Client\WarClient\Src\MobaGoodsPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/7/28  21:25
** 版  本:	1.0
** 描  述:	战场物品部件
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "MobaGoodsPart.h"
#include "WarClient.h"
#include "ExternalFacade.h"
#include "EventDef.h"
#include "SpellDef.h"
#include "IEntityProperty.h"
#include "ISchemeCenter.h"
#include "ISpell.h"
#include "SlotDef.h"
#include "Entity_inl.h"

using namespace SPELL;
/** 
@param   
@param   
@return  
*/
CMobaGoodsPart::CMobaGoodsPart(void)
{
	// 所属实体
	m_pMaster = NULL;
    m_bCanBuyOrSell = false;

}

/** 
@param   
@param   
@return  
*/
CMobaGoodsPart::~CMobaGoodsPart(void)
{
}

///////////////////ISerializable////////////////////////////////////
/** 序列化支持接口
@return  
*/
bool CMobaGoodsPart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
    if (flag & TO_CLIENT_SELF)
    {
        in >> m_bCanBuyOrSell;
    }

	return true;
}

/** 反序列化支持接口
@return  
*/
bool CMobaGoodsPart::deSerialize( rkt::obuf & out,int flag )
{
	return true;
}


///////////////////IEntityPart////////////////////////////////////
/** 取是部件ID
@param   
@param   
@return  
*/
short CMobaGoodsPart::getPartID(void)
{
	return PART_WAR;
}

/** 创建，重新启用，也会调此接口
@param   
@param   
@return  
*/
bool CMobaGoodsPart::onLoad( IEntity* pEntity)
{
	// 所属实体
	m_pMaster = pEntity;
	m_bHaveCampInfo = false;

    IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
    if (pEventEngine == NULL)
    {
        ErrorLn(__FUNCTION__": onLoad pEventEngine==NULL");
        return false;
    }


	// 只有自己的英雄注册
	if ( m_pMaster->getTypeMask()&MASK_ACTOR && ((IClientEntity*)m_pMaster)->isHero())
	{
        //对主英雄注册实体创建事件
        UID uid = m_pMaster->getUID();
        BYTE type  = UID_2_TYPE(uid);

        pEventEngine->Subscibe(this, EVENT_ENTITY_CREATE_VIEW, type, uid,__FILE__);
	}

	if (!m_EquipSchemeMgr.init(m_pMaster))
	{
		ErrorLn(__FUNCTION__": Initial equip scheme manager failed.");
		return false;
	}

	return true;
	
}

/** 释放,会释放内存
@param   
@param   
@return  
*/
void CMobaGoodsPart::release(void)
{
    if (m_pMaster != NULL)
    {
	    // 只有自己的英雄注册
	    if ( m_pMaster->getTypeMask()&MASK_ACTOR && ((IClientEntity*)m_pMaster)->isHero())
	    {
            UID uid = m_pMaster->getUID();
            BYTE type  = UID_2_TYPE(uid);

            // 取消注册的事件
            gClientGlobal->getEventEngine()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE_VIEW, type, uid);
	    }
    }
	// 所属实体
	m_pMaster = NULL;
	

	delete this;
}


bool CMobaGoodsPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	//部件转发消息
	switch( msgId )
	{
	case  SC_MSG_WAR_CAN_BUY_OR_SELL:
		{
			//设置能否购买物品
			if(data == NULL || len != sizeof(msg_entity_can_buy_or_sell))
			{
				return false;
			}
			msg_entity_can_buy_or_sell * pMsg = (msg_entity_can_buy_or_sell *)(data);
            m_bCanBuyOrSell = pMsg->bCanOrSell;
            TraceLn(__FUNCTION__": name ="<< m_pMaster->getName()<<" Current bCanBuyOrSell ="<<m_bCanBuyOrSell);
            
			// 通告显示层更新
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_CAN_BUY_OR_SELL,(int)(pMsg->bCanOrSell),0,0, 0);
		}
		break;
	case  SC_MSG_WAR_CAN_CANCEL_OPREATION:
		{
			//设置能撤销操作
			if(data == NULL || len != sizeof(msg_entity_can_cancel_opration))
			{
				return false;
			}
			msg_entity_can_cancel_opration * pMsg = (msg_entity_can_cancel_opration *)(data);

			// 通告显示层更新
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_CANCEL_OPRATION,(int)(pMsg->bCanCancelOpration),0,0, 0);
		}
		break;
	case SC_MSG_WAR_EQUIPSCHEME_LIST:
		{
			m_EquipSchemeMgr.onMessage(msgId, data, len);
		}
		break;
	default:break;
	}
    return true;
}

bool CMobaGoodsPart::onCommand( int cmdid,void * data,size_t len )
{
	switch( cmdid )
	{
		// 购买装备信息
		case ENTITY_CMD_WAR_BUY_GOOD:						
			{
				if (len != sizeof(cmd_war_personbuygoods))
				{
					ErrorLn(__FUNCTION__": len != sizeof(cmd_war_personbuygoods)");
					return false;
				}
				cmd_war_personbuygoods* pData = (cmd_war_personbuygoods *)data;
				obuf ob;

				msg_entity_fight_buy_good sendData;
				sendData.nGoodID = pData->nGoodID;

                if (sendData.nGoodID == 0)
                {
                    ErrorLn(__FUNCTION__": nGoodID == 0");
                    return false;
                }

				ob.push_back(&sendData, sizeof(sendData));
				SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_BUY_GOOD, ob.data(), ob.size());
				return true;
			}
			break;
		// 出售物品
		case ENTITY_CMD_WAR_SELL_GOOD:						
			{
				if (len != sizeof(cmd_war_personsellgoods))
				{
					ErrorLn(__FUNCTION__": len != sizeof(cmd_war_personsellgoods)");
					return false;
				}
				cmd_war_personsellgoods* pData = (cmd_war_personsellgoods *)data;
				obuf ob;

				msg_entity_fight_sell_good sendData;
				sendData.nGoodID = pData->nGoodID;
				sendData.nSlotIndex = pData->nSlotIndex;

				ob.push_back(&sendData, sizeof(sendData));
				SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_SELL_GOOD, ob.data(), ob.size());
				return true;
			}
			break;
		// 交换槽位物品位置
		case ENTITY_CMD_WAR_EXCHANGE_SLOT:						
			{

				if (len != sizeof(cmd_war_personexchangeslot))
				{
					ErrorLn(__FUNCTION__": len != sizeof(cmd_war_personexchangeslot)");
					return false;
				}
				cmd_war_personexchangeslot* pData = (cmd_war_personexchangeslot *)data;
				
				ExchangeSlot(pData->nSrcSlotIndex, pData->nTargetSlotIndex);
				return true;
			}
			break;
		
		// 撤销操作
		case ENTITY_CMD_WAR_REVOCATION_OPERATION:						
			{

				if (len != sizeof(cmd_war_revocation_operation))
				{
					ErrorLn(__FUNCTION__": len != sizeof(cmd_war_revocation_operation)");
					return false;
				}
				cmd_war_revocation_operation* pData = (cmd_war_revocation_operation *)data;
				obuf ob;

				msg_entity_revocation_operation sendData;
				sendData.uUID = pData->uUID;

				ob.push_back(&sendData, sizeof(sendData));
				SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_REVOCATION_OPERATION, ob.data(), ob.size());
				return true;
			}
			break;
		case ENTITY_CMD_WAR_ADD_EQUIPSCHEME:
		case ENTITY_CMD_WAR_DEL_EQUIPSCHEME:
		case ENTITY_CMD_WAR_MODIFY_EQUIPSCHEME:
		case ENTITY_CMD_WAR_READ_EQUIPSCHEME:
		{
			m_EquipSchemeMgr.onCommand(cmdid, data, len);
			return true;
		}
		break;
	default:
		return false;
	}

	return false;
}

/** 取是主人
@param   
@param   
@return  
*/
IEntity * CMobaGoodsPart::getHost(void)
{
	return m_pMaster;
}

void CMobaGoodsPart::GoodsUse(SSlotGoodsInfo SlotGoodsInfo)
{
	// 客户端先校验使用
	ISpellPart *pSpellPart = (ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);
	if (pSpellPart == NULL)
	{
		return;
	}

	//技能使用
	if (SlotGoodsInfo.nSkillID != 0)
	{
		SSlotSkillInfo tmpSlotSkillInfo;
		tmpSlotSkillInfo.nSkillID = SlotGoodsInfo.nSkillID;
		tmpSlotSkillInfo.nSlotIndex = SlotGoodsInfo.nSlotIndex;
		tmpSlotSkillInfo.nSlotType = SlotGoodsInfo.nSlotType;
		pSpellPart->spellAttribute(tmpSlotSkillInfo);
	}
	else if(SlotGoodsInfo.nBuffID != 0)
	{
		obuf ob;

		msg_entity_fight_use_good sendData;
		sendData.nSlotIndex = SlotGoodsInfo.nSlotIndex;

		ob.push_back(&sendData, sizeof(sendData));
		SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_USE_GOOD, ob.data(), ob.size());
	}
}

void CMobaGoodsPart::ExchangeSlot(int nSrcSlotIndex, int nTargetSlotIndex)
{
    if (nSrcSlotIndex == SPECIAL_SLOT_INDEX || nTargetSlotIndex == SPECIAL_SLOT_INDEX)
    {
        return;
    }
	obuf ob;
	ob.clear();
	msg_entity_fight_exchange_good sendData;
	sendData.nSrcSlot = nSrcSlotIndex;
	sendData.nTargetSlot = nTargetSlotIndex;

	ob.push_back(&sendData, sizeof(sendData));
	SendMessageToEntityScene(m_pMaster->getUID(), PART_MOBAGOODS, CS_MSG_WAR_EXCHANGE_GOOD, ob.data(), ob.size());
}

void CMobaGoodsPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {//实体创建事件
    case EVENT_ENTITY_CREATE_VIEW:
        {  
            // 通告显示层更新
            TraceLn(__FUNCTION__": name ="<< m_pMaster->getName()<<" Current bCanBuyOrSell ="<<m_bCanBuyOrSell);
            m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_CAN_BUY_OR_SELL,(int)(m_bCanBuyOrSell),0,0, 0);
        }
        break;
    default:
        break;
    }
}

void CMobaGoodsPart::restore()
{

}
