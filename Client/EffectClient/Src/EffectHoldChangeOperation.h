/*******************************************************************
** 文件名:	EffectHoldChangeOperation.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	5/25/2016
** 版  本:	1.0
** 描  述:	效果-保持改变模式
需求描述：http://172.16.0.120/redmine/issues/1831
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
#include "IClientGlobal.h"
#include "IEventEngine.h"
using namespace EFFECT_CLIENT;

class CEffectHoldChangeOperation : public IEffect, public IEventVoteSink
{
public:
	typedef EffectClient_HoldChangeOperation SCHEME_DATA;

	CEffectHoldChangeOperation( SCHEME_DATA & data ):m_data(data),m_pEntity(0)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

		m_pEntity = pContext->pEntity;

        UID uid = m_pEntity->getUID();
        BYTE type  = UID_2_TYPE(uid);

        IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
        if (pEventEngine == NULL)
            return false;
		pEventEngine->Subscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CHANGE_OPERATION, type, uid, __FILE__);

        cmd_creature_changeOperation changeOperation;
        changeOperation.nType = m_data.nType;
        m_pEntity->sendCommandToEntityView( ENTITY_TOVIEW_CHANGE_OPERATION, 0, 0, &changeOperation, sizeof(changeOperation));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if ( m_pEntity!=0 )
		{
			UID uid = m_pEntity->getUID();
			BYTE type  = UID_2_TYPE(uid);

			IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
			if (pEventEngine != NULL)
			{
				pEventEngine->UnSubscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CHANGE_OPERATION, type, uid);
			}

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectHoldChangeOperation(m_data); }

	// 取得效果ID
	virtual int				GetEffectID(){ return m_data.nID; }

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
        case EVENT_ENTITY_CHANGE_OPERATION:
            {
                if (m_pEntity == NULL)
                {
                    return false;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_change_operation))
                {
                    return false;
                }

                event_entity_change_operation *pOperation = (event_entity_change_operation *)pszContext;
                if (pOperation->nType !=m_data.nType)
                {
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
	IEntity					*m_pEntity;
};
