/*******************************************************************
** 文件名:	EffectRecordPointBack.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/01/2016
** 版  本:	1.0
** 描  述:	效果-记录位置返回
            http://172.16.0.120/redmine/issues/2134
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ExternalHelper.h"
#include "PathFindHelper.h"
#include "IPathFindService.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectRecordPointBack : public IEffectEx, public IEventExecuteSink
{
public:
	typedef  EffectServer_RecordPointBack    SCHEME_DATA;

	CEffectRecordPointBack( SCHEME_DATA & data ) : m_data(data), m_pEntity(0), m_vLoc(0,0,0)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_SpellIDVector.clear();
        StringHelper::split(tempVector, m_data.strSpellID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_SpellIDVector.push_back(StringHelper::toInt(tempVector[nCount]));
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
		if ( context == 0 || context->pEntity == 0 )
			return false;

        m_pEntity = context->pEntity;

        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL, "CEffectRecordPointBack");
         
        m_vLoc = m_pEntity->getLocation();

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != 0)
        {
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL);

            m_pEntity = 0;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectRecordPointBack(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    /** 
    */
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch (wEventID)
        {
        case EVENT_ENTITY_CAST_SPELL:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_cast_spell))
                {
                    break;
                }
                event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

                bool bFind = false;
                int nCount = (int)m_SpellIDVector.size();
                for (int i=0; i<nCount; ++i)
                {
                    if (pCastSpell->nID == m_SpellIDVector[i])
                    {
                        bFind = true;
                        break;
                    }
                }
                // 未找到
                if (!bFind)
                {
                    break;
                }


                // 传送
                g_EHelper.Transport(m_pEntity, m_vLoc, true, 0, INT_MAX, TRANSPORT_REASON_SET_POS);
            }
            break;
        default:
            break;
        }
    }

private:
	SCHEME_DATA             m_data;

    __IEntity               *m_pEntity;

    // 记录点
    Vector3                 m_vLoc;

    // 技能ID容器
    vector<int>				m_SpellIDVector;
};