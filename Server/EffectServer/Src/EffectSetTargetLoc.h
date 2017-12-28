/*******************************************************************
** 文件名:	EffectSetTargetLoc.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/01/2016
** 版  本:	1.0
** 描  述:	效果-设置目标位置
            http://172.16.0.120/redmine/issues/2134
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ExternalHelper.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectSetTargetLoc : public IEffectEx
{
public:
	typedef  EffectServer_SetTargetLoc    SCHEME_DATA;

	CEffectSetTargetLoc( SCHEME_DATA & data ) : m_data(data)
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
		if ( context == 0 || context->pEntity == 0 )
			return false;

        // 场景ID
        int nSceneID = context->pEntity->getSceneID();

        Vector3 vLoc = context->pEntity->getLocation();

        Vector3 vEndLoc(0,0,0);
        switch (m_data.nType)
        {
        case USE_SPELL_POS_TILE:
            {
                vEndLoc = context->ptTargetTile;
            }
            break;
        case USE_SPELL_POS_SELF:
            {
                vEndLoc = vLoc;
            }
            break;
        case USE_SPELL_POS_TARGET:
            {
                UID uidTarget = context->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }
                vEndLoc = getLocation(uidTarget);
            }
            break;
        default:
            {
                ErrorLn("CEffectSetTargetLoc error nType="<<m_data.nType);
                return false;
            }
            break;
        }

        if (!vEndLoc.checkDistance(vLoc, m_data.fDistance))
        {
            Vector3 vDir = (vEndLoc - vLoc).normalize();
            vEndLoc = vLoc + vDir*m_data.fDistance;
        }
        
        // 传送
        g_EHelper.Transport(context->pEntity, vEndLoc, true, 0, INT_MAX, TRANSPORT_REASON_FLASH);

		return true;
    }

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectSetTargetLoc(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
};