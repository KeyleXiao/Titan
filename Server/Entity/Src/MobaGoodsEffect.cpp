#include "StdAfx.h"
#include "MobaGoodsEffect.h"
#include "ICondition.h"
#include "IServerGlobal.h"


CMobaGoodsEffect::CMobaGoodsEffect(__IEntity *pEntity): 
    m_nEffectIndexMax(1),
    m_pEntitiy(pEntity)
{
}


CMobaGoodsEffect::~CMobaGoodsEffect(void)
{
}

int CMobaGoodsEffect::mobaGoodsAddEffect(int nEffectID,void * pAddContext, size_t len)
{
    if (m_pEntitiy == NULL)
    {
        return 0;
    }
    if (pAddContext == NULL || len == 0)
    {
        return 0;
    }

    if (len != sizeof(EFFECT_CONTEXT))
    {
        return 0;
    }

    EFFECT_CONTEXT * pContext = (EFFECT_CONTEXT *)pAddContext;
    pContext->pEntity = m_pEntitiy;


    IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
    if(pEffectFactory == NULL)
    {
        ErrorLn(__FUNCTION__": -- pEffectFactory == NULL")
        return 0;
    }

    IEffect *pEffect = pEffectFactory->BuildEffect(nEffectID);
    if (pEffect == NULL)
    {
        // 处理效果加不上的情况
        ErrorLn(__FUNCTION__": -- EffectID = "<< nEffectID);
        return 0;	
    }

    if (!pEffect->Start(pContext, NULL))
    {
        ErrorLn(__FUNCTION__": -- effect cant start EffectID = "<< nEffectID);
        return 0;
    }

    map<int, IEffect*>::iterator iterFind = m_mapMobaGoodsEffect.find(m_nEffectIndexMax);
    if (iterFind != m_mapMobaGoodsEffect.end())
    {
        iterFind->second->Stop();
        iterFind->second->Release();
        WarningLn(__FUNCTION__": -- effect cant insert EffectID = "<< nEffectID<<" Index= "<< m_nEffectIndexMax);
    }

    m_mapMobaGoodsEffect.insert(pair<int, IEffect*>(m_nEffectIndexMax, pEffect));
    int nRecIndex = m_nEffectIndexMax++;
    return nRecIndex;
}

void CMobaGoodsEffect::mobaGoodsRemoveEffect(int nID)
{
    map<int, IEffect*>::iterator iterFind = m_mapMobaGoodsEffect.find(nID);
    if(iterFind == m_mapMobaGoodsEffect.end())
    {
        return;
    }
    if (iterFind->second)
    {
        iterFind->second->Stop();
        iterFind->second->Release();
    }
    
    m_mapMobaGoodsEffect.erase(iterFind);
}

void CMobaGoodsEffect::releaseMobaGoodsEffect()
{
    map<int, IEffect*>::iterator iterFind = m_mapMobaGoodsEffect.begin();
    for (;iterFind != m_mapMobaGoodsEffect.end(); ++ iterFind)
    {
        if (iterFind->second)
        {
            iterFind->second->Stop();
            iterFind->second->Release();
        }
        
    }

    m_mapMobaGoodsEffect.clear();
}

void CMobaGoodsEffect::release()
{
    delete this;
}
