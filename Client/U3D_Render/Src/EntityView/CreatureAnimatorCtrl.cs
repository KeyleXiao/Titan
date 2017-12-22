using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using GameLogic;

/**
CreatureAnimatorCtrl
CreatureStateMachine的子部件，负责控制动画，
包括控制animator播放动画，为animator设置参数，
在动画播放时根据需求控制动作和位移
 */
public class CreatureAnimatorCtrl
{
    private CreatureStateMachine m_pFSM = null;
    public Animator weaponAnimator = null;
    public Animator weaponAnimator2 = null;
    //上下身分离用
    public Animator ana;    //移动
    public AnimatorOverrideController overControllerA;
    public Animator anb;    //攻击
    public AnimatorOverrideController overControllerB;
    public BipedIKAnimator anc;


    //--------------------------------------------------------------------------------
    //以下为每帧根据状态机stateInfo更新动画或状态机操作，例如添加动画事件更改动画状态
    //主要是为了实现攻击动作上下身动作分离，以及动作和光效配齐
    //--------------------------------------------------------------------------------


    public bool bAnimatorAttack = false;
    private bool m_bAnimatorMoving = false;
    private int m_lastAnbHash = 0;
    public bool firsttime = true;

    //攻击时根据攻击动作边移动时所需要的配置
    //private Vector3 attackAnimatorOrgPos;    
    private Vector3 lastAnimatorBOffset; //animatorB offset of last frame

    private bool m_animatorUpStateAttackMove = false;
    private bool m_animatorUpStateForceAttackMove = false; //无视与目标之间的距离，必须AttackMove
    private bool m_animatorUpSatateBlendAttack = false;
    private bool m_animatorUpStateIdle = false;
    public bool bDoingAttackMove = false;
    public bool bAttackMoveBlending = false;
    public bool bAttackMoveZero = false;
    private Transform goBMainTransform = null; //记录攻击动作所用gameObject的根节点，用于边打边走的位置更新
    private Transform goBRootATransform = null; //记录攻击动作所用gameObject的节点，用于边打边走的位置更新
    private Transform goBRootMTransform = null; //记录攻击动作所用gameObject的节点，用于边打边走的位置更新    
    private Transform goBBackAMTransform = null; //记录攻击动作所用gameObject的节点，用于边打边走的位置更新

    private Vector3 goBBackAMLocalPosition = Vector3.zero; //记录加载是Back_AM的相对位移，以后切父节点时始终保持相对位移

    //param id in animator
    private int id_nMoveForward;
    private int id_nMoveRight;
    private int id_nSkillLeft;
    private int id_nSkillRight;
    private int id_fAttackSpeed;
    private int id_fMoveSpeed;
    private int id_bIsAttackAnim;
    private ScreenRaycast m_screenRaycast = null; //边走边打要求有瞄准对象的时候，在瞄准对象3米内停下，这里用来去瞄准对象

    // 构造
    public CreatureAnimatorCtrl(CreatureStateMachine pFsm)
    {
        m_pFSM = pFsm;

        id_nMoveForward = Animator.StringToHash("nMoveForward");
        id_nMoveRight = Animator.StringToHash("nMoveRight");
        id_nSkillLeft = Animator.StringToHash("skillLeft");
        id_nSkillRight = Animator.StringToHash("skillRight");
        id_fAttackSpeed = Animator.StringToHash("attackSpeed");
        id_fMoveSpeed = Animator.StringToHash("moveSpeed");
        id_bIsAttackAnim = Animator.StringToHash("isAttackAnim");
        anbSpeedDict = new List<speedPair>();
    }




    #region 播放动作
    private string m_crossfadeName = "";
    private float m_crossfadeDuratioTime = 0.0f;
    private bool m_crossfadeReact = false;
    private float m_crossfadeSpeed = 1.0f;
    private float m_crossNormalizedTime = 0.0f;
    private string m_weaponCrossfadeName = "";
    private float m_weaponCrossfadeSpeed = 1.0f;
    private string m_weaponCrossfadeName2 = "";
    private float m_weaponCrossfadeSpeed2 = 1.0f;
    private Transform m_weaponTarget = null;
    private WeaponAnimaionCtrl m_weaponAnimatorCtrl = null;
    private WeaponAnimaionCtrl m_weaponAnimatorCtrl2 = null;


    private float m_oldMoveSpeed = 1.0f;
    private float m_oldAttackSpeed = 1.0f;
    private bool m_usingUniSpeed = false;
    //是否假死状态
    public bool Dying { get { return m_dying; } set { m_dying = value; } }
    private bool m_dying = false;
    public void setAnimatorSpeed(bool revert, float speed)
    {
        if (!ana || !anb)
        {
            return;
        }

        if (revert)
        {
            ana.SetFloat(id_fMoveSpeed, m_oldMoveSpeed);
            anb.SetFloat(id_fAttackSpeed, m_oldAttackSpeed);
            m_usingUniSpeed = false;
            m_oldMoveSpeed = 1.0f;
            m_oldAttackSpeed = 1.0f;
        }
        else
        {
            m_usingUniSpeed = true;
            m_oldMoveSpeed = ana.GetFloat(id_fMoveSpeed);
            m_oldAttackSpeed = anb.GetFloat(id_fAttackSpeed);
            ana.SetFloat(id_fMoveSpeed, speed);
            anb.SetFloat(id_fAttackSpeed, speed);
        }
    }

    public void PlayAnim(string name, bool react = false, float speed = 1.0f, float durationTime = 0.0f, float normalizedTime = 0.0f)
    {
        //if (m_crossfadeName == "IDLE") //之前的打断动作未播，后边的动作就不播了？不记得为什么要这么干所以先屏蔽掉
        //{
        //    return;
        //}
        if (anb == null || anb.speed < 0.01f)
        {
            return;
        }

        if (react && bAnimatorAttack) //受击动作不打断攻击动作
        {
            return;
        }

        //处理一下假死情况
        if (m_dying)
        {
            if (name.ToLower() != "qishen")
            {
                Debug.Log("Dying!! AnimatorCtrl Filter Play:" + name);
                return;
            }
            m_dying = false;
        }

        m_crossfadeName = name;
        m_crossfadeSpeed = speed;
        m_crossfadeDuratioTime = durationTime;
        m_crossNormalizedTime = normalizedTime;
    }

    private float m_attackMoveDistanceScale = 1.0f;
    private void updatePlayAnim(string name, bool react = false, float speed = 1.0f, float durationTime = 0.0f, float normalizedTime = 0.0f)
    {
        durationTime = 0.0f; //暂时关掉动作融合

        if (anb != null && anb.runtimeAnimatorController != null)
        {
            if (ContainState(anb, name) == false)
            {
                Trace.LogWarning(GameLogicAPI.getTickCount().ToString() + "PlayAnim:" + name + " fail,AnimationController:" + anb.name + " doesn't contain state:" + name);
                return;
            }

            if (name.Contains("_NoRe")) //名字里包含这个字符串的，不可以打断正在播放的同名动作
            {
                if (anb.GetCurrentAnimatorStateInfo(m_pFSM.attackAnimatorLayer).IsName(name))
                {
                    return;
                }
            }

            if (name.ToLower() == "jiasi")
            {
                m_dying = true;
            }

            anb.CrossFade(name, durationTime, -1, normalizedTime);

            if (!m_usingUniSpeed)
            {
                if (m_pFSM.attackAnimatorLayer > 0)
                {
                    anb.CrossFade(name, durationTime, m_pFSM.attackAnimatorLayer + 1, normalizedTime);
                    anb.SetFloat(id_fAttackSpeed, speed);
                }
                else
                {
                    anb.speed = speed;
                }

                if (m_pFSM.creaturePropety.maxAttackMoveSpeedScale > 0.0f && speed > m_pFSM.creaturePropety.maxAttackMoveSpeedScale)
                {
                    m_attackMoveDistanceScale = m_pFSM.creaturePropety.maxAttackMoveSpeedScale / speed;
                }
                else
                {
                    m_attackMoveDistanceScale = 1.0f;
                }
            }
        }
    }

    public void PlayWeaponAnim(string name, int targetID, float speed = 1.0f)
    {
        if (name == "IDLE")
        {
            m_weaponCrossfadeName = name;
            m_weaponCrossfadeSpeed = speed;
            m_weaponCrossfadeName2 = name;
            m_weaponCrossfadeSpeed2 = speed;
        }
        //跟策划规定， W_开头的是武器动作
        //跟策划规定， W_2_开头的是2号武器动作
        else if ((name.Length > 4) && (name[2] == '2') && (name[3] == '_'))
        {
            m_weaponCrossfadeName2 = name;
            m_weaponCrossfadeSpeed2 = speed;
        }
        else if (weaponAnimator != null)
        {
            m_weaponCrossfadeName = name;
            m_weaponCrossfadeSpeed = speed;
        }

        m_weaponTarget = null;
        if (targetID > 0)
        {
            U3D_Render.EntityView targetView = EntityFactory.getEntityViewByID(targetID);
            if (targetView != null && targetView.StateMachine)
            {
                m_weaponTarget = targetView.StateMachine.FindTransformEx("Wound");
                if (m_weaponTarget == null && targetView.gameObject != null)
                {
                    m_weaponTarget = targetView.gameObject.transform;
                }
            }
        }
    }

    private void updatePlayWeaponAnim(string name, float speed = 1.0f)
    {
        if (anb == null)
        {
            return;
        }

        if (weaponAnimator != null && weaponAnimator.runtimeAnimatorController != null)
        {
            if (m_weaponAnimatorCtrl == null)
            {
                m_weaponAnimatorCtrl = weaponAnimator.transform.GetComponent<WeaponAnimaionCtrl>();
            }

            if (m_weaponAnimatorCtrl != null)
            {
                m_weaponAnimatorCtrl.targetTransform = m_weaponTarget;
                m_weaponAnimatorCtrl.startAnim();
            }

            weaponAnimator.speed = speed;
            weaponAnimator.CrossFade(name, 0.1f, 0, 0.0f);
        }

    }

    private void updatePlayWeaponAnim2(string name, float speed = 1.0f)
    {
        if (!anb)
        {
            return;
        }

        if (!weaponAnimator2 && !weaponAnimator2.runtimeAnimatorController)
        {
            if (!m_weaponAnimatorCtrl2)
            {
                m_weaponAnimatorCtrl2 = weaponAnimator.transform.GetComponent<WeaponAnimaionCtrl>();
            }

            if (m_weaponAnimatorCtrl2)
            {
                m_weaponAnimatorCtrl2.targetTransform = m_weaponTarget;
                m_weaponAnimatorCtrl2.startAnim();
            }

            weaponAnimator2.speed = speed;
            weaponAnimator2.CrossFade(name, 0.1f, 0, 0.0f);
        }
    }

    public void BreakAttackAnim(string animName = "", float crossFadeTime = 0.1f)
    {
        if (m_dying)
        {
            Debug.Log("Dying!! AnimatorCtrl Filter Break:" + animName);
            return;
        }
        //if (animName == "" || m_attackAnimatorStateInfo.IsName(animName))
        {
            m_crossfadeName = "IDLE";
            m_crossfadeDuratioTime = crossFadeTime;
            m_weaponCrossfadeName = "IDLE";
            m_weaponCrossfadeSpeed = 1.0f;
            m_crossNormalizedTime = 0.0f;
            m_weaponTarget = null;
        }
    }

    //服务器传下来的移动速度改变时，调用这个函数，更新下移动动画的速率，造成快跑或者慢跑的效果
    public void updateMovingAnimSpeed(float speed)
    {
        if (ana)
        {
            if (m_pFSM.attackAnimatorLayer > 0)
            {
                ana.SetFloat(id_fMoveSpeed, speed);
            }
            else
            {
                ana.speed = speed;
            }

        }
        //Debug.Log("ana speed=" + ana.speed);
    }

    public void UpdateAttackAnimSpeed(float speed)
    {
        if (ana)
        {
            if (m_pFSM.attackAnimatorLayer > 0)
            {
                ana.SetFloat(id_fAttackSpeed, speed);
            }
            else
            {
                ana.speed = speed;
            }

        }
    }

    class speedPair
    {
        public int effectID;
        public float speed;
    }
    private List<speedPair> anbSpeedDict;
    public void changeAnbSpeed(int effectID, float speed, bool enable)
    {
        if (anb == null)
        {
            return;
        }
        if (enable)
        {
            foreach (speedPair sp in anbSpeedDict)
            {
                if (sp.effectID == effectID)
                {
                    anb.speed = speed;
                    return;
                }
            }

            speedPair newpair = new speedPair();
            newpair.effectID = effectID;
            newpair.speed = speed;
            anbSpeedDict.Add(newpair);
        }
        else
        {
            foreach (speedPair sp in anbSpeedDict)
            {
                if (sp.effectID == effectID)
                {
                    anbSpeedDict.Remove(sp);
                    break;
                }
            }
            if (anbSpeedDict.Count < 1)
            {
                anb.speed = 1.0f;
            }
            else
            {
                anb.speed = anbSpeedDict[anbSpeedDict.Count - 1].speed;
            }
        }

    }



    //Dictionary<Animator, Hashtable> statemap = new Dictionary<Animator, Hashtable>();

    public bool ContainState(Animator animator, string name)
    {
        for (int i = 0; i < animator.layerCount; i++)
        {
            if (animator.HasState(i, Animator.StringToHash(name)))
            {
                return true;
            }
        }
        return false;

        #region 废弃代码
        //if (!Application.isEditor)
        //{
        //    return true;
        //}

        //Hashtable statedic=null;
        //if (statemap.ContainsKey(animator)==false)
        //{
        //    statedic = new Hashtable();
        //    if (BuildStateMapFun != null)
        //    {
        //        bool ret = BuildStateMapFun(animator, ref statedic);
        //    }
        //    statemap[animator] = statedic;
        //}

        //statedic = statemap[animator];
        //return statedic.ContainsKey(name);
        #endregion
    }
    #endregion


    #region 转向
    private bool m_bDelayFaceTo = false;
    private Vector3 m_delayFaceToTargetPos = Vector3.zero;
    public bool FaceTo(Vector3 targetPos)
    {
        if (bDoingAttackMove) //在b_doingAttackMove情况下转向滞后
        {
            m_bDelayFaceTo = true;
            m_delayFaceToTargetPos = targetPos;
            return false;
        }

        m_pFSM.transform.LookAt(new Vector3(targetPos.x, m_pFSM.transform.position.y, targetPos.z));

        if (m_pFSM.soliderController != null)
        {
            m_pFSM.soliderController.desireAngle = m_pFSM.transform.eulerAngles;
        }

        // 主角才需要往服务器同步当前的朝向。 
        // PS:非主角龙母在骑龙过程中 扫射会一直同步龙母的朝向，一直调用这个函数 
        //     而下面的syncPos函数会把服务器同步的位置以及瞬时速度冲掉，造成看龙母位置很不同步。
        //if (m_pFSM.isHero)
        //{
        //    m_pFSM.syncPos();
        //}
        return true;

    }

    private bool delayFaceTo()
    {
        if (m_bDelayFaceTo)
        {
            m_bDelayFaceTo = false;
            return FaceTo(m_delayFaceToTargetPos);
        }
        return false;
    }

    #endregion

    #region 动作替换

    private Effect.AnimationReplacePair[] m_clipPairsA;
    private bool m_recoverPairsA;
    private Effect.AnimationReplacePair[] m_clipPairsB;
    private bool m_recoverPairsB;

    //  record the next clip in m_rePlayAnimA/B and return the current frame and play it in the next frame    
    private int m_rePlayAnimA = 0;
    private int m_rePlayAnimB = 0;

    public void replaceAnimationClipA(Effect.AnimationReplacePair[] clipPairs, bool recoverPairs)
    {
        m_clipPairsA = clipPairs;
        m_recoverPairsA = recoverPairs;
    }
    public void replaceAnimationClipB(Effect.AnimationReplacePair[] clipPairs, bool recoverPairs)
    {
        m_clipPairsB = clipPairs;
        m_recoverPairsB = recoverPairs;
    }

    private void replaceAnimA()
    {
        if (m_clipPairsA == null)
        {
            return;
        }
        if (!overControllerA)
        {
            return;
        }
        for (int i = 0; i < m_clipPairsA.Length; i++)
        {
            if (!m_clipPairsA[i].orgClip)
            {
                continue;
            }
            if (m_recoverPairsA)
            {
                overControllerA[m_clipPairsA[i].orgClip.name] = m_clipPairsA[i].orgClip;
            }
            else
            {
                overControllerA[m_clipPairsA[i].orgClip.name] = m_clipPairsA[i].replaceClip;
            }
        }
        m_clipPairsA = null;
    }

    private void replaceAnimB()
    {
        if (m_clipPairsB == null)
        {
            return;
        }
        if (!overControllerB)
        {
            return;
        }
        for (int i = 0; i < m_clipPairsB.Length; i++)
        {
            if (!m_clipPairsB[i].orgClip)
            {
                continue;
            }
            if (m_recoverPairsB)
            {
                overControllerB[m_clipPairsB[i].orgClip.name] = m_clipPairsB[i].orgClip;
            }
            else
            {
                overControllerB[m_clipPairsB[i].orgClip.name] = m_clipPairsB[i].replaceClip;
            }
        }
        m_clipPairsB = null;
    }

    public void ResetBoneAfterReplace()
    {
        ResetBones();
    }

    //--记录Animator运行状态--
    private int m_lastAnAClipHash = 0;          //替换时state
    private int m_lastAnANextClipHash = 0;      //替换时处于过渡,目标state
    private float m_lastAnAClipTime = 0;        //归一化时间

    private int m_lastAnBClipHash = 0;
    private int m_lastAnBNextClipHash = 0;
    private float m_lastAnBClipTime = 0;
    //-------------------------

    /// <summary>
    /// AnimatorOverrideController替换动画片段 实现控制器重用
    /// </summary>
    /// <returns>是否有动画替换</returns>
    private bool updateAnimaiontReplace()
    {
        //in the frame of replacing the animation clip in override controller, the animator will restart from the default state
        //so, we record the current state info, and crossFace to this info in the next frame

        if (m_clipPairsA == null && m_clipPairsB == null)
        {
            return false;
        }
        //bool a_isStateDefault = ana.GetCurrentAnimatorStateInfo(m_pFSM.moveAnimatorLayer).IsTag("MoveTree");
        m_lastAnAClipHash = ana.GetCurrentAnimatorStateInfo(m_pFSM.moveAnimatorLayer).fullPathHash;
        m_lastAnAClipTime = (int)ana.GetCurrentAnimatorStateInfo(m_pFSM.moveAnimatorLayer).normalizedTime;
        bool AnaIsInTransition = ana.IsInTransition(m_pFSM.moveAnimatorLayer);

        //bool b_isStateDefault = anb.GetCurrentAnimatorStateInfo(m_pFSM.attackAnimatorLayer).IsTag("IDLE");
        m_lastAnBClipHash = anb.GetCurrentAnimatorStateInfo(m_pFSM.attackAnimatorLayer).fullPathHash;
        m_lastAnBClipTime = (int)anb.GetCurrentAnimatorStateInfo(m_pFSM.attackAnimatorLayer).normalizedTime;
        bool AnbIsInTransition = anb.IsInTransition(m_pFSM.attackAnimatorLayer);

        if (AnaIsInTransition)//AnA处在过渡中
        {
            m_lastAnANextClipHash = ana.GetNextAnimatorStateInfo(m_pFSM.moveAnimatorLayer).fullPathHash;
            m_lastAnAClipTime = (int)ana.GetNextAnimatorStateInfo(m_pFSM.moveAnimatorLayer).normalizedTime;
        }
        if (AnbIsInTransition)//AnB处在过渡中
        {
            m_lastAnBNextClipHash = anb.GetNextAnimatorStateInfo(m_pFSM.attackAnimatorLayer).fullPathHash;
            m_lastAnAClipTime = (int)anb.GetNextAnimatorStateInfo(m_pFSM.attackAnimatorLayer).normalizedTime;
        }



        if (goBBackAMTransform && goBBackAMTransform.parent != goBRootMTransform)
        {
            //还原BackAM到RootM下，不然reset animator的时候动作会无效
            goBBackAMTransform.parent = goBRootMTransform;
            m_DesireBackAMParent = goBRootMTransform;
            m_DesireBackAMParentProp = 1.0f;
        }
        //替换动画
        if (m_clipPairsA != null)
        {
            replaceAnimA();
        }

        if (m_clipPairsB != null)
        {
            replaceAnimB();
        }

        return true;
    }
    /// <summary>
    ///  AnimatorOverrideController替换动画片段后 控制器进入默认State 根据记录恢复控制器运行状态
    /// </summary>
    private void AfterAnimationReplaceBack()
    {
        if (m_lastAnAClipHash != 0)
        {
            if (m_lastAnANextClipHash == 0)//未处于过渡
            {
                ana.CrossFade(m_lastAnAClipHash, 0.0f, m_pFSM.moveAnimatorLayer, m_lastAnAClipTime);
            }
            else                            //处于过渡
            {
                ana.CrossFade(m_lastAnAClipHash, 0.0f, m_pFSM.moveAnimatorLayer, 1);
                ana.CrossFade(m_lastAnANextClipHash, 0.0f, m_pFSM.moveAnimatorLayer, m_lastAnAClipTime);
            }
            m_lastAnAClipHash = 0;
            m_lastAnANextClipHash = 0;
        }

        if (m_lastAnBClipHash != 0)
        {
            if (m_lastAnBNextClipHash == 0)
            {
                anb.CrossFade(m_lastAnBClipHash, 0.0f, m_pFSM.attackAnimatorLayer, m_lastAnBClipTime);
            }
            else
            {
                anb.CrossFade(m_lastAnBClipHash, 0.0f, m_pFSM.attackAnimatorLayer, 1);
                anb.CrossFade(m_lastAnBNextClipHash, 0.0f, m_pFSM.attackAnimatorLayer, m_lastAnBClipTime);
            }

            m_lastAnBClipHash = 0;
            m_lastAnBNextClipHash = 0;
        }
    }
    #endregion

    #region 骨骼和动作初始化
    public void OnSkinChanged(SkinInstance sk)
    {
        if (!m_pFSM.SkinControl)
        {
            return;
        }
        if (null == sk || !sk.isValid)
        {
            return;
        }
        if (m_pFSM.attackAnimatorLayer > 0)
        {
            overControllerA = sk.overrideController;
            if (!overControllerA)
            {
                return;
            }

            overControllerA.name = "overControllerMain";
            //ana.runtimeAnimatorController = m_pFSM.SkinConfig.controllerA;
            //UpdateAnimatorScript();
            overControllerB = overControllerA;
        }
        else
        {
            Debug.LogError("CreatureAnimatorCtrl <= 0");
        }
        //else
        //{
        //    if (ana != null)
        //    {
        //        overControllerA = sk.GetOverrideController(ana.gameObject);
        //        if (!overControllerA)
        //        {
        //            return;
        //        }

        //        overControllerA.name = "overControllerA";
        //        //ana.runtimeAnimatorController = m_pFSM.SkinConfig.controllerA;
        //        //UpdateAnimatorScript();
        //    }
        //    else { Debug.LogError("ana==null!!!"); }

        //    if (anb != null)
        //    {
        //        overControllerB = sk.GetOverrideController(anb.gameObject);
        //        if (!overControllerA)
        //        {
        //            return;
        //        }
        //        overControllerB.name = "overControllerB";
        //        //anb.runtimeAnimatorController = m_pFSM.SkinConfig.controllerB;
        //    }

        //}
        UpdateBones(true);
        m_screenRaycast = m_pFSM.GetComponent<ScreenRaycast>();

    }

    private bool NeedToUpDateAnimatorSript = true;
    private bool NeedToUpDateBones = true;

    public void OnSkinCull()
    {

    }

    public void OnSkinNotCull()
    {
        goBMainTransform = null; //记录攻击动作所用gameObject的根节点，用于边打边走的位置更新
        goBRootATransform = null; //记录攻击动作所用gameObject的节点，用于边打边走的位置更新
        goBRootMTransform = null; //记录攻击动作所用gameObject的节点，用于边打边走的位置更新    
        goBBackAMTransform = null; //记录攻击动作所用gameObject的节点，用于边打边走的位置更新z
        goBBackAMLocalPosition = Vector3.zero; //记录加载是Back_AM的相对位移，以后切父节点时始终保持相对位移

        NeedToUpDateAnimatorSript = true;
        NeedToUpDateBones = true;
    }

    private void UpdateBones(bool force = false)
    {
        if (!NeedToUpDateBones && !force)
        {
            return;
        }

        SkinInstance si = m_pFSM.SkinControl.CurrentSkinInstance;
        if (null == si)
        {
            return;
        }
        NeedToUpDateBones = false;
        if (anb)
        {
            goBMainTransform = m_pFSM.FindTransformEx("Main");
            goBRootATransform = m_pFSM.FindTransformEx("Root_A");
            goBRootMTransform = m_pFSM.FindTransformEx("Root_M");
            goBBackAMTransform = m_pFSM.FindTransformEx("BackA_M");

            if (m_pFSM.attackAnimatorLayer > 0)
            {
                if (goBMainTransform == null)
                {
                    Trace.Warning(m_pFSM.name + "找不到骨骼点Main，通知美术修改？");
                }
                if (goBRootATransform == null)
                {
                    Trace.Warning(m_pFSM.name + "找不到骨骼点Root_A，通知美术修改？");
                }
                if (goBRootMTransform == null)
                {
                    Trace.Warning(m_pFSM.name + "找不到骨骼点Root_M，通知美术修改？");
                }
                if (goBBackAMTransform == null)
                {
                    Trace.Warning(m_pFSM.name + "找不到骨骼点BackA_M，通知美术修改？");
                }

                //因为Reset的时候，重置了RootA的parent，导致RootA的旋转出问题了，这里要重新设置回来，不然的话RootA会乱掉
                if (goBRootATransform && goBRootMTransform && goBRootATransform.parent != goBRootMTransform.parent)
                {
                    //按照实际需求，RootA应该与RootM并列，但是美术为了输出方便，直接放到了上层节点，这里修正一下
                    goBRootATransform.parent = goBRootMTransform.parent;
                }

                if (goBBackAMTransform)
                {
                    goBBackAMLocalPosition = goBBackAMTransform.localPosition;
                }
            }

            m_DesireBackAMParent = null;
            m_DesireBackAMParentProp = 0.0f;

        }
    }

    public void UpdateAnimatorScript(bool force = false)
    {
        //在动作上绑定光效，目前主要为跑步和跳跃增加烟雾，a状态机加光效，b状态机处理函数为空不加光效
        if (!NeedToUpDateAnimatorSript && !force)
        {
            return;
        }
        NeedToUpDateAnimatorSript = false;
        LightingEffectManager leManager = m_pFSM.GetComponent<LightingEffectManager>();
        if (leManager == null)
        {
            return;
        }
        if (!ana)
        {
            return;
        }
        SkinInstance si = m_pFSM.SkinControl.CurrentSkinInstance;
        if (null == si)
        {
            return;
        }

        si.bindEffect_A.bindLightingEvent -= leManager.BindLighting;
        si.bindEffect_B.bindLightingEvent -= leManager.BindLightingEmpty;
        si.bindSound.bindSoundEvent -= SoundManager.CreateSound;

        si.bindEffect_A.bindLightingEvent += leManager.BindLighting;
        si.bindEffect_B.bindLightingEvent += leManager.BindLightingEmpty;
        si.bindSound.bindSoundEvent += SoundManager.CreateSound;

    }


    public void ResetBones()
    {
        //还原层次结构，避免节点动画丢失
        if (goBBackAMTransform && goBRootMTransform && goBBackAMTransform.parent != goBRootMTransform)
        {
            goBBackAMTransform.parent = goBRootMTransform;
        }

        if (goBRootATransform)
        {
            goBRootATransform.parent = goBMainTransform;
        }

        if (goBBackAMTransform)
        {
            goBBackAMTransform.localPosition = goBBackAMLocalPosition;
        }
    }

    #endregion

    bool m_skillLeft = false;
    bool m_skillRight = false;
    bool m_animatorASkillLeft = false;
    bool m_animatorASkillRight = false;


    //为两个攻击层做一个过渡，如果全身攻击到一半，突然行走，变成上下半身融合，则利用这个过渡解决动作突变
    float m_DesireAttackLayerWeight = 0.0f;
    float m_DesireAttackUpLayerWeight = 0.0f;
    private void updateLayerWeight()
    {
        if (m_pFSM.attackAnimatorLayer <= 0) return;
        float deta = m_pFSM.creaturePropety.animatorCrossSpeed * Time.deltaTime;
        float layerWeight = anb.GetLayerWeight(m_pFSM.attackAnimatorLayer);
        float layerStep = m_DesireAttackLayerWeight - layerWeight;
        if (Mathf.Abs(layerStep) > 0.01f)
        {
            if (Mathf.Abs(layerStep) > deta)
            {
                layerStep = Mathf.Sign(layerStep) * deta;
            }
            anb.SetLayerWeight(m_pFSM.attackAnimatorLayer, layerWeight + layerStep);
        }
        float layerUpWeight = anb.GetLayerWeight(m_pFSM.attackAnimatorLayer + 1);
        float layerUpStep = m_DesireAttackUpLayerWeight - layerUpWeight;
        if (Mathf.Abs(layerUpStep) > 0.01f)
        {
            if (Mathf.Abs(layerUpStep) > deta)
            {
                layerUpStep = Mathf.Sign(layerUpStep) * deta;
            }
            anb.SetLayerWeight(m_pFSM.attackAnimatorLayer + 1, layerUpWeight + layerUpStep);
        }
    }

    private AnimatorClipInfo[] m_attackAnimatorClipInfo;
    private AnimatorStateInfo m_attackAnimatorStateInfo;
    private Transform m_DesireBackAMParent = null;
    private float m_DesireBackAMParentProp = 0.0f;
    private bool m_animatorUpStateAttackUp1 = false;

    public void update()
    {
        if ((ana == null || anb == null) || ((m_pFSM.attackAnimatorLayer == 0) && (anc == null)))
        {
            return;
        }

        if (!m_pFSM.SkinConfig)
        {
            return;
        }
        if (!m_pFSM.GetVisible())
        {
            return;
        }

        if (m_rePlayAnimA != 0)
        {
            ana.Play(m_rePlayAnimA);
            m_rePlayAnimA = 0;
        }

        if (m_rePlayAnimB != 0)
        {
            anb.Play(m_rePlayAnimB, -1);
            m_rePlayAnimB = 0;
        }
        #region 如果需要切换动作，在这里切换，每帧一次避免冲突
        if (m_crossfadeName != "")
        {
            updatePlayAnim(m_crossfadeName, m_crossfadeReact, m_crossfadeSpeed, m_crossfadeDuratioTime, m_crossNormalizedTime);
            m_skillLeft = false;
            m_skillRight = false;
            if ((m_crossfadeName.Length >= 3) && (m_crossfadeName[0] == 'L') && (m_crossfadeName[1] == 'J') && (m_crossfadeName[2] == '_'))
            {
                m_skillLeft = true;
            }
            if ((m_crossfadeName.Length >= 3) && (m_crossfadeName[0] == 'R') && (m_crossfadeName[1] == 'J') && (m_crossfadeName[2] == '_'))
            {
                m_skillRight = true;
            }
        }
        if (m_weaponCrossfadeName != "")
        {
            updatePlayWeaponAnim(m_weaponCrossfadeName, m_weaponCrossfadeSpeed);
        }
        if (m_weaponCrossfadeName2 != "")
        {
            updatePlayWeaponAnim2(m_weaponCrossfadeName2, m_weaponCrossfadeSpeed2);
        }

        if (m_pFSM.SkinConfig.RootAReplace)
        {
            if (m_animatorASkillLeft != m_skillLeft)
            {
                ana.SetBool(id_nSkillLeft, m_skillLeft);
                m_animatorASkillLeft = m_skillLeft;
                m_skillLeft = false;
            }
            if (m_animatorASkillRight != m_skillRight)
            {
                ana.SetBool(id_nSkillRight, m_skillRight);
                m_animatorASkillRight = m_skillRight;
                m_skillRight = false;
            }
        }



        m_crossfadeName = "";
        m_crossfadeDuratioTime = 0.0f;
        m_crossNormalizedTime = 0.0f;
        m_crossfadeReact = false;
        m_crossfadeSpeed = 1.0f;
        m_weaponCrossfadeName = "";
        m_weaponCrossfadeSpeed = 1.0f;
        m_weaponCrossfadeName2 = "";
        m_weaponCrossfadeSpeed2 = 1.0f;
        #endregion

    

        #region 查看动作状态知否改变，包括AttackUp，AttackMove, BlendAttack,以及跑步/静止的切换

        m_attackAnimatorClipInfo = anb.GetCurrentAnimatorClipInfo(m_pFSM.attackAnimatorLayer);
        m_attackAnimatorStateInfo = anb.GetCurrentAnimatorStateInfo(m_pFSM.attackAnimatorLayer);

        bool stateChange = false;
        bool animChange = false;
        m_animatorUpStateIdle = m_attackAnimatorStateInfo.IsTag("IDLE");
        if (m_pFSM.attackAnimatorLayer > 0)
        {
            anb.SetBool(id_bIsAttackAnim, !m_animatorUpStateIdle);
        }
        int currentAnbHash = m_lastAnbHash;
        bAnimatorAttack = !m_animatorUpStateIdle;
        currentAnbHash = m_attackAnimatorStateInfo.fullPathHash;
        if (m_lastAnbHash != currentAnbHash)
        {
            stateChange = true;
            animChange = true;
            m_lastAnbHash = currentAnbHash;
        }

        bool animatorDownStateMoving = ana.GetBool("isMoving");
        bool isMovingChange = false;
        if (!m_animatorUpSatateBlendAttack && (((animatorDownStateMoving) && (m_bAnimatorMoving == false)) || ((!animatorDownStateMoving) && (m_bAnimatorMoving == true))))
        {
            stateChange = true;
            isMovingChange = true;
        }
        m_bAnimatorMoving = animatorDownStateMoving;

        if (firsttime)
        {
            stateChange = true;
            firsttime = false;
        }
        #endregion

        if (updateAnimaiontReplace())
        {
            return;
        }

        updateLayerWeight();

        #region 如果动作状态切换了，调整上下半身骨骼混合设置，对于AttackMove在攻击范围内则原地动作
        if (stateChange)
        {
            if (animChange && m_pFSM.reportSkillMove && (m_animatorUpStateAttackMove || m_animatorUpSatateBlendAttack))
            {
                //如果刚刚脱离的是位移移动动作，则上报同步位置，用于同步电脑玩家
                m_pFSM.reportPos();
            }

            if (goBBackAMTransform && goBRootMTransform)
            {
                m_DesireBackAMParent = goBRootMTransform;
                m_DesireBackAMParentProp = 0.0f;

                if (m_animatorUpStateAttackUp1) //animatorUpStateAttackUp1配置下，开始和结束都需要瞬切
                {
                    m_DesireBackAMParentProp = 1.0f;
                }
            }

            if (m_pFSM.attackAnimatorLayer > 0)
            {
                if (m_bAnimatorMoving)
                {
                    m_DesireAttackLayerWeight = 0.0f;
                    m_DesireAttackUpLayerWeight = 1.0f;
                    //anb.SetLayerWeight(m_pFSM.attackAnimatorLayer, 0.0f);
                    //anb.SetLayerWeight(m_pFSM.attackAnimatorLayer + 1, 1.0f);
                }
                else
                {
                    m_DesireAttackLayerWeight = 1.0f;
                    m_DesireAttackUpLayerWeight = 0.0f;

                    //anb.SetLayerWeight(m_pFSM.attackAnimatorLayer, 1.0f);
                    //anb.SetLayerWeight(m_pFSM.attackAnimatorLayer + 1, 0.0f);
                }
            }

            if (m_animatorUpStateIdle)
            {
                if (m_pFSM.attackAnimatorLayer > 0)
                {
                    anb.SetFloat(id_fAttackSpeed, 1.0f);
                }
                else
                {
                    anb.speed = 1.0f;
                }
            }
            bDoingAttackMove = false;
            bAttackMoveBlending = false;
            if (delayFaceTo() && m_pFSM.isHero)
            {
                //=delayFaceTo(); //在b_doingAttackMove情况下转向滞后，到此时才转。
                m_pFSM.syncPos();
            }
            m_animatorUpStateAttackMove = false;
            m_animatorUpSatateBlendAttack = false;
            if (bAnimatorAttack)
            {
                m_animatorUpStateAttackUp1 = m_attackAnimatorStateInfo.IsTag("AttackUp1"); //AttackUp1的时候，RootM生切到RootA，避免正面跑步和侧身攻击之间的腰部过渡（因为这样会向左扭曲）
                bool animatorUpStateAttackUp = (m_attackAnimatorStateInfo.IsTag("AttackUp") || m_animatorUpStateAttackUp1);
                m_animatorUpStateForceAttackMove = m_attackAnimatorStateInfo.IsTag("ForceAttackMove");
                m_animatorUpStateAttackMove = (m_attackAnimatorStateInfo.IsTag("AttackMove") || m_animatorUpStateForceAttackMove);
                m_animatorUpSatateBlendAttack = m_attackAnimatorStateInfo.IsTag("BlendAttack");

                //blendAttack支持attackMove
                if (m_animatorUpSatateBlendAttack)
                {
                    if (animChange)
                    {
                        //恢复成默认值
                        anb.SetFloat(id_nMoveForward, 1.0f);
                        anb.SetFloat(id_nMoveRight, 0.0f);
                        bAttackMoveZero = true; //如果没有强制位移的话，就原地滚了
                    }
                }

                if ((!animChange || m_bAnimatorMoving || (m_pFSM.getEntityState() == EntityState.Jumping)) && (animatorUpStateAttackUp))//上下身分离                
                {
                    if (m_pFSM.attackAnimatorLayer > 0)
                    {
                        //anb.SetLayerWeight(m_pFSM.attackAnimatorLayer, 0.0f);
                        //anb.SetLayerWeight(m_pFSM.attackAnimatorLayer+1, 1.0f);
                        m_DesireAttackLayerWeight = 0.0f;
                        m_DesireAttackUpLayerWeight = 1.0f;

                        if (goBBackAMTransform && goBRootATransform)
                        {
                            //上下半身分离，上半身继承RootA的旋转，RootA旋转记录了攻击动作的旋转，并考虑了移动动作对腰部旋转的消减
                            //脚部则继承下半身移动动作RootM
                            m_DesireBackAMParent = goBRootATransform;
                            m_DesireBackAMParentProp = 0.0f;

                            if (m_animatorUpStateAttackUp1)
                            {
                                m_DesireBackAMParentProp = 0.99f;
                            }
                        }

                    }
                    else if (anc)
                    {
                        if (m_pFSM.SkinConfig.MorphHip)
                        {
                            anc.SetDefaultWeightsSetting1(m_pFSM.SkinConfig.RootAReplace && goBRootATransform);
                        }
                        else
                        {
                            anc.SetDefaultWeightsSetting3(m_pFSM.SkinConfig.RootAReplace && goBRootATransform);
                        }
                    }
                    bAttackMoveBlending = true;
                }
                else//纯攻击动作
                {
                    if (m_pFSM.SkinConfig.RootAReplace && animChange && animatorUpStateAttackUp) //如果是刚开始攻击动作，而且没有移动，则根据攻击动作移动controller
                    {
                        m_animatorUpStateAttackMove = true;
                    }

                    bool animatorUpStateMovingUp = m_attackAnimatorStateInfo.IsTag("MovingUp");
                    if ((isMovingChange) && (m_bAnimatorMoving == true) && (animatorUpStateMovingUp))//移动动作覆盖攻击动作
                    {
                        BreakAttackAnim("", 0.0f);
                    }
                    else
                    {
                        if (m_pFSM.attackAnimatorLayer > 0)
                        {
                            //全身攻击动作直接切
                            anb.SetLayerWeight(m_pFSM.attackAnimatorLayer, 1.0f);
                            anb.SetLayerWeight(m_pFSM.attackAnimatorLayer + 1, 0.0f);
                            m_DesireAttackLayerWeight = 1.0f;
                            m_DesireAttackUpLayerWeight = 0.0f;
                        }
                        else if (anc)
                        {
                            anc.SetAll(false, 1.0f, false, 1.0f);
                        }
                    }
                }

                if (animChange && m_animatorUpStateAttackMove)
                {
                    //animatorUpStateAttackMove = false;
                    //attackAnimatorOrgPos = anb.transform.position;
                    lastAnimatorBOffset = Vector3.zero;
                    bAttackMoveZero = false;

                    if (!m_animatorUpStateForceAttackMove)  //无视与目标之间的距离，必须AttackMove
                    {
                        float dis2 = 100.0f;
                        if (m_pFSM.animationTarget)
                        {
                            Vector3 deta = m_pFSM.animationTarget.position - m_pFSM.controller.transform.position;
                            if (deta.y > m_pFSM.creaturePropety.attackMoveRange)
                            {
                                deta.y = 0;
                            }
                            dis2 = deta.magnitude;
                        }

                        //if (animatorUpSatateBlendAttack || dis > creaturePropety.attackMoveRange) //在攻击范围内的话就原地动作
                        if (dis2 < m_pFSM.creaturePropety.attackMoveRange) //在攻击范围内的话就原地动作
                        {
                            bAttackMoveZero = true;
                        }
                    }
                }

            }
            else if (anc)//纯下身
            {
                anc.SetAll(false, 0.0f, false, 0.0f);
            }

            m_pFSM.bAnimPauseControl = m_animatorUpSatateBlendAttack;
        }
        #endregion

        #region 为BlendAttack确定翻滚方向
        if (m_animatorUpSatateBlendAttack && m_pFSM.moveCtrl.bForceMoving)
        {
            //翻滚动作，根据面向和强制位置的方向选择动作
            Vector3 faceDir = m_pFSM.moveCtrl.getForceMovePos() - m_pFSM.transform.position;
            faceDir.y = 0.0f;
            if (faceDir.sqrMagnitude < 0.25f)
            {
                bAttackMoveZero = true;

                //开始翻滚的话，就把强制位移停掉
                m_pFSM.moveCtrl.StopForceMove();
            }
            else
            {
                faceDir.Normalize();
                Vector3 faceRight = Vector3.Cross(m_pFSM.transform.up, faceDir);
                faceRight.Normalize();
                Vector3 turnTo = faceDir;
                float forward = Vector3.Dot(faceDir, m_pFSM.transform.forward);
                float right = Vector3.Dot(faceDir, m_pFSM.transform.right);

                //开始翻滚的话，就把强制位移停掉
                m_pFSM.moveCtrl.StopForceMove();

                if (Mathf.Abs(forward) > Mathf.Abs(right))
                {
                    right = 0.0f;
                    if (forward < 0.0f)
                    {
                        forward = -1.0f;
                        turnTo *= -1.0f;
                    }
                    else
                    {
                        forward = 1.0f;
                    }
                }
                else
                {
                    forward = 0.0f;
                    if (right < 0.0f)
                    {
                        right = -1.0f;
                        turnTo = faceRight;
                    }
                    else
                    {
                        right = 1.0f;
                        turnTo = -faceRight;
                    }
                }
                anb.SetFloat(id_nMoveForward, forward);
                anb.SetFloat(id_nMoveRight, right);

                //转向微调，保证沿指定方向翻滚
                Vector3 targetPos = m_pFSM.transform.position + turnTo;
                m_pFSM.transform.LookAt(new Vector3(targetPos.x, m_pFSM.transform.position.y, targetPos.z));
                if (m_pFSM.soliderController)
                {
                    m_pFSM.soliderController.desireAngle = m_pFSM.transform.eulerAngles;
                }

                //animatorUpSatateBlendAttack = false;
                //animatorUpStateAttackMove = true;
                lastAnimatorBOffset = Vector3.zero;
                bAttackMoveZero = false;

            }
        }
        #endregion
        return;
        #region 废弃代码，分层设置动画
        //if (null == m_pFSM.SkinConfig || m_pFSM.animator == null)
        //{
        //    return;
        //}

        //if (m_pFSM.SkinConfig.progressWeight == true)
        //{
        //    float weight = m_pFSM.animator.GetLayerWeight(2);
        //    weight = weight + weightDelta;
        //    if (weight > 1.0f)
        //    {
        //        weight = 1.0f;
        //        weightDelta = 0.0f;
        //    }

        //    if (weight < 0.0f)
        //    {
        //        weight = 0.0f;
        //        weightDelta = 0.0f;
        //    }

        //    m_pFSM.animator.SetLayerWeight(2, weight);

        //}

        //AnimatorStateInfo sta = m_pFSM.animator.GetCurrentAnimatorStateInfo(1);

        ////        if (!sta.IsTag("IDLE"))
        ////        {
        ////            AnimationInfo[] animInfo = animator.GetCurrentAnimationClipState(1);
        ////            if (animInfo.Length >= 1)
        ////            {
        ////                foreach (AnimationInfo anim in animInfo)
        ////                {
        ////                    anim.clip.frameRate = 0.0f;
        ////                }
        ////                
        ////            }
        ////        }

        //int currentAnimeStateTagHash = sta.tagHash;
        //if (currentAnimeStateTagHash == lastAnimStateTagHash)
        //{
        //    return;
        //}

        ////只有状态tag不同的时候才进到这里，所以每次切状态最多调用一次
        //lastAnimStateTagHash = currentAnimeStateTagHash;
        ////设置第三层的权重，用于上下半身分离
        //if (sta.IsTag("AttackUp"))
        //{
        //    weightDelta = 0.1f;
        //    m_pFSM.SkinConfig.separingBody = true;
        //    if (m_pFSM.SkinConfig.progressWeight == false)
        //    {
        //        m_pFSM.animator.SetLayerWeight(2, 1);
        //    }

        //}
        //else
        //{
        //    weightDelta = -0.1f;
        //    m_pFSM.SkinConfig.separingBody = false;
        //    if (m_pFSM.SkinConfig.progressWeight == false)
        //    {
        //        m_pFSM.animator.SetLayerWeight(2, 0);
        //    }
        //}
        #endregion
    }

    #region 废弃代码，保存骨骼信息
    ////保存旧的骨骼信息
    //public void ReserveBoneTransformSelf(Transform t, Transform animroot, ref Dictionary<string, BoneTransform> bones)
    //{
    //    string bonename = t.name;
    //    BoneTransform bt;
    //    bt.pos = animroot.InverseTransformPoint(t.position);
    //    bt.angle = t.eulerAngles - animroot.eulerAngles;
    //    bt.scale = t.lossyScale;
    //    bones[bonename] = bt;
    //}

    ////保存旧的骨骼信息（递归方法）
    //public void ReserveBoneTransformRecursive(Transform t, Transform animroot, ref Dictionary<string, BoneTransform> bones)
    //{
    //    ReserveBoneTransformSelf(t,animroot,ref bones);

    //    int n = t.childCount;
    //    for (int i = 0; i < n; i++)
    //    {
    //        Transform tchild = t.GetChild(i);
    //        if (tchild != null)
    //        {
    //            ReserveBoneTransformRecursive(tchild, animroot, ref bones);
    //        }
    //    }
    //}

    ////恢复旧的骨骼信息
    //public void RestoreBoneTransformSelf(Transform t, Transform animroot, ref Dictionary<string, BoneTransform> bones)
    //{
    //    string bonename = t.name;
    //    if (bones.ContainsKey(bonename))
    //    {
    //        BoneTransform bt = bones[bonename];
    //        t.eulerAngles = animroot.eulerAngles + bt.angle;
    //        t.SetPosition( animroot.TransformPoint(bt.pos);
    //    }
    //}

    ////恢复旧的骨骼信息（递归方法）
    //public void RestoreBoneTransformRecursive(Transform t, Transform animroot, ref Dictionary<string, BoneTransform> bones)
    //{
    //    RestoreBoneTransformSelf(t, animroot, ref bones);

    //    int n = t.childCount;
    //    for (int i = 0; i < n; i++)
    //    {
    //        Transform tchild = t.GetChild(i);
    //        if (tchild != null)
    //        {
    //            RestoreBoneTransformRecursive(tchild, animroot, ref bones);
    //        }
    //    }
    //}
    #endregion

    private ScreenRaycast raycast = null;
    public float lookAngle = 0.0f;

    public void LateUpdate()
    {
        if (null == m_pFSM.SkinConfig)
        {
            return;
        }


        if (!m_pFSM.GetVisible())
        {
            return;
        }

        UpdateAnimatorScript();

        UpdateBones();

        AfterAnimationReplaceBack();

        #region 上半身反扭（废弃）
        ////动作分离用代码。将spine点进行一定的反向旋转。
        //if (SkinConfig.adjustSpine && SkinConfig.separingBody && SkinConfig.BackA_M != null && SkinConfig.Root_MCopy != null) //hips!=null 不用判了，因为spine是根据hips取的
        //{
        //    if (animator.GetBool("isMoving"))
        //    {
        //        ////reserve all
        //        //if (HipBlend > 0.0f && m_CurStateMachineParams.root != null && m_CurStateMachineParams.root.parent != null)
        //        //{
        //        //    oldtrans.Clear();
        //        //    ReserveBoneTransformRecursive(m_CurStateMachineParams.root, m_CurStateMachineParams.root.parent, ref oldtrans);
        //        //}

        //        //是否拉直上身
        //        if (HipPull)
        //        {
        //            Quaternion hipRot = SkinConfig.Root_M.localRotation;
        //            Quaternion spineRot = SkinConfig.BackA_M.localRotation;
        //            //hipR = hipRot;
        //            //Trace.Log(hipR.ToString());
        //            hipRot.w *= -1.0f;

        //            //上下身分离用的特殊处理
        //            //骨骼层次关系为Root->Hips->Spine
        //            //跑步动作要扭屁股，所以要用到Root和Hips，因此Mask中将Root和Hips两个骨骼分配到LegMask层
        //            //但是直接应用的话上半身攻击时Spine会跟着Hips扭，所以Spine节点先乘以Hips旋转的负数，用以抵消这个扭动
        //            //接下来，攻击动作本身也是需要扭动Hips点来表现全身力度的，可攻击的Hips旋转已经被跑步的Hips旋转覆盖了，
        //            //所以我们把攻击动作的Hips点又输出了一份到HipsCopy点，HipsCopy不在LegMask，
        //            //然后将HipsCopy的旋转传递给Spine点，那么Spine点就能继承攻击动作Hips的扭动了
        //            //最终，spine的局部旋转=(-跑步hips旋转)*(攻击hips旋转)*(spine旋转)
        //            //而spine的全局旋转=(跑步Root旋转)*(跑步hips旋转)*(-跑步hips旋转)*(攻击hips旋转)*(spine旋转)=面向+攻击时spine的旋转

        //            hipRot = hipRot * SkinConfig.Root_MCopy.localRotation;
        //            SkinConfig.BackA_M.localRotation = hipRot * spineRot;
        //        }

        //        ////如果有混合情况，需要在LateUpdate保存算好的给下一帧Update时恢复，因为每帧顺序是Update->Animator.update->LateUpdate，这段代码目前上下身混合时用到
        //        //if (HipBlend > 0.0f && m_CurStateMachineParams.root != null && m_CurStateMachineParams.root.parent != null)
        //        //{
        //        //    Transform animroot = m_CurStateMachineParams.root.parent;

        //        //    Vector3 hipAngleDown = m_CurStateMachineParams.hips.eulerAngles - animroot.eulerAngles;
        //        //    Vector3 hipAngleUp = m_CurStateMachineParams.hipsCopy.eulerAngles - animroot.eulerAngles;
        //        //    float weightangle = HipBlend;
        //        //    Vector3 hipAngleBlend;
        //        //    hipAngleBlend.x = Mathf.LerpAngle(hipAngleDown.x, hipAngleUp.x, weightangle);
        //        //    hipAngleBlend.y = Mathf.LerpAngle(hipAngleDown.y, hipAngleUp.y, weightangle);
        //        //    hipAngleBlend.z = Mathf.LerpAngle(hipAngleDown.z, hipAngleUp.z, weightangle);
        //        //    m_CurStateMachineParams.hips.eulerAngles = animroot.eulerAngles + hipAngleBlend;

        //        //    //restore
        //        //    int n = m_CurStateMachineParams.hips.childCount;
        //        //    for (int i = 0; i < n; i++)
        //        //    {
        //        //        Transform tchild = m_CurStateMachineParams.hips.GetChild(i);
        //        //        string bonename = tchild.name;
        //        //        if (bonename != "BackA_M")
        //        //        {
        //        //            RestoreBoneTransformRecursive(tchild, animroot, ref oldtrans);
        //        //        }
        //        //    }

        //        //}
        //    }
        //}
        #endregion

        #region 上下半身分离时，腰部使用RootA的动作，去除多余的位移和旋转
        if (m_pFSM.SkinConfig.RootAReplace)
        {
            if (goBRootATransform)
            {
                goBRootATransform.localPosition = Vector3.zero;
            }

            if (bAttackMoveBlending && goBMainTransform)
            {
                //RootA只有旋转部分有效，其他的是美术输出冗余，此外上下半身融合时，只考虑下半身行走位移，所以Main点位移无效
                goBMainTransform.localPosition = Vector3.zero;
            }
        }
        #endregion


        #region update Bone-BackAM, according to its desire parent

        if (m_pFSM.attackAnimatorLayer > 0 && m_DesireBackAMParent && goBBackAMTransform && m_DesireBackAMParent != goBBackAMTransform.parent)
        {
            //RootA的位移经常是错的，用RootM的覆盖
            goBRootATransform.localPosition = goBRootMTransform.localPosition;

            Quaternion currentRotation = goBBackAMTransform.parent.localRotation;
            Quaternion desireRotation = m_DesireBackAMParent.localRotation;
            float angle = Quaternion.Angle(currentRotation, desireRotation);
            if (angle < 10.0f || m_DesireBackAMParentProp > 0.9f)
            {
                Vector3 position = goBBackAMTransform.localPosition;
                Quaternion localRotation = goBBackAMTransform.localRotation;
                goBBackAMTransform.parent = m_DesireBackAMParent;

                //Vector3 boneOffset = goBBackAMTransform.localPosition - goBBackAMLocalPosition;
                //if (boneOffset.sqrMagnitude > 0.01f)
                //{
                //    Debug.Log(m_pFSM.name+" boneOffset =" + boneOffset.magnitude + "goBBackAMTransform.localPosition=" + goBBackAMTransform.localPosition + "goBBackAMLocalPosition=" + goBBackAMLocalPosition);
                //    if (anb.GetCurrentAnimatorClipInfo(1).Length > 0)
                //    {
                //        Debug.Log("CurrentAnimatorClip:" + anb.GetCurrentAnimatorClipInfo(1)[0].clip.name + "State time:"+anb.GetCurrentAnimatorStateInfo(1).normalizedTime);
                //    }
                //    if (anb.GetNextAnimatorClipInfo(1).Length > 0)
                //    {
                //        Debug.Log("NextAnimatorClip:"+anb.GetNextAnimatorClipInfo(1)[0].clip.name);
                //    }
                //}


                goBBackAMTransform.localPosition = goBBackAMLocalPosition;
                goBBackAMTransform.localRotation = localRotation;
                m_DesireBackAMParentProp = 1.0f;
            }
            else
            {
                if (m_pFSM.showAnimLog)
                {
                    string output;
                    output = "BackAM Desire Parent " + m_DesireBackAMParent.name + ". RootA rotation" + goBRootATransform.localRotation.eulerAngles + ". RootM rotation" + goBRootMTransform.localRotation.eulerAngles + " angle" + angle;
                    output += "Move Anim:" + ana.GetCurrentAnimatorClipInfo(0)[0].clip.name + ",Time:" + ana.GetCurrentAnimatorStateInfo(0).normalizedTime;
                    if (anb.GetCurrentAnimatorClipInfo(m_pFSM.attackAnimatorLayer).Length > 0)
                    {
                        output += " Attack Anim:" + anb.GetCurrentAnimatorClipInfo(m_pFSM.attackAnimatorLayer)[0].clip.name + ",Time:" + ana.GetCurrentAnimatorStateInfo(m_pFSM.attackAnimatorLayer).normalizedTime;
                    }
                    Trace.Log(output);
                }
                Quaternion rotateQuat = Quaternion.Slerp(currentRotation, desireRotation, m_DesireBackAMParentProp);
                Quaternion rev = currentRotation;
                rev.w *= -1.0f;
                goBBackAMTransform.localRotation = rev * rotateQuat * goBBackAMTransform.localRotation;
                m_DesireBackAMParentProp += Time.deltaTime * m_pFSM.creaturePropety.animatorCrossSpeed;
            }
        }

        #endregion

        #region 弯腰瞄准
        //Transform cameraTarget = SoldierCamera.MainInstance<SoldierCamera>().target;
        lookAngle = 0.0f;
        if (m_pFSM.isHero == true && m_pFSM.SkinConfig.lookRefTransform && (Mathf.Abs(m_pFSM.SkinConfig.lookAngleScale)) > 0.001f)// && m_pFSM.soliderController && m_pFSM.soliderController.isAttackMode())// && m_pFSM.SkinConfig.BackA_M != null)
        {
            if (raycast == null)
            {
                raycast = m_pFSM.GetComponent<ScreenRaycast>();
            }


            if (raycast != null && raycast.targetPos.sqrMagnitude > 0.01f)
            {
                //float cameraAngle = cameraTarget.rotation.eulerAngles.x;
                //if (cameraAngle > 180.0f)
                //{
                //    cameraAngle = cameraAngle - 360.0f;
                //}

                if (!goBBackAMTransform || raycast.targetPos.sqrMagnitude < 0.01f)//正常情况下，这玩意不会为null，但是不知道为什么，又来null
                {
                    return;
                }

                Vector3 lookDir = raycast.targetPos - goBBackAMTransform.position;
                if (lookDir.sqrMagnitude > 0)
                {
                    lookAngle = -Mathf.Asin(lookDir.y / lookDir.magnitude) / Mathf.PI * 180;
                }

                //此处约定，如果某动作不需要弯腰瞄准，则lookRefTransform.localPosition设置为0
                if (m_pFSM.SkinConfig.lookRefTransform && m_pFSM.SkinConfig.lookRefTransform.localPosition.sqrMagnitude > 0.00001f)
                {
                    if (Mathf.Abs(lookAngle) > 0.1f)
                    {
                        //此处扭曲BackAM骨骼，用于弯身瞄准
                        //特别约定：lookRefTransform必须为BackAM的后代节点
                        //lookAngle计算出需要弯曲的角度，然后将lookRefTransform的x弯曲角度
                        //计算方法：
                        //设Ra为BackAM之前的global rotation，设Rb为BackAM的local rotation，Rc为BackAM至lookRefTransform的rotation叠加，RnRc为BackAM至lookRefTransform的local rotation
                        //设Q1为弯腰前lookRefTransform的global rotation，Q2为弯腰后lookRefTransform的global rotation，则
                        //Ra*Rb*Rc*Rn=Q1
                        //Ra*Rx*Rc*Rn=Q2
                        //将Rb改为Rx，可以达到效果
                        //解方程得
                        //               -1
                        //RaRx=Q2*(Rc*Rn)
                        //
                        //             -1
                        //Rc*Rn=(Ra*Rb)   *Q1

                        //RaRb=弯腰前BackAM.globalRotation
                        //RaRx=弯腰后BackAM.globalRotation

                        Vector3 offsetAngle = Vector3.zero;
                        offsetAngle.x += lookAngle;
                        Quaternion Q1 = Quaternion.Euler(m_pFSM.SkinConfig.lookRefTransform.eulerAngles);
                        Quaternion Q2 = Quaternion.Euler(offsetAngle + m_pFSM.SkinConfig.lookRefTransform.eulerAngles);
                        Quaternion RaRb = goBBackAMTransform.rotation;
                        Quaternion RaRb_1 = RaRb;
                        RaRb_1.w *= -1.0f;
                        Quaternion RcRn = RaRb_1 * Q1;
                        Quaternion RcRn_1 = RcRn;
                        RcRn_1.w *= -1.0f;
                        Quaternion Rt = Q2 * RcRn_1;
                        goBBackAMTransform.rotation = Rt;
                    }
                }

                //m_pFSM.SkinConfig.BackA_M.localRotation = hipsOldRevRot * rootOldRevRot * rootNewRot * m_pFSM.SkinConfig.Root_M.localRotation * m_pFSM.SkinConfig.BackA_M.localRotation;
            }

        }
        #endregion

        #region move character controller for AttackMove and BlendAttack each frame
        bDoingAttackMove = false;

        //进行了层融合后，在LateUpdate阶段，才根据动作移动controller
        if ((m_animatorUpStateAttackMove || m_animatorUpSatateBlendAttack) && !m_animatorUpStateIdle && goBMainTransform != null)
        {
            if ((anb.GetCurrentAnimatorStateInfo(m_pFSM.attackAnimatorLayer).fullPathHash == m_attackAnimatorStateInfo.fullPathHash) && m_attackAnimatorClipInfo.Length > 0 && m_attackAnimatorClipInfo[0].weight > 0.99f && m_attackAnimatorStateInfo.normalizedTime < 0.99f)
            {
                //如果进行了动作切换，LateUpdate阶段和Update阶段的动作可以不一样，所以当fullPathHash不一致时，不移动controller
                //带有位移的攻击动作，把位移转移到controller上
                //但是这种操作只在攻击动作权重为1的时候有效，在攻击动作权重下降，与IDLE融合的时候，不这么做，否则与IDLE融合会倒退
                //这种操作攻击动作完成前有效，攻击动作完成，normalizedTime>1（实测必须是0.99f，否则最后一帧会拉回来），与IDLE融合的时候，不这么做，否则与IDLE融合会倒退

                if (!m_pFSM.gameObject.activeInHierarchy) //预防CharacterController.Move called in inactive controller
                {
                    bAttackMoveZero = true;
                }

                if (!bAttackMoveZero)
                {
                    if (m_pFSM.soliderController && m_pFSM.soliderController.isStopMove())
                    {
                        bAttackMoveZero = true;
                    }


                    float dis2 = 100.0f;
                    if (!m_animatorUpStateForceAttackMove && m_pFSM.animationTarget)
                    {
                        Vector3 deta = m_pFSM.animationTarget.position - m_pFSM.controller.transform.position;
                        if (deta.y > m_pFSM.creaturePropety.attackMoveRange)
                        {
                            deta.y = 0;
                        }
                        dis2 = deta.magnitude;
                    }

                    //if (animatorUpSatateBlendAttack || dis > creaturePropety.attackMoveRange) //在攻击范围内的话就原地动作
                    if (dis2 < m_pFSM.creaturePropety.attackMoveRange) //在攻击范围内的话就原地动作
                    {
                        bAttackMoveZero = true;
                    }

                    Vector3 animatorOffset = (goBMainTransform.localPosition - lastAnimatorBOffset) * m_attackMoveDistanceScale;
                    //Debug.Log("time" + m_attackAnimatorStateInfo.normalizedTime + "main" + goBMainTransform.localPosition + "last" + lastAnimatorBOffset);
                    //Debug.Log("length" + m_attackAnimatorStateInfo.length);
                    lastAnimatorBOffset = goBMainTransform.localPosition;
                    if (goBMainTransform.parent != null)
                    {
                        animatorOffset = goBMainTransform.parent.rotation * animatorOffset;
                    }
                    Vector3 controllerOffset = animatorOffset;

                    if (m_animatorUpStateAttackMove)
                    {
                        //如果在有AttackMove动作播放时受到和动作方向相反的操作则动作不产生位移
                        //Vector3 moveDirection = controllerOffset.normalized;
                        //float cosAngle = Vector3.Dot(moveDirection, m_pFSM.rawInputMoveDirection);
                        //if (cosAngle < 0.0f)

                        //键盘控制产生位移的时候不播动作产生的位移
                        if (m_pFSM.inputMoveDirection.sqrMagnitude > 0.0001f)
                        {
                            bAttackMoveZero = true;
                        }
                    }
                    if ((!m_pFSM.gameObject.activeInHierarchy) || (!m_pFSM.controller.enabled))//预防CharacterController.Move called in inactive controller
                    {
                        bAttackMoveZero = true;
                    }

                    if (!bAttackMoveZero)
                    {
                        Vector3 beforePos = m_pFSM.controller.transform.position;
                        controllerOffset.y = -0.01f;        //y不能大于0，不然会掉下来触发 CollisionFlags.CollidedSides
                        CollisionFlags flag = m_pFSM.controller.Move(controllerOffset);
                        //Debug.Log("controllerOffset: " + controllerOffset);
                        Vector3 afterPos = m_pFSM.controller.transform.position;
                        float dis = (afterPos - beforePos).magnitude;
                        if (!m_animatorUpStateForceAttackMove && (((flag & CollisionFlags.CollidedSides) != 0) || ((controllerOffset.sqrMagnitude > 0.01f) && (dis < controllerOffset.magnitude / 3.0f))))
                        {
                            //卡住了，不动了
                            bAttackMoveZero = true;
                            Debug.Log("offset: " + (controllerOffset).ToString() + "  dis: " + dis.ToString());
                        }
                        //Debug.Log("offset"+(controllerOffset*100.0f).ToString()+"dis"+dis.ToString());
                    }

                }
                bDoingAttackMove = true;
            }
            goBMainTransform.localPosition = Vector3.zero;
        }
        #endregion

    }
}