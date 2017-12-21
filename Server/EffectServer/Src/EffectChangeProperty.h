/*******************************************************************
** 文件名:	EffectChangeProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	ljw
** 日  期:	3/26/2015
** 版  本:	1.0
** 描  述:	改变实体属性
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: rjc
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "TimerHandler.h"
#include "IServerGlobal.h"
#include "IDiePart.h"
using namespace EFFECT_SERVER;

class CEffectChangeProperty : public IEffectEx
{
public:
    typedef  EffectServer_ChangeProperty    SCHEME_DATA;

    CEffectChangeProperty( const SCHEME_DATA & data ) : 
        m_data(data), 
        m_nOldVal(0),
        m_bBusying(false),
        m_pEntity(NULL)
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

        m_pEntity = context->pEntity;
        if(m_pEntity == NULL)
        {
            return false;
        }
        __IEntityEvent *pEntityEvent = context->pEntity->getEntityEvent();
        if (pEntityEvent == NULL)
        {
            return false;
        }
        __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return false;
        }

        m_bBusying = true;

        // 取得原来值
        m_nOldVal = pEntityProperty->getProperty_Integer(m_data.nType);

        if (m_data.nChangeType == MA_CHANGE_SET)
        {
            if(m_data.nType == CHANGE_SKIN)
            {
                pEntityProperty->setProperty_Integer(m_data.nType, m_data.nVal);
            }
            else
            {
                // 算出差值
                m_nOldVal = m_data.nVal - m_nOldVal;

                // 增加数值
                pEntityProperty->addProperty_Integer(m_data.nType, m_nOldVal);
            }
        }
        else if (m_data.nChangeType == MA_CHANGE_ADD)
        {
            // 皮肤不能累加
            if(m_data.nType == CHANGE_SKIN)
            {
                ErrorLn("CEffectChangeProperty MA_CHANGE_ADD error nChangeType="<<m_data.nChangeType)
                return false;
            }

            if (m_data.nEvent == MA_TRUE)
            {
                event_prepare_change_property prepareChangeProperty;
                prepareChangeProperty.nID = m_data.nType;
                prepareChangeProperty.dwFlag = context->dwFlag;
                prepareChangeProperty.nValue = m_data.nVal;
                pEntityEvent->FireExecute(EVENT_ENTITY_PREPARE_CHANGE_PROPERTY, (char *)&prepareChangeProperty, sizeof(prepareChangeProperty));
            }

            m_nOldVal = m_data.nVal;

            // 增加数值
            pEntityProperty->addProperty_Integer(m_data.nType, m_nOldVal);
        }

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
        if (m_bBusying && m_pEntity != NULL)
        {
            __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
            if (pEntityProperty != NULL)
            {
                // 皮肤类型等
                if(m_data.nType == CHANGE_SKIN)
                {
					// 如果实体死亡 要等复活后才能换模型
					if (m_pEntity->getState() == ENTITY_STATE_DIE)
					{
						__IDiePart* pDiePart = (__IDiePart*)m_pEntity->getEntityPart(PART_DIE);
						if (pDiePart != NULL)
						{
							pDiePart->setIntPropertyOnRelive(m_data.nType, m_nOldVal);
						}
						else
						{
							pEntityProperty->setProperty_Integer(m_data.nType, m_nOldVal);
						}
					}
					else
					{
						pEntityProperty->setProperty_Integer(m_data.nType, m_nOldVal);
					}
                }
                else
                {
                    // 递减数值
                    pEntityProperty->addProperty_Integer(m_data.nType, -m_nOldVal);
                }
            }

            m_nOldVal = 0;
            m_bBusying = false;
            m_pEntity = NULL;
        }
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectChangeProperty(m_data);
    }

    // 释放
    virtual void			Release()
    {
        Stop();
        delete this;
    }

private:
    SCHEME_DATA				m_data;
    bool					m_bBusying;
    int						m_nOldVal; // 原值或差值

    __IEntity				*m_pEntity;
};
