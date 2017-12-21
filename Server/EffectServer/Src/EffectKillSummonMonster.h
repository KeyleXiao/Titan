/*******************************************************************
** 文件名:	EffectKillSummonMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/19/2016
** 版  本:	1.0
** 描  述:	效果-击杀召唤怪物
            http://172.16.0.120/redmine/issues/1657
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;

class CEffectKillSummonMonster : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_KillSummonMonster    SCHEME_DATA;

	CEffectKillSummonMonster( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

        DWORD dwSceneID = m_pEntity->getSceneID();

        SceneLogicEventHelper eventHelper(dwSceneID);
        IEventEngine *pGlobalEvent = eventHelper.m_ptr;
        if (pGlobalEvent == NULL)
        {
            ErrorLn(_GT("CEffectKillSummonMonster Start 取得全局事件失败，问题严重！"));
            return false;
        }
        if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
        {
            // 注册英雄死亡事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0, "CEffectKillSummonMonster");
            // 注册怪物死亡事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0, "CEffectKillSummonMonster");
            // 注册Tank死亡事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_TANK, 0, "CEffectKillSummonMonster");
        }
        else
        {
            // 注册类型死亡事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, m_data.nEntityType, 0, "CEffectKillSummonMonster");
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            DWORD dwSceneID = m_pEntity->getSceneID();
            SceneLogicEventHelper eventHelper(dwSceneID);
            IEventEngine *pGlobalEvent = eventHelper.m_ptr;
            if (pGlobalEvent != NULL)
            {
                if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
                {
                    // 注销英雄死亡事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0);
                    // 注销怪物死亡事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0);
                    // 注销Tank死亡事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_TANK, 0);
                }
                else
                {
                    // 注册类型死亡事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, m_data.nEntityType, 0);
                }
            }

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectKillSummonMonster(m_data);
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
        case EVENT_GAME_ENTITY_DIE:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                IPathFindManager *pPathFindManager = gServerGlobal->getPathFindManager();
                if (pPathFindManager == NULL)
                {
                    break;
                }
                PathFindHelper pathHelper(m_pEntity->getSceneID());
                IPathFindService *pPathService = pathHelper.m_ptr;
                if (pPathService == NULL)
                {
                    break;
                }

                DWORD dwTick = getTickCountEx();
                if (dwTick < m_dwLastTick + m_data.nInterval)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_die))
                {
                    break;
                }
                event_entity_die *pDie = (event_entity_die *)pszContext;

                UID uidSelf = m_pEntity->getUID();
                if (isInvalidUID(uidSelf))
                {
                    break;
                }

                MonsterHelper helper(uidSelf);
                INPCService * pNpcService = helper.m_ptr;		
                if (pNpcService == NULL)
                {
                    break;
                }

                UID uidOperator = pDie->uidOperator;
                if (isInvalidUID(uidOperator))
                {
                    break;
                }

                UID uidTarget = pDie->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    break;
                }

                if (!isMonster(uidTarget))
                {
                    break;
                }
                // 取得怪物ID
                int nMonsterID = getProperty_Integer(uidTarget, PROPERTY_ID);
                if (nMonsterID == 0)
                {
                    break;
                }
                if (m_data.nMonsterID > 0 && m_data.nMonsterID != nMonsterID)
                {
                    break;
                }

                // 检测击杀目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nKillTargetFilter, uidOperator))
                {
                    break;
                }

                // 检测死亡目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nDieTargetFilter, uidTarget))
                {
                    break;
                }

                UID uidCheck = INVALID_UID;
                switch (m_data.nKillTagetLocType)
                {
                case KILL_TARGET_LOC_KILL:
                    {
                        uidCheck = uidOperator;
                    }
                    break;
                case KILL_TARGET_LOC_DIE:
                    {
                        uidCheck = uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectKillSummonMonster error nKillTagetLocType="<<m_data.nKillTagetLocType);
                        return;
                    }
                    break;
                }

                if (!CheckDistance(uidCheck, uidSelf))
                {
                    break;
                }

                // 取得目标位置
                Vector3 Loc = getLocation(uidTarget);

                // 找到地面的点
                pPathService->getHeight(&Loc, Loc.y);

                CreateNpcContext NpcContext;
                NpcContext.nCamp = m_pEntity->getIntProperty(PROPERTY_CAMP);
                NpcContext.nWarID = getWarID(uidSelf);
                NpcContext.uidCreator= uidSelf;
                NpcContext.nLevel = m_data.nSummonLevel;
                NpcContext.nAIID = -1;

				// 设置PK信息
				__IPlayerKillPart* pPKPart = (__IPlayerKillPart*)m_pEntity->getEntityPart(PART_PK);
				if (pPKPart)
				{
					NpcContext.nPKType = pPKPart->getPKType();
					NpcContext.nPKState = pPKPart->getPKState();
				}
                // 数据长度
                int nLen = sizeof(NpcContext);

                for (int i=0; i<m_data.nSummonCount; ++i)
                {
                    pNpcService->createNPC(m_data.nSummonMonsterID, Loc, NpcContext);
                }

                // 设置上次时间
                m_dwLastTick = dwTick;
            }
            break;
		default:
			break;
		}
	}

    bool CheckDistance(UID uidCheck, UID uidAddCheck)
    {
        // 小于0表示配置不需要检测距离
        if (m_data.fDieLocRange <= 0.01f)
        {
            return true;
        }

        Vector3 vCheckLoc = getLocation(uidCheck);
        Vector3 vAddCheckLoc = getLocation(uidAddCheck);
        if (!vCheckLoc.checkDistance(vAddCheckLoc, m_data.fDieLocRange))
        {
            return false;
        }

        return true;
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

    // 上次时间
    DWORD					m_dwLastTick;
};
