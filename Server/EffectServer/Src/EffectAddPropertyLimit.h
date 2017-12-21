/*******************************************************************
** 文件名:	EffectAddPropertyLimit.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭政林
** 日  期:	7/27/2016
** 版  本:	1.0
** 描  述:	效果-增加属性上限
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "TimerHandler.h"
#include "ExternalHelper.h"

class CEffectAddPropertyLimit : public IEffectEx
{
public:
	typedef  EffectServer_AddPropertyLimit SCHEME_DATA;

	CEffectAddPropertyLimit( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

        m_pEntity = context->pEntity;

        switch (m_data.nPropertyType)
        {
        case ADD_PROPERTY_LIMIT_APPEND_HP:
            {
                addLimitProperty(PROPERTY_HP, m_data.nPropertyType, m_data.nValue);
            }
            break;
        case ADD_PROPERTY_LIMIT_APPEND_MP:
            {
                addLimitProperty(PROPERTY_MP, m_data.nPropertyType, m_data.nValue);
            }
            break;
        case ADD_PROPERTY_LIMIT_APPEND_ENERGY:
            {
                addLimitProperty(PROPERTY_ENERGY, m_data.nPropertyType, m_data.nValue);
            }
            break;
        case ADD_PROPERTY_LIMIT_MAX_STAMINA:
            {
                addLimitProperty(PROPERTY_STAMINA, m_data.nPropertyType, m_data.nValue);
            }
            break;
        default:
            {
                ErrorLn("CEffectAddPropertyLimit error nPropertyType="<<m_data.nPropertyType);
                return false;
            }
            break;
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        switch (m_data.nPropertyType)
        {
        case ADD_PROPERTY_LIMIT_APPEND_HP:
            {
                addLimitProperty(PROPERTY_HP, m_data.nPropertyType, -m_data.nValue);
            }
            break;
        case ADD_PROPERTY_LIMIT_APPEND_MP:
            {
                addLimitProperty(PROPERTY_MP, m_data.nPropertyType, -m_data.nValue);
            }
            break;
        case ADD_PROPERTY_LIMIT_APPEND_ENERGY:
            {
                addLimitProperty(PROPERTY_ENERGY, m_data.nPropertyType, -m_data.nValue);
            }
            break;
        case ADD_PROPERTY_LIMIT_MAX_STAMINA:
            {
                addLimitProperty(PROPERTY_STAMINA, m_data.nPropertyType, -m_data.nValue);
            }
            break;
        default:
            {
                ErrorLn("CEffectAddPropertyLimit error Stop nPropertyType="<<m_data.nPropertyType);
            }
            break;
        }

        m_pEntity = 0;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectAddPropertyLimit(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    void addLimitProperty(int nPropID, int nPropertyType, int nAddValue)
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return;
        }

        // 取得设置前值
        int nOldValue = pEntityProperty->getProperty_Integer(nPropID);
        // 增加附加值会改变当前值
        pEntityProperty->addProperty_Integer(nPropertyType, nAddValue);
        // 取得当前值
        int nCurValue = pEntityProperty->getProperty_Integer(nPropID);

        // 附加值是扩大了BASE_IN_MULTIPLE倍,需要除掉
        switch (m_data.nPropertyType)
        {
        case ADD_PROPERTY_LIMIT_APPEND_HP:
        case ADD_PROPERTY_LIMIT_APPEND_MP:
        case ADD_PROPERTY_LIMIT_APPEND_ENERGY:
            {
                nAddValue = (int)(nAddValue/BASE_IN_MULTIPLE);
            }
            break;
        default:
            break;
        }

        int nChangeValue = 0;
        // 此情况表示减的时候
        if (nOldValue > nCurValue)
        {
            nChangeValue = nOldValue - nCurValue;
        }
        else
        {
            nChangeValue = nOldValue + nAddValue - nCurValue;
        }

        if (nChangeValue != 0)
        {
            pEntityProperty->addProperty_Integer(nPropID, nChangeValue);
        }
    }

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;
};
