/*******************************************************************
** �ļ���:	EffectAddSummonMonster.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/21/2015
** ��  ��:	1.0
** ��  ��:	Ч��-�����ٻ�����
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

    // ��ȡЧ��ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// Ч������
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
			// ����λ��
		case MONSTER_POS_STATIC:
			{
				loc = Vector3(m_data.fPosx, m_data.fPosy, m_data.fPosz);
			}
			break;
			// �Լ���Χ
		case MONSTER_POS_SELF:
			{
				loc = pEntity->getLocation();
			}
			break;
			// Ŀ��ID��Χ
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
			// Ŀ�����Χ
		case MONSTER_POS_TAGETILE:
			{
				loc = context->ptTargetTile;
			}
			break;
		default:
			break;
		}

		// ȡ���Լ�λ��
		Vector3 myLoc = pEntity->getLocation();
		// ����������λ��
		Vector3 dir = (loc - myLoc).normalize();
		// ���ȡ�Լ���Χ�㣬����ķ�����0������Ӧ��������
		if (m_data.nType == MONSTER_POS_SELF)
		{
			// ȡ�ó���Ƕ�
			Vector3 angle = pEntity->getAngle();

			float fAngleY = (float)(angle.y*fPI/180);

			dir.x = sin(fAngleY);
			dir.y = 0;
			dir.z = cos(fAngleY);
		}

		// Ŀ�귽��
		switch (m_data.nDirType)
		{
			// ԭ��
		case SPELL_DIR_ORIGIN:
			{
				// ԭ�㲻����
			}
			break;
			// ǰ��
		case SPELL_DIR_FRONT:
			{
				loc += dir*m_data.fDistance;
			}
			break;
			// ��
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

        // ȡ�ó���Ƕ�
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
                // ǰ��������
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

        // ����ȼ�(�������յȼ��������ȼ�+Ӱ������*���԰ٷֱ�)
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

        // ����AI
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

		// ���ݳ���
		int nLen = sizeof(NpcContext);
		for (int i=0; i<m_data.nBuildNum; ++i)
		{
			UID uidMonster = pNpcService->createNPC(m_data.nMonsterID, loc, NpcContext);
			if (isInvalidUID(uidMonster))
			{
				continue;
			}

            // ����0�����õ�������������
            if (m_data.nMaxCount > 0)
            {
			    // �����ٻ�ʵ��
			    pSpell->addEntity(uidMonster, m_data.nMaxCount);
            }
		}

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectAddSummonMonster(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

    // �����������
    int OnCalculate_Base(UID uid, int nBaseID)
    {
        ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            return 0;
        }
        // ȡ����������������Զ���
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
        // ְҵ
        int nVocation = getProperty_Integer(uid, PROPERTY_VOCATION);
        SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
        if (pPersonBasePropScheme == NULL)
        {
            return 0;
        }

        // ��������
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

        // �ȼ�
        int nLevel = getProperty_Integer(uid, PROPERTY_LEVEL);
        // ����ֵ = �ȼ�����ֵ + (�ȼ�-1)*����ֵ�ɳ���
        int nBase = (int)(pPersonBasePropScheme->sProp[nPropIndex].nBase + (nLevel-1)*pPersonBasePropScheme->sProp[nPropIndex].fGrowRate);

        return nBase;
    }

private:
	SCHEME_DATA               m_data;
};
