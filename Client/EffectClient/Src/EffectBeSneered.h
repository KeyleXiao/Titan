/*******************************************************************
** �ļ���:	EffectBeSneered.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	�ΰ
** ��  ��:	2/09/2017
** ��  ��:	1.0
** ��  ��:	Ч��-������
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

    // Ч������
    virtual bool			Start( EFFECT_CONTEXT * context )
    {
        return false;
    }

    // Ч��ֹͣ
    virtual void			Stop()
    {

    }

    // ��¡һ����Ч��
    virtual IEffect *       Clone() 
    {
        return new CEffectBeSneered(m_data);
    }

    // �ͷ�
    virtual void			Release()
    {
        Stop();
        delete this;
    }

    // ȡ��Ч��ID
    virtual int				GetEffectID()
    {
        return m_data.nID;
    }

private:
    SCHEME_DATA               m_data;
};
