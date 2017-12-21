/*******************************************************************
** 文件名:	EffectReplicationMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/24/2016
** 版  本:	1.0
** 描  述:	效果-召唤分身怪物
********************************************************************/

#pragma once

#include "IEffect.h"
#include "IEntity.h"
#include "INPCService.h"
#include "EffectDef.h"
#include "EntityHelper.h"
#include "IWarService.h"
#include "WarHelper.h"

using namespace EFFECT_SERVER;

class CEffectReplicationMonster : public IEffectEx, public IEventExecuteSink, public rkt::TimerHandler
{
public:
	typedef  EffectServer_ReplicationMonster    SCHEME_DATA;

	CEffectReplicationMonster( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_bDieEvent(false),m_nSpellID(0)
	{
        m_MonsterVector.clear();

        vector<string> tempVector;
        tempVector.clear();

        m_DirVector.clear();
        StringHelper::split(tempVector, m_data.strDir, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_DirVector.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        // 取得技能ID数量
        int nDirCount = (int)m_DirVector.size();

        tempVector.clear();
        m_DistanceVector.clear();
        StringHelper::split(tempVector, m_data.strDistance, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_DistanceVector.push_back(StringHelper::toFloat(tempVector[nCount]));
        }
        // 距离数量
        int nDistanceCount = (int)m_DistanceVector.size();
        if (nDirCount != m_data.nMonsterCount || nDirCount != nDistanceCount)
        {
            ErrorLn("CEffectReplicationMonster number does not match,ID="<< GetID() <<",nDirCount="<<nDirCount<<",nDistanceCount="<<nDistanceCount<<",nMonsterCount="<<m_data.nMonsterCount);
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}

        m_pEntity = context->pEntity;
		UID uid = m_pEntity->getUID();
		if (isInvalidUID(uid))
		{
			return false;
		}

        MonsterHelper helper(uid);
		INPCService * pNpcService = helper.m_ptr;		
		if (pNpcService == NULL)
		{
			return false;
		}

        m_nSpellID = m_data.nSpellID;
        if (m_nSpellID <= 0)
        {
            m_nSpellID = context->nID;
        }

        bool bAdd = false;
        // 结束要移除,或者有时限分身,需要记录,删除的时候用
        if (m_data.nRemove || m_data.nTime > 0)
        {
            bAdd = true;
        }

        if (bAdd && !m_bDieEvent)
        {
            DWORD dwSceneID = m_pEntity->getSceneID();
            SceneLogicEventHelper eventHelper(dwSceneID);
            IEventEngine *pGlobalEvent = eventHelper.m_ptr;
            if (pGlobalEvent == NULL)
            {
                ErrorLn(_GT("CEffectReplicationMonster Start 取得全局事件失败，问题严重！"));
                return false;
            }
            // 注册怪物死亡事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0, "CEffectReplicationMonster");
            
            m_bDieEvent = true;
        }

        // 取得朝向角度
        Vector3 angle = m_pEntity->getAngle();
        float fAngleY = (float)(angle.y*fPI/180);
        Vector3 vMonsterDir;
        vMonsterDir.x = sin(fAngleY);
        vMonsterDir.y = 0;
        vMonsterDir.z = cos(fAngleY);
        switch (m_data.nMonsterDirType)
        {
        case DIRECTION_FRONT:
            {
                // 前方不出理
            }
            break;
        case DIRECTION_BACK:
            {
                vMonsterDir *= -1;
            }
            break;
        case DIRECTION_LEFT:
            {
                Vector3 vUp(0,1,0);
                vMonsterDir = vMonsterDir.getCrossProduct(vUp);
            }
            break;
        case DIRECTION_RIGHT:
            {
                Vector3 vUp(0,1,0);
                vMonsterDir = vUp.getCrossProduct(vMonsterDir);
            }
            break;
        default:
            {
                ErrorLn("CEffectReplicationMonster error nMonsterDirType="<<m_data.nMonsterDirType);
                return false;
            }
            break;
        }

		CreateNpcContext NpcContext;
		NpcContext.nCamp    = m_pEntity->getIntProperty(PROPERTY_CAMP);
		NpcContext.Angle    = vMonsterDir.toEulerAngles();
        NpcContext.nWarID   = getWarID(uid);
        NpcContext.uidCreator=uid;
        NpcContext.nAIID    = -1;
        NpcContext.nLevel   = m_pEntity->getIntProperty(PROPERTY_LEVEL);
        // 分身怪物 
        NpcContext.nFlag    = MONSTER_CREATE_CLONE; 

		// 设置PK信息
		__IPlayerKillPart* pPKPart = (__IPlayerKillPart*)m_pEntity->getEntityPart(PART_PK);
		if (pPKPart)
		{
			NpcContext.nPKType = pPKPart->getPKType();
			NpcContext.nPKState = pPKPart->getPKState();
		}

        // 设置AI
        int nWarType = 0;
        do 
        {
            if(NpcContext.nWarID < 0)
            {
                break;
            }
            WarHelper helper(NpcContext.nWarID);
            IWarService* pWarService = helper.m_ptr;
            if(pWarService == NULL)
            {
                break;
            }

            nWarType = pWarService->getWarTypeId();
        } while (false);

        ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (pSchemeCenter != NULL)
        {
            ISchemeAIIDTable* pAITable = pSchemeCenter->getAISchemeIDTable();
            if(pAITable != NULL)
            {
                NpcContext.nAIID = pAITable->getNpcAISchemeID(nWarType, m_data.nMonsterID);
            }
        }

		// 数据长度
		int nLen = sizeof(NpcContext);

        Vector3 vLoc(0,0,0);
        // 取得自己位置
        Vector3 myLoc = m_pEntity->getLocation();

        switch (m_data.nPosType)
        {
        case REPLICATION_MONSTER_POS_SELF:
            {
                vLoc = myLoc;
            }
            break;
        case REPLICATION_MONSTER_POS_TARGET:
            {
                if (isInvalidUID(context->uidTarget))
                {
                    return false;
                }
                vLoc = getLocation(context->uidTarget);
            }
            break;
        case REPLICATION_MONSTER_POS_OPERATOR:
            {
                if (isInvalidUID(context->uidOperator))
                {
                    return false;
                }
                vLoc = getLocation(context->uidOperator);
            }
            break;
        case REPLICATION_MONSTER_POS_TILE:
            {
                vLoc = context->ptTargetTile;
            }
            break;
        default:
            {
                ErrorLn("CEffectReplicationMonster error nPosType="<<m_data.nPosType);
                return false;
            }
            break;
        }

        // 方向向量单位化
        Vector3 vDir = (vLoc - myLoc).normalize();
        // 如果是0向量,取面向
        if (equalto0(vDir.getLength()))
        {
            // 取得朝向角度
            Vector3 angle = m_pEntity->getAngle();

            float fAngleY = (float)(angle.y*fPI/180);

            vDir.x = sin(fAngleY);
            vDir.y = 0;
            vDir.z = cos(fAngleY);
        }

        string szName;
        // 取召唤者名字
        if (m_data.bSummonName)
        {
            szName.append(m_pEntity->getName());
        }
        if (strcmp(m_data.strAppendName, "0") != 0)
            szName.append(m_data.strAppendName);
		
		for (int i=0; i<m_data.nMonsterCount; ++i)
		{
            // 设置怪物坐标
            setMonsterLoc(vLoc, vDir, i);

            // 创建怪物
			UID uid = pNpcService->createNPC(m_data.nMonsterID, vLoc, NpcContext);

            // 增加技能召唤怪物
            changeSpellSummon(uid, true);

            // 设置召唤名字
            setProperty_String(uid, PROPERTY_NAME, szName.c_str());

			// 等级
			if (m_data.fLvFactor > 0.01f)
			{
                int nValue = (int)(m_pEntity->getIntProperty(PROPERTY_LEVEL)*m_data.fLvFactor);
                setProperty_Integer(uid, PROPERTY_LEVEL, nValue);
			}

            // 血量
            if (m_data.fHPFactor > 0.01f)
            {
                int nMax = (int)(m_pEntity->getIntProperty(PROPERTY_MAX_HP)*m_data.fHPFactor);
                setProperty_Integer(uid, PROPERTY_MAX_HP, nMax);

                int nValue = (int)(m_pEntity->getIntProperty(PROPERTY_HP)*m_data.fHPFactor);
                setProperty_Integer(uid, PROPERTY_HP, nValue);
            }

            // 魔法
            if (m_data.fMPFactor > 0.01f)
            {
                int nMax = (int)(m_pEntity->getIntProperty(PROPERTY_MAX_MP)*m_data.fMPFactor);
                setProperty_Integer(uid, PROPERTY_MAX_MP, nMax);

                int nValue = (int)(m_pEntity->getIntProperty(PROPERTY_MP)*m_data.fMPFactor);
                setProperty_Integer(uid, PROPERTY_MP, nValue);
            }

            // 能量
            if (m_data.fENERGYFactor > 0.01f)
            {
                int nMax = (int)(m_pEntity->getIntProperty(PROPERTY_MAX_ENERGY)*m_data.fENERGYFactor);
                setProperty_Integer(uid, PROPERTY_MAX_ENERGY, nMax);

                int nValue = (int)(m_pEntity->getIntProperty(PROPERTY_ENERGY)*m_data.fENERGYFactor);
                setProperty_Integer(uid, PROPERTY_ENERGY, nValue);
            }

            // 物理攻击
            if (m_data.fPAFactor > 0.01f)
            {
                int nBaseValue = (int)(m_pEntity->getIntProperty(PROPERTY_BASE_PA)*m_data.fPAFactor);
                setProperty_Integer(uid, PROPERTY_BASE_PA, nBaseValue);

                int nAppendValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PA)*m_data.fPAFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PA, nAppendValue);

                int nAppendPCTValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PCT_PA)*m_data.fPAFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PCT_PA, nAppendPCTValue);
            }

            // 法术攻击
            if (m_data.fMAFactor > 0.01f)
            {
                int nBaseValue = (int)(m_pEntity->getIntProperty(PROPERTY_BASE_MA)*m_data.fMAFactor);
                setProperty_Integer(uid, PROPERTY_BASE_MA, nBaseValue);

                int nAppendValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_MA)*m_data.fMAFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_MA, nAppendValue);

                int nAppendPCTValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PCT_MA)*m_data.fMAFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PCT_MA, nAppendPCTValue);
            }

            // 物理防御
            if (m_data.fPDFactor > 0.01f)
            {
                int nBaseValue = (int)(m_pEntity->getIntProperty(PROPERTY_BASE_PD)*m_data.fPDFactor);
                setProperty_Integer(uid, PROPERTY_BASE_PD, nBaseValue);

                int nAppendValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PD)*m_data.fPDFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PD, nAppendValue);

                int nAppendPCTValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PCT_PD)*m_data.fPDFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PCT_PD, nAppendPCTValue);
            }

            // 法术防御
            if (m_data.fMDFactor > 0.01f)
            {
                int nBaseValue = (int)(m_pEntity->getIntProperty(PROPERTY_BASE_MD)*m_data.fMDFactor);
                setProperty_Integer(uid, PROPERTY_BASE_MD, nBaseValue);

                int nAppendValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_MD)*m_data.fMDFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_MD, nAppendValue);

                int nAppendPCTValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PCT_MD)*m_data.fMDFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PCT_MD, nAppendPCTValue);
            }

            // 物理穿透
            if (m_data.fPDPFactor > 0.01f)
            {
                int nPDP = (int)(m_pEntity->getIntProperty(PROPERTY_PDP)*m_data.fPDPFactor);
                setProperty_Integer(uid, PROPERTY_PDP, nPDP);

                int nPCT_PDP = (int)(m_pEntity->getIntProperty(PROPERTY_PCT_PDP)*m_data.fPDPFactor);
                setProperty_Integer(uid, PROPERTY_PCT_PDP, nPCT_PDP);
            }

            // 法术穿透
            if (m_data.fMDPFactor > 0.01f)
            {
                int nMDP = (int)(m_pEntity->getIntProperty(PROPERTY_MDP)*m_data.fMDPFactor);
                setProperty_Integer(uid, PROPERTY_MDP, nMDP);

                int nPCT_MDP = (int)(m_pEntity->getIntProperty(PROPERTY_PCT_MDP)*m_data.fMDPFactor);
                setProperty_Integer(uid, PROPERTY_PCT_MDP, nPCT_MDP);
            }

            // 移动速度
            if (m_data.fSDFactor > 0.01f)
            {
                int nBaseValue = (int)(m_pEntity->getIntProperty(PROPERTY_BASE_SD)*m_data.fSDFactor);
                setProperty_Integer(uid, PROPERTY_BASE_SD, nBaseValue);

                int nAppendValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_SD)*m_data.fSDFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_SD, nAppendValue);

                int nAppendPCTValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PCT_SD)*m_data.fSDFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PCT_SD, nAppendPCTValue);
            }

            // 攻击速度
            if (m_data.fPASDFactor > 0.01f)
            {
                int nBaseValue = (int)(m_pEntity->getIntProperty(PROPERTY_BASE_PASD)*m_data.fPASDFactor);
                setProperty_Integer(uid, PROPERTY_BASE_PASD, nBaseValue);

                int nAppendValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PASD)*m_data.fPASDFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PASD, nAppendValue);

                int nAppendPCTValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PCT_PASD)*m_data.fPASDFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PCT_PASD, nAppendPCTValue);
            }

            // 普攻系数
            if (m_data.fAFFactor > 0.01f)
            {
                int nBaseValue = (int)(m_pEntity->getIntProperty(PROPERTY_BASE_AF)*m_data.fAFFactor);
                setProperty_Integer(uid, PROPERTY_BASE_AF, nBaseValue);

                int nAppendValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_AF)*m_data.fAFFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_AF, nAppendValue);

                int nAppendPCTValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PCT_AF)*m_data.fAFFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PCT_AF, nAppendPCTValue);
            }

            // 暴击概率
            if (m_data.fCRCFactor > 0.01f)
            {
                int nValue = (int)(m_pEntity->getIntProperty(PROPERTY_BASE_CRC)*m_data.fCRCFactor);
                setProperty_Integer(uid, PROPERTY_BASE_CRC, nValue);

                int nAppendValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_CRC)*m_data.fCRCFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_CRC, nAppendValue);

                int nAppendPCTValue = (int)(m_pEntity->getIntProperty(PROPERTY_APPEND_PCT_CRC)*m_data.fCRCFactor);
                setProperty_Integer(uid, PROPERTY_APPEND_PCT_CRC, nAppendPCTValue);
            }

            if (bAdd)
            {
                m_MonsterVector.push_back(uid);
            }
		}

        if (m_data.nTime > 0)
        {
            g_EHelper.SetTimer(0, m_data.nTime, this, 1, "CEffectReplicationMonster");
        }

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != 0)
        {
            g_EHelper.KillTimer(0, this);

            // 注册了死亡事件
            if (m_bDieEvent)
            {
                DWORD dwSceneID = m_pEntity->getSceneID();
                SceneLogicEventHelper eventHelper(dwSceneID);
                IEventEngine *pGlobalEvent = eventHelper.m_ptr;
                if (pGlobalEvent != NULL)
                {
                    // 注销怪物死亡事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0);
                }
                m_bDieEvent = false;
            }

            do 
            {
                // 结束不要移除
                if (!m_data.nRemove)
                {
                    break;
                }
                int nCount = (int)m_MonsterVector.size();
                if (nCount <= 0)
                {
                    break;
                }

                UID uid = m_pEntity->getUID();
                if (isInvalidUID(uid))
                {
                    break;
                }

                MonsterHelper helper(uid);
                INPCService * pNpcService = helper.m_ptr;		
                if (pNpcService == NULL)
                {
                    break;
                }

                for (int i=0; i<nCount; ++i)
                {
                    UID uidMonster = m_MonsterVector[i];
                    // 减少技能召唤怪物
                    changeSpellSummon(uidMonster, false);

                    pNpcService->destroyNPC(uidMonster);
                }
            } while (false);
            
            m_MonsterVector.clear();

            m_pEntity = 0;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectReplicationMonster(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_GAME_ENTITY_DIE:
			{
                if (m_pEntity == NULL)
                {
                    break;
                }

				if (pszContext == NULL || nLen != sizeof(event_entity_die))
				{
					break;
				}
				event_entity_die *pDie = (event_entity_die *)pszContext;

                if (!m_MonsterVector.empty())
                {
                    break;
                }

                vector<UID>::iterator Iter = m_MonsterVector.begin();
                while (Iter != m_MonsterVector.end())
                {
                    if (pDie->uidTarget == *Iter)
                    {
                        UID uidMonster = *Iter;
                        // 减少技能召唤怪物
                        changeSpellSummon(uidMonster, false);

                        Iter = m_MonsterVector.erase(Iter);
                    }
                    else
                    {
                        ++Iter;
                    }
                }
			}
			break;
		default:
			break;
		}
    }

    virtual void OnTimer( unsigned long dwTimerID )
    {
        int nCount = (int)m_MonsterVector.size();
        if (nCount <= 0)
        {
            return;
        }

        UID uid = m_pEntity->getUID();
        if (isInvalidUID(uid))
        {
            return;
        }

        MonsterHelper helper(uid);
        INPCService * pNpcService = helper.m_ptr;		
        if (pNpcService == NULL)
        {
            return;
        }

        for (int i=0; i<nCount; ++i)
        {
            UID uidMonster = m_MonsterVector[i];
            // 减少技能召唤怪物
            changeSpellSummon(uidMonster, false);

            pNpcService->destroyNPC(uidMonster);
        }
        m_MonsterVector.clear();
    }

    void changeSpellSummon(UID uid, bool bAdd)
    {
        if (m_nSpellID <= 0)
        {
            return;
        }

        if (m_data.nMaxCount <= 0)
        {
            return;
        }

        if (m_pEntity == NULL)
        {
            return;
        }

        __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return;
        }

        ISpell *pSpell = pSpellPart->findSpell(m_nSpellID);
        if (pSpell == NULL)
        {
            return;
        }

        if (bAdd)
        {
            pSpell->addEntity(uid, m_data.nMaxCount);
        }
        else
        {
            pSpell->removeEntity(uid);
            
        }
    }

    void setMonsterLoc(Vector3 &vLoc, Vector3 vDir, int nIndex)
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        // 场景ID
        int nSceneID = m_pEntity->getSceneID();
        PathFindHelper helper(nSceneID);
        IPathFindService *pPathService = helper.m_ptr;
        if (pPathService == NULL)
        {
            return;
        }

        if (equalto0(vDir.getLength()))
        {
            return;
        }

        if (nIndex < 0 || nIndex >= (int)m_DirVector.size())
        {
            return;
        }

        if (nIndex < 0 || nIndex >= (int)m_DistanceVector.size())
        {
            return;
        }

        DIRECTION_TYPE nDir = (DIRECTION_TYPE)m_DirVector[nIndex];
        float fDistance = m_DistanceVector[nIndex];
        switch(nDir)
        {
        case DIRECTION_FRONT: // 前方
            {
                vDir = vDir;
                vLoc = pPathService->getRaycast(vLoc, vDir, fDistance);
            }
            break;
        case DIRECTION_BACK: // 后方
            {
                vDir = vDir*(-1.0);
                vLoc = pPathService->getRaycast(vLoc, vDir, fDistance);
            }
            break;
        case DIRECTION_LEFT: // 左方
            {
                Vector3 vUp(0,1,0);
                vDir = vDir.getCrossProduct(vUp);
                vLoc = pPathService->getRaycast(vLoc, vDir, fDistance);
            }
            break;
        case DIRECTION_RIGHT: // 右方
            {
                Vector3 vUp(0,1,0);
                vDir = vUp.getCrossProduct(vDir);
                vLoc = pPathService->getRaycast(vLoc, vDir, fDistance);
            }
            break;
        case DIRECTION_INSITU_RAND:
            {
                int nRandNum = 0;
                rktRangeRand(nRandNum, -(int)(fDistance*100), (int)(fDistance*100));
                vLoc.x += (float)nRandNum/100.0f;
                rktRangeRand(nRandNum, -(int)(fDistance*100), (int)(fDistance*100));
                vLoc.z += (float)nRandNum/100.0f;
            }
            break;
        default:
            return;
            break;
        }
        
        
        // 找到地面的点 创建NPC时有贴地
        // pPathService->getHeight(&vLoc, vLoc.y);
    }

private:
	SCHEME_DATA               m_data;

    __IEntity                 *m_pEntity;

    // 怪物容器
    vector<UID>               m_MonsterVector;

    // 是否注册死亡事件
    bool                      m_bDieEvent;

    // 技能ID
    int                       m_nSpellID;

    // 方向容器
    vector<int>				  m_DirVector;

    // 距离容器
    vector<float>		      m_DistanceVector;
};
