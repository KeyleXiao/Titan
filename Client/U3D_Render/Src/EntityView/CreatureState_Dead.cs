using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using GameLogic;
using USpeedUI;
using UnityEngine.EventSystems;
using U3D_Render.CameraControl;

public class CreatureState_Dead : ICreatureState
{
    // 状态机指针
    CreatureStateMachine m_pFSM;

    private uint tick_cameraSwitchAfterDead = 0;



    public CreatureState_Dead(CreatureStateMachine pFsm)
    {
        
        m_pFSM = pFsm;
    }

    // 取得姿态ID
    public int GetID() { return (int)EntityState.Dead; }

    private uint startTick=0;
    private uint endTick=0;
    private int m_nDeadInterval = 0; //进入状态时已经死亡的时间，用于死亡动作的播放时间偏移

    public void ClearState()
    {
        if(m_pFSM && m_pFSM.getEntityState() == EntityState.Dead)
        {
            OnLeave();
        }
    }

    ///// <summary>
    ///// 死亡前的碰撞层
    ///// </summary>
    //private int nOldCollisionLayer = -1;
    // 进入状态
    public bool OnEnter(IntPtr lParam)
    {
        //退出假死状态
        if (m_pFSM != null && m_pFSM.animatorCtrl != null)
        {
            m_pFSM.animatorCtrl.Dying = false;
        }

        Debug.Log("on OnEnter CreaturePosture_Dead "  + this.m_pFSM.name);
        if (m_pFSM.showLog)
        {
            Trace.Log(m_pFSM.name + "=======OnEnter CreaturePosture_Dead======");
        }

        cmd_creature_die context = IntPtrHelper.toData<cmd_creature_die>(lParam);
        if (m_pFSM.showLog)
        {
            Trace.Log("DeadInterval:" + context.nDeadInterval);
            Trace.Log("reborn second:" + context.nInterval);
        }



        m_pFSM.rebornTick = context.nInterval - context.nDeadInterval;
        if (m_pFSM.rebornTick <= 0)
        {
            Trace.Log("rebornTick <= 0!");
            //return false;                   吃鸡地图，这个时差是0，所以先不要返回
        }
        m_nDeadInterval = context.nDeadInterval;

        //暂时让角色死亡不让射线检测该层
        //if (!m_pFSM.isHero)
        //{
        //    nOldLayer = m_pFSM.gameObject.layer;
        //    if (m_pFSM.gameObject.layer != LayerMask.NameToLayer(Config.LayerTransparentFX))
        //    {                
        //        GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerNeutrality), m_pFSM.gameObject, true);
        //    }

        //    //m_pFSM.gameObject.layer = LayerMask.NameToLayer(Config.LayerPlayerFriend);
        //    //if (m_pFSM.SkinConfig.ScreenCastObject != null)
        //    //{
        //    //    m_pFSM.SkinConfig.ScreenCastObject.layer = LayerMask.NameToLayer(Config.LayerPlayerFriend); 
        //    //}
        //}

        SkillCtrl sk = m_pFSM.GetComponent<SkillCtrl>();
        if (sk)
        {
            sk.enabled = false;            
        }

        SoldierController sco = m_pFSM.GetComponent<SoldierController>();
        if (sco)
        {
            sco.dead = true;
        }

        if (m_pFSM.animator)
        {
            m_pFSM.animator.SetTrigger("Dead");

            m_pFSM.animator.ResetTrigger("Reborn");
        }

        //关闭死亡时要求关闭的光效
        LightingEffectManager light = m_pFSM.GetComponent<LightingEffectManager>();
        if (light)
        {
            light.CloseLightingWhenDead();
        }

        startTick = GameLogicAPI.getSyncTickCount(); // GameLogicAPI.getTickCount();
        endTick = startTick + (uint)m_pFSM.rebornTick;
        tick_cameraSwitchAfterDead = startTick + m_pFSM.cameraSwitchAfterDead;

        if(m_pFSM.isHero)
        {
            //GFxGameHud hud = SFGFxMovieManager.GetWindow<GFxGameHud>();
            //if (SFGFxMovieManager.CheckTheMovieCanUse(hud))
            //{
            //    hud.ShowReliveTime(((float)m_pFSM.rebornTick));
            //}
            ScreenREC.GetInstance().SwitchRECState(ScreenREC.EMRECState.EMREC_STOP);

            //ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.FullScreenGray>();
            if(ImageSetting.GetData().currImageQuality == ImageQuality.Fastest)
            {
                LightingEffectFactory.EnableDeadEffect();
                ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.FullScreenSaturation>();
                ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.DepthOfFieldScatter>();
            }
            else
            {
                LightingEffectFactory.DisableDeadEffect();
                ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.FullScreenSaturation>();
                ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.DepthOfFieldScatter>();
            }
        }

        //死亡不碰撞
        if (m_pFSM.controller)
        {
            //nOldCollisionLayer = m_pFSM.gameObject.layer;
            //m_pFSM.gameObject.layer = LayerMask.NameToLayer(Config.LayerIngoreRayCast);
            m_pFSM.pickableCount = 0;
        }

        int nEntityID = m_pFSM.entityID;
        U3D_Render.EntityView ev = EntityFactory.getPlayerViewByID(nEntityID);

        //添加金币掉落光效
        if(GameLogicAPI.isInWarScene() > 0)
            addMoneyEffect(ev, context.uMurderID);

        USpeedUI.Blood.UBloodManager.Instance.OnEntityDead(m_pFSM.entityID, ENTITY_TYPE.TYPE_PLAYER_ROLE);
        // 小地图隐藏
//         U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(m_pFSM.entityID);
//         if (objev != null && GFxMinimapManager.Instance != null)
//         {
//             GFxMinimapManager.Instance.SetObjectVisible(m_pFSM.entityID, objev.createinfo.nMinimapIcon, 0);
//         }
        return true;
    }

    // 反复进入状态，例如跳跃时再次起跳
    public bool ReEnter(IntPtr lParam)
    {
        return false;
    }

    // 离开状态
    public void OnLeave()
    {
        if (!m_pFSM)
            return;

        Trace.Log(m_pFSM.name + "=======OnLeave CreaturePosture_Dead======");

        //死亡动作在攻击层调用，这里结束掉死亡动作
        m_pFSM.BreakAttackAnim();

        SkillCtrl sk = m_pFSM.GetComponent<SkillCtrl>();
        if (sk)
        {
            sk.enabled = true;
        }

        if (m_pFSM.isHero) //复活时切回主角镜头
        {
            SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.DeadControl, false);
        }

        SoldierController sco = m_pFSM.GetComponent<SoldierController>();
        if (sco)
        {
            sco.dead = false;
            sco.desireAngle = m_pFSM.transform.localEulerAngles;
            //sco.bindSoldierCamera.Reset(); 2016-09-22，策划需求，复活后摄像机位置保持原来跟角色的位置，不重置，改为仅取消effect
            CameraEffectUtil.MainInstance.stopEffect();
        }
        if (m_pFSM.isHero)
        {
            //ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.FullScreenGray>();
            LightingEffectFactory.DisableDeadEffect();
            ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.FullScreenSaturation>();
            ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.DepthOfFieldScatter>();
        }


        //恢复死亡不碰撞
        if (m_pFSM.controller)
        {
            m_pFSM.pickableCount++;
            //if (true==m_pFSM.GetVisible())
            //{
            //    m_pFSM.gameObject.layer = m_pFSM.baseLayer;
            //}
        }

        ////实体复活不是重新发下来的，在这里改回原来的层
        //if (!m_pFSM.isHero)
        //{
        //    GameUtil.SetLayer(nOldLayer, m_pFSM.gameObject, true);
        //    //m_pFSM.gameObject.layer = nOldLayer;
        //    //if (m_pFSM.SkinConfig.ScreenCastObject != null)
        //    //{
        //    //    m_pFSM.SkinConfig.ScreenCastObject.layer = nOldLayer;
        //    //}
        //}

        if (m_pFSM.animator)
        {
            Trace.Log("SetTrigger(Reborn)");
            m_pFSM.animator.SetTrigger("Reborn");
            m_pFSM.animator.ResetTrigger("Dead");
            m_pFSM.animator.speed = 1;
        }

        //GfxBloodManager.OnEntityRelive(m_pFSM.entityID, ENTITY_TYPE.TYPE_PLAYER_ROLE);
        USpeedUI.Blood.UBloodManager.Instance.OnEntityRelive(m_pFSM.entityID, ENTITY_TYPE.TYPE_PLAYER_ROLE);
        //TopNameManager.OnEntityRelive(m_pFSM.entityID, ENTITY_TYPE.TYPE_PLAYER_ROLE);

        //某些特效，在模型死亡时，会将模型隐藏起来，复活的时候设置为可见        
        m_pFSM.SetVisible(m_pFSM.m_OnViewVisible);

        //         // 小地图显示
        //         U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(m_pFSM.entityID);
        //         if (objev != null)
        //         {
        //             GFxMinimapManager.Instance.SetObjectVisible(m_pFSM.entityID, objev.createinfo.nMinimapIcon, 1);
        //         }
        U3D_Render.EntityView objev = EntityFactory.getPlayerViewByID(m_pFSM.entityID);
        if (objev != null && objev.gameObject != null && objev.Property != null)
        {
            int nHeroID = objev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
            SoundPlayHandler.PlayHeroRandomSound(nHeroID, m_pFSM.entityID, objev.gameObject, ASpeedGame.Data.Scheme.EMHeroSoundType.EHeroSound_Relive);
        }
    }

    // 更新状态逻辑
    public void Update()
    {

        //死亡动作偏移
        if (m_nDeadInterval > 0)
        {
            if (m_pFSM.SkinControl && m_pFSM.SkinControl.CurrentSkinInstance != null)
            {
                AnimatorStateInfo stateInfo = m_pFSM.animatorCtrl.ana.GetCurrentAnimatorStateInfo(m_pFSM.moveAnimatorLayer);
                if (stateInfo.IsTag("Dead"))
                {
                    m_pFSM.animatorCtrl.ana.CrossFade(stateInfo.fullPathHash, 0, m_pFSM.moveAnimatorLayer, (float)m_nDeadInterval / 1000.0f / stateInfo.length);

                    //攻击层可能也有播放，顺便偏移下
                    AnimatorStateInfo stateInfo2 = m_pFSM.animatorCtrl.anb.GetCurrentAnimatorStateInfo(m_pFSM.attackAnimatorLayer);
                    if (stateInfo2.IsTag("Dead"))
                    {
                        m_pFSM.animatorCtrl.anb.CrossFade(stateInfo2.fullPathHash, 0, m_pFSM.attackAnimatorLayer, (float)m_nDeadInterval / 1000.0f / stateInfo2.length);
                    }

                    m_nDeadInterval = 0;
                }
            }

        }



        if (!m_pFSM.moveCtrl.isCharge() && !IsGrounded())
        {
            Vector3 downOffset = Vector3.zero;
            downOffset.y=-m_pFSM.creaturePropety.downGravity * Time.deltaTime;
            m_pFSM.controller.Move(downOffset);
        }

        uint tick = GameLogicAPI.getSyncTickCount();// GameLogicAPI.getTickCount();
        //if (tick > endTick)
        //{
        //    int eventID = (int)EntityLogicDef.ENTITY_TOVIEW_RELIVE;
        //    GameLogicAPI.onEntityEvent(m_pFSM.entityID, eventID, 0, "", IntPtr.Zero, 0);
        //}

        if (tick_cameraSwitchAfterDead==0 && m_pFSM.isHero && InputManager.GetKeyUp(KeyCode.Mouse0) && !EventSystem.current.IsPointerOverGameObject()) //左键切换下一个队友        
        {
            if (!UISystem.Instance.IsMouseOverUI())
            {
                SoldierCamera.MainInstance<SoldierCamera>().SwitchLookAtSolider(EntityFactory.getFriendPlayer(true));
            }
        }
        else if (tick_cameraSwitchAfterDead>0 && tick>tick_cameraSwitchAfterDead)
        {
            tick_cameraSwitchAfterDead = 0;
            if (m_pFSM.isHero)
            {
                SoldierCamera.MainInstance<SoldierCamera>().EnableMode(CameraMode.DeadControl, true);
                SoldierCamera.MainInstance<SoldierCamera>().SwitchLookAtSolider(EntityFactory.getFriendPlayer(false));
            }
            else if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.StateMachine!=null
                && EntityFactory.MainHeroView.StateMachine.GetState() == EntityState.Dead)
            {
                SoldierCamera.MainInstance<SoldierCamera>().SwitchLookAtSolider(EntityFactory.skipDeadFriendPlayer((uint)m_pFSM.entityID));
            }

        }
    }

    //public void OnAnimatorIK(int layerIndex)
    //{
    //}

    //返回是否在地面
    public bool IsGrounded()
    {
        return (m_pFSM.groundNormal.y > 0.01f);
    }

    public bool isFloating()
    {
        return false;
    }

    public void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {

    }

    public void Send_Sync_Position(MoveSyncFlag reportToSever)
    {
    }

    private Vector3 m_Velocity;

    // 获得基本速度
    public Vector3 getVelocity()
    {
        return m_Velocity;
    }
    public void setVelocity(Vector3 vel)
    {
        m_Velocity = vel;
    }

    /// <summary>
    /// 传送，清楚掉移动消息
    /// </summary>
    public void OnTransport()
    {

    }

    public void OnForceMove()
    {
    }

    public void OnForceFlight()
    {
    }

    public void ChangeState(IntPtr ptrParam)
    {
    }

    public void ApplyAnimation()
    {
        if (!m_pFSM.animator)
        {
            return;
        }
        m_pFSM.animator.speed = 1000; 
        m_pFSM.animator.SetTrigger("Dead");

        m_pFSM.animator.ResetTrigger("Reborn");
    }

    public void addMoneyEffect(U3D_Render.EntityView dieEv, ulong uMurderID)
    {
        if (dieEv == null || dieEv.CampFlag != SPELL.CampFlag.CampFlag_Enemy)
            return;

        U3D_Render.EntityView mainEv = EntityFactory.getMainPlayer();

        if (mainEv == null || mainEv.StateMachine.GetState() == GameLogic.EntityState.Dead)
            return;

        Vector3 deadLoc = dieEv.gameObject.transform.position;
        Vector3 heroLoc = mainEv.gameObject.transform.position;

        if ((ulong)mainEv.ID == uMurderID)
        {
            int effectCount = 0;
            //根据死亡对象类型控制光效数量
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


            LightingEffectBehavior eff = EffectHelper.PlayAndReturnEffect(ref mainEv, GameLogicConfigApi.MoneyEffect, mainEv.ID, mainEv.ID, deadLoc, 0, 2);
            if (eff != null)
            {
                LightingEffectBehavior_Area effect = (LightingEffectBehavior_Area)eff;
                effect.SetEffectCount(effectCount);
                return;
            }
        }

        //别人杀死就没金币掉落
        
        //int nMonsterDistance = GameLogicConfigApi.MonsterDistance;
        //int nTowerDistance = GameLogicConfigApi.TowerDistance;

        //if ((deadLoc - heroLoc).sqrMagnitude > (nMonsterDistance * nMonsterDistance))
        //    return;

        //IList<U3D_Render.EntityView> monster_Entities = MapFinderManager.Nearest(deadLoc, /*nTowerDistance*/nMonsterDistance, SPELL.CampFlag.CampFlag_Self);
        //if (monster_Entities == null)
        //    return;

        //int nMainID = 0;
        //float fMinDis = 0.0f;
        //foreach (U3D_Render.EntityView monsterEv in monster_Entities)
        //{
        //    if (HeroDistanceMgr.CheckDistance(dieEv, monsterEv, nMonsterDistance) == false || monsterEv == null)
        //        continue;

        //    if ((int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER == monsterEv.Flag)
        //    {

        //        //return;
        //    }
        //    else if (ENTITY_TYPE.TYPE_PLAYER_ROLE == monsterEv.Type)
        //    {
        //        float fEntityDistance = (deadLoc - monsterEv.gameObject.transform.position).sqrMagnitude;
        //        if (fMinDis == 0 || fEntityDistance < fMinDis)
        //        {
        //            fMinDis = fEntityDistance;
        //            nMainID = monsterEv.ID;
        //        }
        //    }
        //}

        //U3D_Render.EntityView view = EntityFactory.getEntityViewByID((int)uMurderID);

        //if (nMainID == mainEv.ID && (ulong)nMainID != uMurderID && (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER != view.Flag)
        //{
        //    if ((view.InitMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_DIE_NO_COMMONLIGHT) != (int)ENTITY_MASKFLAG.MASKFLAG_DIE_NO_COMMONLIGHT)
        //        EffectHelper.Play(ref mainEv, GameLogicConfigApi.MonsterEffect, mainEv.ID, mainEv.ID, monsterLoc);
        //}
    }
}