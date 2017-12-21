/*******************************************************************
** 文件名:	EffectBeLongsKillAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/01/2016
** 版  本:	1.0
** 描  述:	效果-归属击杀获得BUFF
            http://172.16.0.120/redmine/issues/2212
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EntityHelper.h"
class CEffectBeLongsKillAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_BeLongsKillAddBuff    SCHEME_DATA;

	CEffectBeLongsKillAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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

        UID uid = m_pEntity->getUID();
        // 获得事件中心接口
        SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
        IEventEngine *pEventEngine = eventHelper.m_ptr;
        if (pEventEngine==NULL)
        {
            return false;
        }
        pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_KILL_BELONGS, UID_2_TYPE(uid), uid, "CEffectBeLongsKillAddBuff");

		return true;
	}


	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            UID uid = m_pEntity->getUID();

            // 获得事件中心接口
            SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
            IEventEngine *pEventEngine = eventHelper.m_ptr;
            if (pEventEngine != NULL)
            {
                pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_KILL_BELONGS, UID_2_TYPE(uid), uid);
            }

			m_pEntity = 0;
		}
	}
	 
	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectBeLongsKillAddBuff(m_data);
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
		case EVENT_GAME_ACTOR_KILL_BELONGS:
			{
                if (m_pEntity == NULL)
                {
                    break;
                }

				if (pszContext == NULL || nLen != sizeof(event_kill_belongs))
				{
					break;
				}
				event_kill_belongs *pBeLongs = (event_kill_belongs *)pszContext;

                __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
                if (pBuffPart == NULL)
                {
                    break;
                }

				UID uidSelf = m_pEntity->getUID();
				if (isInvalidUID(uidSelf))
				{
					break;
				}

				UID uidOperator = pBeLongs->uMurderId;
				if (isInvalidUID(uidOperator))
				{
					break;
				}

				UID uidTarget = pBeLongs->uDeadId;
				if (isInvalidUID(uidTarget))
				{
					break;
				}

                // 取得检测BUFF剩余时间
                int nLeftTime = pBuffPart->GetLeftTime(m_data.nCheckBuffID, uidSelf);

				// 检测死亡目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nDieTargetFilter, uidTarget))
				{
					break;
				}
                // 死亡者增加BUFF
                AddBuff(uidTarget, m_data.nDieBuffID, m_data.nDieBuffLevel, uidTarget);

                // 检测击杀目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nKillTargetFilter, uidOperator))
                {
                    break;
                }
                // 操作者增加BUFF
                AddBuff(uidOperator, m_data.nCheckBuffID, m_data.nCheckBuffLevel, uidOperator, nLeftTime);
			}
			break;
		default:
			break;
		}	
	}
private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
