/*******************************************************************
** 文件名:	e:\Rocket\Client\FreezeClient\Src\SlotLogicPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2008/2/28  21:30
** 版  本:	1.0
** 描  述:	槽位逻辑判断处理
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "IFreezeClient.h"
#include "SlotLogicPart.h"
#include "IWarClient.h"
#include "SlotMessageDef.h"
#include "WarMessageDef.h"
#include "MobaGoodsDef.h"
#include "IEntityProperty.h"
#include "GameViewDef.h"


#define SLOTPART_INNER_EFFECT( typeName,obj,pContext)  gClientGlobal->getEffectFactory()->DoInnerEffect( #typeName,&obj,sizeof(obj),pContext)
/** 
@param   
@param   
@return  
*/
CSlotLogicPart::CSlotLogicPart(void)
{
    // 所属实体
    m_bHaveCreated = false;
    m_pMaster = NULL;
    m_GoodsSlotMap.clear();
    m_SkillSlotMap.clear();
    m_enterViewGoodsRecord.clear();
}

/** 
@param   
@param   
@return  
*/
CSlotLogicPart::~CSlotLogicPart(void)
{

}

///////////////////ISerializable////////////////////////////////////
/** 序列化支持接口
@return  
*/
bool CSlotLogicPart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
    if (flag & TO_CLIENT_SELF)
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
    else if(flag & TO_CLIENT_OTHER)
    {
        // 取得物品数量
        int nGoodsSlotCount = 0;
        PDBID pDbid = 0;;
        in >> nGoodsSlotCount>>pDbid;
        obuf obdata;
        cmd_war_persongoodsinfo_count cmdWarPersonGoodsInfoCount;
        cmdWarPersonGoodsInfoCount.nCount = nGoodsSlotCount;
        cmdWarPersonGoodsInfoCount.idActor = pDbid;

        obdata.push_back(&cmdWarPersonGoodsInfoCount, sizeof(cmdWarPersonGoodsInfoCount));

        for (int n =0; n< nGoodsSlotCount; ++n)
        {
            // 取得技能ID
            SSlotGoodsInfo tmpSlotGoodsInfo;
            in >> tmpSlotGoodsInfo;

            //显示层命令数据
            cmd_war_persongoodsinfo cmdPersonGoodsInfo;
            cmdPersonGoodsInfo.idActor = pDbid;
            cmdPersonGoodsInfo.nGoodID = tmpSlotGoodsInfo.nGoodID;
            cmdPersonGoodsInfo.nSlotIndex = tmpSlotGoodsInfo.nSlotIndex;
            cmdPersonGoodsInfo.nSlotType = tmpSlotGoodsInfo.nSlotType;
            cmdPersonGoodsInfo.nCanUse = tmpSlotGoodsInfo.bCanUse == false ? 0:1;// 能否主动使用(0不能，1能)
            cmdPersonGoodsInfo.nCanUseTimes = tmpSlotGoodsInfo.nCanUseTimes;	// 能使用次数
            cmdPersonGoodsInfo.nFreezeID	= tmpSlotGoodsInfo.nFreezeID;		// 使用冷却ID

            // buff或者技能冷却ID
            cmdPersonGoodsInfo.nBuffID = tmpSlotGoodsInfo.nBuffID;
            cmdPersonGoodsInfo.nSkillID = tmpSlotGoodsInfo.nSkillID;
            obdata.push_back(&cmdPersonGoodsInfo,sizeof(cmdPersonGoodsInfo));
        }
        
        m_enterViewGoodsRecord.clear();

		if(nGoodsSlotCount > 0)
			m_enterViewGoodsRecord.push_back(obdata.data(),obdata.size());
    }


    return true;
}

/** 反序列化支持接口
@return  
*/
bool CSlotLogicPart::deSerialize( rkt::obuf & out,int flag )
{
    return true;
}


///////////////////IEntityPart////////////////////////////////////
/** 取是部件ID
@param   
@param   
@return  
*/
short CSlotLogicPart::getPartID(void)
{
    return PART_SLOTLOGIC;
}

/** 创建，重新启用，也会调此接口
@param   
@param   
@return  
*/
bool CSlotLogicPart::onLoad( IEntity* pEntity)
{
    // 所属实体
    m_pMaster = pEntity;

    UID uid = m_pMaster->getUID();
    BYTE type  = UID_2_TYPE(uid);

	IEventEngine * pEventEngine = gClientGlobal->getEventEngine();

	// 只有自己的英雄注册
	if (pEventEngine && (((IClientEntity*)m_pMaster)->isHero() || isOBClient() ) )
	{
		gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_WAR_GOOD, this);

		if (pEventEngine)
		{
			pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_FREEZE_START, type, uid, __FILE__);
			pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_SKIN_CHANGED, type, uid, __FILE__);
			pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE_VIEW, type, uid, __FILE__);
		}
	}


    if (pEventEngine && (m_pMaster->getTypeMask()&MASK_ACTOR))
    {
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ENTER_VIEWPORT, type, uid, __FILE__);
    }
    return true;
}

/** 释放,会释放内存
@param   
@param   
@return  
*/
void CSlotLogicPart::release(void)
{
	if (m_pMaster == NULL)
		return;

	UID uid = m_pMaster->getUID();
	BYTE type = UID_2_TYPE(uid);

	IEventEngine * pEventEngine = gClientGlobal->getEventEngine();

	if (pEventEngine && (((IClientEntity*)m_pMaster)->isHero() || isOBClient()) )
	{
		gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_WAR_GOOD, this);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_FREEZE_START, type, uid);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_SKIN_CHANGED, type, uid);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE_VIEW, type, uid);
	}

    if (pEventEngine && (m_pMaster->getTypeMask()&MASK_ACTOR))
    {
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ENTER_VIEWPORT, type, uid);
    }

    m_pMaster = NULL;
    m_GoodsSlotMap.clear();
    m_SkillSlotMap.clear();

    delete this;
}

/** 发送命令,命令与事件不同，命令是外界通知实体做某件事情，实体做了，再发出事件
@param   
@param   
@return  
*/
bool CSlotLogicPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch( msgId )
    {
    case SC_MSG_UPDATE_SKILL_SLOT:
        {
            //激活技能
            if(data == NULL || len != sizeof(msg_slot_update_skill_slot))
            {
                return false;
            }
            msg_slot_update_skill_slot * pMsg = (msg_slot_update_skill_slot *)(data);

            UpdateSkillSlot(pMsg);
        }
        break;

    case SC_MSG_UPDATE_GOODS_SLOT:
        {
            UpdateGoodsSlot(data, len);
        }
        break;
    default:break;

    }
    return true;
}

void CSlotLogicPart::onMessage(SNetMsgHead* head, void* data, size_t len)
{
    // 全局消息
    if (head->byKeyModule == MSG_MODULEID_WAR_GOOD)
    {
        // 大厅服消息
        msg_entity_fight_good_list *pMsgGoodListHead = (msg_entity_fight_good_list *)data;
        int nCount = pMsgGoodListHead->nCount;
        PDBID idActor = pMsgGoodListHead->idActor;
        if(len != sizeof(msg_entity_fight_good)*nCount + sizeof(msg_entity_fight_good_list))
        {
            return;
        }
        msg_entity_fight_good *pFirstGood = (msg_entity_fight_good *)(pMsgGoodListHead + 1);
        obuf obdata;
        cmd_war_persongoodsinfo_count cmdWarPersonGoodsInfoCount;
        cmdWarPersonGoodsInfoCount.nCount = nCount;
        cmdWarPersonGoodsInfoCount.idActor = idActor;

        obdata.push_back(&cmdWarPersonGoodsInfoCount, sizeof(cmdWarPersonGoodsInfoCount));

        for ( int i = 0; i < nCount; ++i )
        {
            //显示层命令数据
            cmd_war_persongoodsinfo cmdPersonGoodsInfo;
            cmdPersonGoodsInfo.idActor = idActor;
            cmdPersonGoodsInfo.uID = pFirstGood->uUID;
            cmdPersonGoodsInfo.nGoodID = pFirstGood->nGoodID;
            cmdPersonGoodsInfo.nSlotIndex = pFirstGood->nSlotIndex;
            cmdPersonGoodsInfo.nSlotType = pFirstGood->nSlotType;
            cmdPersonGoodsInfo.nCanUse = pFirstGood->bCanUse == false? 0 : 1;  // 能否主动使用(0不能，1能)
            cmdPersonGoodsInfo.nCanUseTimes = pFirstGood->nCanUseTimes;	// 能使用次数
            cmdPersonGoodsInfo.nFreezeID	= pFirstGood->nFreezeID;		// 使用冷却ID

            // buff或者技能冷却ID
            cmdPersonGoodsInfo.nBuffID = pFirstGood->nBuffID;
            cmdPersonGoodsInfo.nSkillID = pFirstGood->nSkillID;
            // 通告显示层更新
            obdata.push_back(&cmdPersonGoodsInfo,sizeof(cmdPersonGoodsInfo));
            ++pFirstGood;
        }

        m_mapAllPlayerGoodsRecord[idActor] = obdata;
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_WAR_MOBA_GOODS_INFO,0, 0, obdata.data(), obdata.size());
    }
}

bool CSlotLogicPart::onCommand( int cmdid,void * data,size_t len )
{
    switch( cmdid )
    {
    case ENTITY_CMD_CHANGE_SLOT:						// 显示层发给逻辑层
        {
            /*if ( data==0 || len!=sizeof(cmd_creature_slotview) )
            {
            ErrorLn("ENTITY_CMD_POSITION_SYNC data error!len="<<len);
            }
            else
            {
            onCmdSlotCheck((cmd_creature_slotview*)data);
            }*/
            return true;
        }
    case ENTITY_CMD_INITAL_SLOT:						// 显示层发给逻辑层
        {
            InitialSlots();
            return true;
        }

    case ENTITY_CMD_ASKSPELLATTRIBUTE:					// 询问技能或者物品相关属性
        {
            // 判断参数
            if (data == 0 || len != sizeof(cmd_creature_spellAttribute))
            {
                ErrorLn("ENTITY_CMD_ASKSPELLATTRIBUTE pData==NULL || nLen != sizeof(cmd_creature_spellAttribute)");
                return true;
            }
            if (m_pMaster == NULL)
            {
                ErrorLn("ENTITY_CMD_ASKSPELLATTRIBUTE m_pEntity==NULL");
                return true;
            }

            cmd_creature_spellAttribute* pData = (cmd_creature_spellAttribute *)data;

            if (pData->nSlotType == SLOT_TYPE::SLOT_TYPE_SKILL)
            {
                // 客户端先校验使用
                ISpellPart *pSpellPart = (ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);
                if (pSpellPart == NULL)
                {
                    return false;
                }

                map<int, SSlotSkillInfo>::iterator iter =  m_SkillSlotMap.find(pData->nSlotIndex);
                //技能使用
                if (iter != m_SkillSlotMap.end() && iter->second.nSkillID != 0 )
                {
                    SSlotSkillInfo tmpSlotSkillInfo;
                    tmpSlotSkillInfo.nSkillID = iter->second.nSkillID;
                    tmpSlotSkillInfo.nSlotIndex = iter->second.nSlotIndex;
                    tmpSlotSkillInfo.nSlotType = iter->second.nSlotType;
                    pSpellPart->spellAttribute(tmpSlotSkillInfo);
                }
            }
            else if (pData->nSlotType == SLOT_TYPE::SLOT_TYPE_GOODS)
            {
                // 客户端先校验使用
                map<int, SSlotGoodsInfo>::iterator  iter =  m_GoodsSlotMap.find(pData->nSlotIndex);
                if (iter == m_GoodsSlotMap.end())
                {
                    return false;
                }
                IMobaGoodsPart *pMobaGoodsPart = (IMobaGoodsPart *)m_pMaster->getEntityPart(PART_MOBAGOODS);
                if (pMobaGoodsPart == NULL)
                {
                    return true;
                }

                pMobaGoodsPart->GoodsUse(iter->second);
            }

            return true;
        }
        break;
    default:
        return false;
    }

}

/** 取是主人
@param   
@param   
@return  
*/
IEntity * CSlotLogicPart::getHost(void)
{
    return m_pMaster;
}

bool CSlotLogicPart::onCmdSlotCheck(cmd_creature_slotview *SlotViewData )
{
    if (SlotViewData->nType == EFreeze_ClassID_Spell)
    {
        //技能冷却检查
        if(SlotViewData->nId != 0)
        {
            // Buff客户端
            IFreezeClient *pFreezeClient = gClientGlobal->getFreezeClient();
            if (pFreezeClient == 0)
            {
                ErrorLn("get Freeze Client Failed!");
                return false;
            }

            ISpellPart *pSpellPart = (ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);
            if (pSpellPart == NULL)
            {
                return false;
            }
            ISpell * pSpell = pSpellPart->findSpell( SlotViewData->nId );
            if ( pSpell == NULL )
                return false;

            // 取得冷却部件
            IFreezePart *pFreezePart = (IFreezePart*)m_pMaster->getEntityPart(PART_FREEZE);
            if (pFreezePart == NULL)
            {
                return false;
            }

            //  取得冷却ID
            return pFreezePart->CanWork(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID));
        }
        else
        {
            return false;
        }
    }
    else
    {
        //处理其他
        return false;
    }
}

void CSlotLogicPart::UpdateSlot(cmd_creature_set_slot *SlotData)
{
    // 通告显示层更新
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CHANGE_SLOT,sizeof(SlotData),0,SlotData, sizeof(cmd_creature_set_slot));

}

void CSlotLogicPart::InitialSlots()
{
    // 初始化槽位信息 物品相关
	cmd_creature_set_slot GoodsSlot;
	int nGoodID = 0;
	for (int n = 0; n < MAX_GOODS_SLOT_COUNT; ++n)
	{
		GoodsSlot.nSlotIndex = n;
		GoodsSlot.nSlotLevel = 0;
		GoodsSlot.nSlotType = SLOT_TYPE_GOODS;

		//组合信息发送到View交互
		GoodsSlot.slotview.bRemove = 0;
		GoodsSlot.slotview.nType = EFreeze_ClassID_Goods;
		GoodsSlot.slotview.nId = 0;
		GoodsSlot.slotview.nViewType = ViewType_LeftCircle;	// 显示类型（方式）：0101 1111
		GoodsSlot.slotview.nCurData = 0;					// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
		GoodsSlot.slotview.nMaxData = 0;					// 最大相关数据
		GoodsSlot.slotview.nIconID = 0;						// 图片ID
		GoodsSlot.slotview.bEnable = 1;						// 是否可用
		if (n == SPECIAL_SLOT_INDEX)				        // 能否拖动
		{
			GoodsSlot.slotview.bCanDrag = 0;
		}
		else
		{
			GoodsSlot.slotview.bCanDrag = 1;
		}
		GoodsSlot.slotview.bIsOnDrag = 0;
		GoodsSlot.slotview.nSlotGroupType = 1;				// 槽位组类型

		UpdateSlot(&GoodsSlot);
	}

}

void CSlotLogicPart::OnExecute( WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen )
{
    switch(wEventID)
    {
    case EVENT_FREEZE_START:
        {
            UpdateFreezeInfo(bSrcType,dwSrcID, pszContext, nLen);
        }
        break;		

    case EVENT_ENTITY_SPELL_CHANGE:
        {
            UpdateSlotInfo(bSrcType,dwSrcID, pszContext, nLen);
        }
        break;

    case EVENT_ENTITY_ENTER_VIEWPORT:
        {
			EnterViewSendGoodsRecord();
        }
        break;
        
    case EVENT_ENTITY_SKIN_CHANGED:
        {
            if ( nLen!=sizeof(event_entity_skin_changed) || pszContext==0 )
                return;
            event_entity_skin_changed *pData = (event_entity_skin_changed *)pszContext;

            // 通告显示层更新映射信息
            m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_CHANGE_SKIN,pData->uChangedUID,0,0, 0);

            // 发送槽位对应映射
            ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
            if ( NULL==pCenter )
            {
                return;
            }
            ISchemeSlotMapedShortcut* pSlotMapedShortcut = pCenter->getSchemeSlotMapedShortcut();
            if ( NULL==pSlotMapedShortcut )
            {
                return;
            }

            // 快捷键映射
            cmd_count cmd_map_count;
            obuf obMapData;
            map<int, SSlotSkillInfo>::iterator iter =  m_SkillSlotMap.begin();
            for (;iter != m_SkillSlotMap.end();++iter)
            {
                EFFECT_CONTEXT context;
                context.pEntity = m_pMaster;

                EffectClient_SetSpellSlot SetSpellSlot;
                SetSpellSlot.nSlotIndex = iter->second.nSlotIndex;
                SetSpellSlot.nSlotType = iter->second.nSlotType;
                SetSpellSlot.nSlotLevel = iter->second.nSlotLevel;
                SetSpellSlot.nSpellID	= iter->second.nSkillID;
                SetSpellSlot.nSpellFindType = SPELL_FIND_SPELL_FACTORY;
                SLOTPART_INNER_EFFECT(EffectClient_SetSpellSlot, SetSpellSlot, &context);

                SSlotMapedShortcutScheme *pSlotMapedShortcutScheme =  pSlotMapedShortcut->getSlotMapedShortcutShemeInfo(iter->second.nSlotIndex, iter->second.nSlotType);
                if (pSlotMapedShortcutScheme == NULL)
                {
                    continue;
                }

				//获得该英雄槽位的推荐施法方式
				ISpellFactory* spellFactory = gClientGlobal->getSpellFactory();
				if (spellFactory == NULL)
				{
					ErrorLn("getSpellData failed, spellFactory == NULL");
					return;
				}

				if (iter->second.nSkillID <= 0)
				{
					return;
				}

				SPELL_DATA* pSpellData = spellFactory->getSpellData(iter->second.nSkillID);
				if (pSpellData == NULL)
				{
					return;
				}

				int nSpellIAdvanceUseType = pSpellData->nDefaultSelectType;

                //槽位单独数据
                cmd_creature_slot_maped_shortcut node;
                node.nSlotIndex = iter->second.nSlotIndex;
                node.nSlotType  = iter->second.nSlotType;
                node.nAirCons   = pSlotMapedShortcutScheme->nAirCons;
				node.nAdvanceUseType = nSpellIAdvanceUseType;
				node.isAdd = true;
                sstrcpyn( node.szKeyName, pSlotMapedShortcutScheme->szKeyName, sizeof(node.szKeyName) );

                ++cmd_map_count.nCount;
                obMapData.push_back(&node, sizeof(node));
            }
            // 通告显示层更新映射信息
            if (cmd_map_count.nCount > 0) {
                obuf ob_updates;
                ob_updates.push_back(&cmd_map_count, sizeof(cmd_map_count));
                ob_updates.push_back(obMapData.data(), obMapData.size());
                m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SLOT_MAPED_SHORTCUT, 0, 0, ob_updates.data(), ob_updates.size());
            }


            //// 发送功能性快捷键信息
            //vector<SSlotMapedShortcutScheme> functionTypeSlotVec = pSlotMapedShortcut->getOneTypeSlotInfo(ShortCutSlotType_Function);
            //cmd_count cmdCount;
            //cmdCount.nCount = functionTypeSlotVec.size();
            //if (cmdCount.nCount > 0)
            //{
            //    obuf obData;
            //    obData << cmdCount;
            //    
            //    for (vector<SSlotMapedShortcutScheme>::iterator itBegin = functionTypeSlotVec.begin(); itBegin != functionTypeSlotVec.end(); ++itBegin)
            //    {
            //        cmd_Slot_Shortcut cmdSlotShortcut;
            //        cmdSlotShortcut.nSlotId = itBegin->nSlotIndex;
            //        cmdSlotShortcut.nType = itBegin->nSlotType;
            //        cmdSlotShortcut.nSkillId = 0;
            //        sstrcpyn(cmdSlotShortcut.szShortcut, itBegin->szKeyName, sizeof(itBegin->szKeyName));
            //        obData.push_back(&cmdSlotShortcut, sizeof(cmd_Slot_Shortcut));
            //    }

            //    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SHORTCUTSCHEME_FUNCTION_INFO, 0, 0, obData.data(), obData.size());
            //}
        }
        break;

    case EVENT_ENTITY_CREATE_VIEW:
        {  
            m_bHaveCreated = true;
            // 通告显示层更新
            if (m_pMaster == NULL)
            {
                return;
            }
			InitialSlots();

            // 更新未完成创建时下发的装备
            map<PDBID, obuf>::iterator iter =  m_mapAllPlayerGoodsRecord.begin();
            for (;iter != m_mapAllPlayerGoodsRecord.end(); ++iter)
            {
                obuf& obdata = iter->second;
                gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_WAR_MOBA_GOODS_INFO,0, 0, obdata.data(), obdata.size());
            }

            // 进入战场 和 重连相关
            UpdateViewSlotInfo();
        }
        break;
    default:
        break;
    }
}

void CSlotLogicPart::UpdateFreezeInfo( BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen )
{
    if ( nLen!=sizeof(event_freeze_start_c) || pszContext==0 )
        return ;

    event_freeze_start_c * pContext = (event_freeze_start_c*)pszContext;
    int viewID = ((IClientEntity*)m_pMaster)->getViewID();

    IFreezeClient *pFreezeClient = gClientGlobal->getFreezeClient();
    if (pFreezeClient == NULL)
    {
        return;
    }

    // 通告显示层更新
    switch (pContext->dwClassID)
    {
    case EFreeze_ClassID_Spell:			// 技能
        {
            // 技能获取
            IFreezePart *pFreezePart = (IFreezePart*)m_pMaster->getEntityPart(PART_FREEZE);
            if (pFreezePart == NULL)
            {
                return;
            }

            // 初始化槽位信息 技能相关
            ISpellPart *pSpellPart = (ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);
            if (pSpellPart == NULL)
            {
                return;
            }

            // 取得第一个技能
            ISpell *pSpell = pSpellPart->getFirstSpell();
            while (pSpell != NULL)
            {
                int nSpellIcon = pSpell->getSpellData(SPDATA_ICON);
                int nSpellCoolID = pSpell->getSpellData(SPDATA_COOLID);

                int nTotalTime = 0;
                if (!pContext->bIsBreak)
                {
                    nTotalTime = pFreezeClient->GetFreezeShowTime(pContext->dwClassID, nSpellCoolID) + pFreezePart->GetChangeTime(pContext->dwClassID, nSpellCoolID);
                }
                else
                {
                    nTotalTime = pContext->dwFreezeTime;
                }

                if(nSpellCoolID == pContext->dwFreezeID)
                {
                    // 是否找到
                    bool bFind = false;

                    // 查找技能相关槽位信息
                    cmd_creature_set_slot SpellSlot;
                    map<int, SSlotSkillInfo>::iterator iter = m_SkillSlotMap.begin();
                    for (;iter != m_SkillSlotMap.end(); ++iter)
                    {
                        if (pSpell->getID() == iter->second.nSkillID)
                        {
                            SpellSlot.nSlotIndex = iter->second.nSlotIndex;
                            SpellSlot.nSlotLevel = iter->second.nSlotLevel;
                            SpellSlot.nSlotType = iter->second.nSlotType;

                            bFind = true;
                            break;
                        }
                    }

                    // 未找到
                    if (!bFind)
                    {
                        // 取得下一个技能
                        pSpell = pSpellPart->getNextSpell();
                        continue;
                    }

                    //组合信息发送到View交互
                    SpellSlot.slotview.bRemove	= 0;
                    SpellSlot.slotview.nType		= pContext->dwClassID;
                    SpellSlot.slotview.nId		= pSpell->getID();
                    SpellSlot.slotview.nViewType	= ViewType_LeftCircle;			// 显示类型（方式）：0101 1111
                    SpellSlot.slotview.nCurData	= pContext->dwFreezeTime;		// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
                    SpellSlot.slotview.nMaxData	= nTotalTime;					// 最大相关数据
                    SpellSlot.slotview.nIconID	= nSpellIcon;					// 图片ID
                    SpellSlot.slotview.bEnable	= 1;							// 是否可用
                    SpellSlot.slotview.bCanDrag = 0;							// 能否拖动
                    SpellSlot.slotview.bIsOnDrag	= 0;						// 拖动状态
                    SpellSlot.slotview.nSlotGroupType = 1;						// 槽位组类型	
                    sstrcpyn(SpellSlot.slotview.szSlotDesc, a2utf8(pSpell->getSpellDataStr(SPDATA_DESC)), sizeof(SpellSlot.slotview.szSlotDesc));
                    UpdateSlot(&SpellSlot);
                }

                // 取得下一个技能
                pSpell = pSpellPart->getNextSpell();
            }
        }
        break;
    case EFreeze_ClassID_Goods:			// 物品冷却
        {
            // 技能获取
            IFreezePart *pFreezePart = (IFreezePart*)m_pMaster->getEntityPart(PART_FREEZE);
            if (pFreezePart == NULL)
            {
                return;
            }
            int nTotalTime = 0;
            if (!pContext->bIsBreak)
            {
                nTotalTime = pFreezeClient->GetFreezeShowTime(pContext->dwClassID, pContext->dwFreezeID) + pFreezePart->GetChangeTime(pContext->dwClassID, pContext->dwFreezeID);
            }
            else
            {
                nTotalTime = pContext->dwFreezeTime;
            }

            // 查找物品相关槽位信息
            cmd_creature_set_slot GoodsSlot;
            int nGoodID = 0;
            map<int, SSlotGoodsInfo>::iterator iter = m_GoodsSlotMap.begin();
            for (;iter != m_GoodsSlotMap.end(); ++iter)
            {
                if (pContext->dwFreezeID == iter->second.nFreezeID)
                {
                    GoodsSlot.nSlotIndex	= iter->second.nSlotIndex;
                    GoodsSlot.nSlotLevel	= 0;
                    GoodsSlot.nSlotType		= iter->second.nSlotType;

                    //组合信息发送到View交互
                    GoodsSlot.slotview.bRemove		= 0;
                    GoodsSlot.slotview.nType		= pContext->dwClassID;
                    GoodsSlot.slotview.nId			= iter->second.nGoodID;
                    GoodsSlot.slotview.nViewType	= ViewType_LeftCircle;			// 显示类型（方式）：0101 1111
                    GoodsSlot.slotview.nCurData		= pContext->dwFreezeTime;		// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
                    GoodsSlot.slotview.nMaxData		= nTotalTime;					// 最大相关数据
                    GoodsSlot.slotview.nIconID		= 0;							// 图片ID
                    GoodsSlot.slotview.bEnable		= 1;							// 是否可用
                    if (iter->second.nSlotIndex == SPECIAL_SLOT_INDEX)				// 能否拖动
                    {
                        GoodsSlot.slotview.bCanDrag =	0;
                    }
                    else
                    {
                        GoodsSlot.slotview.bCanDrag = 1;
                    }
                    GoodsSlot.slotview.bIsOnDrag = 0;								// 拖动状态

                    GoodsSlot.slotview.nSlotGroupType = 1;						// 槽位组类型	
                    UpdateSlot(&GoodsSlot);
                }
            }
        }
        break;

    default:
        break;
    }
}

void CSlotLogicPart::UpdateSlotInfo(BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    if ( nLen!=sizeof(event_spell_change) || pszContext==0 )
        return ;
    event_spell_change * pContext = (event_spell_change*)pszContext;

    int viewID = ((IClientEntity*)m_pMaster)->getViewID();

    // 技能获取
    IFreezePart *pFreezePart = (IFreezePart*)m_pMaster->getEntityPart(PART_FREEZE);
    if (pFreezePart == NULL)
    {
        ErrorLn(__FUNCTION__": pFreezePart == NULL");
        return;
    }
    // 初始化槽位信息 技能相关
    ISpellPart *pSpellPart = (ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        ErrorLn(__FUNCTION__": pSpellPart == NULL");
        return;
    }
    ISpell *pSpell = pSpellPart->findSpell(pContext->nSpellID);
    if (pSpell == NULL)
    {
        ErrorLn(__FUNCTION__": pSpell == NULL, nSpellID=" << pContext->nSpellID);
        return;
    }

    int nTotalTime = 0; 
    int nSpellIcon = pSpell->getSpellData(SPDATA_ICON);
    int nSpellFreezeTime = pFreezePart->GetFreezeLeftTime(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID));
    int nChangeTime = pFreezePart->GetChangeTime(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID));
    nTotalTime = gClientGlobal->getFreezeClient()->GetFreezeShowTime(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID));
    nTotalTime = nTotalTime + nChangeTime;

    bool bFind = false;

    cmd_creature_set_slot SpellSlot;
    map<int, SSlotSkillInfo>::iterator iter = m_SkillSlotMap.begin();
    for (;iter != m_SkillSlotMap.end(); ++iter)
    {
        if (pSpell->getID() == iter->second.nSkillID)
        {
            SpellSlot.nSlotIndex = iter->second.nSlotIndex;
            SpellSlot.nSlotLevel = iter->second.nSlotLevel;
            SpellSlot.nSlotType = iter->second.nSlotType;

            bFind = true;
            break;
        }
    }
    if (!bFind)
    {
        return;
    }

    //组合信息发送到View交互
    SpellSlot.slotview.bRemove    = (int)(pContext->bAdd);
    SpellSlot.slotview.nType		= EFreeze_ClassID_Spell;
    SpellSlot.slotview.nId		= pSpell->getID();
    SpellSlot.slotview.nViewType	= ViewType_LeftCircle;		// 显示类型（方式）：0101 1111
    SpellSlot.slotview.nCurData	= nSpellFreezeTime;				// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
    SpellSlot.slotview.nMaxData	= nTotalTime;					// 最大相关数据
    SpellSlot.slotview.nIconID	= nSpellIcon;					// 图片ID
    SpellSlot.slotview.bEnable	= 1;							// 是否可用
    SpellSlot.slotview.bCanDrag =	0;							// 能否拖动
    SpellSlot.slotview.bIsOnDrag	= 0;						// 拖动状态
    SpellSlot.slotview.nSlotGroupType = 1;						// 槽位组类型
    sstrcpyn(SpellSlot.slotview.szSlotDesc, a2utf8(pSpell->getSpellDataStr(SPDATA_DESC)), sizeof(SpellSlot.slotview.szSlotDesc));
    UpdateSlot(&SpellSlot);
}

void CSlotLogicPart::UpdateSkillSlot(msg_slot_update_skill_slot * pSkillSlot)
{
    if (pSkillSlot == NULL)
    {
        return;
    }
    if (m_pMaster == NULL)
    {
        return;
    }
    IEventEngine * pEventEngine = gClientGlobal->getEventEngine();
    if(pEventEngine == NULL)
    {
        return;
    }

    if (pSkillSlot->bIsAdd == true)
    {
        SSlotSkillInfo upgradeSpell;
        upgradeSpell.nSlotIndex = pSkillSlot->nSlotIndex;
        upgradeSpell.nSlotType = pSkillSlot->nSlotType;
        upgradeSpell.nSkillID = pSkillSlot->nSkillID;
        upgradeSpell.nSlotLevel = pSkillSlot->nSlotLevel;
        m_SkillSlotMap[pSkillSlot->nSlotIndex] = upgradeSpell;

        EFFECT_CONTEXT context;
        context.pEntity = m_pMaster;

        EffectClient_SetSpellSlot SetSpellSlot;
        SetSpellSlot.nSlotIndex = pSkillSlot->nSlotIndex;
        SetSpellSlot.nSlotType = pSkillSlot->nSlotType;
        SetSpellSlot.nSlotLevel = pSkillSlot->nSlotLevel;
        SetSpellSlot.nSpellID = pSkillSlot->nSkillID;
        SetSpellSlot.nSpellFindType = SPELL_FIND_SPELL_FACTORY;
        SLOTPART_INNER_EFFECT(EffectClient_SetSpellSlot, SetSpellSlot, &context);

        // 发送槽位对应映射
        ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
        if ( NULL==pCenter )
        {
            return;
        }
        ISchemeSlotMapedShortcut* pSlotMapedShortcut = pCenter->getSchemeSlotMapedShortcut();
        if ( NULL==pSlotMapedShortcut )
        {
            return;
        }
        SSlotMapedShortcutScheme *pSlotMapedShortcutScheme =  pSlotMapedShortcut->getSlotMapedShortcutShemeInfo(pSkillSlot->nSlotIndex, pSkillSlot->nSlotType);
        if (pSlotMapedShortcutScheme == NULL)
        {
            return;
        }

        cmd_count cmd_map_count;
        cmd_map_count.nCount = 1;

		//获得该英雄槽位的推荐施法方式
		ISpellFactory* spellFactory = gClientGlobal->getSpellFactory();
		if (spellFactory == NULL)
		{
			ErrorLn("getSpellData failed, spellFactory == NULL");
			return;
		}

		if (pSkillSlot->nSkillID <= 0)
		{
			return;
		}

		SPELL_DATA* pSpellData = spellFactory->getSpellData(pSkillSlot->nSkillID);
		if (pSpellData == NULL)
		{
			return;
		}

		int nSpellIAdvanceUseType = pSpellData->nDefaultSelectType;
        //槽位单独数据
        cmd_creature_slot_maped_shortcut cmdSlotMapedShortcut;
        cmdSlotMapedShortcut.nSlotIndex = pSkillSlot->nSlotIndex;
        cmdSlotMapedShortcut.nSlotType = pSkillSlot->nSlotType;
        cmdSlotMapedShortcut.nAirCons = pSlotMapedShortcutScheme->nAirCons;
		cmdSlotMapedShortcut.nAdvanceUseType = nSpellIAdvanceUseType;
		cmdSlotMapedShortcut.isAdd = pSkillSlot->bIsAdd;
        sstrcpyn(cmdSlotMapedShortcut.szKeyName, cmdSlotMapedShortcut.szKeyName, sizeof(cmdSlotMapedShortcut.szKeyName));

        obuf ob_updates;
        ob_updates.push_back(&cmd_map_count, sizeof(cmd_map_count));
        ob_updates.push_back(&cmdSlotMapedShortcut, sizeof(cmdSlotMapedShortcut));
        // 通告显示层更新
        m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SLOT_MAPED_SHORTCUT, 0, 0, ob_updates.data(), ob_updates.size());
    }
    else
    {
        map<int, SSlotSkillInfo>::iterator iter = m_SkillSlotMap.find(pSkillSlot->nSlotIndex);
        if (iter != m_SkillSlotMap.end())
        {
            if(pSkillSlot->nSkillID == iter->second.nSkillID)
            {
                m_SkillSlotMap.erase(iter);
            }
        }

        // 发送槽位对应映射
        ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
        if ( NULL==pCenter )
        {
            return;
        }

        ISchemeSlotMapedShortcut* pSlotMapedShortcut = pCenter->getSchemeSlotMapedShortcut();
        if ( NULL==pSlotMapedShortcut )
        {
            return;
        }

        SSlotMapedShortcutScheme *pSlotMapedShortcutScheme =  pSlotMapedShortcut->getSlotMapedShortcutShemeInfo(pSkillSlot->nSlotIndex, pSkillSlot->nSlotType);
        if (pSlotMapedShortcutScheme == NULL)
        {
            return;
        }

        cmd_count cmd_map_count;
        cmd_map_count.nCount = 1;

		//获得该英雄槽位的推荐施法方式
		ISpellFactory* spellFactory = gClientGlobal->getSpellFactory();
		if (spellFactory == NULL)
		{
			ErrorLn("getSpellData failed, spellFactory == NULL");
			return;
		}

		if (pSkillSlot->nSkillID <= 0)
		{
			return;
		}

		SPELL_DATA* pSpellData = spellFactory->getSpellData(pSkillSlot->nSkillID);
		if (pSpellData == NULL)
		{
			return;
		}

		int nSpellIAdvanceUseType = pSpellData->nDefaultSelectType;

        //槽位单独数据
        cmd_creature_slot_maped_shortcut cmdSlotMapedShortcut;
        cmdSlotMapedShortcut.nSlotIndex = pSkillSlot->nSlotIndex;
        cmdSlotMapedShortcut.nSlotType = pSkillSlot->nSlotType;
        cmdSlotMapedShortcut.nAirCons = pSlotMapedShortcutScheme->nAirCons;
		cmdSlotMapedShortcut.nAdvanceUseType = nSpellIAdvanceUseType;
		cmdSlotMapedShortcut.isAdd = pSkillSlot->bIsAdd;
        obuf ob_updates;
        ob_updates.push_back(&cmd_map_count, sizeof(cmd_map_count));
        ob_updates.push_back(&cmdSlotMapedShortcut, sizeof(cmdSlotMapedShortcut));
        // 通告显示层更新
        m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SLOT_MAPED_SHORTCUT, 0, 0, ob_updates.data(), ob_updates.size());
    }

    UID uid = m_pMaster->getUID();
    BYTE type = UID_2_TYPE(uid);

    // 槽位索引更新
    event_slotindex_update slotIndexUpdate;
    slotIndexUpdate.bAdd = pSkillSlot->bIsAdd;
    slotIndexUpdate.nSlotIndex = pSkillSlot->nSlotIndex;
    slotIndexUpdate.nSlotLevel = pSkillSlot->nSlotLevel;
    slotIndexUpdate.nSlotType = pSkillSlot->nSlotType;
    slotIndexUpdate.nID = pSkillSlot->nSkillID;
    // 槽位索引更新事件
    pEventEngine->FireExecute(EVENT_SLOTINDEX_UPDATE, type, uid, (char *)&slotIndexUpdate, sizeof(slotIndexUpdate));
}

void CSlotLogicPart::UpdateGoodsSlot(void * data,size_t len)
{
    // 更新槽位
    ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
    if ( NULL==pCenter )
    {
        return;
    }

    ISchemeSlotMapedShortcut* pSlotMapedShortcut = pCenter->getSchemeSlotMapedShortcut();
    if ( NULL==pSlotMapedShortcut )
    {
        return;
    }

    msg_entity_fight_good_list *pMsgGoodListHead = (msg_entity_fight_good_list *)data;
    int nCount = pMsgGoodListHead->nCount;
    PDBID idActor = pMsgGoodListHead->idActor;
    if(len != sizeof(msg_entity_fight_good)*nCount + sizeof(msg_entity_fight_good_list))
    {
        return;
    }

    msg_entity_fight_good *pFirstGood = (msg_entity_fight_good *)(pMsgGoodListHead + 1);

    cmd_count cmd_map_count;
    obuf obMapData;

    obuf obdata;
    cmd_war_persongoodsinfo_count cmdWarPersonGoodsInfoCount;
    cmdWarPersonGoodsInfoCount.nCount = nCount;
    cmdWarPersonGoodsInfoCount.idActor = idActor;
    obdata.push_back(&cmdWarPersonGoodsInfoCount, sizeof(cmdWarPersonGoodsInfoCount));

    //所有物品
    for (int i = 0; i < nCount; ++i)
    {        
        SSlotGoodsInfo PersonGoodsInfo;
        PersonGoodsInfo.nGoodID     = pFirstGood->nGoodID;
        PersonGoodsInfo.nSlotIndex  = pFirstGood->nSlotIndex;
        PersonGoodsInfo.nSlotType   = pFirstGood->nSlotType;
        PersonGoodsInfo.bCanUse     = pFirstGood->bCanUse;      // 能否主动使用(0不能，1能)
        PersonGoodsInfo.nCanUseTimes= pFirstGood->nCanUseTimes;	// 能使用次数
        PersonGoodsInfo.nFreezeID	= pFirstGood->nFreezeID;	// 使用冷却ID

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

        //显示层命令数据
        cmd_war_persongoodsinfo cmdPersonGoodsInfo;
        cmdPersonGoodsInfo.idActor      = idActor;
        cmdPersonGoodsInfo.uID          = pFirstGood->uUID;
        cmdPersonGoodsInfo.nGoodID      = pFirstGood->nGoodID;
        cmdPersonGoodsInfo.nSlotIndex   = pFirstGood->nSlotIndex;
        cmdPersonGoodsInfo.nSlotType    = pFirstGood->nSlotType;
        cmdPersonGoodsInfo.nCanUse      = pFirstGood->bCanUse == false? 0 : 1;  // 能否主动使用(0不能，1能)
        cmdPersonGoodsInfo.nCanUseTimes = pFirstGood->nCanUseTimes;	            // 能使用次数
        cmdPersonGoodsInfo.nFreezeID	= pFirstGood->nFreezeID;		        // 使用冷却ID

        // buff或者技能冷却ID
        cmdPersonGoodsInfo.nBuffID = pFirstGood->nBuffID;
        cmdPersonGoodsInfo.nSkillID = pFirstGood->nSkillID;

        obdata.push_back(&cmdPersonGoodsInfo, sizeof(cmdPersonGoodsInfo));

        // 发送槽位对应映射
        SSlotMapedShortcutScheme *pSlotMapedShortcutScheme =  pSlotMapedShortcut->getSlotMapedShortcutShemeInfo(pFirstGood->nSlotIndex, pFirstGood->nSlotType);
        if (pSlotMapedShortcutScheme == NULL)
        {
            continue;
        }
        cmd_creature_slot_maped_shortcut cmdSlotMapedShortcut;
        cmdSlotMapedShortcut.nSlotIndex = pFirstGood->nSlotIndex;
        cmdSlotMapedShortcut.nSlotType = pFirstGood->nSlotType;
        cmdSlotMapedShortcut.nAirCons = pSlotMapedShortcutScheme->nAirCons;
        if (pFirstGood->nGoodID != 0)
        {
            sstrcpyn(cmdSlotMapedShortcut.szKeyName, pSlotMapedShortcutScheme->szKeyName, sizeof(cmdSlotMapedShortcut.szKeyName));
        }


        ++cmd_map_count.nCount;
        obMapData.push_back(&cmdSlotMapedShortcut, sizeof(cmdSlotMapedShortcut));

        ++pFirstGood;
    }
    if (cmd_map_count.nCount > 0)
    {
        obuf ob_updates;
        ob_updates.push_back(&cmd_map_count, sizeof(cmd_map_count));
        ob_updates.push_back(obMapData.data(), obMapData.size() );
        // 通告显示层更新
        m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SLOT_MAPED_SHORTCUT, 0, 0, ob_updates.data(), ob_updates.size());
    }

    // 通告显示层更新
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SET_WAR_PERSON_GOOD_INFO,0, 0, obdata.data(), obdata.size());

    // 设置下发冷却相关
    SetGoodsFreezeInfo(data, len);
}

int CSlotLogicPart::GetSlotSkillID(SLOT_TYPE SlotType, int nIndex)
{
    switch (SlotType)
    {
    case SLOT_TYPE_SKILL:
        {
            map<int, SSlotSkillInfo>::iterator iter = m_SkillSlotMap.find(nIndex);
            if (iter != m_SkillSlotMap.end())
            {
                return iter->second.nSkillID;
            }
        }
        break;
    case SLOT_TYPE_GOODS:
        {
            map<int, SSlotGoodsInfo>::iterator iter = m_GoodsSlotMap.find(nIndex);
            if (iter != m_GoodsSlotMap.end())
            {
                return iter->second.nSkillID;
            }
        }
        break;
    }
    return 0;
}

bool CSlotLogicPart::GetSkillIDInfo(int SkillID, SLOT_TYPE &SlotType, int &nIndex)
{
    map<int, SSlotSkillInfo>::iterator iter = m_SkillSlotMap.begin();
    for (; iter != m_SkillSlotMap.end(); ++iter)
    {
        if (SkillID == iter->second.nSkillID)
        {
            nIndex = iter->first;
            SlotType = SLOT_TYPE_SKILL;
            return true;
        }
    }

    map<int, SSlotGoodsInfo>::iterator iterGood = m_GoodsSlotMap.begin();
    for (; iterGood != m_GoodsSlotMap.end(); ++iterGood)
    {
        if (SkillID == iterGood->second.nSkillID)
        {
            nIndex = iterGood->first;
            SlotType = SLOT_TYPE_GOODS;
            return true;
        }
    }

    SlotType = SLOT_TYPE_NONE;
    nIndex = 0;
    return false;
}

void CSlotLogicPart::SetGoodsFreezeInfo(void * data,size_t len)
{
    // 设置冷却信息
    if (data == NULL)
    {
        return;
    }
    msg_entity_fight_good_list *pMsgGoodListHead = (msg_entity_fight_good_list *)data;
    int nCount = pMsgGoodListHead->nCount;
    PDBID idActor = pMsgGoodListHead->idActor;
    if(len != sizeof(msg_entity_fight_good)*nCount + sizeof(msg_entity_fight_good_list))
    {
        return;
    }

    msg_entity_fight_good *pFirstGood = (msg_entity_fight_good *)(pMsgGoodListHead + 1);

    for (int i = 0; i < nCount; ++i)
    {
        //新加物品冷却下发
        if(pFirstGood->bIsNewAdd == true && pFirstGood->nFreezeID != 0)
        {
            // 技能获取
            IFreezePart *pFreezePart = (IFreezePart*)m_pMaster->getEntityPart(PART_FREEZE);
            if (pFreezePart == NULL)
            {
                return;
            }
            int nTotalTime = 0;
            int nFreezeTime = pFreezePart->GetFreezeLeftTime(EFreeze_ClassID_Goods, pFirstGood->nFreezeID);
            nTotalTime = gClientGlobal->getFreezeClient()->GetFreezeShowTime(EFreeze_ClassID_Goods, pFirstGood->nFreezeID);
            int nChangeTime = pFreezePart->GetChangeTime(EFreeze_ClassID_Goods, pFirstGood->nFreezeID);
            nTotalTime = nTotalTime + nChangeTime;

            // 查找物品相关槽位信息
            cmd_creature_set_slot GoodsSlot;
            int nGoodID = 0;
            map<int, SSlotGoodsInfo>::iterator iter = m_GoodsSlotMap.begin();
            for (;iter != m_GoodsSlotMap.end(); ++iter)
            {
                if (pFirstGood->nFreezeID == iter->second.nFreezeID && iter->second.nSlotIndex == pFirstGood->nSlotIndex)
                {
                    GoodsSlot.nSlotIndex	= iter->second.nSlotIndex;
                    GoodsSlot.nSlotLevel	= 0;
                    GoodsSlot.nSlotType		= iter->second.nSlotType;

                    //组合信息发送到View交互
                    GoodsSlot.slotview.bRemove		= 0;
                    GoodsSlot.slotview.nType		= EFreeze_ClassID_Goods;
                    GoodsSlot.slotview.nId			= iter->second.nGoodID;
                    GoodsSlot.slotview.nViewType	= ViewType_LeftCircle;			// 显示类型（方式）：0101 1111
                    GoodsSlot.slotview.nCurData		= nFreezeTime;					// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
                    GoodsSlot.slotview.nMaxData		= nTotalTime;					// 最大相关数据
                    GoodsSlot.slotview.nIconID		= 0;							// 图片ID
                    GoodsSlot.slotview.bEnable		= 1;							// 是否可用
                    if (iter->second.nSlotIndex == SPECIAL_SLOT_INDEX)				// 能否拖动
                    {
                        GoodsSlot.slotview.bCanDrag =	0;
                    }
                    else
                    {
                        GoodsSlot.slotview.bCanDrag = 1;
                    }
                    GoodsSlot.slotview.bIsOnDrag	= 0;							
                    GoodsSlot.slotview.nSlotGroupType = 1;							// 槽位组类型	
                    UpdateSlot(&GoodsSlot);
                    break;
                }
            }
        }

        ++pFirstGood;
    }
}

void CSlotLogicPart::UpdateViewSlotInfo()
{
    // 技能槽位更新
    // 取得冷却部件
    IFreezePart *pFreezePart = (IFreezePart*)m_pMaster->getEntityPart(PART_FREEZE);
    if (pFreezePart == NULL)
    {
        ErrorLn(__FUNCTION__"-- pFreezePart == NULL!");
        return;
    }

    // 初始化槽位信息 技能相关
    ISpellPart *pSpellPart = (ISpellPart *)m_pMaster->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        ErrorLn(__FUNCTION__"-- pSpellPart == NULL!");
        return;
    }


    cmd_creature_set_slot SpellSlot;
    map<int, SSlotSkillInfo>::iterator iterSkill = m_SkillSlotMap.begin();
    for (;iterSkill != m_SkillSlotMap.end(); ++iterSkill)
    {

        ISpell *pSpell = pSpellPart->findSpell(iterSkill->second.nSkillID);
        if (pSpell == NULL)
        {
            continue;
        }

        int nTotalTime = 0; 
        int nSpellIcon = pSpell->getSpellData(SPDATA_ICON);
        int nSpellFreezeTime = pFreezePart->GetFreezeLeftTime(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID));
        int nChangeTime = pFreezePart->GetChangeTime(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID));
        nTotalTime = gClientGlobal->getFreezeClient()->GetFreezeShowTime(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID));
        nTotalTime = nTotalTime + nChangeTime;

        if (pSpell->getID() == iterSkill->second.nSkillID)
        {
            SpellSlot.nSlotIndex = iterSkill->second.nSlotIndex;
            SpellSlot.nSlotLevel = iterSkill->second.nSlotLevel;
            SpellSlot.nSlotType = iterSkill->second.nSlotType;

            //组合信息发送到View交互
            SpellSlot.slotview.bRemove      = (int)(false);
            SpellSlot.slotview.nType		= EFreeze_ClassID_Spell;
            SpellSlot.slotview.nId		    = pSpell->getID();
            SpellSlot.slotview.nViewType	= ViewType_LeftCircle;		// 显示类型（方式）：0101 1111
            SpellSlot.slotview.nCurData	    = nSpellFreezeTime;				// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
            SpellSlot.slotview.nMaxData	    = nTotalTime;					// 最大相关数据
            SpellSlot.slotview.nIconID	    = nSpellIcon;					// 图片ID
            SpellSlot.slotview.bEnable	    = 1;							// 是否可用
            SpellSlot.slotview.bCanDrag     = 0;							// 能否拖动
            SpellSlot.slotview.bIsOnDrag	= 0;						// 拖动状态
            SpellSlot.slotview.nSlotGroupType = 1;						// 槽位组类型
            sstrcpyn(SpellSlot.slotview.szSlotDesc, a2utf8(pSpell->getSpellDataStr(SPDATA_DESC)), sizeof(SpellSlot.slotview.szSlotDesc));
            UpdateSlot(&SpellSlot);
            
        }
    }


    // 装备槽位更新
    if (m_GoodsSlotMap.size() > 0)
    {
        obuf256 oAllGoods;
        msg_entity_fight_good_list tmpGoodList;
        tmpGoodList.nCount = m_GoodsSlotMap.size();

        IEntityProperty *pEntityProperty = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            ErrorLn(__FUNCTION__": pEntityProperty == NULL");
            return;
        }
        tmpGoodList.idActor =  pEntityProperty->getProperty_Integer(PROPERTY_ID);
        oAllGoods<<tmpGoodList;
        //所有装备信息
        map<int, SSlotGoodsInfo>::iterator iterGood = m_GoodsSlotMap.begin();
        for (;iterGood != m_GoodsSlotMap.end(); ++iterGood)
        {
            msg_entity_fight_good tmpGood;
            tmpGood.uUID            = m_pMaster->getUID();
            tmpGood.nSlotIndex      = iterGood->second.nSlotIndex;
            tmpGood.nSlotType       = iterGood->second.nSlotType;

            tmpGood.nGoodID         = iterGood->second.nGoodID;
            tmpGood.nCanUseTimes    = iterGood->second.nCanUseTimes;
            tmpGood.bCanUse         = iterGood->second.bCanUse;
            // buff或者技能冷却ID
            tmpGood.nFreezeID       = iterGood->second.nFreezeID;
            tmpGood.nBuffID         = iterGood->second.nBuffID;
            tmpGood.nSkillID        = iterGood->second.nSkillID;
            tmpGood.bIsNewAdd       = iterGood->second.bIsNewAdd;

            oAllGoods<<tmpGood;
        }
        UpdateGoodsSlot(oAllGoods.data(), oAllGoods.size());
    }
}

void CSlotLogicPart::EnterViewSendGoodsRecord()
{
    if (m_enterViewGoodsRecord.size() <= 0)
    {
        return;
    }
    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_WAR_MOBA_GOODS_INFO,0, 0, m_enterViewGoodsRecord.data(), m_enterViewGoodsRecord.size());
}

void CSlotLogicPart::restore()
{

}
