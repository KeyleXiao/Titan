/*******************************************************************
** 文件名:	EffectPropertyTransform.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	13/4/2017
** 版  本:	1.0
** 描  述:	效果-带目标类型属性转换
** http://172.16.0.120/redmine/issues/4291
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"

class CEffectTargetPropertyTransform : public IEffectEx
{
    enum
    {
        INVALID_PROPERTY_ID = -1,   /* 无效的属性ID */
    };

public:
    typedef  EffectServer_TargetPropertyTransform    SCHEME_DATA;

    CEffectTargetPropertyTransform(SCHEME_DATA &data)
        : m_data(data)
        , m_pEntity(0)
        , m_uidSource(INVALID_UID)
        , m_uidTarget(INVALID_UID)
        , m_nOldValue(-1)
    {

    }


    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }


    // 效果启用
    virtual bool			Start(EFFECT_CONTEXT * context, CONDITION_CONTEXT *pConditionContext)
    {
        if (context == 0 || context->pEntity == 0)
            return false;

        m_pEntity = context->pEntity;

        m_uidSource = context->uidOperator;
        m_uidTarget = context->uidTarget;
        // 无效的UID转换成统一无效标识
        if (m_uidSource == 0) m_uidSource = INVALID_UID;
        if (m_uidTarget == 0) m_uidTarget = INVALID_UID;

        // 改变属性
        changeProperty();

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
        // 还原修改前的属性值
        restoreTargetProperty();

        if (m_pEntity != 0)
        {
            m_pEntity = 0;
        }
    }


    // 克隆一个新效果
    virtual IEffect *       Clone()
    {
        return new CEffectTargetPropertyTransform(m_data);
    }

    // 释放
    virtual void			Release()
    {
        Stop();
        delete this;
    }


    int getPropertyID(int nVirtualProperyID)
    {
        if (nVirtualProperyID < ENTITY_PROPERTY::PROPERTY_MAX)
        {
            return nVirtualProperyID;
        }
        else
        {
            switch (nVirtualProperyID)
            {
            case PROPERTY_TRANSFROM_LOSE_HP:   return PROPERTY_MAX_HP;
            case PROPERTY_TRANSFROM_LOSE_HP_PCT: return PROPERTY_MAX_HP;
            case PROPERTY_TRANSFROM_WAR_AVERAGE_LEVEL: return PROPERTY_TRANSFROM_WAR_AVERAGE_LEVEL;
            default: return INVALID_PROPERTY_ID;
            }
        }
    }

    // 改变属性
    void changeProperty()
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        // 取得属性ID
        int nSourcePropertyID = getPropertyID(m_data.nPropertyTransfromType);        
        int nTargetPropertyID = getPropertyID(m_data.nPropertyTransfromBackType);
        // 无效的属性ID
        if (nSourcePropertyID == INVALID_PROPERTY_ID || nTargetPropertyID == INVALID_PROPERTY_ID)
        {
            return;
        }

        UID uidSource = (m_data.nSourceType == MA_TAGET_TO_TARGET) ? m_uidTarget : m_uidSource;    // 默认为操作者
        UID uidTarget = (m_data.nTargetType == MA_TAGET_TO_OPERATOR) ? m_uidSource : m_uidTarget;

        // 取得源对象属性
        int nSourcePropValue = getProperty_Integer(uidSource, nSourcePropertyID);
        // 取得目标对象属性
        int nTargetPropValue = getProperty_Integer(uidTarget, nTargetPropertyID);
        m_nOldValue = nTargetPropValue;

        double fChangeValue = (double)nSourcePropValue * m_data.fValue;

        // 设置目标对象属性
        setProperty_Integer(m_uidTarget, nTargetPropertyID, (int)fChangeValue);

        //WarningLn(__FUNCTION__", uid=" << uidTarget << ", oldvalue=" << m_nOldValue <<", sourcePropvalue="<< nSourcePropValue <<", newvalue="<< fChangeValue);
    }

    // 还原修改前的属性值
    void restoreTargetProperty()
    {
        if (m_nOldValue == -1)
        {
            return;
        }

        int nTargetPropertyID = getPropertyID(m_data.nPropertyTransfromBackType);
        // 无效的属性ID
        if (nTargetPropertyID == INVALID_PROPERTY_ID)
        {
            return;
        }
        UID uidTarget = (m_data.nTargetType == MA_TAGET_TO_OPERATOR) ? m_uidSource : m_uidTarget;

        // 设置目标对象属性
        setProperty_Integer(uidTarget, nTargetPropertyID, m_nOldValue);

        //WarningLn(__FUNCTION__", uid=" << uidTarget << ", oldvalue=" << m_nOldValue);
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity *				m_pEntity;

    UID                     m_uidSource;		// 源对象UID
    UID                     m_uidTarget;		// 目标对象UID
    int                     m_nOldValue;        // 未改变前属性值
};
