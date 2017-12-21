/*******************************************************************
** 文件名:	EffectImmuneEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/10/2015
** 版  本:	1.0
** 描  述:	效果-免疫效果(伤害,治疗)
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"

class CEffectImmuneVoteEffect : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_ImmuneVoteEffect    SCHEME_DATA;

    CEffectImmuneVoteEffect( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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

		// 注册免疫效果事件
		g_EHelper.Subscibe(m_pEntity, this, (EventKey)m_data.nType, "CEffectImmuneVoteEffect");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 注销免疫效果事件
            g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)m_data.nType);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectImmuneVoteEffect(m_data);
	}

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
	virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
        if (wEventID == m_data.nType)
        {
            return false;
        }

		return true;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
