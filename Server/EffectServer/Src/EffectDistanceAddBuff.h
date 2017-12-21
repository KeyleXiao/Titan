/*******************************************************************
** 文件名:	EffectDistanceAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭政林
** 日  期:	11/16/2016
** 版  本:	1.0
** 描  述:	效果-距离添加不同buff
            http://172.16.0.120/redmine/issues/3033
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;

class CEffectDistanceAddBuff : public IEffectEx
{
public:
	typedef  EffectServer_DistanceAddBuff    SCHEME_DATA;

	CEffectDistanceAddBuff( SCHEME_DATA & data ) : m_data(data)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_vDistance.clear();
        StringHelper::split(tempVector, m_data.strDistance, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vDistance.push_back(StringHelper::toFloat(tempVector[nCount]));
        }
        // 距离容器数量
        int nDistanceCount = (int)m_vDistance.size();

        tempVector.clear();
        m_vBuffID.clear();
        StringHelper::split(tempVector, m_data.strBuffID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vBuffID.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        // BuffID容器数量
        int nBuffIDCount = (int)m_vBuffID.size();
        if (nDistanceCount != nBuffIDCount)
        {
            ErrorLn("CEffectDistanceAddBuff error,nDistanceCount="<<nDistanceCount<<",nBuffIDCount="<<nBuffIDCount);
        }

        tempVector.clear();
        m_vBuffLevel.clear();
        StringHelper::split(tempVector, m_data.strBuffLevel, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vBuffLevel.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        // BuffLevel容器数量
        int nBuffLevelCount = (int)m_vBuffLevel.size();
        if (nBuffIDCount != nBuffLevelCount)
        {
            ErrorLn("CEffectDistanceAddBuff error,nBuffIDCount="<<nBuffIDCount<<",nBuffLevelCount="<<nBuffLevelCount);
        }
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

        // 距离容器数量
        int nDistanceCount = (int)m_vDistance.size();
        // BuffID容器数量
        int nBuffIDCount = (int)m_vBuffID.size();
        // BuffLevel容器数量
        int nBuffLevelCount = (int)m_vBuffLevel.size();
        if (nDistanceCount == 0 || nDistanceCount != nBuffIDCount || nBuffIDCount != nBuffLevelCount)
        {
            return false;
        }

        Vector3 vStartLoc(0,0,0);
        switch (m_data.nStartType)
        {
        case DISTANCE_ADD_BUFF_SPELL_START:
            {
                vStartLoc = context->vSelfLoc;
            }
            break;
        case DISTANCE_ADD_BUFF_SPELL_TILE:
            {
                vStartLoc = context->ptTargetTile;
            }
            break;
        case DISTANCE_ADD_BUFF_SELF:
            {
                vStartLoc = context->pEntity->getLocation();
            }
            break;
        case DISTANCE_ADD_BUFF_TARGET:
            {
                UID uidTarget = context->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }

                vStartLoc = getLocation(uidTarget);
            }
            break;
        default:
            {
                ErrorLn("CEffectDistanceAddBuff error nStartType="<<m_data.nStartType);
                return false;
            }
            break;
        }

        Vector3 vEndLoc(0,0,0);
        switch (m_data.nEndType)
        {
        case DISTANCE_ADD_BUFF_SPELL_START:
            {
                vEndLoc = context->vSelfLoc;
            }
            break;
        case DISTANCE_ADD_BUFF_SPELL_TILE:
            {
                vEndLoc = context->ptTargetTile;
            }
            break;
        case DISTANCE_ADD_BUFF_SELF:
            {
                vEndLoc = context->pEntity->getLocation();
            }
            break;
        case DISTANCE_ADD_BUFF_TARGET:
            {
                UID uidTarget = context->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    return false;
                }

                vEndLoc = getLocation(uidTarget);
            }
            break;
        default:
            {
                ErrorLn("CEffectDistanceAddBuff error nEndType="<<m_data.nEndType);
                return false;
            }
            break;
        }

        // 自己UID
        UID uidSelf = context->pEntity->getUID();
        UID uidTarget = INVALID_UID;
        switch (m_data.nAddBuffType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidTarget = uidSelf;
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidTarget = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidTarget = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectDistanceAddBuff error nAddBuffType="<<m_data.nAddBuffType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidTarget))
        {
            return false;
        }

        SBuffContext BuffContext;
        BuffContext.nID = context->nID;

        float fDistance = (vEndLoc - vStartLoc).getLength();

        int nCount = (int)m_vDistance.size();
        for (int i=0; i<nCount; ++i)
        {
            if (fDistance <= m_vDistance[i])
            {
                int nBuffID = m_vBuffID[i];
                int nBuffLevel = m_vBuffLevel[i];
                if (nBuffID > 0 && nBuffLevel > 0)
                {
                    // 给目标增加对应buff
                    AddBuff(uidTarget, m_vBuffID[i], m_vBuffLevel[i], uidSelf, 0, &BuffContext, sizeof(BuffContext));
                }
                break;
            }
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectDistanceAddBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
    // BuffID容器
    vector<float>			m_vDistance;
    // BuffID容器
    vector<int>				m_vBuffID;
    // BuffLevel容器
    vector<int>				m_vBuffLevel;
};
