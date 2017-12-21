/*******************************************************************
** 文件名:	EffectChangePropertyAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/14/2015
** 版  本:	1.0
** 描  述:	效果-改变属性增加Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "EntityHelper.h"
#include "ExternalHelper.h"

class CEffectChangePropertyAddBuff : public IEffectEx, public IEventExecuteSink
{
public:
	typedef  EffectServer_ChangePropertyAddBuff    SCHEME_DATA;

	CEffectChangePropertyAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTime(0)
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

		m_pEntity = context->pEntity;

		// 注册实体属性发生变化
		g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + m_data.nChangePropertyType), "CEffectChangePropertyAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销实体属性发生变化
			g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + m_data.nChangePropertyType));

			UID uid = m_pEntity->getUID();
			// 移除Buff
			RemoveBuff(uid, m_data.nBuffID, uid, uid);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectChangePropertyAddBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
        if(wEventID != EVENT_ENTITY_PROPERTY_CHANGE + m_data.nChangePropertyType)
        {
            ErrorLn(__FUNCTION__ << __LINE__);
            return;
        }

        if (pszContext == NULL || nLen != sizeof(event_entity_property_change))
        {
            return;
        }

        event_entity_property_change *pPropertyChange = (event_entity_property_change *)pszContext;
        // 属性ID
        int nPropertyID = pPropertyChange->nPropertyID;
        if (nPropertyID != m_data.nChangePropertyType)
        {
            ErrorLn(__FUNCTION__ << __LINE__);
            return;
        }

        // 取得当前时间
        DWORD dwTick = getTickCountEx();
        if (dwTick < m_dwLastTime + m_data.nInterval)
        {
            return;
        }

        if (m_pEntity == NULL)
        {
            return;
        }

        __IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return;
        }

        switch (m_data.nValueType)
        {
        case MA_VALUE_NUM:  // 固定值
            {
                int nValue = pEntityProperty->getProperty_Integer(nPropertyID);
                if (nValue < m_data.nLowValue || nValue > m_data.nHighValue)
                {
                    return;
                }
            }
            break;
        case MA_VALUE_PER:  // 百分比
            {
                int nValue = pEntityProperty->getProperty_Integer(nPropertyID);
                int nMaxValue = pEntityProperty->getProperty_Integer(nPropertyID+1);
                if (nValue > nMaxValue)
                {
                    // 初略判断下，说明当前属性和下个属性不是当前值和最大值，配置出问题了
                    ErrorLn("CEffectChangePropertyAddBuff MA_VALUE_PER error nMaxValue<nValue nPropertyID="<<nPropertyID);
                    return;
                }
                if (nMaxValue <= 0)
                {
                    return;
                }

                float fPercent = (float)nValue/nMaxValue;
                if (fPercent < m_data.nLowValue/EFFECT_RATE || fPercent > m_data.nHighValue/EFFECT_RATE)
                {
                    return;
                }
            }
            break;
        case MA_VALUE_LOSE_PER: // 损失百分比
            {
                int nValue = pEntityProperty->getProperty_Integer(nPropertyID);
                int nMaxValue = pEntityProperty->getProperty_Integer(nPropertyID+1);
                if (nValue > nMaxValue)
                {
                    // 初略判断下，说明当前属性和下个属性不是当前值和最大值，配置出问题了
                    ErrorLn("CEffectChangePropertyAddBuff MA_VALUE_LOSE_PER error nMaxValue<nValue nPropertyID="<<nPropertyID);
                    return;
                }
                if (nMaxValue <= 0)
                {
                    return;
                }

                float fPercent = (float)(nMaxValue-nValue)/nMaxValue;
                if (fPercent < m_data.nLowValue/EFFECT_RATE || fPercent > m_data.nHighValue/EFFECT_RATE)
                {
                    return;
                }
            }
            break;
        default:
            {
                ErrorLn("CEffectChangePropertyAddBuff error nValueType="<<m_data.nValueType);
                return;
            }
            break;
        }

        UID uid = m_pEntity->getUID();
        // 增加buff
        AddBuff(uid, m_data.nBuffID, m_data.nBuffLevel, uid);

        // 设置上次时间
        m_dwLastTime = dwTick;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 上次时间
	DWORD					m_dwLastTime;
};
