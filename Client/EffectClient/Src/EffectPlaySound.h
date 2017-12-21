/*******************************************************************
** 文件名:	EffectPlaySound.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	10/21/2015
** 版  本:	1.0
** 描  述:	效果-播放声音
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_CLIENT;

class CEffectPlaySound : public IEffect
{
public:
	typedef  EffectClient_PlaySound    SCHEME_DATA;

	CEffectPlaySound( SCHEME_DATA & data ) : m_data(data)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_SoundIDVector.clear();
        StringHelper::split(tempVector, m_data.strSoundID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_SoundIDVector.push_back(StringHelper::toInt(tempVector[nCount]));
        }
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if (context == 0 || context->pEntity == 0)
		{
			return false;
		}

        ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            return false;
        }

        ISchemeSkinSpell *pSchemeSkinSpell = pSchemeCenter->getSchemeSkinSpell();
        if (pSchemeSkinSpell == NULL)
        {
            return false;
        }

        IEntityProperty *pProperty = (IEntityProperty*)context->pEntity->getEntityPart(PART_PROPERTY);
        if (pProperty == NULL)
        {
            return false;
        }
        // 取得玩家皮肤
        int nSkinID = pProperty->getProperty_Integer(PROPERTY_SKIN);

        cmd_creature_play_sound playSound;
        
        playSound.nStop = m_data.nStop;
        int nCount = m_SoundIDVector.size();
        for (int i=0; i<nCount; ++i)
        {
            int nSoundID = m_SoundIDVector[i];
            // 替换皮肤声音
            SSkinSpell *pSkinSpell = pSchemeSkinSpell->getSkinSpell(nSkinID, SKIN_SPELL_SOUND, nSoundID);
            if (pSkinSpell != NULL)
            {
                nSoundID = pSkinSpell->nNewValue;
            }
            if (nSoundID == 0)
            {
                continue;
            }
		
            playSound.nSoundID = nSoundID;
            context->pEntity->sendCommandToEntityView(ENTITY_TOVIEW_PLAY_SOUND, 0, 0, &playSound, sizeof(playSound));
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectPlaySound(m_data); }

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

    // 声音ID容器
    vector<int>				m_SoundIDVector;
};
