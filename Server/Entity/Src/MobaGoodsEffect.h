#pragma once

#include "IEffect.h"
#include "IEntity.h"
class CMobaGoodsEffect
{
public:
    CMobaGoodsEffect(__IEntity *pEntity);
    ~CMobaGoodsEffect(void);
public:
    // 释放
    void release();

    // 添加效果 返回效果ID记录
    int mobaGoodsAddEffect(int nEffectID,void * pAddContext, size_t len);

    // 移除效果
    void mobaGoodsRemoveEffect(int nID);


    // 移除效果
    void releaseMobaGoodsEffect();

private:
    __IEntity*          m_pEntitiy;
    int                 m_nEffectIndexMax;    // 计数索引最大值
    map<int, IEffect*>  m_mapMobaGoodsEffect;// 装备效果相关
};

