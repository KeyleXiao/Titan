/*******************************************************************
** 文件名:	EffectUpgradeChangeProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭政林
** 日  期:	3/21/2016
** 版  本:	1.0
** 描  述:	效果-升级改变属性
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "TimerHandler.h"
#include "IServerGlobal.h"
using namespace EFFECT_SERVER;

class CEffectUpgradeChangeProperty : public IEffectChangeValue, public IEventExecuteSink
{
public:
	typedef  EffectServer_UpgradeChangeProperty    SCHEME_DATA;

	CEffectUpgradeChangeProperty( SCHEME_DATA & data ) : 
		m_data(data), m_pEntity(0), m_nLastVal(0), m_nAddVal(0)
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

        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_UPGRADE_LEVEL, "CEffectUpgradeChangeProperty");

        // 直接学会的时候也要设置一次
        changeProperty();

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity)
		{
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_UPGRADE_LEVEL);

            do 
            {
                if (m_nAddVal == 0)
                {
                    break;
                }

                IEntityProperty *pEntityProperty = (IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
                if (pEntityProperty == NULL)
                {
                    break;
                }

                // 皮肤不处理
                if(m_data.nType == PROPERTY_SKIN)
                {
                    break;
                }

                pEntityProperty->addProperty_Integer(m_data.nType, -m_nAddVal);

            } while (false);

			m_pEntity = 0;		
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectUpgradeChangeProperty(m_data);
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

    // 设置数值
    virtual void			SetValue(EFFECT_STRING_LIST *pListValue)
    {
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
		switch (wEventID)
		{
		case EVENT_ENTITY_UPGRADE_LEVEL:
			{
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_upgrade_level))
                {
                    break;
                }

                changeProperty();
            }
            break;
        default:
            break;
        }
    }

    void changeProperty()
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        IEntityProperty *pEntityProperty = (IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return;
        }

        // 取得玩家等级
        int nLevel = pEntityProperty->getProperty_Integer(PROPERTY_LEVEL);

        int nCurVal = pEntityProperty->getProperty_Integer(m_data.nType);
        // 扩大EFFECT_RATE倍，减少误差
        int nNewVal = (int)ceil((nCurVal*EFFECT_RATE + m_data.nVal*(nLevel-1))/EFFECT_RATE - nCurVal);

#ifdef ENTITY_CHECK_SKIN
        if(m_data.nType == PROPERTY_SKIN)
        {
            TraceLn(__FUNCTION__"["<< __LINE__ <<"]: change skin, uid="<< m_pEntity->getUID() <<", value="<< (nNewVal-m_nLastVal));
        }
#endif

        int nAddVal = nNewVal - m_nLastVal;
        m_nAddVal += nAddVal;
        pEntityProperty->addProperty_Integer( m_data.nType, nAddVal );

        m_nLastVal = nNewVal;
    }

public:
    SCHEME_DATA               m_data;

private:

    IEntity                   *m_pEntity;

    int                       m_nLastVal;

    // 增加值,结束要还原
    int                       m_nAddVal;
};
