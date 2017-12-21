/*******************************************************************
** 文件名:	EffectChangePropertyChangeValue.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	陈言蒙
** 日  期:	3/5/2016
** 版  本:	1.0
** 描  述:	改变实体属性(可以改变参数,与EffectChangeProperty Ai版区分)
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "TimerHandler.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "EventDef.h"
#include "IEntityEvent.h"
using namespace EFFECT_SERVER;
using namespace rkt;

class CEffectChangePropertyChangeValue : public IEffectChangeValue
{
public:
    typedef  EffectServer_ChangePropertyChangeValue    SCHEME_DATA;

    CEffectChangePropertyChangeValue( SCHEME_DATA & data ) : 
    m_data(data), 
        m_nOldVal(0),
        m_uidTarget(INVALID_UID),
        m_bBusying(false)
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

        if (m_bBusying)
        {
            return false;
        }

        m_uidTarget = context->pEntity->getUID();
        if(isInvalidUID(m_uidTarget))
        {
            return false;
        }
        __IEntityEvent *pEntityEvent = context->pEntity->getEntityEvent();
        if (pEntityEvent == NULL)
        {
            return false;
        }

        if (m_data.nChangeType == MA_CHANGE_SET)
        {
            m_nOldVal = getProperty_Integer( m_uidTarget, m_data.nType);
            m_bBusying = true;
#ifdef ENTITY_CHECK_SKIN
            if(m_data.nType == PROPERTY_SKIN)
            {
                TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_uidTarget <<", value="<< m_data.nVal);
            }
#endif
            setProperty_Integer( m_uidTarget, m_data.nType, m_data.nVal );
        }
        else if (m_data.nChangeType == MA_CHANGE_ADD)
        {
            m_bBusying = true;

            if (m_data.nEvent == MA_TRUE)
            {
                event_prepare_change_property prepareChangeProperty;
                prepareChangeProperty.nID = m_data.nType;
                prepareChangeProperty.dwFlag = context->dwFlag;
                prepareChangeProperty.nValue = m_data.nVal;
                pEntityEvent->FireExecute(EVENT_ENTITY_PREPARE_CHANGE_PROPERTY, (char *)&prepareChangeProperty, sizeof(prepareChangeProperty));

                m_nOldVal = prepareChangeProperty.nValue;
            }
            else
            {
                m_nOldVal = (int)ceil((m_data.nVal/EFFECT_RATE));
            }

#ifdef ENTITY_CHECK_SKIN
            if(m_data.nType == PROPERTY_SKIN)
            {
                TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_uidTarget <<", value="<< m_nOldVal);
            }
#endif
            addProperty_Integer( m_uidTarget, m_data.nType, m_nOldVal );
        }

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
        if (m_bBusying && !isInvalidUID(m_uidTarget))
        {
            if (m_data.nChangeType == MA_CHANGE_SET)
            {				
#ifdef ENTITY_CHECK_SKIN
                if(m_data.nType == PROPERTY_SKIN)
                {
                    TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_uidTarget <<", value="<< m_nOldVal);
                }
#endif
                setProperty_Integer( m_uidTarget, m_data.nType, m_nOldVal);
                m_nOldVal = 0;
                m_uidTarget = INVALID_UID;
                m_bBusying = false;
            }
            else if (m_data.nChangeType == MA_CHANGE_ADD)
            {
#ifdef ENTITY_CHECK_SKIN
                if(m_data.nType == PROPERTY_SKIN)
                {
                    TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_uidTarget <<", value="<< m_nOldVal);
                }
#endif
                addProperty_Integer( m_uidTarget, m_data.nType, -m_nOldVal);
                m_nOldVal = 0;
                m_uidTarget = INVALID_UID;
                m_bBusying = false;
            }			
        }

    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {

        return new CEffectChangePropertyChangeValue(m_data);
    }

    // 释放
    virtual void			Release()
    {
        Stop();
        delete this;
    }

    // 设置数值
    virtual void			SetValue(EFFECT_VALUE_LIST *pListValue)
    {
        if (pListValue->empty())
        {
            return;
        }

        EFFECT_VALUE_LIST::iterator Iter = pListValue->begin();
        // 设置数据
        m_data.nVal = (int)*Iter;
    }


	// 获取数值类型
	virtual int				GetValueType()
	{
		return m_data.nType;
	}
private:
    SCHEME_DATA               m_data;
    UID						  m_uidTarget;
    bool					  m_bBusying;
    int						  m_nOldVal;
};
