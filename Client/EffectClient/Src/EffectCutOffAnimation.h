/*******************************************************************
** 文件名:	EffectCutOffAnimation.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/14/2015
** 版  本:	1.0
** 描  述:	效果-截断动作(用于截取后续动作，不冲掉当前播放动作)
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
#include "IClientGlobal.h"
#include "IEventEngine.h"
using namespace EFFECT_CLIENT;

class CEffectCutOffAnimation : public IEffect, public IEventVoteSink
{
public:
	typedef EffectClient_CutOffAnimation SCHEME_DATA;

	CEffectCutOffAnimation( SCHEME_DATA & data ):m_data(data),m_pEntity(0)
	{
        m_SchemeVector.clear();
        StringHelper::split(m_SchemeVector, m_data.strAnimation, ';', " ");
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
		pEventEngine->Subscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PLAY_ANIMATION, type, uid, __FILE__);

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
				pEventEngine->UnSubscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PLAY_ANIMATION, type, uid);
			}

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectCutOffAnimation(m_data); }

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
        case EVENT_ENTITY_PLAY_ANIMATION:
            {
                if (pszContext == NULL || nLen != sizeof(event_entity_play_animation))
                {
                    return false;
                }
                event_entity_play_animation *pPlayAnimation = (event_entity_play_animation *)pszContext;

                if (m_SchemeVector.empty())
                {
                    return true;
                }
                else
                {
                    int nCount = m_SchemeVector.size();
                    if (nCount == 1 && strcmp(m_SchemeVector[0].c_str(), "0") == 0)
                    {
                        return false;
                    }

                    for (int i=0; i<nCount; ++i)
                    {
                        if (strcmp(pPlayAnimation->szName, m_SchemeVector[i].c_str()) == 0)
                        {
                            return false;
                        }
                    }
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

    typedef vector<string>	SCHEME_VECTOR;	
    SCHEME_VECTOR		    m_SchemeVector;
};
