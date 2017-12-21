/*******************************************************************
** 文件名:	EffectUseSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/02/2015
** 版  本:	1.0
** 描  述:	效果-使用技能
			通过使用技能效果，可实现在技能事件中使用技能，从而实现后续技能使用
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

				// 取得攻击对象序号
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

				// 更新点
				ptStartTile = pAttackObject->getTargetTile();
                ptTargetTile = ptStartTile;
                // 设置目标点
                setTargetTile(uidOperator, context->vSelfLoc, ptTargetTile);
			}
			break;
		case USE_SPELL_POS_SELF:
			{
				ptStartTile = context->pEntity->getLocation();
                ptTargetTile = ptStartTile;
                // 设置目标点
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
                // 设置目标点
                setTargetTile(uidOperator, context->vSelfLoc, ptTargetTile);
			}
			break;
        case USE_SPELL_POS_ANGLE:
            {
                // 场景ID
                int nSceneID = UID_2_SCENE(uidOperator);
                PathFindHelper helper(nSceneID);
                IPathFindService *pPathService = helper.m_ptr;
                if (pPathService == NULL)
                {
                    return false;
                }

                // 取得朝向角度
                Vector3 angle = context->pEntity->getAngle();
                float fAngleY = (float)(angle.y*fPI/180);

                // 方向
                Vector3 dirNomalize(0,0,0);
                dirNomalize.x = sin(fAngleY);
                dirNomalize.y = 0;
                dirNomalize.z = cos(fAngleY);

                // 算出最后方向
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
                // 算出最后位置
                ptTargetTile = pPathService->getRaycast(ptStartTile, vEndAngle.normalize(), m_data.fDistance);
            }
            break;
        case USE_SPELL_POS_LAST_SPELL:
            {
                // 更新点
                ptStartTile = context->ptStartTile;
                ptTargetTile = context->ptTargetTile;
                // 设置目标点
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
		// 技能施法现场
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

		// 施法技能
		pSpellPart->castSpell(&spellContext);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectUseSpell(m_data);
	}

	// 释放
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

        // 场景ID
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

        // 算出最后方向
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

        // 算出最后位置
        Vector3 vNowLoc = pPathService->getRaycast(ptTargetTile, vEndAngle.normalize(), m_data.fDistance);
        // 找到地面的点
        pPathService->getHeight(&vNowLoc, vNowLoc.y);

        ptTargetTile = vNowLoc;
    }

private:
	SCHEME_DATA               m_data;
};
