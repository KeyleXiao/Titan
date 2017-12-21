/*******************************************************************
** 文件名:	CEffectDieIncreaseMoneyExp.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	9/29/2015
** 版  本:	1.0
** 描  述:	效果---击杀死亡怪物金币经验效果提升
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"
#include "IAttackObject.h"
#include "buffer.h"
#include "IGameSceneService.h"
#include "EventDef.h"
#include "EntityDef.h"
#include "ExternalHelper.h"
#include "StringHelper.h"

class CEffectDieIncreaseMoneyExp : public IEffectEx,public IEventExecuteSink
{
public:
    typedef  EffectServer_DieIncreaseMonneyExp    SCHEME_DATA;

    CEffectDieIncreaseMoneyExp( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
    {

        vector<std::string> tempVector;
        tempVector.clear();
        m_SpellVector.clear();
        StringHelper::split(tempVector, m_data.strComboID, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_SpellVector.push_back(StringHelper::toInt(tempVector[nCount]));
        }
        tempVector.clear();
        m_MonLocMap.clear();
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

        m_pEntity = context->pEntity;

        UID uid = m_pEntity->getUID();
        SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
        IEventEngine *pEventEngine = eventHelper.m_ptr;
        if (pEventEngine==NULL)
        {
            return false;
        }
        if(!m_SpellVector.size())
        {
            return false;
        }
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK, "CEffectDieIncreaseMoneyExp");
        if(m_data.nIsLotsSpellAttackAddBuffEffect)
        {
            // 如果是多技能效果触发加buff，则不注册怪物死亡事件
            return true;
        }
        pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_KILL_MONSTER, UID_2_TYPE(uid), uid, "CEffectDieIncreaseMoneyExp");
        return true;
    }


    // 效果停止
    virtual void			Stop()
    {
        if (m_pEntity != 0)
        {
            UID uid = m_pEntity->getUID();

            // 获得事件中心接口
            SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
            IEventEngine *pEventEngine = eventHelper.m_ptr;
            if (pEventEngine!=NULL)
            {
                pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_KILL_MONSTER, UID_2_TYPE(uid), uid);
            }
            
            // 注销攻击事件
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK);
            m_MonLocMap.clear();
            m_pEntity = 0;
        }
        return;
    }

    // 克隆一个新效果
    virtual IEffect *       Clone()
    {
        return new CEffectDieIncreaseMoneyExp(m_data);
    }

    // 释放
    virtual void			Release()
    {
        Stop();
        delete this;
    }

    void AttackAddBuff(UID uidTarget,UID uidOperator,int nSpellID)
    {
        SBuffContext BuffContext;
        BuffContext.nID = nSpellID;

        vector<std::string> tempVector;
        //读取buffid和buff等级
        StringHelper::split(tempVector, m_data.nAddTargetBuffID , ';', " ");
        for (UINT nCount=0; nCount<tempVector.size();nCount+=2)
        {
            int targetBuffID=StringHelper::toInt(tempVector[nCount]);
            int targetBuffLevel=0;
            // 防止数组越界，判断下
            if(nCount+1<tempVector.size())
            {
                targetBuffLevel=StringHelper::toInt(tempVector[nCount+1]);
            }
            // 还是判断下是否为0
            if(targetBuffID==0||targetBuffLevel==0)
            {
                continue;
            }
            //目标添加特殊buff，如致死buff
            AddBuff(uidTarget, targetBuffID,targetBuffLevel,uidOperator, 0, &BuffContext, sizeof(BuffContext));
        }
        tempVector.clear();
        //读取buffid和buff等级
        StringHelper::split(tempVector, m_data.nAddSelfBuffID , ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); nCount+=2)
        {

            int selfBuffID=StringHelper::toInt(tempVector[nCount]);
            int selfBuffLevel=0;
            // 防止数组越界，判断下
            if(nCount+1<tempVector.size())
            {
                selfBuffLevel=StringHelper::toInt(tempVector[nCount+1]);
            }
            if(selfBuffID==0||selfBuffLevel==0)
            {
                continue;
            }
            //自己添加特殊buff
            AddBuff(uidOperator,selfBuffID,selfBuffLevel,uidOperator, 0, &BuffContext, sizeof(BuffContext));	
        }
        tempVector.clear();
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
        case EVENT_ENTITY_ATTACK:	// 技能攻击事件
            {
                if (m_pEntity == NULL)
                {
                    return;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_attack))
                {
                    return;
                }

                event_entity_attack *pAttack = (event_entity_attack *)pszContext;


                //匹配技能
                vector<int>::iterator nSpellResult = find(m_SpellVector.begin(),m_SpellVector.end(),pAttack->nID); 
                if (nSpellResult ==m_SpellVector.end( ) )
                { 
                    //没找到
                    return;
                }
                DWORD dwTick = getTickCountEx();
                if (dwTick < m_dwLastTick + m_data.nCDTime)
                {
                    return;
                }
                UID uidTarget=pAttack->uidTarget;
                UID uidOperator=pAttack->uidOperator;
                if(m_data.nIsLotsSpellAttackAddBuffEffect)
                {
                    // 检测目标
                    if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
                    {
                        return;
                    }
                }
                else
                {
                    if (UID_2_TYPE(uidTarget) != TYPE_MONSTER)
                    {
                        return;
                    }
                    int nMonsterMask=getProperty_Integer(uidTarget, PROPERTY_MASK);
                    if((nMonsterMask&m_data.nKillTargetFilter ) != m_data.nKillTargetFilter)
                    {
                        return;
                    }
                    //将被技能击中的怪物id存进容器,和位置
                    m_MonLocMap[uidTarget]=getLocation(uidTarget);
                }

                //记录当前计数
                m_dwLastTick=dwTick;
                // 给自己和被攻击的实体加buff
                AttackAddBuff(uidTarget,uidOperator,pAttack->nID);
            }
            break;

        case EVENT_GAME_ACTOR_KILL_MONSTER:	// 击杀获得天赋点数
            {
                if (m_pEntity == NULL)
                {
                    return;
                }

                if (pszContext == NULL || nLen != sizeof(event_kill_monster))
                {
                    return;
                }
                event_kill_monster *pKillMonster = (event_kill_monster *)pszContext;
                //死亡者id
                UID uidTarget=pKillMonster->uDeadId;

                //查找技能是否为击到怪物的技能
                map<UID,Vector3>::iterator result=m_MonLocMap.find(uidTarget);
                if(result==m_MonLocMap.end())
                {
                    return ;
                }
                //另一次额外的金币和经验加成
                int nCMonney=pKillMonster->nTalentValue*m_data.nMonneyCoefficient/100;
                int nCEXP=pKillMonster->nEXP*m_data.nExpCoefficient/100;

                UID uidSelf = m_pEntity->getUID();
                //给自己和周围的人家钱和经验
                //获得实体
                int nSceneID = UID_2_SCENE(uidSelf);
                SceneServiceHelper sceneHelper(nSceneID);
                if ( sceneHelper.m_ptr==0 )
                {
                    return;
                }
                //获得被击杀的小兵位置
                Vector3 vCheckLoc = m_MonLocMap[uidTarget];
                //删除匹配的怪物信息，死亡的怪物的信息后面用不到
                m_MonLocMap.erase(result);
                // 取得一定范围内的所有实体
                AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
                int num = sceneHelper.m_ptr->k_nearest(vCheckLoc,m_data.fDieLocRange, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true );
                if ( num<=0 )
                {
                    return;
                }
                for ( int i=0;i<num;++i )
                {
                    // 目标UID
					UID uidAddTarget = pProxyArray[i]->m_uid;
                    if (isInvalidUID(uidAddTarget))
                    {
                        continue;
                    }
                    if (UID_2_TYPE(uidAddTarget) == TYPE_MONSTER)
                    {
                        continue;
                    }

                    // 检测buff添加者，过滤添加者
                    //m_data.nAddTargetFilter
                    if (!g_EHelper.chooseTarget(m_pEntity, m_data.nAddTargetFilter, uidAddTarget))
                    {
                        continue;
                    }
                    //添加金币
                    addProperty_Integer(uidAddTarget, PROPERTY_TALENT_POINT, nCMonney);
                    // 添加经验
                    addExp(uidAddTarget,nCEXP, 0);
                }
            }
            break;
        default:
            break;
        }
    }

private:

    SCHEME_DATA              m_data;

    __IEntity				*m_pEntity;

    // 技能ID容器
    vector<int>				m_SpellVector;

    vector<int>             m_SelfBuffLevel;

    vector<int>             m_TargetBuffLevel;
    // 上次计数
    DWORD					m_dwLastTick;
    // 被击中怪物ID容器
    map<UID,Vector3>	    m_MonLocMap;
};
