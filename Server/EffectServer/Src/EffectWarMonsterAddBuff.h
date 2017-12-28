/*******************************************************************
** 文件名:	EffectWarMonsterAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/10/2017
** 版  本:	1.0
** 描  述:	效果-战场怪物增加Buff
            http://172.16.0.120/redmine/issues/3427
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "IEntity.h"
#include "IBuffServer.h"
#include "BuffMessageDef.h"
#include "ExternalHelper.h"
#include "WarHelper.h"
#include "IWarPart.h"

class CEffectWarMonsterAddBuff : public IEffectEx
{
public:
	typedef  EffectServer_WarMonsterAddBuff    SCHEME_DATA;

	CEffectWarMonsterAddBuff( SCHEME_DATA & data ) : m_data(data)
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
        {
			return false;
        }

        __IWarPart* pWarPart = (__IWarPart*)context->pEntity->getEntityPart(PART_WAR);
        if(pWarPart == NULL)
        {
            return false;
        }
        int nWarID = pWarPart->getWarID();

        WarHelper helper(nWarID);
        IWarService *pWarService = helper.m_ptr;
        if (pWarService == NULL)
        {
            return false;
        }

        int nSelfCamp = context->pEntity->getIntProperty(PROPERTY_CAMP);
		SWarEffectAddBuff sData;
		sData.uidSrcEntity = context->pEntity->getUID();      // 添加实体UID
		sData.nID			= context->nID;				// spellID
		sData.bySelfCamp	= nSelfCamp;
		sData.byGetCampType = m_data.nCampType;
		sData.bySerchType	= m_data.nMonsterExec;
		sData.byAddType		= m_data.nType;
		sData.nBuffID		= m_data.nBuffID;
		sData.nBuffLevel	= m_data.nBuffLevel;
		pWarService->warAddSerchTypeBuff(sData);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectWarMonsterAddBuff(m_data);
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
