/*******************************************************************
** �ļ���:	EffectUseSpell.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/02/2015
** ��  ��:	1.0
** ��  ��:	Ч��-ʹ�ü���
			ͨ��ʹ�ü���Ч������ʵ���ڼ����¼���ʹ�ü��ܣ��Ӷ�ʵ�ֺ�������ʹ��
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ISpell.h"
#include "IAttackObject.h"
#include "SpellContextDef.h"

using namespace EFFECT_SERVER;

class CEffectUseSpell : public IEffectEx
{
public:
	typedef  EffectServer_UseSpell    SCHEME_DATA;

	CEffectUseSpell( SCHEME_DATA & data ) : m_data(data)
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

		__ISpellPart *pSpellPart = (__ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
		{
			return false;
        }

        UID uidOperator = context->pEntity->getUID();

        Vector3 ptStartTile;
        Vector3 ptTargetTile;
		switch (m_data.nUseSpellPosType)
		{
		case USE_SPELL_POS_TILE:
			{
				ptStartTile = context->ptTargetTile;

				// ȡ�ù����������
				DWORD dwAttackSerialID = context->dwAttackSerialID;
				if (dwAttackSerialID == 0)
				{
					break;
				}

				IAttackObject *pAttackObject = pSpellPart->getAttackObject(dwAttackSerialID);
				if (pAttackObject == NULL)
				{
					break;
				}

				// ���µ�
				ptStartTile = pAttackObject->getTargetTile();
                ptTargetTile = ptStartTile;
                // ����Ŀ���
                setTargetTile(uidOperator, context->vSelfLoc, ptTargetTile);
			}
			break;
		case USE_SPELL_POS_SELF:
			{
				ptStartTile = context->pEntity->getLocation();
                ptTargetTile = ptStartTile;
                // ����Ŀ���
                setTargetTile(uidOperator, context->vSelfLoc, ptTargetTile);
			}
			break;
		case USE_SPELL_POS_TARGET:
			{
				UID uidTarget = context->uidTarget;
				if (isInvalidUID(uidTarget))
				{
					return false;
				}

				ptStartTile = getLocation(uidTarget);
                ptTargetTile = ptStartTile;
                // ����Ŀ���
                setTargetTile(uidOperator, context->vSelfLoc, ptTargetTile);
			}
			break;
        case USE_SPELL_POS_ANGLE:
            {
                // ����ID
                int nSceneID = UID_2_SCENE(uidOperator);
                PathFindHelper helper(nSceneID);
                IPathFindService *pPathService = helper.m_ptr;
                if (pPathService == NULL)
                {
                    return false;
                }

                // ȡ�ó���Ƕ�
                Vector3 angle = context->pEntity->getAngle();
                float fAngleY = (float)(angle.y*fPI/180);

                // ����
                Vector3 dirNomalize(0,0,0);
                dirNomalize.x = sin(fAngleY);
                dirNomalize.y = 0;
                dirNomalize.z = cos(fAngleY);

                // ��������
                Vector3 vEndAngle;
                switch (m_data.nDirectionType)
                {
                case DIRECTION_FRONT:
                    {
                        vEndAngle = dirNomalize;
                    }
                    break;
                case DIRECTION_BACK:
                    {
                        vEndAngle = dirNomalize*(-1.0);
                    }
                    break;
                case DIRECTION_LEFT:
                    {
                        Vector3 vUp(0,1,0);
                        vEndAngle = dirNomalize.getCrossProduct(vUp);
                    }
                    break;
                case DIRECTION_RIGHT:
                    {
                        Vector3 vUp(0,1,0);
                        vEndAngle = vUp.getCrossProduct(dirNomalize);
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectUseSpell error nDirectionType="<<m_data.nDirectionType);
                        return false;
                    }
                    break;
                }

                ptStartTile = context->pEntity->getLocation();
                // ������λ��
                ptTargetTile = pPathService->getRaycast(ptStartTile, vEndAngle.normalize(), m_data.fDistance);
            }
            break;
        case USE_SPELL_POS_LAST_SPELL:
            {
                // ���µ�
                ptStartTile = context->ptStartTile;
                ptTargetTile = context->ptTargetTile;
                // ����Ŀ���
                setTargetTile(uidOperator, context->vSelfLoc, ptTargetTile);
            }
            break;
		default:
			{
				ErrorLn("CEffectUseSpell error nUseSpellPosType="<<m_data.nUseSpellPosType);
				return false;
			}
			break;
		}

        DWORD dwStartTick = getTickCountEx();
		// ����ʩ���ֳ�
		SPELL_CONTEXT spellContext;
		spellContext.CC.pEntity = context->pEntity;
		spellContext.CC.uidOperator = uidOperator;
		spellContext.CC.uidTarget = context->uidTarget;
        spellContext.CC.ptStartTile = ptStartTile;
		spellContext.CC.ptTargetTile = ptTargetTile;
		spellContext.CC.nID = m_data.nSpellID;
		spellContext.CC.dwStartTick = dwStartTick;
		spellContext.CC.dwUpdateFrame = 0;
		spellContext.EC.pEntity = context->pEntity;
		spellContext.EC.uidOperator = uidOperator;
		spellContext.EC.uidTarget = context->uidTarget;
        spellContext.EC.ptStartTile = ptStartTile;
		spellContext.EC.ptTargetTile = ptTargetTile;
		spellContext.EC.nID = m_data.nSpellID;
        spellContext.EC.vSelfLoc = context->pEntity->getLocation();
        spellContext.EC.dwStartTick = dwStartTick;

		// ʩ������
		pSpellPart->castSpell(&spellContext);

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectUseSpell(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

    void setTargetTile(UID uidOperator, Vector3 vStartTile, Vector3 &ptTargetTile)
    {
        if (!m_data.nUseDirPos)
        {
            return;
        }

        if (isInvalidUID(uidOperator) || equalto0(vStartTile.getLength()) || equalto0(ptTargetTile.getLength()))
        {
            return;
        }

        // ����ID
        int nSceneID = UID_2_SCENE(uidOperator);
        PathFindHelper helper(nSceneID);
        IPathFindService *pPathService = helper.m_ptr;
        if (pPathService == NULL)
        {
            return;
        }

        Vector3 vAngle = (ptTargetTile - vStartTile).normalize();
        if (equalto0(vAngle.getLength()))
        {
            return;
        }

        // ��������
        Vector3 vEndAngle;
        switch (m_data.nDirectionType)
        {
        case DIRECTION_FRONT:
            {
                vEndAngle = vAngle;
            }
            break;
        case DIRECTION_BACK:
            {
                vEndAngle = vAngle*(-1.0);
            }
            break;
        case DIRECTION_LEFT:
            {
                Vector3 vUp(0,1,0);
                vEndAngle = vAngle.getCrossProduct(vUp);
            }
            break;
        case DIRECTION_RIGHT:
            {
                Vector3 vUp(0,1,0);
                vEndAngle = vUp.getCrossProduct(vAngle);
            }
            break;
        default:
            {
                ErrorLn("CEffectUseSpell error nDirectionType="<<m_data.nDirectionType);
                return;
            }
            break;
        }

        // ������λ��
        Vector3 vNowLoc = pPathService->getRaycast(ptTargetTile, vEndAngle.normalize(), m_data.fDistance);
        // �ҵ�����ĵ�
        pPathService->getHeight(&vNowLoc, vNowLoc.y);

        ptTargetTile = vNowLoc;
    }

private:
	SCHEME_DATA               m_data;
};
