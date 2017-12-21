/*******************************************************************
** 文件名:	SlotPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/6/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "SlotPart.h"
#include "EntityHelper.h"
#include "IServiceContainer.h"
#include "IServerGlobal.h"
#include "ISceneManager.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "INPCService.h"
#include "EntityDef.h"
#include "ServiceHelper.h"
#include "ITankService.h"
#include <math.h>
#include "WarMessageDef.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "IPlayerRole.h"
#include "SlotMessageDef.h"
//#include "LuaHelper.h"
#define  NOTLUA_CALL_LINE
////////////////////* SlotPart */////////////////////////////////////////////
SlotPart::SlotPart()
	:m_pMaster(NULL),m_bCreateEntity(false)
{
    m_SkillSlotMap.clear();
    m_GoodsSlotMap.clear();
}

SlotPart::~SlotPart()
{

}

////////////////////* IEntityPart */////////////////////////////////////////////
short SlotPart::getPartID()
{
	return PART_SLOTLOGIC;
}


bool SlotPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	if (!pEntity)
	{
		return false;
	}

	m_pMaster = pEntity;

    if(m_pMaster->getType() != TYPE_PLAYER_ROLE)
    {
        return true;
    }

	// 注册实体创建消息
	m_pMaster->getEntityEvent()->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_SLOTINDEX_UPDATE, "SlotPart");
	
	// 注册实体创建消息
	m_pMaster->getEntityEvent()->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CREATE,"SlotPart");

    // 注册自己进入某个OB视野消息
    m_pMaster->getEntityEvent()->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ENTER_A_OB_SIGHT, "SlotPart");

    // 注册自己进入OB阵营视野消息
    m_pMaster->getEntityEvent()->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ENTER_A_CAMP_SIGHT, "SlotPart");

	return true;
}

bool SlotPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch(msgId)
	{
	case SC_MSG_SLOT_START:
		{
           //
		}
		break;
	default:
		break;
	}
	return true;
}

__IEntity * SlotPart::getHost()
{
	return m_pMaster;
}

void SlotPart::release()
{

	if (m_pMaster != NULL)
	{
		// 注销实体创建消息
		m_pMaster->getEntityEvent()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_SLOTINDEX_UPDATE);
		m_pMaster->getEntityEvent()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE);
        m_pMaster->getEntityEvent()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ENTER_A_OB_SIGHT);
        m_pMaster->getEntityEvent()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ENTER_A_CAMP_SIGHT);
	}

	m_pMaster = 0;
	m_SkillSlotMap.clear();
	m_GoodsSlotMap.clear();
	delete this;
}

/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
/** 容器事件触发后执行函数
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  
*/
void SlotPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
		// 更新槽位
	case EVENT_SLOTINDEX_UPDATE:
		{
			if ( pszContext == NULL || nLen !=sizeof(event_slotindex_update) )
			{
                return;
            }
			event_slotindex_update * pData = (event_slotindex_update *)pszContext;
			if (pData->bAdd == true)
			{
				// 添加技能
				SSlotSkillInfo upgradeSpell;
				upgradeSpell.nSlotIndex = pData->nSlotIndex;
				upgradeSpell.nSlotType = pData->nSlotType;
				upgradeSpell.nSkillID = pData->nID;
				upgradeSpell.nSlotLevel = pData->nSlotLevel;
				m_SkillSlotMap[pData->nSlotIndex] = upgradeSpell;
			}
			else
			{
				//移除技能
				map<int, SSlotSkillInfo>::iterator iter = m_SkillSlotMap.find(pData->nSlotIndex);
				if (iter != m_SkillSlotMap.end())
				{
					if (iter->second.nSkillID == pData->nID )
					{
						m_SkillSlotMap.erase(iter);
					}
				}
					
			}

			//下发到客户端
			if (m_bCreateEntity == true)
			{
				obuf oSkillSlotInfo;

				msg_slot_update_skill_slot tmpSkillSlot;
				tmpSkillSlot.bIsAdd = pData->bAdd;
				tmpSkillSlot.nSlotIndex  = pData->nSlotIndex;
				tmpSkillSlot.nSlotType = pData->nSlotType;
				tmpSkillSlot.nSkillID = pData->nID;
				tmpSkillSlot.nSlotLevel = pData->nSlotLevel;
				// 单个槽位数据下发
				oSkillSlotInfo<<tmpSkillSlot;
				SendEntityMessage(m_pMaster->getUID(), getPartID(), SC_MSG_UPDATE_SKILL_SLOT, oSkillSlotInfo.data(), oSkillSlotInfo.size());
			}
		}
		break;
	case EVENT_ENTITY_CREATE:
		{
			// 设置已创建实体
			m_bCreateEntity = true;
		}
		break;
    case EVENT_ENTITY_ENTER_A_CAMP_SIGHT:
        {
            int nCamp = *((int*)pszContext);

            SceneHelper sceneHelper(m_pMaster->getSceneID());
            IGameSceneService * pScene = sceneHelper.m_ptr;
            if(pScene)
            {
                if(nCamp == pScene->getObCampID())
                {
                    updateSpellSlot2Client(INVALID_UID, nCamp);
                }
            }
        }
        break;
    case EVENT_ENTITY_ENTER_A_OB_SIGHT:
        {
            UID uidOb = *((UID*)pszContext);
            updateSpellSlot2Client(uidOb, -1);
        }
        break;
	default:
		break;
	}
}

void SlotPart::updateSpellSlot2Client(UID uid, int nCamp)
{
    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
    head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
    head.byKeyModule	= MSG_MODULEID_ENTITY;
    head.byKeyAction	= SC_MSG_UPDATE_SKILL_SLOT;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();
    entityHead.byPartID = getPartID();

    // 进入ob视野 给ob下发技能栏信息
    for(map<int, SSlotSkillInfo>::iterator it = m_SkillSlotMap.begin(); it != m_SkillSlotMap.end(); ++it)
    {
        obuf obufData;
        obufData << head << entityHead;

        msg_slot_update_skill_slot tmpSkillSlot;
        tmpSkillSlot.bIsAdd = true;
        tmpSkillSlot.nSlotIndex  = it->second.nSlotIndex;
        tmpSkillSlot.nSlotType = it->second.nSlotType;
        tmpSkillSlot.nSkillID = it->second.nSkillID;
        tmpSkillSlot.nSlotLevel = it->second.nSlotLevel;
        // 单个槽位数据下发
        obufData << tmpSkillSlot;

        if(uid != INVALID_UID)
        {
            send2Client(uid, PACKAGE_PTR(new std::string(obufData.data(),obufData.size())));
        }
        else if(nCamp >= 0)
        {
            SceneHelper sceneHelper(m_pMaster->getSceneID());
            IGameSceneService * pScene = sceneHelper.m_ptr;
            if (pScene )
            {
                pScene->broadcast_all(0, std::string(obufData.data(), obufData.size()), CAMP_MASK(nCamp));
            }    
        }            
    }
}

bool SlotPart::onSerialize( rkt::ibuffer & in,int flag )
{
    if (flag & TO_OTHER_SEVER)
    {
        m_SkillSlotMap.clear();
        // 取得技能数量
        int nSpellSlotCount = 0;
        in >> nSpellSlotCount;
        for (int n =0; n< nSpellSlotCount; ++n)
        {
            // 取得技能ID
            SSlotSkillInfo tmpSlotSkillInfo;
            in >> tmpSlotSkillInfo;

            m_SkillSlotMap.insert(pair<int,SSlotSkillInfo>(tmpSlotSkillInfo.nSlotIndex, tmpSlotSkillInfo));
        }

        // 取得物品数量
        m_GoodsSlotMap.clear();
        int nGoodsSlotCount = 0;
        in >> nGoodsSlotCount;
        for (int n =0; n< nGoodsSlotCount; ++n)
        {
            // 取得技能ID
            SSlotGoodsInfo tmpSlotGoodsInfo;
            in >> tmpSlotGoodsInfo;

            m_GoodsSlotMap.insert(pair<int,SSlotGoodsInfo>(tmpSlotGoodsInfo.nSlotIndex, tmpSlotGoodsInfo));
        }
    }

	return true;
}

bool SlotPart::deSerialize( rkt::obuf & out,int flag )
{
	if ((flag & TO_CLIENT_SELF) || (flag & TO_OTHER_SEVER))
	{
		//技能栏
		obuf obSpellSlotData;
		// 取得技能数量
		int nSpellSlotCount = (int)m_SkillSlotMap.size();

		map<int, SSlotSkillInfo>::iterator iterSpell =  m_SkillSlotMap.begin();

		for (; iterSpell != m_SkillSlotMap.end(); ++iterSpell)
		{
			// 压入技能槽位相关
			obSpellSlotData << iterSpell->second;
		}
		// 压入数量
		out << nSpellSlotCount;
		out.push_back(obSpellSlotData.data(), obSpellSlotData.size());

		//物品栏
		obuf obGoodsSlotData;
		// 取得物品相关数量
		int nGoodsSlotCount = (int)m_GoodsSlotMap.size();

		map<int, SSlotGoodsInfo>::iterator iter =  m_GoodsSlotMap.begin();

		for (; iter != m_GoodsSlotMap.end(); ++iter)
		{
			// 压入技能槽位相关
			obGoodsSlotData << iter->second;
		}
		// 压入数量
		out << nGoodsSlotCount;
		out.push_back(obGoodsSlotData.data(), obGoodsSlotData.size());
	}
	else if (flag & TO_CLIENT_OTHER)
	{
		//物品栏
		obuf obGoodsSlotData;
		// 取得物品相关
		int nGoodsSlotCount = (int)m_GoodsSlotMap.size();

 		__IEntityProperty* pEntityProperty = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
		PDBID idActor = pEntityProperty->getProperty_Integer(PROPERTY_ID);

			
		map<int, SSlotGoodsInfo>::iterator iter =  m_GoodsSlotMap.begin();

		for (; iter != m_GoodsSlotMap.end(); ++iter)
		{
			// 压入技能槽位相关
			obGoodsSlotData << iter->second;
		}
		// 压入数量
		out << nGoodsSlotCount;
		out << idActor;
		out.push_back(obGoodsSlotData.data(), obGoodsSlotData.size());
	}

	return true;
}

bool SlotPart::Parse(string linkContent)
{
	return true;
}


void SlotPart::SendEntityMessage(UID uid, short byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
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

    // 发给OB
    SceneHelper sceneHelper(m_pMaster->getSceneID());
    IGameSceneService * pScene = sceneHelper.m_ptr;
    if (pScene )
    {
        int nObCamp = pScene->getObCampID();
        if(nObCamp > 0)
        {
            pScene->broadcast_all(0, std::string(obufData.data(), obufData.size()), CAMP_MASK(nObCamp));
        }
    }
}

void SlotPart::SetGoodsSlotList(LPCSTR pszData, int nDataLen)
{
	msg_entity_fight_good_list *pMsgGoodListHead = (msg_entity_fight_good_list *)pszData;
	int nCount = pMsgGoodListHead->nCount;

	if(nDataLen != sizeof(msg_entity_fight_good)*nCount + sizeof(msg_entity_fight_good_list))
	{
		return;
	}

	msg_entity_fight_good *pFirstGood = (msg_entity_fight_good *)(pMsgGoodListHead + 1);
	
	for (int i = 0; i < nCount; ++i)
	{
		//所有物品槽位数据
		SSlotGoodsInfo PersonGoodsInfo;

		PersonGoodsInfo.nGoodID = pFirstGood->nGoodID;
		PersonGoodsInfo.nSlotIndex = pFirstGood->nSlotIndex;
		PersonGoodsInfo.nSlotType = pFirstGood->nSlotType;
		PersonGoodsInfo.bCanUse = pFirstGood->bCanUse;  // 能否主动使用(0不能，1能)
		PersonGoodsInfo.nCanUseTimes = pFirstGood->nCanUseTimes;	// 能使用次数
		PersonGoodsInfo.nFreezeID	= pFirstGood->nFreezeID;		// 使用冷却ID

		// buff或者技能冷却ID
		PersonGoodsInfo.nBuffID = pFirstGood->nBuffID;
		PersonGoodsInfo.nSkillID = pFirstGood->nSkillID;
		PersonGoodsInfo.bIsNewAdd = pFirstGood->bIsNewAdd;
        map<int, SSlotGoodsInfo>::iterator iter = m_GoodsSlotMap.find(pFirstGood->nSlotIndex);
        if (iter != m_GoodsSlotMap.end())
        {
            m_GoodsSlotMap.erase(iter);
        }
		m_GoodsSlotMap.insert(pair<int, SSlotGoodsInfo>(pFirstGood->nSlotIndex,PersonGoodsInfo));
		pFirstGood++;
	}

	// 物品槽位数据下发
	SendEntityMessage(m_pMaster->getUID(), getPartID(), SC_MSG_UPDATE_GOODS_SLOT, pszData, nDataLen);
}

int SlotPart::GetSlotSkillID(SLOT_TYPE SlotType, int nIndex)
{
	if (SlotType == SLOT_TYPE_SKILL)
	{
		map<int, SSlotSkillInfo>::iterator iter = m_SkillSlotMap.find(nIndex);
		if (iter != m_SkillSlotMap.end())
		{
			return iter->second.nSkillID;
		}
		return 0;
	}

	if (SlotType == SLOT_TYPE_GOODS)
	{
		map<int, SSlotGoodsInfo>::iterator iter = m_GoodsSlotMap.find(nIndex);
		if (iter != m_GoodsSlotMap.end())
		{
			return iter->second.nSkillID;
		}
		return 0;
	}

	return 0;
}

void SlotPart::restore()
{

}


//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __ISlotPart * CreateSlotPart()
{
	return new SlotPart;
}