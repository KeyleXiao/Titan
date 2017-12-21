/*******************************************************************
** 文件名:	EffectPropertyTransformLoop.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/28/2015
** 版  本:	1.0
** 描  述:	效果-属性转换循环
			如血量影响法强，法强影响血量，但不循环叠加效果
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectPropertyTransformLoop : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_PropertyTransformLoop    SCHEME_DATA;

	CEffectPropertyTransformLoop( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
	{
		// 清空已转换表
		m_TransformMap.clear();
		// 清空转换值表
		m_TransformValueMap.clear();
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

		if (m_data.nFrontType == m_data.nBackType)
		{
			ErrorLn("CEffectPropertyTransformLoop error Loop nFrontType="<<m_data.nFrontType<<",nBackType="<<m_data.nBackType);
			return false;
		}

		m_pEntity = context->pEntity;

		// 注册实体属性发生变化
        g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + m_data.nFrontType), "CEffectPropertyTransformLoop");
        g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + m_data.nBackType), "CEffectPropertyTransformLoop");

		// 改变属性
		changeProperty(m_data.nFrontType);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销实体属性发生变化
            g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + m_data.nFrontType));
            g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + m_data.nBackType));

			__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
			if (pEntityProperty != NULL)
			{
				for (TRANSFORM_VALUE_MAP::iterator Iter = m_TransformValueMap.begin(); Iter != m_TransformValueMap.end(); ++Iter)
				{
					// 要附加属性ID
					int nAppendPropertyID = 0;
					if (Iter->first == m_data.nFrontType)
					{
						nAppendPropertyID = m_data.nAppendFrontType;
					}
					else
					{
						nAppendPropertyID = m_data.nAppendBackType;
					}

#ifdef ENTITY_CHECK_SKIN
                    if(nAppendPropertyID == PROPERTY_SKIN)
                    {
                        TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_pEntity->getUID() <<", value="<< -Iter->second);
                    }
#endif
					// 去除增加值
					pEntityProperty->addProperty_Integer(nAppendPropertyID, -Iter->second);
				}
				// 清空转换值表
				m_TransformValueMap.clear();
			}
			// 清空已转换表
			m_TransformMap.clear();

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectPropertyTransformLoop(m_data);
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
        if(wEventID < EVENT_ENTITY_PROPERTY_CHANGE || wEventID > EVENT_ENTITY_PROPERTY_CHANGE_END)
            return;

        if (pszContext == NULL || nLen != sizeof(event_entity_property_change))
        {
            return;
        }
        event_entity_property_change *pPropertyChange = (event_entity_property_change *)pszContext;

        // 改变属性
        changeProperty(pPropertyChange->nPropertyID);
	}

	// 改变属性
	void changeProperty(int nPropertyID)
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

		int nValue = 0;
		
		if (!(nPropertyID == m_data.nFrontType || nPropertyID == m_data.nBackType))
		{
			return;
		}

		// 已转换表查找
		TRANSFORM_MAP::iterator IterTransForm = m_TransformMap.find(nPropertyID);
		if (IterTransForm != m_TransformMap.end())
		{
			// 2个都转换了，清空表，等待下次改变设置
			if (m_TransformMap.size() >= 2)
			{
				m_TransformMap.clear();
			}
			return;
		}
		else
		{
			// 设置已转换表
			m_TransformMap[nPropertyID] = true;

			// 增加值
			int nValue = 0;
			// 要增加的属性ID
			int nAddPropertyID = 0;
			// 要附加属性ID
			int nAppendPropertyID = 0;
			// 取得属性值
			int nPropertyValue = pEntityProperty->getProperty_Integer(nPropertyID);
			// 影响属性值
			int nChangePropertyValue = 0;
			// 转换值表查找
			TRANSFORM_VALUE_MAP::iterator IterChangeValue = m_TransformValueMap.find(nPropertyID);
			if (IterChangeValue != m_TransformValueMap.end())
			{
				nChangePropertyValue = IterChangeValue->second;
			}
			// 减去影响属性
			nPropertyValue -= nChangePropertyValue;
			// 是前者转换属性
			if (nPropertyID == m_data.nFrontType)
			{
				nValue = (int)(m_data.fFrontValue*nPropertyValue);

				nAddPropertyID = m_data.nBackType;

				nAppendPropertyID = m_data.nAppendBackType;
			}
			else
			{
				nValue = (int)(m_data.fBackValue*nPropertyValue);

				nAddPropertyID = m_data.nFrontType;

				nAppendPropertyID = m_data.nAppendFrontType;
			}

			// 上次值
			int nLastValue = 0;
			// 转换值表查找
			TRANSFORM_VALUE_MAP::iterator IterAddValue = m_TransformValueMap.find(nAddPropertyID);
			if (IterAddValue != m_TransformValueMap.end())
			{
				// 设置上次值
				nLastValue = IterAddValue->second;
			}

			// 差值(本次值-上次值-改变属性值)
			int nDifferenceValue = nValue - nLastValue;
			// 记录转换值
			m_TransformValueMap[nAddPropertyID] = nValue;
			// 为0时,不会抛下个事件，主动设置，清除
			if (nDifferenceValue == 0)
			{
				// 设置已转换表
				m_TransformMap[nAddPropertyID] = true;

				// 2个都转换了，清空表，等待下次改变设置
				if (m_TransformMap.size() >= 2)
				{
					m_TransformMap.clear();
				}
			}
			else
			{
#ifdef ENTITY_CHECK_SKIN
                if(nAppendPropertyID == PROPERTY_SKIN)
                {
                    TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_pEntity->getUID() <<", value="<< nDifferenceValue);
                }
#endif
				// 增加属性
				addProperty_Integer(m_pEntity->getUID(), nAppendPropertyID, nDifferenceValue);
			}
		}
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 已转换表
	typedef	map<int,bool>	TRANSFORM_MAP;
	TRANSFORM_MAP			m_TransformMap;

	// 转换值表
	typedef	map<int,int>	TRANSFORM_VALUE_MAP;
	TRANSFORM_VALUE_MAP		m_TransformValueMap;
};
