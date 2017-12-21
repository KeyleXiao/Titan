/*******************************************************************
** 文件名:	EffectStickyCamerra.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/24/2016
** 版  本:	1.0
** 描  述:	效果-跟随镜头
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "ISpell.h"
using namespace EFFECT_CLIENT;

class CEffectStickyCamerra : public IEffect
{
public:
	typedef EffectClient_StickyCamerra SCHEME_DATA;

	CEffectStickyCamerra( SCHEME_DATA & data ):m_data(data),m_pEntity(0)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

        // 取得主角对象
        IClientEntity *pClientEntity = gClientGlobal->getHero();
        if (pClientEntity == NULL)
        {
            return false;
        }

        if (pClientEntity->getUID() != pContext->pEntity->getUID())
        {
            return false;
        }

        m_pEntity = pContext->pEntity;

        UID uid = 0;
        switch (m_data.nType)
        {
        case STICKY_CAMERRA_SELF:
            {
                uid = m_pEntity->getUID();
            }
            break;
        case STICKY_CAMERRA_OPERATOR:
            {
                uid = pContext->uidOperator;
            }
            break;
        case STICKY_CAMERRA_TARGET:
            {
                uid = pContext->uidTarget;
            }
            break;
        default:
            {
                ErrorLn("CEffectStickyCamerra error nType="<<m_data.nType);
                return false;
            }
            break;
        }

        if (uid == 0 || uid == INVALID_UID)
        {
            return false;
        }
		// 发送给显示层
		cmd_creature_Sticky_Camerra StickyCamerra;
        StickyCamerra.uid = uid;
        // 镜头跟随
        m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_STICKY_CAMERRA, 0, 0, &StickyCamerra, sizeof(StickyCamerra));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if ( m_pEntity!=0 )
		{
            do 
            {
                // 取得主角对象
                IClientEntity *pClientEntity = gClientGlobal->getHero();
                if (pClientEntity == NULL)
                {
                    break;
                }

                if (pClientEntity->getUID() != m_pEntity->getUID())
                {
                    break;
                }

                // 发送给显示层
                cmd_creature_Sticky_Camerra StickyCamerra;
                StickyCamerra.uid = m_pEntity->getUID();
                // 镜头跟随
                m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_STICKY_CAMERRA, 0, 0, &StickyCamerra, sizeof(StickyCamerra));
            } while (false);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectStickyCamerra(m_data); }

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
};
