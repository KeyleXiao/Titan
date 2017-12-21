/*******************************************************************
** 文件名:	EffectChooseRangeAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/09/2017
** 版  本:	1.0
** 描  述:	效果-检测目标增加buff效果
            http://172.16.0.120/redmine/issues/3255
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "ISpell.h"
#include "IBuffServer.h"

class CEffectChooseRangeAddBuff : public IEffectEx
{
public:
	typedef  EffectServer_ChooseRangeAddBuff    SCHEME_DATA;

	CEffectChooseRangeAddBuff( SCHEME_DATA &data ) : m_data(data)
	{
        vector<string> vTemp;

        vTemp.clear();
        m_vCountID.clear();
        StringHelper::split(vTemp, m_data.strCountID, ';', " ");
        for (UINT nCount=0; nCount<vTemp.size(); ++nCount)
        {
            m_vCountID.push_back(StringHelper::toInt(vTemp[nCount]));
        }
        // 次数数量
        int nCount = (int)m_vCountID.size();

        vTemp.clear();
        m_vBuffID.clear();
        StringHelper::split(vTemp, m_data.strBuffID, ';', " ");
        for (UINT nCount=0; nCount<vTemp.size(); ++nCount)
        {
            m_vBuffID.push_back(StringHelper::toInt(vTemp[nCount]));
        }
        // BUFFID数量
        int nBuffIDCount = (int)m_vBuffID.size();

        vTemp.clear();
        m_vBuffLevel.clear();
        StringHelper::split(vTemp, m_data.strBuffLevel, ';', " ");
        for (UINT nCount=0; nCount<vTemp.size(); ++nCount)
        {
            m_vBuffLevel.push_back(StringHelper::toInt(vTemp[nCount]));
        }
        // BUFFLevel数量
        int nBuffLevelCount = (int)m_vBuffLevel.size();
        if (nCount != nBuffIDCount || nBuffIDCount != nBuffLevelCount)
        {
            ErrorLn("CEffectChooseRangeAddBuff not match,ID="<< GetID() << ",nCount=" << nCount << ",nBuffIDCount=" << nBuffIDCount << ",nBuffLevelCount=" << nBuffLevelCount);
        }
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
        {
			return false;
        }

		__IEntity *pEntity = context->pEntity;

        __ISpellPart *pSpellPart = (__ISpellPart *)pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return false;
        }
        if (context->nID <= 0)
        {
            return false;
        }
        ISpell *pSpell = pSpellPart->findSpell(context->nID);
        if (pSpell == NULL)
        {
            return false;
        }
        __IBuffPart *pBuffPart = (__IBuffPart *)pEntity->getEntityPart(PART_BUFF);
        if (pBuffPart == NULL)
        {
            return false;
        }

        Vector3 vTargetLoc;
        switch (m_data.nUseSpellPosType)
        {
        case USE_SPELL_POS_TILE:
            {
                vTargetLoc = context->ptTargetTile;
            }
            break;
        case USE_SPELL_POS_SELF:
            {
                vTargetLoc = pEntity->getLocation();
            }
            break;
        case USE_SPELL_POS_TARGET:
            {
                UID uidTarget = context->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }
                vTargetLoc = getLocation(uidTarget);
            }
            break;
        default:
            {
                ErrorLn("CEffectChooseRangeAddBuff error nUseSpellPosType=" << m_data.nUseSpellPosType);
                return false;
            }
            break;
        }

        UID uidSelf = pEntity->getUID();

        int nSceneID = UID_2_SCENE(uidSelf);
        SceneServiceHelper sceneHelper(nSceneID);
        if ( sceneHelper.m_ptr==0 )
        {
            return false;
        }

        AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
        int num = 0;
        
        switch (m_data.nChooseRangeType)
        {
        case CHOOSE_RANGE_SECTOR:
            {
                // 取得技能距离，半径
                float fr = m_data.fDistance;
                // 半径平方
                float squaredR = pow(fr, 2);
                // 攻击范围做扇形角度用
                float fAngle = m_data.fAttackRange;
                // cos角度
                float cosTheta = cos(fAngle*fPI/180);
                // 技能攻击高度
                float fAttackHeight = m_data.fAttackHeight;

                Vector3 vDirNormalize = (context->ptTargetTile - context->ptStartTile).normalize();
                num = sceneHelper.m_ptr->k_nearSectorest(vTargetLoc, vDirNormalize, fr, squaredR, cosTheta, fAttackHeight, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL);
            }
            break;
        case CHOOSE_RANGE_LINE:
            {
                Vector3 vDirNormalize = (context->ptTargetTile - context->ptStartTile).normalize();
                if (equalto0(vDirNormalize.getLength()))
                {
                    return false;
                }
                num = sceneHelper.m_ptr->k_nearLineest(vTargetLoc, vDirNormalize, m_data.fDistance, m_data.fAttackRange, m_data.fAttackHeight, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL);
            }
            break;
        case CHOOSE_RANGE_AOE:
            {
                num = sceneHelper.m_ptr->k_nearestInSpell(vTargetLoc, m_data.fAttackRange, m_data.fAttackHeight, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL);
            }
            break;
        default:
            {
                ErrorLn("CEffectChooseRangeAddBuff error nChooseRangeType=" << m_data.nChooseRangeType);
                return false;
            }
            break;
        }
        if ( num <= 0 )
        {
            return false;
        }

        int nCastType = pSpell->getSpellData(SPDATA_CAST_TYPE);
        int nCastFlag = pSpell->getSpellData(SPDATA_CAST_FLAG);
        int nTargetCount = 0;
        for ( int i=0;i<num;++i )
        {
            if (nTargetCount >= m_data.nMaxCheckCount)
            {
                break;
            }

            // 目标UID
            UID uidTarget = pProxyArray[i]->m_uid;
            if (isInvalidUID(uidTarget))
            {
                continue;
            }
            // 排除目标
            if ((nCastType&SpellCastType_ExcludeTarget) == SpellCastType_ExcludeTarget)
            {
                if (uidTarget == context->uidTarget)
                {
                    continue;
                }
            }
            // 排除隐形
            if ((nCastFlag&SpellCastType_ExcludeStealth) == SpellCastType_ExcludeStealth)
            {
                // 取得玩家场景
                if (nSceneID <= MAP_MAXCOUNT)
                {
                    // 玩家隐形
                    if (pProxyArray[i]->m_bIsStealing)
                    {
                        __IEntityAOI *pEntityAOI = (__IEntityAOI *)pEntity->getEntityPart(PART_AOI);
                        if (pEntityAOI != NULL)
                        {
                            AOI_PROXY_PTR pAOI = pEntityAOI->getAOIProxy();
                            // 自己不反隐
                            if (!pAOI->m_bIsAntiStealth)
                            {
                                continue;
                            }
                        }
                    }
                }
                else
                {
                    // 不在视野的排除
                    bool bCanSee = ((pProxyArray[i]->m_nVisibaleFlag & CAMP_MASK(pEntity->getIntProperty(PROPERTY_CAMP))) != 0);
                    if ( !bCanSee )
                    {
                        continue;
                    }
                }
            }
            // 排除不可选择
            if ((nCastFlag&SpellCastType_ExcludeNoSelect) == SpellCastType_ExcludeNoSelect)
            {
                // 取得实体标志
                int nMask = getProperty_Integer(uidTarget, PROPERTY_MASK);
                // 不可选择排除
                if ((nMask&MASKFLAG_NOT_SELECT) == MASKFLAG_NOT_SELECT)
                {
                    continue;
                }
            }
            // 检测目标
            if (!g_EHelper.chooseTarget(pEntity, m_data.nTargetFilter, uidTarget))
            {
                continue;
            }

            ++nTargetCount;
        }

        // 算出当前次数索引
        int nCount = -1;
        int nCountIDSize = (int)m_vCountID.size();
        for (int i=0; i<nCountIDSize - 1; ++i)
        {
            if (nTargetCount >= m_vCountID[i] && nTargetCount <= m_vCountID[i+i])
            {
                nCount = i;
                break;
            }
        }
        if (nCount < 0 || nCount >= nCountIDSize)
        {
            return false;
        }

        if (nCount >= (int)m_vBuffID.size() || nCount >= (int)m_vBuffLevel.size())
        {
            return false;
        }

        int nBuffID = m_vBuffID[nCount];
        int nBuffLevel = m_vBuffLevel[nCount];
        // 增加Buff
        pBuffPart->Add(nBuffID, nBuffLevel, uidSelf);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectChooseRangeAddBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

    // 次数容器
    vector<int>				m_vCountID;
    // BuffID容器
    vector<int>				m_vBuffID;
    // BuffLevel容器
    vector<int>				m_vBuffLevel;
};
