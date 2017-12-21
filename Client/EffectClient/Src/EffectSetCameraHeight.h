/*******************************************************************
** 文件名:	EffectSetCameraHeight.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	ljw
** 日  期:	3/30/2017
** 版  本:	1.0
** 描  述:	设置摄像机高度(Y坐标)
** 应  用:  	
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
#include "IClientGlobal.h"
#include "IRenderView.h"
#include "GameViewDef.h"

using namespace EFFECT_CLIENT;

class CEffecSetCameraHeight : public IEffect
{
public:
    typedef  EffectClient_SetCameraHeight    SCHEME_DATA;

    CEffecSetCameraHeight( SCHEME_DATA & data ) : m_data(data){}

    // 效果启用
    virtual bool			Start( EFFECT_CONTEXT * context )
    {
        if ( context==0 || context->pEntity==0 )
            return false;

        if(!context->pEntity->isHero())
            return false;

        gameview_set_camera_height cmd;
        cmd.fHeight = m_data.fHeight;
        gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_SET_CAMERA_HEIGHT,0,0,&cmd,sizeof(cmd) );

        return true;
    }

    // 效果停止
    virtual void			Stop()
    {
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() { return new CEffecSetCameraHeight(m_data); }

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
};
