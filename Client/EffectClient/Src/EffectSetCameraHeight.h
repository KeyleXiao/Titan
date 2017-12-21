/*******************************************************************
** �ļ���:	EffectSetCameraHeight.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	ljw
** ��  ��:	3/30/2017
** ��  ��:	1.0
** ��  ��:	����������߶�(Y����)
** Ӧ  ��:  	
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

    // Ч������
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

    // Ч��ֹͣ
    virtual void			Stop()
    {
    }

    // ��¡һ����Ч��
    virtual IEffect *       Clone() { return new CEffecSetCameraHeight(m_data); }

    // �ͷ�
    virtual void			Release()
    {
        Stop();
        delete this;
    }

    // ȡ��Ч��ID
    virtual int				GetEffectID() { return m_data.nID; }

private:
    SCHEME_DATA             m_data;
};
