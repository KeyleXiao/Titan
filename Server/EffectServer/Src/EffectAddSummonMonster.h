/*******************************************************************
** 文件名:	EffectAddSummonMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/21/2015
** 版  本:	1.0
** 描  述:	效果-增加召唤怪物
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EntityHelper.h"
#include "IWarService.h"
#include "WarHelper.h"
#include "IPlayerKillPart.h"

using namespace EFFECT_SERVER;

class CEffectAddSummonMonster : public IEffectEx
{
public:
	typedef  EffectServer_AddSummonMonster    SCHEME_DATA;

	CEffectAddSummonMonster( SCHEME_DATA & data ) : m_data(data)
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}

        __IEntity *pEntity = context->pEntity;

		UID uid = pEntity->getUID();
		if (isInvalidUID(uid))
		{
			return false;
		}

		__ISpellPart *pSpellPart = (__ISpellPart *)pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
		{
			return false;
		}

		ISpell *pSpell = pSpellPart->findSpell(context->nID);
		if ( pSpell == 0 )
		{
			return false;
		}

        MonsterHelper helper(uid);
		INPCService *pNpcService = helper.m_ptr;		
		if (pNpcService == NULL)
		{
			return false;
		}
		Vector3 loc;
		switch (m_data.nType)
		{
			// 配置位置
		case MONSTER_POS_STATIC:
			{
				loc = Vector3(m_data.fPosx, m_data.fPosy, m_data.fPosz);
			}
			break;
			// 自己周围
		case MONSTER_POS_SELF:
			{
				loc = pEntity->getLocation();
			}
			break;
			// 目标ID周围
		case MONSTER_POS_TAGETID:
			{
				UID uidTarget = context->uidTarget;
				if (isInvalidUID(uidTarget))
				{
					break;
				}
				loc = getLocation(uidTarget);
			}
			break;
			// 目标点周围
		case MONSTER_POS_TAGETILE:
			{
				loc = context->ptTargetTile;
			}
			break;
		default:
			break;
		}

		// 取得自己位置
		Vector3 myLoc = pEntity->getLocation();
		// 方向向量单位化
		Vector3 dir = (loc - myLoc).normalize();
		// 如果取自己周围点，算出的方向是0向量，应该算面向
		if (m_data.nType == MONSTER_POS_SELF)
		{
			// 取得朝向角度
			Vector3 angle = pEntity->getAngle();

			float fAngleY = (float)(angle.y*fPI/180);

			dir.x = sin(fAngleY);
			dir.y = 0;
			dir.z = cos(fAngleY);
		}

		// 目标方向
		switch (m_data.nDirType)
		{
			// 原点
		case SPELL_DIR_ORIGIN:
			{
				// 原点不处理
			}
			break;
			// 前方
		case SPELL_DIR_FRONT:
			{
				loc += dir*m_data.fDistance;
			}
			break;
			// 后方
		case SPELL_DIR_REAR:
			{
				loc -= dir*m_data.fDistance;
			}
			break;
		default:
			break;
		}

        UID uidTarget = INVALID_UID;
        switch (m_data.nTagetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidTarget = pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidTarget = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidTarget = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectAddSummonMonster effect nID="<< GetID() <<"m_data.nTagetType"<<m_data.nTagetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidTarget))
        {
            return false;
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
                    return false;
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
                    return false;
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
                    return false;
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
                    return false;
                }
            }
            break;
        default:
            {
                ErrorLn("CEffectAddSummonMonster effect nID="<< GetID() <<"nExtraAttackType error"<<m_data.nExtraAttackType);
                return false;
            }
            break;
        }

        // 取得朝向角度
        Vector3 angle = pEntity->getAngle();
        float fAngleY = (float)(angle.y*fPI/180);
        Vector3 vMonsterDir;
        vMonsterDir.x = sin(fAngleY);
        vMonsterDir.y = 0;
        vMonsterDir.z = cos(fAngleY);
        switch (m_data.nMonsterDirType)
        {
        case DIRECTION_FRONT:
            {
                // 前方不出理
            }
            break;
        case DIRECTION_BACK:
            {
                vMonsterDir *= -1;
            }
            break;
        case DIRECTION_LEFT:
            {
                Vector3 vUp(0,1,0);
                vMonsterDir = vMonsterDir.getCrossProduct(vUp);
            }
            break;
        case DIRECTION_RIGHT:
            {
                Vector3 vUp(0,1,0);
                vMonsterDir = vUp.getCrossProduct(vMonsterDir);
            }
            break;
        default:
            {
                ErrorLn("CEffectAddSummonMonster error nMonsterDirType="<<m_data.nMonsterDirType);
                return false;
            }
            break;
        }

        // 算出等级(怪物最终等级＝基础等级+影响属性*属性百分比)
        int nLevel = m_data.nBaseLevel + nAttack*m_data.nSpellAttackPCT;
		
		CreateNpcContext NpcContext;
		NpcContext.nCamp = pEntity->getIntProperty(PROPERTY_CAMP);
        NpcContext.nWarID = getWarID(uid);
        NpcContext.uidCreator=uid;
        NpcContext.nAIID = -1;
        NpcContext.nLevel = nLevel;
        NpcContext.Angle = vMonsterDir.toEulerAngles();

		__IPlayerKillPart* pPKPart = (__IPlayerKillPart*)pEntity->getEntityPart(PART_PK);
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
		for (int i=0; i<m_data.nBuildNum; ++i)
		{
			UID uidMonster = pNpcService->createNPC(m_data.nMonsterID, loc, NpcContext);
			if (isInvalidUID(uidMonster))
			{
				continue;
			}

            // 大于0才设置到技能数据里面
            if (m_data.nMaxCount > 0)
            {
			    // 增加召唤实体
			    pSpell->addEntity(uidMonster, m_data.nMaxCount);
            }
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectAddSummonMonster(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
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
        if ( IsMainMap(nMapID ) )
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
	SCHEME_DATA               m_data;
};
