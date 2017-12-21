/*******************************************************************
** 文件名:	EffectBLDSummon.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	6/29/2016
** 版  本:	1.0
** 描  述:	效果-布兰登召唤效果
需求描述：http://172.16.0.120/redmine/issues/2043
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"
#include "IWarService.h"
#include "WarHelper.h"

class CEffectBLDSummon : public IEffectEx,public IEventExecuteSink, public TimerHandler
{
public:
	typedef  EffectServer_BLDSummon    SCHEME_DATA;

	CEffectBLDSummon( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_uidMonster(0),m_bAddEnterBuff(false)
	{
	}

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;
        UID uid = m_pEntity->getUID();

        MonsterHelper helper(uid);
        INPCService * pNpcService = helper.m_ptr;		
        if (pNpcService == NULL)
        {
            return false;
        }

        DWORD dwSceneID = m_pEntity->getSceneID();
        SceneLogicEventHelper eventHelper(dwSceneID);
        IEventEngine *pGlobalEvent = eventHelper.m_ptr;
        if (pGlobalEvent == NULL)
        {
            ErrorLn(_GT("CEffectBLDSummon Start 取得全局事件失败，问题严重！"));
            return false;
        }

		// 注册伤害事件
		g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DAMAGE, "CEffectBLDSummon");

        // 取得朝向角度
        Vector3 angle = m_pEntity->getAngle();
        Vector3 vLoc = m_pEntity->getLocation();

        CreateNpcContext NpcContext;
        NpcContext.nCamp = m_pEntity->getIntProperty(PROPERTY_CAMP);
        NpcContext.Angle = angle;
        NpcContext.nWarID = getWarID(uid);
        NpcContext.uidCreator=uid;
        NpcContext.nAIID = -1;

		// 设置PK信息
		__IPlayerKillPart* pPKPart = (__IPlayerKillPart*)m_pEntity->getEntityPart(PART_PK);
		if (pPKPart)
		{
			NpcContext.nPKType = pPKPart->getPKType();
			NpcContext.nPKState = pPKPart->getPKState();
		}

        // 设置AI
        int nWarType = 0;
        do 
        {
            if(NpcContext.nWarID < 0)
            {
                break;
            }
            WarHelper helper(NpcContext.nWarID);
            IWarService* pWarService = helper.m_ptr;
            if(pWarService == NULL)
            {
                break;
            }

            nWarType = pWarService->getWarTypeId();
        } while (false);

        ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (pSchemeCenter != NULL)
        {
            ISchemeAIIDTable* pAITable = pSchemeCenter->getAISchemeIDTable();
            if(pAITable != NULL)
            {
                NpcContext.nAIID = pAITable->getNpcAISchemeID(nWarType, m_data.nMonsterID);
            }
        }

        // 数据长度
        int nLen = sizeof(NpcContext);

        m_uidMonster = pNpcService->createNPC(m_data.nMonsterID, vLoc, NpcContext);

        // 注册召唤怪物受伤事件
        pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, m_uidMonster, "CEffectBLDSummon");
        // 注册怪物死亡事件
        pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, m_uidMonster, "CEffectBLDSummon");

        g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectBLDSummon");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        g_EHelper.KillTimer(0, this);
        g_EHelper.KillTimer(1, this);

		if (m_pEntity != 0)
		{
			// 注销伤害事件
			g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DAMAGE);

            UID uidSelf = m_pEntity->getUID();
            if (m_bAddEnterBuff)
            {
                if (m_data.nEnterBuffID > 0 && m_data.nEnterBuffLevel > 0)
                {
                    RemoveBuff(uidSelf, m_data.nEnterBuffID, uidSelf, uidSelf);
                }
            }
            else
            {
                if (m_data.nLevelBuffID > 0 && m_data.nLevelBuffLevel > 0)
                {
                    RemoveBuff(uidSelf, m_data.nLevelBuffID, uidSelf, uidSelf);
                }
            }

			m_pEntity = 0;
		}

        if (!isInvalidUID(m_uidMonster))
        {
            DWORD dwSceneID = UID_2_SCENE(m_uidMonster);
            SceneLogicEventHelper eventHelper(dwSceneID);
            IEventEngine *pGlobalEvent = eventHelper.m_ptr;
            if (pGlobalEvent != NULL)
            {
                // 注销召唤怪物受伤事件
                pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, m_uidMonster);
                // 注销怪物死亡事件
                pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, m_uidMonster);
            }

            MonsterHelper helper(m_uidMonster);
            INPCService * pNpcService = helper.m_ptr;		
            if (pNpcService != NULL)
            {
                pNpcService->destroyNPC(m_uidMonster);
            }
        }
        m_uidMonster = 0;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectBLDSummon(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
        case EVENT_GAME_ENTITY_DAMAGE:	// 伤害事件
		case EVENT_ENTITY_DAMAGE:	// 伤害事件
			{
                if (m_pEntity == NULL)
                {
                    return;
                }

				if (pszContext == NULL || nLen != sizeof(event_entity_damage))
				{
					return;
				}
                event_entity_damage *pDamage = (event_entity_damage *)pszContext;

                // 召唤怪受伤
                SummonDamage(pDamage);

                // 布兰登受伤害
                BLDDamage(pDamage);
			}
			break;

        case EVENT_GAME_ENTITY_DIE:
            {
                if (m_pEntity == NULL)
                {
                    return;
                }

                __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
                if (pBuffPart == NULL)
                {
                    return;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_die))
                {
                    return;
                }
                event_entity_die *pDie = (event_entity_die *)pszContext;

                DWORD dwSceneID = UID_2_SCENE(m_uidMonster);
                SceneLogicEventHelper eventHelper(dwSceneID);
                IEventEngine *pGlobalEvent = eventHelper.m_ptr;
                if (pGlobalEvent != NULL)
                {
                    // 注销召唤怪物受伤事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, m_uidMonster);
                    // 注销怪物死亡事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, m_uidMonster);
                }
                m_uidMonster = 0;
                
                if (m_data.nKillSummonBuffID > 0 && m_data.nKillSummonBuffLevel > 0)
                {
                    pBuffPart->Add(m_data.nKillSummonBuffID, m_data.nKillSummonBuffLevel, pDie->uidOperator);
                }
            }
            break;
		default:
			break;
		}
	}

    virtual void OnTimer( unsigned long dwTimerID )
	{
		if (m_pEntity == NULL)
		{
			return;
		}

        if (dwTimerID == 0)
        {
            // 死亡状态，不再加范围BUFF
            if (m_pEntity->getState() == ENTITY_STATE_DIE)
            {
                return;
            }

            if (isInvalidUID(m_uidMonster))
            {
                return;
            }

            // 自己UID
            UID uidSelf = m_pEntity->getUID();
            // 取得自己位置
            Vector3 vSelfLoc = m_pEntity->getLocation();
            // 怪物位置
            Vector3 vMonsterLoc = getLocation(m_uidMonster);

            if (vSelfLoc.checkDistance(vMonsterLoc, m_data.fDistance))
            {
                if (!m_bAddEnterBuff)
                {
                    m_bAddEnterBuff = true;
                    // 移除离开buff
                    if (m_data.nLevelBuffID > 0 && m_data.nLevelBuffLevel > 0)
                    {
                        RemoveBuff(uidSelf, m_data.nLevelBuffID, uidSelf, uidSelf);
                    }
                    // 增加进入buff
                    if (m_data.nEnterBuffID > 0 && m_data.nEnterBuffLevel > 0)
                    {
                        AddBuff(uidSelf, m_data.nEnterBuffID, m_data.nEnterBuffLevel, uidSelf);
                    }
                }
            }
            else
            {
                if (m_bAddEnterBuff)
                {
                    m_bAddEnterBuff = false;
                    // 移除进入buff
                    if (m_data.nEnterBuffID > 0 && m_data.nEnterBuffLevel > 0)
                    {
                        RemoveBuff(uidSelf, m_data.nEnterBuffID, uidSelf, uidSelf);
                    }
                    // 增加离开buff
                    if (m_data.nLevelBuffID > 0 && m_data.nLevelBuffLevel > 0)
                    {
                        AddBuff(uidSelf, m_data.nLevelBuffID, m_data.nLevelBuffLevel, uidSelf);
                    }
                }
            }
        }
        else if (dwTimerID == 1)
        {
            if (isInvalidUID(m_uidMonster))
            {
                return;
            }

            // 取得怪物点
            Vector3 vLoc = getLocation(m_uidMonster);

            // 取得怪物血量
            int nHp = getProperty_Integer(m_uidMonster, PROPERTY_HP);
            int nMaxHp = getProperty_Integer(m_uidMonster, PROPERTY_MAX_HP);

            if (nMaxHp > 0)
            {
                // 取得自己最大血量
                int nMeMaxHp = m_pEntity->getIntProperty(PROPERTY_MAX_HP);
                // 算出自己当前血量
                int nMeHp = (int)(nMeMaxHp * (float)nHp / nMaxHp);
                // 设置自己血量
                m_pEntity->setIntProperty(PROPERTY_HP, nMeHp);
            }

            g_EHelper.Transport(m_pEntity, vLoc, true, 0, INT_MAX, TRANSPORT_REASON_SET_POS);

            DWORD dwSceneID = UID_2_SCENE(m_uidMonster);
            SceneLogicEventHelper eventHelper(dwSceneID);
            IEventEngine *pGlobalEvent = eventHelper.m_ptr;
            if (pGlobalEvent != NULL)
            {
                // 注销召唤怪物受伤事件
                pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, m_uidMonster);
                // 注销怪物死亡事件
                pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, m_uidMonster);
            }

            // 删除怪物
            MonsterHelper helper(m_uidMonster);
            INPCService * pNpcService = helper.m_ptr;		
            if (pNpcService != NULL)
            {
                pNpcService->destroyNPC(m_uidMonster);
            }
            m_uidMonster = 0;
        }
	}

    // 召唤怪受伤
    void SummonDamage(event_entity_damage *pDamage)
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        if (pDamage == NULL)
        {
            return;
        }

        if (isInvalidUID(m_uidMonster))
        {
            return;
        }

        // 不是召唤怪物
        if (m_uidMonster != pDamage->uidTarget)
        {
            return;
        }

        __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
        if (pEntityEvent == NULL)
        {
            return;
        }

        event_entity_warrec_damage warRecDamage;
        warRecDamage.nID = pDamage->nID;
        warRecDamage.uidOperator = pDamage->uidOperator;
        warRecDamage.uidTarget = m_pEntity->getUID();
        warRecDamage.nDamageHP = pDamage->nDamageHP;
        warRecDamage.nDamageType = pDamage->nDamageType;
        warRecDamage.nSpellEffectType = pDamage->nSpellEffectType;
        warRecDamage.bDead = pDamage->bDead;
        warRecDamage.nInitialDamageHP = pDamage->nInitialDamageHP;
        warRecDamage.nAttackResultType = pDamage->nAttackResultType;
        warRecDamage.bRebound = pDamage->bRebound;
        pEntityEvent->FireExecute(EVENT_GAME_ACTOR_WAR_REC_DAMAGE, (char *)&warRecDamage, sizeof(warRecDamage));
    }

    // 布兰登伤害
    void BLDDamage(event_entity_damage *pDamage)
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
        if (pBuffPart == NULL)
        {
            return;
        }

        if (pDamage == NULL)
        {
            return;
        }

        // 没死不处理
        if (!pDamage->bDead)
        {
            return;
        }

        // 不是自己和创建怪物,不处理
        UID uid = m_pEntity->getUID();
        if (uid != pDamage->uidTarget)
        {
            return;
        }

        if (isInvalidUID(m_uidMonster))
        {
            return;
        }

        // 设置不死亡
        pDamage->bDead = false;

        // 策划不能配置给自己设置BUFF里面删除本效果，应该在后续BUFF效果删除
        if (m_data.nBuffID > 0 && m_data.nBuffLevel > 0)
        {
            pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, uid);
        }

        if (m_data.nKillBuffID > 0 && m_data.nBuffLevel > 0)
        {
            SBuffContext BuffContext;
            BuffContext.nID = pDamage->nID;
            AddBuff(pDamage->uidOperator, m_data.nKillBuffID, m_data.nBuffLevel, pDamage->uidOperator, 0, &BuffContext, sizeof(BuffContext));
        }

        // 如果检测清除连杀符合条件
        if (pBuffPart->IsExist(m_data.nCheckClearBuffID, m_data.nCheckClearBuffLevel, uid))
        {
            // 清除检测除连杀BUFF
            pBuffPart->Remove(m_data.nCheckClearBuffID, uid, uid);

            // 战场里面要知道，所以发个事件，告诉战场清除
            __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
            if (pEntityEvent != NULL)
            {
                event_entity_clear_multi_kill clearMultiKill;
                clearMultiKill.uidOperator = uid;
                clearMultiKill.uidTarget = pDamage->uidOperator;
                pEntityEvent->FireExecute(EVENT_ENTITY_CLEAR_MULTI_KILL, (LPCSTR)&clearMultiKill, sizeof(clearMultiKill));
                pEntityEvent->FireExecute(EVENT_GAME_CLEAR_MULTI_KILL, (LPCSTR)&clearMultiKill, sizeof(clearMultiKill));
            }
        }
        
        g_EHelper.SetTimer(1, m_data.nTransportInterval, this, 1, "BLDDamage");
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

    UID                     m_uidMonster;

    bool                    m_bAddEnterBuff;
};
