#pragma once

#include "IEffect.h"
#include "IEntity.h"
class CMobaGoodsEffect
{
public:
    CMobaGoodsEffect(__IEntity *pEntity);
    ~CMobaGoodsEffect(void);
public:
    // �ͷ�
    void release();

    // ���Ч�� ����Ч��ID��¼
    int mobaGoodsAddEffect(int nEffectID,void * pAddContext, size_t len);

    // �Ƴ�Ч��
    void mobaGoodsRemoveEffect(int nID);


    // �Ƴ�Ч��
    void releaseMobaGoodsEffect();

private:
    __IEntity*          m_pEntitiy;
    int                 m_nEffectIndexMax;    // �����������ֵ
    map<int, IEffect*>  m_mapMobaGoodsEffect;// װ��Ч�����
};

