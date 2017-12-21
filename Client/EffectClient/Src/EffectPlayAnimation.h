/*******************************************************************
** 文件名:	EffectPlayAnimation.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/29
** 版  本:	1.0
** 描  述:	效果: 播放动画
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IEntityProperty.h"
using namespace EFFECT_CLIENT;

class CEffectPlayAnimation : public IEffect
{
public:
	typedef EffectClient_PlayAnimation SCHEME_DATA;

	CEffectPlayAnimation( SCHEME_DATA & data ):m_data(data)
	{
        m_AnimationVector.clear();
        StringHelper::split(m_AnimationVector, m_data.strName, ';', " ");
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * pContext )
	{
		if ( pContext==0 )
			return false;

		IEntity *pEntity = pContext->pEntity;
		if (pEntity == 0)
			return false;

		UID uid = pEntity->getUID();

        int nCount = m_AnimationVector.size();
        for (int i=0; i<nCount; ++i)
        {
            string strName = m_AnimationVector[i];
            // 是0号动作不用设置了
            if (strcmp(strName.c_str(), "0") == 0)
            {
                continue;
            }

            // 发送状态变化事件
            event_entity_play_animation ePlayAnimation;
            ePlayAnimation.uid = uid;
            sstrcpyn(ePlayAnimation.szName, strName.c_str(), sizeof(ePlayAnimation.szName));
            if (pEntity->fireVote(EVENT_ENTITY_PLAY_ANIMATION, (char *)&ePlayAnimation, sizeof(ePlayAnimation)))
            {
                pEntity->fireEvent(EVENT_ENTITY_PLAY_ANIMATION, (char *)&ePlayAnimation, sizeof(ePlayAnimation));
                
                cmd_creature_playAnimation playAnimation;
                playAnimation.uidOperator = pContext->uidOperator;
                playAnimation.uidTarget = pContext->uidTarget;
                playAnimation.fPASD = pContext->nPASD/ZOOM_IN_MULTIPLE;
                playAnimation.bAlreadyOver = pContext->bOnAppear; 
                // 设置默认值
                if (playAnimation.fPASD <= 0.01f)
                {
                    playAnimation.fPASD = 1.0f;
                }
                sstrcpyn(playAnimation.szName, strName.c_str(), sizeof(playAnimation.szName));
                // 告诉显示层播放某个动作
                pEntity->sendCommandToEntityView(ENTITY_TOVIEW_PLAY_ANIMATION, 0, 0, &playAnimation, sizeof(playAnimation));
            }
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectPlayAnimation(m_data); }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID() { return m_data.nID; }
private:
	SCHEME_DATA             m_data;

    vector<string>			m_AnimationVector;
};
