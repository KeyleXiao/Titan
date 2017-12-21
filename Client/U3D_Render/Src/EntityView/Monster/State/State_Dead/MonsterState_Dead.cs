/*******************************************************************
** 文件名:	MonsterState_Dead.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	怪物的dead状态
** 应  用:  	怪物的dead状态中的main部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
public sealed partial class MonsterState_Dead : ICreatureState
{

    // 状态机指针
    MonsterStateMachine m_pFSM;

    public MonsterState_Dead(MonsterStateMachine pFsm)
    {
        m_pFSM = pFsm;
    }

    // 取得姿态ID
    public int GetID() { return (int)EntityState.Dead; }


    //是否在本状态，因为状态切换会延时一帧
    public bool isOnThisState = false;

    /// <summary>
    /// 最长死亡动画时间，因为Animator在Render不可见之后是不会更新的，所以需要有一个最长的时间
    /// </summary>
    private float MaxDeadTime = 5.0f;

    /// <summary>
    /// 最长死亡动画时间，计时器
    /// </summary>
    private float MaxDeadTimeCounter = 0.0f;
    public void ClearState()
    {
    }

    // 进入状态
    public bool OnEnter(IntPtr lParam)
    {
        //if (m_pFSM.animator)
        //{
        //    m_pFSM.animator.SetTrigger("BreakAttack");
        //    m_pFSM.animator.SetBool("isDead", true);
        //   m_pFSM.animator.Play("Dead");

        //}

        cmd_creature_die cmdDie = IntPtrHelper.toData<cmd_creature_die>(lParam);
        ulong uMurderID = cmdDie.uMurderID;

        m_pFSM.SetAnimatorTrigger("BreakAttack");
        m_pFSM.SetAnimatorBool("isDead", true);
        m_pFSM.AnimatorPlay("Dead");
        m_pFSM.outLineCount = m_pFSM.outLineCount - 1;
        m_pFSM.setHightLight(false);

        //关闭死亡时要求关闭的光效
        LightingEffectManager light = m_pFSM.GetComponent<LightingEffectManager>();
        if (light)
        {
            light.CloseLightingWhenDead();
        }

        ////暂时让角色死亡不让射线检测该层
        //nOldLayer = m_pFSM.gameObject.layer;        
        //GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerNeutrality), m_pFSM.gameObject, true);

        //if (m_pFSM.SkinConfig)
        //{
        //    if (m_pFSM.SkinConfig.ScreenCastObject)
        //    {
        //        m_pFSM.SkinConfig.ScreenCastObject.layer = LayerMask.NameToLayer(Config.LayerPlayerFriend);
        //    }

        //}
        //else
        //{
        //    Debug.Log(m_pFSM.entityID.ToString());
        //}

        if (m_pFSM.SkinConfig != null && m_pFSM.SkinConfig.ColliderShape != null)
        {
            m_pFSM.SkinConfig.ColliderShape.enabled = false;
        }
        m_pFSM.pickableCount--;

        //GfxBloodManager.OnEntityDead(m_pFSM.entityID,ENTITY_TYPE.TYPE_MONSTER);
        USpeedUI.Blood.UBloodManager.Instance.OnEntityDead(m_pFSM.entityID, ENTITY_TYPE.TYPE_MONSTER);
        //TopNameManager.OnEntityDead(m_pFSM.entityID, ENTITY_TYPE.TYPE_MONSTER);
        isOnThisState = true;
        MaxDeadTimeCounter = 0.0f;

        int nEntityID = m_pFSM.entityID;
        U3D_Render.EntityView ev = EntityFactory.getMonsterViewByID(nEntityID);
        addMonsterEffect(ev, uMurderID);

        //死亡通知引导，死亡时间不确定，目前只想到在这硬编码了
        if (ev.CampFlag == SPELL.CampFlag.CampFlag_Enemy && ev.Type == ENTITY_TYPE.TYPE_MONSTER && ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX) == (int)MONSTER_TYPE.MONSTER_TYPE_TOWER)
        {
            GUIDE.GuideManager.Instance.SetBBValue(GUIDE.EGuideBBKey.DeadEntityID, ev.ID);
        }

        if (ev != null)
        {
            MapFinderManager.RemoveObject(ref ev);
        }

        return true;
    }


    // 反复进入状态，例如跳跃时再次起跳
    public bool ReEnter(IntPtr lParam)
    {
        return false;
    }

    public void addMonsterEffect(U3D_Render.EntityView dieEv, ulong uMurderID)
    {
        if (dieEv == null || dieEv.CampFlag != SPELL.CampFlag.CampFlag_Enemy || !CheckMonsterType(dieEv))
            return;

        U3D_Render.EntityView mainEv = EntityFactory.getMainPlayer();

        if (mainEv == null || mainEv.StateMachine.GetState() == GameLogic.EntityState.Dead)
            return;

        Vector3 monsterLoc = dieEv.gameObject.transform.position;
        Vector3 heroLoc = mainEv.gameObject.transform.position;

        int effectCount = 0;
        bool needPlayMoneyEffect = false;
        //根据死亡对象类型控制光效数量
        int vocationID = dieEv.createinfo.nHeroID;
        string dieEffectMsg = "";
        IntPtr strPtr = GameLogicAPI.toCoinLighting(vocationID, (int)ENTITY_TYPE.TYPE_MONSTER);
        if (strPtr != null)
        {
            dieEffectMsg = IntPtrHelper.Ptr2Str(strPtr);
        }
        if (dieEffectMsg.Contains(GameLogicConfigApi.MoneyEffect.ToString()))
        {
            needPlayMoneyEffect = true;
        }

        if ((ulong)mainEv.ID == uMurderID && GameLogicAPI.isInWarScene() != 0)
        {
            //如果是怪物类型
            if (dieEv.Type == ENTITY_TYPE.TYPE_MONSTER)
            {
                //中立野怪
                //if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_WILD_MONSTER == dieEv.Flag)
                effectCount = 3;    //小兵和野怪都是3个
            }
            else if (dieEv.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
            {
                effectCount = 6;    //英雄6个
            }

            if (effectCount > 0 && needPlayMoneyEffect)
            {
                LightingEffectBehavior moneyEff = EffectHelper.PlayAndReturnEffect(ref mainEv, GameLogicConfigApi.MoneyEffect, mainEv.ID, mainEv.ID, monsterLoc, 0, 2);
                if (moneyEff != null)
                {
                    LightingEffectBehavior_Area effect = (LightingEffectBehavior_Area)moneyEff;
                    effect.SetEffectCount(effectCount);
                }

            }
            LightingEffectBehavior deadEff = EffectHelper.PlayAndReturnEffect(ref dieEv, GameLogicConfigApi.MonsterDeadEffect, dieEv.ID, dieEv.ID, monsterLoc, 0, 2);
            return;
        }

            int nMonsterDistance = GameLogicConfigApi.MonsterDistance;
        int nTowerDistance = GameLogicConfigApi.TowerDistance;

        if ((monsterLoc - heroLoc).sqrMagnitude > (nMonsterDistance * nMonsterDistance))
            return;

        IList<U3D_Render.EntityView> monster_Entities = MapFinderManager.Nearest(monsterLoc, /*nTowerDistance*/nMonsterDistance, SPELL.CampFlag.CampFlag_Self);
        if (monster_Entities == null)
            return;

        int nMainID = 0;
        float fMinDis = 0.0f;
        foreach (U3D_Render.EntityView monsterEv in monster_Entities)
        {
            if (HeroDistanceMgr.CheckDistance(dieEv, monsterEv, nMonsterDistance) == false || monsterEv == null)
                continue;

            if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER == monsterEv.Flag)
            {

                //return;
            }
            else if (ENTITY_TYPE.TYPE_PLAYER_ROLE == monsterEv.Type)
            {
                float fEntityDistance = (monsterLoc - monsterEv.gameObject.transform.position).sqrMagnitude;
                if (fMinDis == 0 || fEntityDistance < fMinDis)
                {
                    fMinDis = fEntityDistance;
                    nMainID = monsterEv.ID;
                }
            }
        }

        U3D_Render.EntityView view = EntityFactory.getEntityViewByID((int)uMurderID);

        if (nMainID == mainEv.ID && (ulong)nMainID != uMurderID && (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER != view.Flag)
        {
            if ((view.InitMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_DIE_NO_COMMONLIGHT) != (int)ENTITY_MASKFLAG.MASKFLAG_DIE_NO_COMMONLIGHT)
            {
                EffectHelper.Play(ref dieEv, GameLogicConfigApi.MonsterDeadEffect, dieEv.ID, mainEv.ID, monsterLoc);
            }
        }
    }

    // 是否是野怪或者小兵
    private bool CheckMonsterType(U3D_Render.EntityView ev)
    {
        if (ev.Type == ENTITY_TYPE.TYPE_MONSTER && ev.Property != null)
        {
            MONSTER_TYPE type = (MONSTER_TYPE)ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);

            return (
                ev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SOLDIER
             || ev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_WILD_MONSTER
             || ev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER
             || ev.Flag == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_CRYSTAL_TOWER);
        }

        return false;
    }

    // 离开状态
    public void OnLeave()
    {
        isOnThisState = false;

        //if (m_pFSM.animator)
        //{
        //    m_pFSM.animator.SetBool("isDead", false);
        //}

        m_pFSM.SetAnimatorBool("isDead", false);
        m_pFSM.AnimatorPlay("Idle");
        //if (m_pFSM.SkinConfig.ScreenCastObject)
        //{
        //    m_pFSM.SkinConfig.ScreenCastObject.layer = nOldLayer;
        //}
        //GameUtil.SetLayer(nOldLayer, m_pFSM.gameObject, true);
        MaxDeadTimeCounter = 0.0f;
        if (m_pFSM.SkinConfig.ColliderShape)
        {
            m_pFSM.SkinConfig.ColliderShape.enabled = true;
        }
        m_pFSM.pickableCount++;
        //GfxBloodManager.OnEntityRelive(m_pFSM.entityID, ENTITY_TYPE.TYPE_MONSTER);
        USpeedUI.Blood.UBloodManager.Instance.OnEntityRelive(m_pFSM.entityID, ENTITY_TYPE.TYPE_MONSTER);
        //TopNameManager.OnEntityRelive(m_pFSM.entityID, ENTITY_TYPE.TYPE_MONSTER);
    }

    // 更新状态逻辑
    public void Update()
    {
        if (!isOnThisState)
        {
            return;
        }


        MaxDeadTimeCounter += Time.deltaTime;
        if (MaxDeadTimeCounter > MaxDeadTime)
        {
            MaxDeadTimeCounter = 0;
            ReleaseEntity();
            return;
        }

        if (m_pFSM.animator)
        {
            AnimatorStateInfo state = m_pFSM.animator.GetCurrentAnimatorStateInfo(0);
            AnimatorClipInfo[] clipinfo = m_pFSM.animator.GetCurrentAnimatorClipInfo(0);
            //可见
            if (m_pFSM.GetVisible())
            {
                //死亡动画播放完,删除
                if (state.IsName("Dead"))
                {
                    if(state.normalizedTime > 1.1f)
                    {
                        ReleaseEntity();
                    }
                    else
                    {
                        if(clipinfo.Length <=0 || !clipinfo[0].clip)
                        {
                            ReleaseEntity();
                        }
                    }
                }
            }
            else
            {
                ReleaseEntity();
            }
        }
        else
        {
            ReleaseEntity();
        }

    }

    // 通知去移除实体
    private void ReleaseEntity()
    {
        if (m_pFSM)
        {
            U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(m_pFSM.entityID);
            if (objev != null)
            {
                //Trace.Log("Release Monster,ID:" + objev.ID);
                EntityFactory.ReleaseEntity(objev);
                m_pFSM = null;
            }
        }
    }


    //返回是否在地面
    public bool IsGrounded()
    {
        return true;
    }

    public bool isFloating()
    {
        return false;
    }

    private Vector3 m_Velocity;

    // 获得基本速度
    public Vector3 getVelocity()
    {
        return Vector3.zero;
    }
    public void setVelocity(Vector3 vel)
    {
        m_Velocity = vel;
    }

    private Vector3 m_syncVelocity = Vector3.zero;

    // 获得同步速度
    public Vector3 getSyncVelocity()
    {
        return m_syncVelocity;
    }
    public void setSyncVelocity(Vector3 vel)
    {
        m_syncVelocity = vel;
    }
}
