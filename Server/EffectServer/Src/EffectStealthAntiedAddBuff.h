/*******************************************************************
** 文件名:	EffectStealthAntiedAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	12/18/2017
** 版  本:	1.0
** 描  述:	效果-当自己被反隐后给自己加buff(用于表现)
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityAOI.h"
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "EntityHelper.h"
#include "IBuffServer.h"
using namespace EFFECT_SERVER;

class CEffectStealthAntiedAddBuff : public IEffectEx, public IEventExecuteSink
{
public:
    typedef  EffectServer_StealthAntiedAddBuff    SCHEME_DATA;

    CEffectStealthAntiedAddBuff( SCHEME_DATA & data ) : m_data(data),m_pMaster(0),m_bBuffAdded(false)
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

        __IEntityEvent* pEntityEvent = context->pEntity->getEntityEvent();
        if(pEntityEvent == NULL)
        {
            return false;
        }

        m_pMaster = context->pEntity;

        m_bBuffAdded = false;

        // 注册自己进入其他阵营视野事件
        pEntityEvent->Subscibe(this, EVENT_ENTITY_ENTER_A_CAMP_SIGHT, __FUNCTION__);
        // 注册自己离开其他阵营视野事件
        pEntityEvent->Subscibe(this, EVENT_ENTITY_LEAVE_A_CAMP_SIGHT, __FUNCTION__);
        // 注册自己隐身状态改变事件
        pEntityEvent->Subscibe(this, EVENT_ENTITY_STEALTH_CHANGE, __FUNCTION__);

        return true;
    }

    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        if(m_pMaster == NULL)
        {
            WarningLn(__FUNCTION__ << __LINE__ << ", m_pMaster is null");
            return;
        }
        switch(wEventID)
        {
        case EVENT_ENTITY_ENTER_A_CAMP_SIGHT:
            {
                // 已经被加了反隐buff
                if(m_bBuffAdded)
                {
                    return;
                }

                __IEntityAOI* pAOIPart = (__IEntityAOI*)m_pMaster->getEntityPart(PART_AOI);
                if(pAOIPart == NULL)
                {
                    return;
                }

                AOI_PROXY_PTR ptrAOIProxy = pAOIPart->getAOIProxy();
                if(ptrAOIProxy == 0)
                {
                    return;
                }

                // 没有开启隐形 肯定不会被反隐
                if(!ptrAOIProxy->m_bIsStealing)
                {
                    return;
                }

                int nCamp = *((int*)pszContext);
                // 进自己阵营视野 不算被反隐
                if(nCamp == ptrAOIProxy->m_nCamp)
                {
                    return;
                }

                // 进ob阵营视野 不算被反隐
                SceneHelper sHelper(m_pMaster->getSceneID());
                if(!sHelper.m_ptr)
                {
                    return;
                }
                int nObCamp = sHelper.m_ptr->getObCampID();
                if(nCamp == nObCamp)
                {
                    return;
                }

                // 自己被反隐了 加buff
                __IBuffPart* pBuffPart = (__IBuffPart*)m_pMaster->getEntityPart(PART_BUFF);
                if(pBuffPart != NULL)
                {
                    pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, m_pMaster->getUID());
                }
                m_bBuffAdded = true;
            }
            break;
        case EVENT_ENTITY_LEAVE_A_CAMP_SIGHT:
            {
                if(!m_bBuffAdded)
                {
                    return;
                }

                __IEntityAOI* pAOIPart = (__IEntityAOI*)m_pMaster->getEntityPart(PART_AOI);
                if(pAOIPart == NULL)
                {
                    return;
                }

                AOI_PROXY_PTR ptrAOIProxy = pAOIPart->getAOIProxy();
                if(ptrAOIProxy == 0)
                {
                    return;
                }
                
                // 当前可见性
                int nVisible = ptrAOIProxy->m_nVisibaleFlag;

                // 除去自己阵营
                nVisible = nVisible & (~CAMP_MASK(ptrAOIProxy->m_nCamp));

                SceneHelper sHelper(m_pMaster->getSceneID());
                if(sHelper.m_ptr)
                {
                    int nObCamp = sHelper.m_ptr->getObCampID();
                    if(nObCamp > 0)
                    {
                        // 除去OB阵营
                        nVisible = nVisible & (~CAMP_MASK(nObCamp));
                    }
                }
                
                // 对其他阵营不可见了 则没被反隐了
                if(nVisible == 0)
                {
                    // 移除buff
                    __IBuffPart* pBuffPart = (__IBuffPart*)m_pMaster->getEntityPart(PART_BUFF);
                    if(pBuffPart != NULL)
                    {
                        pBuffPart->Remove(m_data.nBuffID, m_pMaster->getUID(), m_pMaster->getUID());
                    }
                    m_bBuffAdded = false;
                }
            }
            break;
        case EVENT_ENTITY_STEALTH_CHANGE:
            {
                // 反隐期间 自己的隐身效果结束的话，则自己不再是被反隐了
                if(!m_bBuffAdded)
                {
                    return;
                }

                bool bSteathing = *((bool*)pszContext);
                if(!bSteathing)
                {
                    // 移除buff
                    __IBuffPart* pBuffPart = (__IBuffPart*)m_pMaster->getEntityPart(PART_BUFF);
                    if(pBuffPart != NULL)
                    {
                        pBuffPart->Remove(m_data.nBuffID, m_pMaster->getUID(), m_pMaster->getUID());
                    }
                    m_bBuffAdded = false;
                }
            }
            break;
        }
    }

    // 效果停止
    virtual void			Stop()
    {
        if(m_pMaster != 0)
        {
            __IEntityEvent* pEntityEvent = m_pMaster->getEntityEvent();
            if(pEntityEvent != NULL)
            {
                pEntityEvent->UnSubscibe(this, EVENT_ENTITY_ENTER_A_CAMP_SIGHT);
                pEntityEvent->UnSubscibe(this, EVENT_ENTITY_LEAVE_A_CAMP_SIGHT);
                pEntityEvent->UnSubscibe(this, EVENT_ENTITY_STEALTH_CHANGE);
            }
        }
        m_pMaster = 0;
        m_bBuffAdded = false;
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectStealthAntiedAddBuff(m_data);
    }

    // 释放
    virtual void			Release() 
    {
        Stop();
        delete this;
    }

private:
    SCHEME_DATA               m_data;
    __IEntity*                m_pMaster;

    bool                      m_bBuffAdded; // 是否已经加了buff
};
