/*******************************************************************
** 文件名:	EffectMapFly.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/17/2017
** 版  本:	1.0
** 描  述:	效果-地图飞檐走壁
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "SpellMessageDef.h"

class CEffectMapFly : public IEffectEx
{
public:
	typedef  EffectServer_MapFly    SCHEME_DATA;

	CEffectMapFly( SCHEME_DATA &data ) : m_data(data)
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
        {
			return false;
        }

        __IEntity *pEntity = context->pEntity;

        ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            return false;
        }

        // 直接以区域ID作为丛林ID
        ISchemeAllMapsArea * pAllMapsArea = pSchemeCenter->getSchemeMapArea();
        if (pAllMapsArea == NULL)
        {
            return false;
        }

        int nMapID = pEntity->getMapID();
        ISingleMapArea *pMapArea = pAllMapsArea->getMapArea(nMapID, eAreaTypeFly);
        if (pMapArea == NULL)
        {
            return false;
        }

        ISchemeMapInfo *pSchemeMap = pSchemeCenter->getSchemeMapInfo();
        if (pSchemeMap == NULL)
        {
            return false;
        }

        Vector3 vLoc = pEntity->getLocation();
        int nAreaID = pMapArea->getAreaID(vLoc);
        if (nAreaID < 0)
        {
            return false;
        }

        SSpellMapFly mapFly;
        mapFly.fWalkHeight = m_data.fWalkHeight;
        mapFly.vAngle = pEntity->getAngle();

        Vector3 vConvex;
        if (!pMapArea->getConvex(nAreaID, 0, vConvex))
        {
            return false;
        }
        mapFly.vTileA = vConvex;

        if (!pMapArea->getConvex(nAreaID, 1, vConvex))
        {
            return false;
        }
        mapFly.vTileB = vConvex;

        if (!pMapArea->getConvex(nAreaID, 2, vConvex))
        {
            return false;
        }
        mapFly.vTileC = vConvex;

        if (!pMapArea->getConvex(nAreaID, 3, vConvex))
        {
            return false;
        }
        mapFly.vTileD = vConvex;

        g_EHelper.broadCastInAOI(pEntity, vLoc, PART_SPELL, SPELL_MSG_MAP_FLY, (char *)&mapFly, sizeof(mapFly));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectMapFly(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
};
