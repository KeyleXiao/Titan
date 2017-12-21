/*******************************************************************
** 文件名:	EffectPropertyTransform.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/28/2015
** 版  本:	1.0
** 描  述:	效果-属性转换
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

class CEffectPropertyTransform : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_PropertyTransform    SCHEME_DATA;

	CEffectPropertyTransform( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_nLastValue(0)
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

		if (m_data.nPropertyTransfromType == m_data.nPropertyTransfromBackType)
		{
			ErrorLn("CEffectPropertyTransform error Loop nPropertyTransfromType="<<m_data.nPropertyTransfromType<<",nPropertyTransfromBackType="<<m_data.nPropertyTransfromBackType);
			return false;
		}

		m_pEntity = context->pEntity;

        if (m_data.nPropertyTransfromType < ENTITY_PROPERTY::PROPERTY_MAX)
        {
            g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + m_data.nPropertyTransfromType), "CEffectPropertyTransform");
        }
        else if (m_data.nPropertyTransfromType == PROPERTY_TRANSFROM_WAR_AVERAGE_LEVEL)
        {
            g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LEVEL), "CEffectPropertyTransform");
        }
        else if(m_data.nPropertyTransfromType == PROPERTY_TRANSFROM_LOSE_HP || m_data.nPropertyTransfromType == PROPERTY_TRANSFROM_LOSE_HP_PCT)
        {
            g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_HP), "CEffectPropertyTransform");
            g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MAX_HP), "CEffectPropertyTransform");
        }

		int nPropertyID = 0;
		if (m_data.nPropertyTransfromType < ENTITY_PROPERTY::PROPERTY_MAX)
		{
			nPropertyID = m_data.nPropertyTransfromType;
		}
		else
		{
			switch (m_data.nPropertyTransfromType)
			{
			case PROPERTY_TRANSFROM_LOSE_HP:
				{
					nPropertyID = PROPERTY_MAX_HP;
				}
				break;
			case PROPERTY_TRANSFROM_LOSE_HP_PCT:
				{
					nPropertyID = PROPERTY_MAX_HP;
				}
				break;
            case PROPERTY_TRANSFROM_WAR_AVERAGE_LEVEL:
                {
                    nPropertyID = PROPERTY_TRANSFROM_WAR_AVERAGE_LEVEL;
                }
                break;
			default:
				{
					ErrorLn("CEffectPropertyTransform error nPropertyTransfromType="<<m_data.nPropertyTransfromType);
					return false;
				}
				break;
			}
		}
		// 改变属性
		changeProperty(nPropertyID);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            if (m_data.nPropertyTransfromType < ENTITY_PROPERTY::PROPERTY_MAX)
            {
                g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + m_data.nPropertyTransfromType));
            }
            else if (m_data.nPropertyTransfromType == PROPERTY_TRANSFROM_WAR_AVERAGE_LEVEL)
            {
                g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LEVEL));
            }
            else if(m_data.nPropertyTransfromType == PROPERTY_TRANSFROM_LOSE_HP || m_data.nPropertyTransfromType == PROPERTY_TRANSFROM_LOSE_HP_PCT)
            {
                g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_HP));
                g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MAX_HP));
            }

			__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
			if (pEntityProperty != NULL)
			{
#ifdef ENTITY_CHECK_SKIN
                if(m_data.nPropertyTransfromBackType == PROPERTY_SKIN)
                {
                    TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_pEntity->getUID() <<", value="<< -m_nLastValue);
                }
#endif
				// 去除增加值
				pEntityProperty->addProperty_Integer(m_data.nPropertyTransfromBackType, -m_nLastValue);
			}

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectPropertyTransform(m_data);
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
        {
            return;
        }

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
		
		if (m_data.nPropertyTransfromType < ENTITY_PROPERTY::PROPERTY_MAX)
		{
			if (m_data.nPropertyTransfromType != nPropertyID)
			{
				return;
			}

			int nPropertyValue = pEntityProperty->getProperty_Integer(m_data.nPropertyTransfromType);
			nValue = (int)(m_data.fValue*nPropertyValue);
		}
		else
		{
			switch (m_data.nPropertyTransfromType)
			{
			case PROPERTY_TRANSFROM_LOSE_HP:
				{
					if (!(nPropertyID == PROPERTY_HP || nPropertyID == PROPERTY_MAX_HP))
					{
						return;
					}

					int nHp = pEntityProperty->getProperty_Integer(PROPERTY_HP);
					int nMaxHp = pEntityProperty->getProperty_Integer(PROPERTY_MAX_HP);

					nValue = (int)(m_data.fValue*(nMaxHp - nHp));
				}
				break;
			case PROPERTY_TRANSFROM_LOSE_HP_PCT:
				{
					if (!(nPropertyID == PROPERTY_HP || nPropertyID == PROPERTY_MAX_HP))
					{
						return;
					}

					int nHp = pEntityProperty->getProperty_Integer(PROPERTY_HP);
					int nMaxHp = pEntityProperty->getProperty_Integer(PROPERTY_MAX_HP);
					if (nMaxHp <= 0)
					{
						return;
					}
					float fPercent = (float)(nMaxHp - nHp)/nMaxHp;
					nValue = (int)(m_data.fValue*fPercent);
				}
				break;
            case PROPERTY_TRANSFROM_WAR_AVERAGE_LEVEL:
                {
                    __IWarPart* pWarPart = (__IWarPart*)m_pEntity->getEntityPart(PART_WAR);
                    if(pWarPart == NULL)
                    {
                        return;
                    }
                    int nWarID = pWarPart->getWarID();

                    WarHelper helper(nWarID);
                    IWarService *pWarService = helper.m_ptr;
                    if (pWarService == NULL)
                    {
                        return;
                    }

                    UID uidArray[256]={0};
                    int nArraySize= 256;
                    int nCount = pWarService->getAllPerson(uidArray, nArraySize);
                    if (nCount <= 0)
                    {
                        return;
                    }
                    
                    int nLevel = 0;
                    for (int i=0; i<nCount; ++i)
                    {
                        nLevel += getProperty_Integer(uidArray[i], PROPERTY_LEVEL);
                    }
                    // 算出平均等级
                    nLevel /= nCount;
                    // 算出值
                    nValue = (int)(m_data.fValue*nLevel);
                }
                break;
			default:
				{
					return;
				}
				break;
			}
		}

		// 差值
		int nDifferenceValue = nValue - m_nLastValue;
		if (nDifferenceValue != 0)
		{

#ifdef ENTITY_CHECK_SKIN
            if(m_data.nPropertyTransfromBackType == PROPERTY_SKIN)
            {
                TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_pEntity->getUID() <<", value="<< nDifferenceValue);
            }
#endif
			// 累加差值
			pEntityProperty->addProperty_Integer(m_data.nPropertyTransfromBackType, nDifferenceValue);
		}
		// 设置最后值
		m_nLastValue = nValue;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 最后值
	int						m_nLastValue;
};
