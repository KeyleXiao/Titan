/*******************************************************************
** 文件名:	EffectRandAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭政林
** 日  期:	1/30/2016
** 版  本:	1.0
** 描  述:	效果-随机增加buff
            艾丽娅天赋技能
            http://172.16.0.120/redmine/issues/1615
            修改:随机buff增加可作用buff人数
            http://172.16.0.120/redmine/issues/1671
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;

class CEffectRandAddBuff : public IEffectEx, public TimerHandler,public IEventExecuteSink
{
public:
	typedef  EffectServer_RandAddBuff    SCHEME_DATA;

	CEffectRandAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_vBuffID.clear();
        StringHelper::split(tempVector, m_data.strBuffID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vBuffID.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        // BuffID容器数量
        int nBuffIDCount = (int)m_vBuffID.size();

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
            ErrorLn("CEffectRandAddBuff error,nBuffIDCount="<<nBuffIDCount<<",nBuffLevelCount="<<nBuffLevelCount);
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
        m_pEntity = context->pEntity;

        // BuffID容器数量
        int nBuffIDCount = (int)m_vBuffID.size();
        // BuffLevel容器数量
        int nBuffLevelCount = (int)m_vBuffLevel.size();
        if (nBuffIDCount == 0 || nBuffIDCount != nBuffLevelCount)
        {
            return false;
        }

        //立即执行一次
        OnTimer(0);

        g_EHelper.SetTimer(0, 1000, this, INFINITY_CALL, "CEffectRandAddBuff");

        // 注册攻击事件
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK, "CEffectRandAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity)
        {
            g_EHelper.KillTimer(0, this);

            // 注销攻击事件
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK);

            ADD_BUFF_DATA_MAP::iterator Iter = m_addBuffDataMap.begin();
            while (Iter != m_addBuffDataMap.end())
            {
                // Buff信息
                AddBuffData &data = Iter->second;
                // 移除buff
                RemoveBuff(data.uid, data.nBuffID, data.uidAdd, data.uidAdd);
                // 在表中移除
                Iter = m_addBuffDataMap.erase(Iter);
            }
            m_addBuffDataMap.clear();

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectRandAddBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
        if (m_pEntity == NULL)
        {
            return;
        }
        // 自己UID
        UID uidSelf = m_pEntity->getUID();
        // 取得实体位置
        Vector3 vSelfLocation = m_pEntity->getLocation();

        ADD_BUFF_DATA_MAP::iterator Iter = m_addBuffDataMap.begin();
        while (Iter != m_addBuffDataMap.end())
        {
            // Buff信息
            AddBuffData &data = Iter->second;

            // 是否移除BUFF
            bool bRemove = false;
            // 取得实体位置
            Vector3 vTargetLocation = getLocation(data.uid);
            // 不在范围内,自己死亡或者目标死亡
            if (!vSelfLocation.checkDistance(vTargetLocation, m_data.fDistance) ||
                isDieState(uidSelf) || isDieState(data.uid))
            {
                bRemove = true;
            }

            // 需要移除
            if (bRemove)
            {
                // 移除buff
                RemoveBuff(data.uid, data.nBuffID, data.uidAdd, data.uidAdd);
                // 在表中移除
                Iter = m_addBuffDataMap.erase(Iter);
            }
            else
            {
                ++Iter;
            }
        }

        // 超过数量，不再处理
        if ((int)m_addBuffDataMap.size() >= m_data.nBuffCount)
        {
            return;
        }
        // 死亡状态，不再加范围BUFF
        if (m_pEntity->getState() == ENTITY_STATE_DIE)
        {
            return;
        }
        //取得当前时间
        DWORD dwTick = getTickCountEx();
        // 没到时间
        if (m_dwLastTime > 0 && m_dwLastTime + m_data.nInterval > dwTick)
        {
            return;
        }


        /** 取得实体
		*/
		int nSceneID = UID_2_SCENE(uidSelf);
		SceneServiceHelper sceneHelper(nSceneID);
		if ( sceneHelper.m_ptr==0 )
		{
			return;
		}
        
		AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
		int num = sceneHelper.m_ptr->k_nearest(vSelfLocation, m_data.fDistance, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true);
        if ( num <= 0 )
        {
            return;
        }
        Vector3 vLoc = m_pEntity->getLocation();

        UID uidTargetArray[MAX_INTEREST_OBJECT] = {0};
        int nTargetArrayCount = 0;
        multimap<float, UID> mapUID;
        for ( int i=0; i<num; ++i )
        {
            // 目标UID
			UID uidTarget = pProxyArray[i]->m_uid;
            if (isInvalidUID(uidTarget))
            {
                continue;
            }

            // 检测目标
            if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
            {
                continue;
            }
            
            // 设置目标
            uidTargetArray[nTargetArrayCount++] = uidTarget;
            // 选最近的加
            if (m_data.nRandAddBuffType == RAND_ADD_BUFF_NEAR)
            {
                float fLength = (pProxyArray[i]->m_Loc- vLoc).getLength();
                if (fLength <= m_data.fNearDistance)
                {
                    mapUID.insert(multimap<float, UID>::value_type(fLength, uidTarget));
                }
            }
            else if (m_data.nRandAddBuffType == RAND_ADD_BUFF_HP_LEAST)
            {
                mapUID.insert(multimap<float, UID>::value_type((float)pProxyArray[i]->m_nCurHp, uidTarget));
            }
        }
        if (nTargetArrayCount <= 0)
        {
            return;
        }

        switch (m_data.nRandAddBuffType)
        {
        case RAND_ADD_BUFF_NONAL:
            {
                for (int i=0; i<nTargetArrayCount; ++i)
                {
                    // 超过数量，不再处理
                    if ((int)m_addBuffDataMap.size() >= m_data.nBuffCount)
                    {
                        break;
                    }

                    UID uidTarget = uidTargetArray[i];
                    if (isInvalidUID(uidTarget))
                    {
                        continue;
                    }

                    // 表中查找
                    ADD_BUFF_DATA_MAP::iterator IterFind = m_addBuffDataMap.find(uidTarget);
                    if (IterFind != m_addBuffDataMap.end())
                    {
                        continue;
                    }

                    // 随机Buff索引
                    int nBuffIDCount = (int)m_vBuffID.size();
                    int nNum = 0;
                    if (nBuffIDCount > 1)
                    {
                        rktRangeRand(nNum, 0, nBuffIDCount-1);
                    }
                    int nBuffID = m_vBuffID[nNum];
                    int nBuffLevel = m_vBuffLevel[nNum];

                    if (nBuffID <= 0 || nBuffLevel <= 0)
                    {
                        continue;
                    }

                    // 给实体添加buff
                    AddBuff(uidTarget, nBuffID, nBuffLevel, uidSelf);

                    AddBuffData data;
                    data.uid = uidTarget;
                    data.uidAdd = uidSelf;
                    data.nBuffID = nBuffID;
                    data.nBuffLevel = nBuffLevel;
                    m_addBuffDataMap.insert(ADD_BUFF_DATA_MAP::value_type(uidTarget, data));
                }
            }
            break;
        case RAND_ADD_BUFF_NEAR:
        case RAND_ADD_BUFF_HP_LEAST:
            {
                if (mapUID.empty())
                {
                    break;
                }

                for (multimap<float, UID>::iterator Iter = mapUID.begin(); Iter != mapUID.end(); ++Iter)
                {
                    // 超过数量，不再处理
                    if ((int)m_addBuffDataMap.size() >= m_data.nBuffCount)
                    {
                        break;
                    }

                    UID uidTarget = Iter->second;
                    if (isInvalidUID(uidTarget))
                    {
                        continue;
                    }

                    // 表中查找
                    ADD_BUFF_DATA_MAP::iterator IterFind = m_addBuffDataMap.find(uidTarget);
                    if (IterFind != m_addBuffDataMap.end())
                    {
                        continue;
                    }

                    // 随机Buff索引
                    int nBuffIDCount = (int)m_vBuffID.size();
                    int nNum = 0;
                    if (nBuffIDCount > 1)
                    {
                        rktRangeRand(nNum, 0, nBuffIDCount-1);
                    }
                    int nBuffID = m_vBuffID[nNum];
                    int nBuffLevel = m_vBuffLevel[nNum];

                    if (nBuffID <= 0 || nBuffLevel <= 0)
                    {
                        continue;
                    }

                    // 给实体添加buff
                    AddBuff(uidTarget, nBuffID, nBuffLevel, uidSelf);

                    AddBuffData data;
                    data.uid = uidTarget;
                    data.uidAdd = uidSelf;
                    data.nBuffID = nBuffID;
                    data.nBuffLevel = nBuffLevel;
                    m_addBuffDataMap.insert(ADD_BUFF_DATA_MAP::value_type(uidTarget, data));
                }
            }
            break;
        default:
            break;
        }
    }

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
        switch (wEventID)
        {
        case EVENT_ENTITY_ATTACK:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_attack))
                {
                    break;
                }
                event_entity_attack *pAttack = (event_entity_attack *)pszContext;

                // 表中查找
                ADD_BUFF_DATA_MAP::iterator Iter = m_addBuffDataMap.find(pAttack->uidTarget);
                if (Iter == m_addBuffDataMap.end())
                {
                    break;
                }

                UID uidTarget = INVALID_UID;
                switch (m_data.nBuffType)
                {
                case MA_TAGET_TO_SELF:
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidTarget = pAttack->uidOperator;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidTarget = pAttack->uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectRandAddBuff error nBuffType="<<m_data.nBuffType);
                        break;
                    }
                    break;
                }
                if (isInvalidUID(uidTarget))
                {
                    break;
                }

                // 增加buff者
                UID uidAdd = INVALID_UID;
                switch (m_data.nAddBuffType)
                {
                case MA_TAGET_TO_SELF:
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidAdd = pAttack->uidOperator;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidAdd = pAttack->uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectRandAddBuff error nAddBuffType="<<m_data.nAddBuffType);
                        break;
                    }
                    break;
                }
                if (isInvalidUID(uidAdd))
                {
                    break;
                }

                // 检测BUFF
                if (!CheckBuff(pAttack->uidTarget))
                {
                    break;
                }

                // Buff信息
                AddBuffData &data = Iter->second;
                // 移除buff
                RemoveBuff(data.uid, data.nBuffID, data.uidAdd, data.uidAdd);
                // 设置下次时间
                m_dwLastTime = getTickCountEx();
                // 在表中移除
                m_addBuffDataMap.erase(Iter);

                SBuffContext BuffContext;
                BuffContext.nID = pAttack->nID;
                // 给实体添加buff
                AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidAdd, 0, &BuffContext, sizeof(BuffContext));
            }
            break;
        default:
            break;
        }
    }

    // acos值域跨度为0-π
    double Angle(Vector3 v1, Vector3 v2)
    {
        double n = v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
        double m = sqrt(v1.x*v1.x+v1.y*v1.y+v1.z*v1.z)*sqrt(v2.x*v2.x+v2.y*v2.y+v2.z*v2.z);

        if(m == 0)
        {
            return 0;
        }

        return acos(n/m)*(180/fPI);
    }

    /* 取得向量到水平面夹角
	    vDir	    向量
	    返回点到平面距离
    */
    double Angle(Vector3 vDir)
    {
        // 右向量
        Vector3 vRight(1,0,0);
        // 算出与右向量夹角
        double dAngle = Angle(vRight,vDir);

        // 因为上面算出的值是0-π,所以这里与前向量判断下
        // 前向量
        Vector3 vFront(0,0,1);
        float n = vDir*vFront;
        if (n < 0)
        {
            dAngle = 360-dAngle;
        }
        return dAngle;
    }

    bool CheckBuff(UID uidTarget)
    {
        if (isInvalidUID(uidTarget))
        {
            return false;
        }

        if (m_pEntity == NULL)
        {
            return false;
        }

        // 玩家方向
        Vector3 vDir = getLocation(uidTarget) - m_pEntity->getLocation();

        // 取得与平面夹角
        double fAngle = Angle(vDir);
        ANGLE_TYPE nAngleType = ANGLE_RIGHT;
        if (fAngle > 45 && fAngle <= 135)
        {
            nAngleType = ANGLE_FRONT;
        }
        else if (fAngle > 135 && fAngle <= 225)
        {
            nAngleType = ANGLE_LEFT;
        }
        else if (fAngle > 225 && fAngle <= 315)
        {
            nAngleType = ANGLE_BACK;
        }
        else
        {
            nAngleType = ANGLE_RIGHT;
        }

        // BuffID容器数量
        int nBuffIDCount = (int)m_vBuffID.size();
        // 角度类型超过上限
        if (nAngleType >= nBuffIDCount)
        {
            return false;
        }
        int nBuffID = m_vBuffID[nAngleType];
        int nBuffLevel = m_vBuffLevel[nAngleType];
        if (nBuffID <= 0 || nBuffLevel <= 0)
        {
            return false;
        }

        return IsExistBuff(uidTarget, nBuffID, nBuffLevel, INVALID_UID);
    }

private:
	SCHEME_DATA             m_data;

    // 效果信息
    __IEntity               *m_pEntity;

    // BuffID容器
    vector<int>				m_vBuffID;
    // BuffLevel容器
    vector<int>				m_vBuffLevel;

    struct AddBuffData
    {
        UID		uid;
        UID		uidAdd;
        int		nBuffID;
        int		nBuffLevel;

        AddBuffData()
        {
            ::memset(this, 0, sizeof(*this));
        }
    };
    // 增加buff数据表
    typedef map<UID, AddBuffData>		ADD_BUFF_DATA_MAP;
    ADD_BUFF_DATA_MAP		m_addBuffDataMap;

    // 开始增加buff时间
    DWORD                   m_dwLastTime;
};
