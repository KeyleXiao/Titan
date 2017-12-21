/*******************************************************************
** 文件名:	EffectAddLighting.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/29
** 版  本:	1.0
** 描  述:	效果: 添加光效
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "ISpell.h"
#include "ISchemeCenter.h"
using namespace EFFECT_CLIENT;

class CEffectAddLighting : public IEffect
{
public:
	typedef EffectClient_AddLighting SCHEME_DATA;

	CEffectAddLighting( SCHEME_DATA & data ):m_data(data),m_pEntity(0)
	{
		vector<string> splitVector;
		StringHelper::split(splitVector, m_data.strName, ';', " ");

		m_LightingVector.clear();

		int nCount = splitVector.size();
		for (int i=0; i<nCount; ++i)
		{
			int nLightingID = StringHelper::toInt(splitVector[i]);
			if (nLightingID == 0)
			{
				continue;
			}

			m_LightingVector.push_back(nLightingID);
		}
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

		if (m_LightingVector.empty())
			return false;

        m_pEntity = pContext->pEntity;

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

        IEntityProperty *pProperty = (IEntityProperty*)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pProperty == NULL)
        {
            return false;
        }
        // 取得玩家皮肤
        int nSkinID = pProperty->getProperty_Integer(PROPERTY_SKIN);

        int nSrcEntityID = 0;
        int nFromSrcPos = 0;
        switch (m_data.nSrcType)
        {
        case ADD_LIGHTING_SRC_SELF:
            {
                nSrcEntityID = m_pEntity->getUID();
            }
            break;
        case ADD_LIGHTING_SRC_OPERATOR:
            {
                nSrcEntityID = pContext->uidOperator;
            }
            break;
        case ADD_LIGHTING_SRC_TARGET:
            {
                nSrcEntityID = pContext->uidTarget;
            }
            break;
        case ADD_LIGHTING_SRC_MAINHERO:
            {
                IClientEntity* pHero = gClientGlobal->getHero();
                if(pHero != NULL)
                {
                    nSrcEntityID = pHero->getUID();
                }
            }
            break;
        case ADD_LIGHTING_SRC_LOCATION:
            {
                nFromSrcPos = 1;
            }
            break;
        default:
            break;
        }

        int uidTarget = 0;
		int nFlyToTargetPos = 0;
        switch (m_data.nTargetType)
        {
        case ADD_LIGHTING_TARGET_SELF:
            {
                uidTarget = m_pEntity->getUID();
            }
            break;
        case ADD_LIGHTING_TARGET_OPERATOR:
            {
                uidTarget = pContext->uidOperator;
            }
            break;
        case ADD_LIGHTING_TARGET_TARGET:
            {
                uidTarget = pContext->uidTarget;
            }
            break;
        case ADD_LIGHTING_TARGET_MAINHERO:
            {
                IClientEntity* pHero = gClientGlobal->getHero();
                if(pHero != NULL)
                {
                    uidTarget = pHero->getUID();
                }
            }
            break;
        case ADD_LIGHTING_TARGET_LOCATION:
            {
				nFlyToTargetPos = 1;
            }
			break;
        default:
            break;
        }

		// 发送给显示层
		cmd_creature_skillview Lighting;
		Lighting.nSrcEntityID = nSrcEntityID;
		Lighting.nTargetEntityID = uidTarget;
        Lighting.fSrcPos_x = pContext->ptStartTile.x;
        Lighting.fSrcPos_y = pContext->ptStartTile.y;
        Lighting.fSrcPos_z = pContext->ptStartTile.z;
		Lighting.fTargetPos_x = pContext->ptTargetTile.x;
		Lighting.fTargetPos_y = pContext->ptTargetTile.y;
		Lighting.fTargetPos_z = pContext->ptTargetTile.z;
		Lighting.fDistance = pContext->fDistance;
		Lighting.fAttackRange = pContext->fAttackRange;
		Lighting.fPASD = pContext->nPASD/ZOOM_IN_MULTIPLE;
		Lighting.bFlyToTargetPos = nFlyToTargetPos;
        Lighting.bFromSrcPos = nFromSrcPos;
		
		IClientEntity* pOperator = gClientGlobal->getEntityFactory()->getEntity(pContext->uidOperator);
		IClientEntity* pHero = gClientGlobal->getHero();
		if (pHero != NULL && pOperator != NULL)
		{
			Lighting.nCreatorSide = (pHero->getIntProperty(PROPERTY_CAMP) == pOperator->getIntProperty(PROPERTY_CAMP));
		}

		int nCount = m_LightingVector.size();
		for (int i=0; i<nCount; ++i)
		{
			int nLightingID = m_LightingVector[i];
            // 替换皮肤光效
            SSkinSpell *pSkinSpell = pSchemeSkinSpell->getSkinSpell(nSkinID, SKIN_SPELL_LIGHTING, nLightingID);
            if (pSkinSpell != NULL)
            {
                nLightingID = pSkinSpell->nNewValue;
            }
			if (nLightingID == 0)
			{
				continue;
			}

			// 告诉显示层播放某个光效
			m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_ADD_LIGHTING, nLightingID, 0, &Lighting, sizeof(Lighting));
            //WarningLn(__FUNCTION__":Add light id="<< nLightingID <<",SrcID="<<nSrcEntityID<<",TargetID="<< Lighting.nTargetEntityID <<", x="<<Lighting.fTargetPos_x<<",y="<<Lighting.fTargetPos_y<<",z="<<Lighting.fTargetPos_z );
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if ( m_pEntity!=0 )
		{
			// 不保留光效，需要停止播放的光效
			if (!m_data.nReserveLighting)
			{
                ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
                ISchemeSkinSpell *pSchemeSkinSpell = pSchemeCenter->getSchemeSkinSpell();
                IEntityProperty *pProperty = (IEntityProperty*)m_pEntity->getEntityPart(PART_PROPERTY);

				int nCount = m_LightingVector.size();
				for (int i=0; i<nCount; ++i)
				{
					int nLightingID = m_LightingVector[i];
                    do 
                    {
                        if (pSchemeCenter == NULL)
                        {
                            break;
                        }
                        if (pSchemeSkinSpell == NULL)
                        {
                            break;
                        }
                        if (pProperty == NULL)
                        {
                            break;
                        }
                        // 取得玩家皮肤
                        int nSkinID = pProperty->getProperty_Integer(PROPERTY_SKIN);

                        // 替换皮肤光效
                        SSkinSpell *pSkinSpell = pSchemeSkinSpell->getSkinSpell(nSkinID, SKIN_SPELL_LIGHTING, nLightingID);
                        if (pSkinSpell != NULL)
                        {
                            nLightingID = pSkinSpell->nNewValue;
                        }
                    } while (false);
					if (nLightingID == 0)
					{
						continue;
					}
					// 告诉显示层停止某个光效
					m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_REMOVE_LIGHTING, nLightingID, 0, 0, 0);
                    //WarningLn(__FUNCTION__":Remove light id="<< nLightingID);
				}
			}

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectAddLighting(m_data); }

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
	IEntity *				m_pEntity;
	vector<int>				m_LightingVector;
};
