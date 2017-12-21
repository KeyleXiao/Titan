/*******************************************************************
** 文件名:	EffectWarChangeVoc.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李有红
** 日  期:	2017/11/23
** 版  本:	1.0
** 描  述:	战场改变职业
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "IPlayerRole.h"
#include "IWarManager.h"
#include "IWarPart.h"
#include "WarHelper.h"

class CEffectWarChangeVoc : public IEffectEx
{
public:
	typedef  EffectServer_WarChangeVoc    SCHEME_DATA;

	CEffectWarChangeVoc( SCHEME_DATA & data ) : m_data(data),m_pMaster(NULL)
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
		if( context==0 || context->pEntity==0 )
			return false;

        m_pMaster = context->pEntity;

		__IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
		if (pWarPart == NULL)
		{
			return false;
		}

		DWORD dwWarID = pWarPart->getWarID();
		if (dwWarID == INVALID_WAR_ID)
		{
			return false;
		}

		WarHelper warHelper(dwWarID);
		IWarService* pWarService = warHelper.m_ptr;
		if (pWarService == NULL)
		{
			return false;
		}

		pWarService->changeVocation(m_pMaster->getUID(), context->nReverse1);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pMaster != NULL)
        {
            m_pMaster = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectWarChangeVoc(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
	__IEntity *				  m_pMaster;
};
