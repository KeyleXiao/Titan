/*******************************************************************
** 文件名:	EffectDieSummonMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/28/2016
** 版  本:	1.0
** 描  述:	效果-死亡召唤怪物
需求描述：http://172.16.0.120/redmine/issues/3364
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"

class CEffectDieSummonMonster : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_DieSummonMonster    SCHEME_DATA;

	CEffectDieSummonMonster( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

		// 注册实体死亡事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DIE, "CEffectDieSummonMonster");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销实体死亡事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DIE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectDieSummonMonster(m_data);
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
		case EVENT_ENTITY_DIE:	// 实体死亡事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_die))
				{
					return;
				}
				event_entity_die *pDie = (event_entity_die *)pszContext;

                UID uid = m_pEntity->getUID();
                MonsterHelper helper(uid);
                INPCService *pNpcService = helper.m_ptr;		
                if (pNpcService == NULL)
                {
                    return;
                }

                //添加buff目标
                UID uidTarget = INVALID_UID;
                switch (m_data.nMasterType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uidTarget = uid;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidTarget = pDie->uidTarget;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidTarget = pDie->uidOperator;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectDieSummonMonster error nMasterType="<<m_data.nMasterType);
                        return;
                    }
                    break;
                }
                if (isInvalidUID(uidTarget))
                {
                    return;
                }

                Vector3 vLoc;
                switch (m_data.nPosType)
                {
                    // 自己点
                case REPLICATION_MONSTER_POS_SELF:
                    {
                        vLoc = m_pEntity->getLocation();
                    }
                    break;
                    // 目标点
                case REPLICATION_MONSTER_POS_TARGET:
                    {
                        UID uidTarget = pDie->uidTarget;
                        if (isInvalidUID(uidTarget))
                        {
                            break;
                        }
                        vLoc = getLocation(uidTarget);
                    }
                    break;
                    // 操作者位置
                case REPLICATION_MONSTER_POS_OPERATOR:
                    {
                        UID uidOperator = pDie->uidOperator;
                        if (isInvalidUID(uidOperator))
                        {
                            break;
                        }
                        vLoc = getLocation(uidOperator);
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectDieSummonMonster effect nID="<< GetID() <<"m_data.nPosType"<<m_data.nPosType);
                        return;
                    }
                    break;
                }

                int nAttack = 0;
                switch (m_data.nExtraAttackType)
                {
                case EXTRAATTACK_PA:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_PA);
                    }
                    break;
                case EXTRAATTACK_MA:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_MA);
                    }
                    break;
                case EXTRAATTACK_LEVEL:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_LEVEL);
                    }
                    break;
                case EXTRAATTACK_HP:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_HP);
                    }
                    break;
                case EXTRAATTACK_MAX_HP:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                    }
                    break;
                case EXTRAATTACK_LOSE_HP:
                    {
                        int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
                        int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                        nAttack = nMaxHp-nHp;
                    }
                    break;
                case EXTRAATTACK_LOSE_HP_PCT:
                    {
                        int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
                        int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                        if (nMaxHp <= 0)
                        {
                            return;
                        }
                        float fPercent = (float)(nMaxHp-nHp)/nMaxHp;
                        nAttack = (int)(fPercent*EFFECT_RATE);
                    }
                    break;
                case EXTRAATTACK_MP:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_MP);
                    }
                    break;
                case EXTRAATTACK_MAX_MP:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
                    }
                    break;
                case EXTRAATTACK_LOSE_MP_PCT:
                    {
                        int nMp = getProperty_Integer(uidTarget, PROPERTY_MP);
                        int nMaxMp = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
                        if (nMaxMp <= 0)
                        {
                            return;
                        }
                        float fPercent = (float)(nMaxMp-nMp)/nMaxMp;
                        nAttack = (int)(fPercent*EFFECT_RATE);
                    }
                    break;
                case EXTRAATTACK_PD:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_PD);
                    }
                    break;
                case EXTRAATTACK_MD:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_MD);
                    }
                    break;
                case EXTRAATTACK_SD:
                    {
                        nAttack = getProperty_Integer(uidTarget, PROPERTY_SD);
                    }
                    break;
                case EXTRAATTACK_EXTRA_PA:
                    {
                        int nPA = getProperty_Integer(uidTarget, PROPERTY_PA);
                        int nBasePA = OnCalculate_Base(uidTarget, PROPERTY_BASE_PA);
                        nAttack = nPA-nBasePA;
                        if (nAttack < 0)
                        {
                            return;
                        }
                    }
                    break;
                case EXTRAATTACK_EXTRA_MA:
                    {
                        int nMA = getProperty_Integer(uidTarget, PROPERTY_MA);
                        int nBaseMA = OnCalculate_Base(uidTarget, PROPERTY_BASE_MA);
                        nAttack = nMA-nBaseMA;
                        if (nAttack < 0)
                        {
                            return;
                        }
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectAddSummonMonster effect nID="<< GetID() <<"nExtraAttackType error"<<m_data.nExtraAttackType);
                        return;
                    }
                    break;
                }

                // 算出等级(怪物最终等级＝基础等级+影响属性*属性百分比)
                int nLevel = m_data.nBaseLevel + nAttack*m_data.nSpellAttackPCT;

                CreateNpcContext NpcContext;
                NpcContext.nCamp = getProperty_Integer(uidTarget, PROPERTY_CAMP);
                NpcContext.nWarID = getWarID(uid);
                NpcContext.uidCreator= uidTarget;
                NpcContext.nAIID = -1;
                NpcContext.nLevel = nLevel;
				NpcContext.nPKType = getEntityPKType(uidTarget);
				NpcContext.nPKState = getEntityPKState(uidTarget);

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
                for (int i=0; i<m_data.nMonsterNum; ++i)
                {
                    pNpcService->createNPC(m_data.nMonsterID, vLoc, NpcContext);
                }
			}
			break;
		default:
			break;
		}
	}

    // 计算基础属性
    int OnCalculate_Base(UID uid, int nBaseID)
    {
        ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            return 0;
        }
        // 取得配置人物基础属性对象
        ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
        if (pSchemePersonBaseProp == NULL)
        {
            return 0;
        }

        int nPropType = BASE_PROP_TYPE;
        int nMapID = getMapID(uid);
        if (IsMainMap(nMapID) )
        {
            nPropType = BASE_PROP_HKEE_TYPE;
        }
        // 职业
        int nVocation = getProperty_Integer(uid, PROPERTY_VOCATION);
        SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
        if (pPersonBasePropScheme == NULL)
        {
            return 0;
        }

        // 属性索引
        int nPropIndex = -1;
        for (int nIndex=0; nIndex<PERSON_BASE_PROP_COUNT; ++nIndex)
        {
            if (pPersonBasePropScheme->sProp[nIndex].nPropID == nBaseID)
            {
                nPropIndex = nIndex;
                break;
            }
        }
        if (nPropIndex < 0 || nPropIndex >= PERSON_BASE_PROP_COUNT)
        {
            return 0;
        }

        // 等级
        int nLevel = getProperty_Integer(uid, PROPERTY_LEVEL);
        // 基础值 = 等级基础值 + (等级-1)*基础值成长率
        int nBase = (int)(pPersonBasePropScheme->sProp[nPropIndex].nBase + (nLevel-1)*pPersonBasePropScheme->sProp[nPropIndex].fGrowRate);

        return nBase;
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
