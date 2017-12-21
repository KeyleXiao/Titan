/*******************************************************************
** 文件名:	EffectAddBuffOnRightButtonDown.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	5/3/2017
** 版  本:	1.0
** 描  述:	效果-右键加buff
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

class CEffectAddBuffOnRightButtonDown : public IEffectEx, public IEventExecuteSink
{
public:
	typedef  EffectServer_AddBuffOnRightButtonDown    SCHEME_DATA;

	CEffectAddBuffOnRightButtonDown( SCHEME_DATA & data ) : m_data(data), m_pEntity(NULL)
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
        if(context == NULL || context->pEntity == NULL)
            return false;

        __IEntityEvent* pEntityEvent = context->pEntity->getEntityEvent();
        if(pEntityEvent == NULL)
            return false;

        m_pEntity = context->pEntity;
        
        // 订阅右键按下事件
        pEntityEvent->Subscibe(this, EVENT_ENTITY_RIGHTBUTTON_DOWN, __FUNCTION__);

        return true;
	}

    virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_ENTITY_RIGHTBUTTON_DOWN:
            {
                // 加buff
                if(m_pEntity != NULL)
                {
                    __IBuffPart* pBuffPart = (__IBuffPart*)m_pEntity->getEntityPart(PART_BUFF);
                    if(pBuffPart != NULL)
                    {
                        pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, m_pEntity->getUID());
                    }
                }
            }
            break;
        }
    }

	// 效果停止
	virtual void			Stop()
	{
        if(m_pEntity == NULL)
            return;

        // 反订阅右键按下事件
        __IEntityEvent* pEntityEvent = m_pEntity->getEntityEvent();
        if(pEntityEvent == NULL)
            return;

        pEntityEvent->UnSubscibe(this, EVENT_ENTITY_RIGHTBUTTON_DOWN);
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectAddBuffOnRightButtonDown(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
    __IEntity*                m_pEntity;
};
