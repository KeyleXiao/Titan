/*******************************************************************
** 文件名:	EffectTransLocation.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/02/2015
** 版  本:	1.0
** 描  述:	效果-改变位置
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ExternalHelper.h"
#include "PathFindHelper.h"
#include "IPathFindService.h"
#include "IWarPart.h"
#include "Game_ManagedDef.h"

using namespace rkt;
using namespace EFFECT_SERVER;

/* 效果： 改变位置
*/
class CEffectTransLocation : public IEffectEx
{
public:
	typedef  EffectServer_TransLocation    SCHEME_DATA;

	CEffectTransLocation( SCHEME_DATA & data ) : m_data(data)
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

        __IEntity *pEntity = context->pEntity;
		// 取得实体属性部件
		__IEntityProperty *pProperty = (__IEntityProperty *)pEntity->getEntityPart(PART_PROPERTY);
		if (pProperty == NULL)
		{
			return false;
		}
		// 目标地图	(暂时不支持)
		int targetMapID = 0;
		// 目标坐标
		Vector3 vNowLoc;
        int nAngle = INT_MAX;
        int nReason = TRANSPORT_REASON_FLASH;
        switch(m_data.nType)
        {
        case TRANSLOCATION_TILE:
            {
                vNowLoc = context->ptTargetTile;
            }
            break;
        case TRANSLOCATION_TARGET:
            {
                if (isInvalidUID(context->uidTarget))
                {
                    return false;
                }
                vNowLoc = getLocation(context->uidTarget);
            }
            break;
        case TRANSLOCATION_BACKCITY:
            {
                if (!GetMapBackCityLoc(pEntity, targetMapID, vNowLoc, nAngle))
                {
                    return false;
                }
                nReason = TRANSPORT_REASON_BACKCITY;
            }
            break;
        default:
            {
                TraceLn("Change position not other types,m_data.nType="<<m_data.nType);
                return false;
            }
            break;
        }
        if (equalto0(vNowLoc.getLength()))
        {
            return false;
        }

        UID uid = pEntity->getUID();
        // 场景ID
        int nSceneID = UID_2_SCENE(uid);
        PathFindHelper helper(nSceneID);
        IPathFindService *pPathService = helper.m_ptr;
        if (pPathService == NULL)
        {
            return false;
        }
        // 找到地面的点
        pPathService->getHeight(&vNowLoc, vNowLoc.y);

        g_EHelper.Transport(pEntity, vNowLoc, m_data.nTransPort == MA_TRUE, 0, nAngle, nReason);

        // 这种接口不能用 只设置了属性 并不会触发AOI管理器里的位置改变并且没有检测位置合法性
        //pEntity->setLocation(vNowLoc);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectTransLocation(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	bool					GetMapBackCityLoc(__IEntity *pEntity, int &nBackMapID, Vector3 &ptBackLoc, int &nAngle)
	{
		if (pEntity == NULL)
		{
			return false;
		}
		ptBackLoc = Vector3();

		int nMapID = pEntity->getMapID();
		int nCampID = pEntity->getIntProperty(PROPERTY_CAMP);

		ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
		if (pSchemeCenter)
		{
			ISchemeMapLand* pSchemeMapLand = pSchemeCenter->getSchemeMapLand();
			if (pSchemeMapLand)
			{
				SMapLandSchemeInfo * pMapLandSchemeInfo = pSchemeMapLand->GetMapLandSchemeInfo(nMapID, EGM_Moba, nCampID);
				if (pMapLandSchemeInfo && !pMapLandSchemeInfo->vecTransportBackPos.empty())
				{
                    __IWarPart* pWarPart = (__IWarPart*)pEntity->getEntityPart(PART_WAR);
                    if(pWarPart != NULL)
                    {
                        int nIndex = pWarPart->getIndex();
                        if(nIndex >= 0)
                        {
                            nIndex = nIndex % pMapLandSchemeInfo->vecTransportBackPos.size(); 
                        }
                        else
                        {
                            nIndex = rand() % pMapLandSchemeInfo->vecTransportBackPos.size(); 
                        }

                        ptBackLoc = pMapLandSchemeInfo->vecTransportBackPos[nIndex].ptPos;
                        nAngle = (int)pMapLandSchemeInfo->vecTransportBackPos[nIndex].fAngleY; 
                    }
                    else
                    {
                        ErrorLn(__FUNCTION__ << ", pWarPart == NULL");
                    }
				}
			}
		}
		if (ptBackLoc.getDistance(Vector3()) <= 0.000001f)
		{
			return false;
		}
		return true;
	}

private:
	SCHEME_DATA             m_data;
};