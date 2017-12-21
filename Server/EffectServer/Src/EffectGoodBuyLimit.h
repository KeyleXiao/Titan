/*******************************************************************
** 文件名:	EffectGoodBuyLimit.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/01/2016
** 版  本:	1.0
** 描  述:	效果-物品限制购买
            http://172.16.0.120/redmine/issues/2909
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"

class CEffectGoodBuyLimit : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_GoodBuyLimit    SCHEME_DATA;

	CEffectGoodBuyLimit( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_GoodBuyLimitMap.clear();
        StringHelper::split(tempVector, m_data.strGoodID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            int nGoodID = StringHelper::toInt(tempVector[nCount]);
            if (nGoodID <= 0)
            {
                continue;
            }
            m_GoodBuyLimitMap.insert(GOOD_BUY_LIMIT_MAP::value_type(nGoodID, nGoodID));
        }
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

		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAN_BUY_GOOD, "CEffectGoodBuyLimit");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_CAN_BUY_GOOD);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectGoodBuyLimit(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	/** 
    @param   wEventID ：事件ID
    @param   bSrcType ：发送源类型
    @param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
    @param   pszContext : 上下文
    @param   nLen : 上下文长度
    @return  如果返回false : 则中断执行，否则继续向下执行
    @note     
    @warning 
    @retval buffer 
    */
    virtual bool		OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_CAN_BUY_GOOD:
            {
                if (m_pEntity == NULL)
                {
                    return false;
                }

				if (pszContext == NULL || nLen != sizeof(event_entity_can_buy_good))
				{
					return false;
				}
				event_entity_can_buy_good *pGood = (event_entity_can_buy_good *)pszContext;

                GOOD_BUY_LIMIT_MAP::iterator Iter = m_GoodBuyLimitMap.find(pGood->nGoodID);
                // 是限制购买物品
                if (Iter != m_GoodBuyLimitMap.end())
                {
                    pGood->ErrorCode = m_data.nType;
                    return false;
                }
			}
			break;
		default:
			break;
		}

        return true;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

    // 禁用技能表
    typedef map<int, int>	  GOOD_BUY_LIMIT_MAP;	
    GOOD_BUY_LIMIT_MAP		  m_GoodBuyLimitMap;
};
