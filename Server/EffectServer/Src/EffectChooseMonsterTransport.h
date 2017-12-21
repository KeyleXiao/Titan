/*******************************************************************
** 文件名:	EffectChooseMonsterTransport.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/14/2016
** 版  本:	1.0
** 描  述:	效果-选择怪物传送
            http://172.16.0.120/redmine/issues/1753
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ExternalHelper.h"
#include "PathFindHelper.h"
#include "IPathFindService.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectChooseMonsterTransport : public IEffectEx
{
public:
	typedef  EffectServer_ChooseMonsterTransport    SCHEME_DATA;

	CEffectChooseMonsterTransport( SCHEME_DATA & data ) : m_data(data)
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

        __IEntity *pEntity = context->pEntity;

        // 取得自己UID
        UID uidSelf = pEntity->getUID();
        MonsterHelper monsterHelper(uidSelf);
        INPCService * pNpcService = monsterHelper.m_ptr;		
        if (pNpcService == NULL)
        {
            return false;
        }

        // 场景ID
        int nSceneID = UID_2_SCENE(uidSelf);
        PathFindHelper helper(nSceneID);
        IPathFindService *pPathService = helper.m_ptr;
        if (pPathService == NULL)
        {
            return false;
        }

        UID uidArray[32] = {0};
        int nCount = pNpcService->getMonsterArray(m_data.nMonsterID, 0, uidArray, 32);
        
        UID uidFind = 0;
        for (int i=0; i<nCount; ++i)
        {
            UID uidTarget = uidArray[i];
            if (isInvalidUID(uidTarget))
            {
                continue;
            }

            // 检测击杀目标
            if (!g_EHelper.chooseTarget(pEntity, m_data.nTargetFilter, uidTarget))
            {
                continue;
            }

            uidFind = uidTarget;
            
            break;
        }
        if (isInvalidUID(uidFind))
        {
            return false;
        }

        Vector3 vNowLoc = getLocation(uidFind);
        // 找到地面的点
        pPathService->getHeight(&vNowLoc, vNowLoc.y);

        //传送
        g_EHelper.Transport(pEntity, vNowLoc, true, 0);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectChooseMonsterTransport(m_data);
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