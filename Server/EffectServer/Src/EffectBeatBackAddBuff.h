/*******************************************************************
** 文件名:	EffectBeatBackAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/18/2015
** 版  本:	1.0
** 描  述:	效果-击退获得BUFF
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"

class CEffectBeatBackAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_BeatBackAddBuff    SCHEME_DATA;

	CEffectBeatBackAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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

		// 注册击退事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_BEAT_BACK, "CEffectBeatBackAddBuff");

		return true;
	}


	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销击退事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_BEAT_BACK);

			m_pEntity = 0;
		}
	}
	 
	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectBeatBackAddBuff(m_data);
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
		case EVENT_ENTITY_BEAT_BACK:
			{
				if (m_pEntity == NULL)
				{
					break;
				}

				if (m_data.nBuffID <= 0 || m_data.nBuffLevel <= 0)
				{
					break;
				}

				if (pszContext == NULL || nLen != sizeof(event_beat_back))
				{
					break;
				}
				event_beat_back *pBeatBack = (event_beat_back *)pszContext;

				UID uid = INVALID_UID;
				switch (m_data.nAddBuffType)
				{
				case BEAT_BACK_TARGET_TO_SELF:
					{
						uid = m_pEntity->getUID();
					}
					break;
				case BEAT_BACK_TARGET_TO_OPERATOR:
					{
						uid = pBeatBack->uidOperator;
					}
					break;
				case BEAT_BACK_TARGET_TO_TARGET:
					{
						uid = pBeatBack->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectBeatBackAddBuff nAddBuffType="<<m_data.nAddBuffType);
					}
					break;
				}
				if (isInvalidUID(uid))
				{
					break;
				}

				UID uidAdd = INVALID_UID;
				switch (m_data.nAddBuffTargetType)
				{
				case BEAT_BACK_TARGET_TO_SELF:
					{
						uidAdd = m_pEntity->getUID();
					}
					break;
				case BEAT_BACK_TARGET_TO_OPERATOR:
					{
						uidAdd = pBeatBack->uidOperator;
					}
					break;
				case BEAT_BACK_TARGET_TO_TARGET:
					{
						uidAdd = pBeatBack->uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectBeatBackAddBuff nAddBuffTargetType="<<m_data.nAddBuffType);
					}
					break;
				}
				if (isInvalidUID(uidAdd))
				{
					break;
				}

				// 增加BUFF
				AddBuff(uid, m_data.nBuffID, m_data.nBuffLevel, uidAdd);
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
