/*******************************************************************
** 文件名:	EffectBeSneered.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	2/09/2017
** 版  本:	1.0
** 描  述:	效果-被嘲讽
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IMovePart.h"
using namespace EFFECT_CLIENT;

class CEffectBeSneered : public IEffect
{
public:
    typedef  EffectClient_BeSneered    SCHEME_DATA;

    CEffectBeSneered( SCHEME_DATA & data ) : m_data(data)
    {
    }

    // 效果启用
    virtual bool			Start( EFFECT_CONTEXT * context )
    {
        return false;
    }

    // 效果停止
    virtual void			Stop()
    {

    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectBeSneered(m_data);
    }

    // 释放
    virtual void			Release()
    {
        Stop();
        delete this;
    }

    // 取得效果ID
    virtual int				GetEffectID()
    {
        return m_data.nID;
    }

private:
    SCHEME_DATA               m_data;
};
