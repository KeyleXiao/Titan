/*******************************************************************
** 文件名:	EffectOrderPlayLighting.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/19/2016
** 版  本:	1.0
** 描  述:	效果-顺序播放光效
            http://172.16.0.120/redmine/issues/1622
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ISchemeCenter.h"
using namespace EFFECT_CLIENT;

class CEffectOrderPlayLighting : public IEffect
{
public:
	typedef EffectClient_OrderPlayLighting SCHEME_DATA;

	CEffectOrderPlayLighting( SCHEME_DATA & data ) : m_data(data)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_vLightingID.clear();
        StringHelper::split(tempVector, m_data.strLightingID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vLightingID.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        if (m_vLightingID.size() <= 0)
        {
            ErrorLn("CEffectOrderPlayLighting error,m_vLightingID.size() <= 0");
        }
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

        int nCount = m_vLightingID.size();
        if (nCount <= 0)
        {
            return false;
        }

        ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            return false;
        }

        ISchemeSkinSpell *pSchemeSkinSpell = pSchemeCenter->getSchemeSkinSpell();
        if (pSchemeSkinSpell == NULL)
        {
            return false;
        }

        IEntityProperty *pProperty = (IEntityProperty*)pContext->pEntity->getEntityPart(PART_PROPERTY);
        if (pProperty == NULL)
        {
            return false;
        }
        // 取得玩家皮肤
        int nSkinID = pProperty->getProperty_Integer(PROPERTY_SKIN);

        int nRandIndex = pContext->nAttackCount%nCount;
        int nLightingID = m_vLightingID[nRandIndex];
        // 替换皮肤光效
        SSkinSpell *pSkinSpell = pSchemeSkinSpell->getSkinSpell(nSkinID, SKIN_SPELL_LIGHTING, nLightingID);
        if (pSkinSpell != NULL)
        {
            nLightingID = pSkinSpell->nNewValue;
        }
        if (nLightingID == 0)
        {
            return false;
        }

        // 发送给显示层
        cmd_creature_skillview Lighting;
        Lighting.nSrcEntityID = pContext->pEntity->getUID();
        Lighting.nTargetEntityID = pContext->uidTarget;
        Lighting.fTargetPos_x = pContext->ptTargetTile.x;
        Lighting.fTargetPos_y = pContext->ptTargetTile.y;
        Lighting.fTargetPos_z = pContext->ptTargetTile.z;
        Lighting.fDistance = pContext->fDistance;
        Lighting.fAttackRange = pContext->fAttackRange;
        Lighting.fPASD = pContext->nPASD/ZOOM_IN_MULTIPLE;
        // 告诉显示层播放某个光效
        pContext->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_ADD_LIGHTING, nLightingID, 0, &Lighting, sizeof(Lighting));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectOrderPlayLighting(m_data); }

	// 取得效果ID
	virtual int				GetEffectID(){ return m_data.nID; }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

    // LightingID容器
    vector<int>				m_vLightingID;
};
