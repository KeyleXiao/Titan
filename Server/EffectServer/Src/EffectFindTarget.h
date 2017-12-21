/*******************************************************************
** 文件名:	EffectFindTarget.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/17/2015
** 版  本:	1.0
** 描  述:	效果-寻找目标
            http://172.16.0.120/redmine/issues/1455
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectFindTarget : public IEffectEx
{
public:
	typedef  EffectServer_FindTarget    SCHEME_DATA;

	CEffectFindTarget( SCHEME_DATA & data ) : m_data(data)
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
		if ( context==0 || context->pEntity==0 )
			return false;

        // 有目标时不处理，没目标时，从点范围找目标
        if (!isInvalidUID(context->uidTarget))
        {
            return true;
        }
        
        UID uidSelf = context->pEntity->getUID();
        int nSceneID = UID_2_SCENE(uidSelf);
        SceneServiceHelper sceneHelper(nSceneID);
        if ( sceneHelper.m_ptr==0 )
        {
            return false;
        }

        // 取得一定范围内的所有实体
        AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
        int num = sceneHelper.m_ptr->k_nearest(context->ptTargetTile,m_data.fDist, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true);
        if ( num<=0 )
        {
            return false;
        }

        UID uidTarget = 0;
        for ( int i=0; i<num; ++i )
        {
            // 目标UID
			UID uid = pProxyArray[i]->m_uid;
            if (isInvalidUID(uid))
            {
                continue;
            }

            // 检测目标
            if (!g_EHelper.chooseTarget(context->pEntity, m_data.nTargetFilter, uid))
            {
                continue;
            }

            uidTarget = uid;

            break;
        }
        if (isInvalidUID(uidTarget))
        {
            return false;
        }

        // 设置目标
        context->uidTarget = uidTarget;
        if (pConditionContext != NULL)
        {
            pConditionContext->uidTarget = uidTarget;
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
		return new CEffectFindTarget(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
