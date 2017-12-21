/*******************************************************************
** 文件名:	EffectRandPlayAnimation.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/09/2016
** 版  本:	1.0
** 描  述:	效果-随机播放动作
********************************************************************/
#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectRandPlayAnimation : public IEffect, public TimerHandler
{
public:
	typedef EffectClient_RandPlayAnimation SCHEME_DATA;

	CEffectRandPlayAnimation( SCHEME_DATA & data ):m_data(data),m_pEntity(NULL)
	{
        m_AnimationVector.clear();
        StringHelper::split(m_AnimationVector, m_data.strName, ';', " ");
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * pContext )
	{
		if ( pContext == 0 || pContext->pEntity == 0)
        {
			return false;
        }

		m_pEntity = pContext->pEntity;

        g_EHelper.SetTimer(0, m_data.nInterval, this);

        OnTimer(0);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            g_EHelper.KillTimer(0, this);

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectRandPlayAnimation(m_data); }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID() { return m_data.nID; }

    /////////////////////////////////////////TimerHandler/////////////////////////////////////////
    // 定时器到了
    virtual void OnTimer(unsigned long dwTimerID)
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        int nCount = m_AnimationVector.size();
        if (nCount <= 0)
        {
            return;
        }

        IEntityProperty *pEntityProperty = (IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return;
        }

        int nPASD = pEntityProperty->getProperty_Integer(PROPERTY_PASD);
        if (nPASD <= 0)
        {
            return;
        }

        // 计算概率
        int nRandNum = 0;
        rktRangeRand(nRandNum, 1, nCount);

        UID uid = m_pEntity->getUID();
        
        string strName = m_AnimationVector[nRandNum-1];
        // 是0号动作不用设置了
        if (strcmp(strName.c_str(), "0") == 0)
        {
            return;
        }

        // 发送状态变化事件
        event_entity_play_animation ePlayAnimation;
        ePlayAnimation.uid = uid;
        sstrcpyn(ePlayAnimation.szName, strName.c_str(), sizeof(ePlayAnimation.szName));
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
            sstrcpyn(playAnimation.szName, strName.c_str(), sizeof(playAnimation.szName));
            // 告诉显示层播放某个动作
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_PLAY_ANIMATION, 0, 0, &playAnimation, sizeof(playAnimation));
        }
    }

private:
	SCHEME_DATA             m_data;

    vector<string>			m_AnimationVector;

    IEntity                 *m_pEntity;
};
