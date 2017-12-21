/*******************************************************************
** 文件名:	EffectSubscibePlayAnimation.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/09/2016
** 版  本:	1.0
** 描  述:	效果-订阅动作播放动作
********************************************************************/
#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectSubscibePlayAnimation : public IEffect, public IEventExecuteSink
{
public:
	typedef EffectClient_SubscibePlayAnimation SCHEME_DATA;

	CEffectSubscibePlayAnimation( SCHEME_DATA & data ):m_data(data),m_pEntity(NULL),m_uidTarget(0)
	{
        m_SubscibeAnimationVector.clear();
        StringHelper::split(m_SubscibeAnimationVector, m_data.strSubscibeAnimationName, ';', " ");

        m_PlayAnimationVector.clear();
        StringHelper::split(m_PlayAnimationVector, m_data.strPlayAnimationName, ';', " ");

        if (m_SubscibeAnimationVector.size() != m_PlayAnimationVector.size())
        {
            ErrorLn("CEffectSubscibePlayAnimation m_SubscibeAnimationVector != m_PlayAnimationVector");
        }
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * pContext )
	{
		if ( pContext == 0 || pContext->pEntity == 0)
        {
			return false;
        }

		m_pEntity = pContext->pEntity;

        switch (m_data.nTagetType)
        {
        case MA_TAGET_TO_SELF:
            {
                m_uidTarget = m_pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uidTarget = pContext->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                m_uidTarget = pContext->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectSubscibePlayAnimation error nTagetType="<<m_data.nTagetType);
                return false;
            }
            break;
        }
        if (m_uidTarget == 0 || m_uidTarget == INVALID_UID)
        {
            return false;
        }

        UID uid = m_uidTarget;
        BYTE type = UID_2_TYPE(uid);
        IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
        if (pEventEngine == NULL)
        {
            return false;
        }
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_PLAY_ANIMATION, type, uid, __FILE__);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            if (m_uidTarget != 0 && m_uidTarget != INVALID_UID)
            {
                UID uid = m_uidTarget;
                BYTE type = UID_2_TYPE(uid);
                IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
                if (pEventEngine != NULL)
                {
                    pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_PLAY_ANIMATION, type, uid);
                }
            }
            m_uidTarget = 0;

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectSubscibePlayAnimation(m_data); }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID() { return m_data.nID; }

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
    virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch (wEventID)
        {
        case EVENT_ENTITY_PLAY_ANIMATION:
            {
                if (pszContext == NULL || nLen != sizeof(event_entity_play_animation))
                {
                    break;
                }
                event_entity_play_animation *pPlayAnimation = (event_entity_play_animation *)pszContext;

                int nFindIndex = -1;
                int nCount = m_SubscibeAnimationVector.size();
                for (int i=0; i<nCount; ++i)
                {
                    string strName = m_SubscibeAnimationVector[i].c_str();
                    if (strcmp(strName.c_str(), "0") == 0)
                    {
                        continue;
                    }

                    if (strcmp(strName.c_str(), pPlayAnimation->szName) == 0)
                    {
                        continue;
                    }

                    nFindIndex = i;
                    break;
                }
                // 未找到
                if (nFindIndex < 0)
                {
                    break;
                }
                if (nFindIndex >= (int)m_PlayAnimationVector.size())
                {
                    break;
                }

                string strSubscibeAnimationName = m_SubscibeAnimationVector[nFindIndex].c_str();
                string strPlayAnimationName = m_PlayAnimationVector[nFindIndex].c_str();
                // 如果相当会死循环
                if (strcmp(strSubscibeAnimationName.c_str(), strPlayAnimationName.c_str()) == 0)
                {
                    break;
                }

                IEntityProperty *pEntityProperty = (IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
                if (pEntityProperty == NULL)
                {
                    break;
                }

                int nPASD = pEntityProperty->getProperty_Integer(PROPERTY_PASD);
                if (nPASD <= 0)
                {
                    break;
                }

                UID uid = m_pEntity->getUID();

                // 发送状态变化事件
                event_entity_play_animation ePlayAnimation;
                ePlayAnimation.uid = uid;
                sstrcpyn(ePlayAnimation.szName, strPlayAnimationName.c_str(), sizeof(ePlayAnimation.szName));
                if (m_pEntity->fireVote(EVENT_ENTITY_PLAY_ANIMATION, (char *)&ePlayAnimation, sizeof(ePlayAnimation)))
                {
                    m_pEntity->fireEvent(EVENT_ENTITY_PLAY_ANIMATION, (char *)&ePlayAnimation, sizeof(ePlayAnimation));

                    cmd_creature_playAnimation playAnimation;
                    playAnimation.uidOperator = uid;
                    playAnimation.uidTarget = uid;
                    playAnimation.fPASD = nPASD/ZOOM_IN_MULTIPLE;
                    playAnimation.bAlreadyOver = 0;
                    // 设置默认值
                    if (playAnimation.fPASD <= 0.01f)
                    {
                        playAnimation.fPASD = 1.0f;
                    }
                    sstrcpyn(playAnimation.szName, strPlayAnimationName.c_str(), sizeof(playAnimation.szName));
                    // 告诉显示层播放某个动作
                    m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_PLAY_ANIMATION, 0, 0, &playAnimation, sizeof(playAnimation));
                }
            }
            break;
        default:
            break;
        }
    }

private:
	SCHEME_DATA             m_data;

    IEntity                 *m_pEntity;

    UID                     m_uidTarget;

    vector<string>			m_SubscibeAnimationVector;
    vector<string>			m_PlayAnimationVector;
};
