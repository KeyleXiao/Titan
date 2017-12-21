/*******************************************************************
** �ļ���:	EffectCreateMonster.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/02/2015
** ��  ��:	1.0
** ��  ��:	Ч��-��������
********************************************************************/

#pragma once

#include "IEffect.h"
#include "IEntity.h"
#include "INPCService.h"
#include "EffectDef.h"
#include "ServiceHelper.h"
#include "EntityHelper.h"
#include "IPathFindService.h"
#include "PathFindHelper.h"
#include "IWarService.h"
#include "WarHelper.h"
#include "IPlayerKillPart.h"

using namespace EFFECT_SERVER;

class CEffectCreateMonster : public IEffectEx
{
public:
	typedef  EffectServer_CreateMonster    SCHEME_DATA;

	CEffectCreateMonster( SCHEME_DATA & data ) : m_data(data)
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

		PathFindHelper pathHelper(pEntity->getSceneID());
		IPathFindService *pPathService = pathHelper.m_ptr;
		if (pPathService == NULL)
		{
			return false;
		}

        MonsterHelper helper(uid);
		INPCService * pNpcService = helper.m_ptr;		
		if (pNpcService == NULL)
		{
			return false;
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
                ErrorLn("CEffectCreateMonster effect nID="<< GetID() <<"m_data.nTagetType"<<m_data.nTagetType);
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
                ErrorLn("CEffectCreateMonster effect nID="<< GetID() <<"nExtraAttackType error"<<m_data.nExtraAttackType);
                return false;
            }
            break;
        }

        // ����ȼ�(�������յȼ��������ȼ�+Ӱ������*���԰ٷֱ�)
        int nLevel = m_data.nBaseLevel + nAttack*m_data.nSpellAttackPCT;

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

		// �ҵ�����ĵ�
        pPathService->getHeight(&loc, loc.y);

        // ��ӪID
        int nCampID = m_data.nCampID;
        if(nCampID < 0)
        {
            // Ĭ���ٻ�����Ӫ
            nCampID = pEntity->getIntProperty(PROPERTY_CAMP);  
        }

		// �ͻ��˱���Z����ǰ����˳ʱ��ת������X��Z�Ե�
		float fAngleY = (float)(atan2(dir.x, dir.z)/fPI*180) + m_data.fAngle;
		Vector3 Angle(0,fAngleY,0);

		CreateNpcContext NpcContext;
		NpcContext.nCamp = nCampID;
		NpcContext.Angle = Angle;
        NpcContext.nWarID = getWarID(uid);
        NpcContext.uidCreator=uid;
        NpcContext.nAIID = -1;
        NpcContext.nLevel = nLevel;

		// ����PK��Ϣ
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

        // ���һ������ID
        std::vector<int> vMonsterID;
        std::vector<string> vMonsterIDStr; 
        StringHelper::split(vMonsterIDStr, m_data.nMonsterID, ';', " ");
        for (UINT nCount=0; nCount<vMonsterIDStr.size(); ++nCount)
        {
            vMonsterID.push_back(StringHelper::toInt(vMonsterIDStr[nCount]));
        }
        if(vMonsterID.empty())
        {
            ErrorLn(__FUNCTION__ << __LINE__ << ", parse monster id list failed:" << m_data.nMonsterID);
            return false;
        }
        int nRandMonsterID = vMonsterID[mt_range_rand(0, vMonsterID.size() - 1)];

        ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (pSchemeCenter != NULL)
        {
            ISchemeAIIDTable* pAITable = pSchemeCenter->getAISchemeIDTable();
            if(pAITable != NULL)
            {
                NpcContext.nAIID = pAITable->getNpcAISchemeID(nWarType, nRandMonsterID);
            }
        }

		// ���ݳ���
		int nLen = sizeof(NpcContext);
		
		for (int i=0; i<m_data.nBuildNum; ++i)
		{
			pNpcService->createNPC(nRandMonsterID, loc, NpcContext);
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
		return new CEffectCreateMonster(m_data);
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
        if (IsMainMap(nMapID))
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
