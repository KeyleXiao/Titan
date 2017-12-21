/*******************************************************************
** 文件名:	EffectAccumulateDamageAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/26/2016
** 版  本:	1.0
** 描  述:	效果-累积伤害增加Buff
            http://172.16.0.120/redmine/issues/1640
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

class CEffectAccumulateDamageAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AccumulateDamageAddBuff    SCHEME_DATA;

	CEffectAccumulateDamageAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_nAccumulateValue(0)
	{
        m_DamageVetor.clear();
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

		// 注册伤害事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectAccumulateDamageAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销伤害事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAccumulateDamageAddBuff(m_data);
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
		switch (wEventID)
		{
		case EVENT_ENTITY_DAMAGE:	// 伤害事件
			{
				if (m_pEntity == NULL)
				{
					break;
				}

                // 取得冷却部件
                __IFreezePart *pFreezePart = (__IFreezePart*)m_pEntity->getEntityPart(PART_FREEZE);
                if (pFreezePart == NULL)
                {
                    break;
                }
                //  检测冷却
                if (!pFreezePart->CanWork(m_data.nClassID, m_data.nCoolDownID))
                {
                    break;
                }
                
				if (pszContext == NULL || nLen != sizeof(event_entity_damage))
				{
					break;
				}
				event_entity_damage *pDamage = (event_entity_damage*)pszContext;

				// 自己UID
				UID uidSelf = m_pEntity->getUID();
				// 是自己伤害自己不处理
				if (uidSelf == pDamage->uidOperator)
				{
					break;
				}

                if (pDamage->nDamageHP <= 0)
                {
                    break;
                }

                // 伤害类型判断
                if ((pDamage->nDamageType&m_data.nDamageType) != m_data.nDamageType)
                {
                    break;
                }

                UID uidPropertyTarget = INVALID_UID;
                switch (m_data.nPropertyTargetType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uidPropertyTarget = uidSelf;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidPropertyTarget = pDamage->uidOperator;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidPropertyTarget = pDamage->uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectAccumulateDamageAddBuff error nPropertyTargetType="<<m_data.nPropertyTargetType);
                        return;
                    }
                    break;
                }
                if (isInvalidUID(uidPropertyTarget))
                {
                    return;
                }

                // 检测目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nPropertyTargetFilter, uidPropertyTarget))
                {
                    return;
                }

                UID uidAddTargetBuff = INVALID_UID;
                switch (m_data.nAddBuffTargetType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uidAddTargetBuff = uidSelf;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidAddTargetBuff = pDamage->uidOperator;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidAddTargetBuff = pDamage->uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectAccumulateDamageAddBuff error nAddBuffTargetType="<<m_data.nAddBuffTargetType);
                        return;
                    }
                    break;
                }
                if (isInvalidUID(uidAddTargetBuff))
                {
                    return;
                }

                // 取得属性值
                int nPropertyValue = getProperty_Integer(uidPropertyTarget, m_data.nType);
                // 算出伤害累积值
                m_nAccumulateValue = (int)(m_data.nFixedValue + nPropertyValue*m_data.fFactor);

                AccumulateDamageInfo info;
                info.nDamageHp = pDamage->nDamageHP;
                info.dwTime = getTickCountEx();
                // 插入容器
                m_DamageVetor.push_back(info);

                // 检测增加Buff
                if (checkAddBuff())
                {
                    // 清除容器
                    m_DamageVetor.clear();

                    // 设置冷却
                    pFreezePart->Work(m_data.nClassID, m_data.nCoolDownID);

                    SBuffContext BuffContext;
                    BuffContext.nID = pDamage->nID;
                    // 增加Buff
                    AddBuff(uidAddTargetBuff, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
                }
			}
			break;
		default:
			break;
		}
	}

    // 检测增加Buff
    bool checkAddBuff()
    {
        if (m_DamageVetor.empty())
        {
            return false;
        }

        bool bAddBuff = false;

        int nDamageHp = 0;
        DWORD dwTime = 0;
        vector<AccumulateDamageInfo>::iterator Iter = m_DamageVetor.begin(); 
        while (Iter != m_DamageVetor.end())
        {
            if (dwTime == 0)
            {
                nDamageHp += Iter->nDamageHp;
                dwTime = Iter->dwTime;
            }
            else
            {
                // 不足时间，累积伤害
                if (dwTime + m_data.nAccumulateTime > Iter->dwTime)
                {
                    nDamageHp += Iter->nDamageHp;
                }
                else    // 第一个元素和最后一个元素相差累积时间，需要删除第一个元素，并重新递归遍历
                {
                    // 移除第一个元素
                    vector<AccumulateDamageInfo>::iterator Iter = m_DamageVetor.begin();
                    m_DamageVetor.erase(Iter);

                    //递归检测
                    return checkAddBuff();
                }
            }

            // 大于累积伤害值
            if (nDamageHp >= m_nAccumulateValue)
            {
                bAddBuff = true;
                break;
            }
            else
            {
                ++Iter;
            }
        }

        return bAddBuff;
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

    struct AccumulateDamageInfo
    {
        int nDamageHp;
        DWORD dwTime;

        AccumulateDamageInfo()
        {
            memset(this, 0, sizeof(*this));
        }
    };
    vector<AccumulateDamageInfo>    m_DamageVetor;

    // 伤害累积值
    int     m_nAccumulateValue;
};
