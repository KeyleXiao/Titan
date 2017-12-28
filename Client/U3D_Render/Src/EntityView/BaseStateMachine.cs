using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;
using System;
using System.Reflection;
using U3D_Render.SkinCtrl;
using ImageEffects;
using System.Diagnostics;
using SPELL;

// 每新增一个成员，都要在ResetAllMembers()写一个初始值，
// 要不然缓存会有错哦
#if USE_TIMMER_BEHAVIOUR
public class BaseStateMachine : UpdateBehaviourOnTimer
#else
public class BaseStateMachine : MonoBehaviour
#endif
{
    protected ICreatureState[] m_stateHandlers = new ICreatureState[(int)EntityState.MaxCount];
    protected ICreatureState m_CurStateHandler;                // 当前状态

    public bool m_MaskVisible = true;                       // Mask控制的可见性
    public bool m_OnViewVisible = false;                    // 视野控制的可见,默认不在视野
    public bool m_bVisible = true;                          // 可见性，这个可见性由上面的多个可见性一起决定

    [System.NonSerialized]
    public float currentGravity;
    [System.NonSerialized]
    public bool isHero = false;
    [System.NonSerialized]
    public bool canControl = true;

    [System.NonSerialized]
    public bool isInScreen = true;

    public bool bPauseMachineUpdate = false;
    public bool bControlledBySever  //玩家拾取对自己英雄的控制权，改由服务器发送同步指令控制
    {
        set
        {
            m_controlledBySever = value;
        }
        get
        {
            return m_controlledBySever;
        }
    }
    private bool m_controlledBySever;

    /// <summary>
    /// 禁止移动
    /// </summary>
   [System.NonSerialized]
    public bool bNotMove = false;

    //瞄准和勾边需要的参数
    public Color outlineColorFriend=new Color(0.51f,0.84f,0.137f,1.0f);
    public Color outlineColorEnemy = new Color(1.0f, 0.0f, 0.0f, 1.0f);
    public float outlineWitdh=0.008f;

    [System.NonSerialized]
    public float maxForwardSpeed = 5.0f; //服务器传下来的控制速度，前进时最大此速度
    [System.NonSerialized]
    public float maxRunForwardSpeed = 5.0f; //考虑行走/轻功状态后，前进时最大此速度
    [System.NonSerialized]
    public float maxRunBackwardSpeed = 5.0f; //服务器传下来的控制速度，后退时最大此速度
    [System.NonSerialized]
    public float maxRunSidewaySpeed = 5.0f; //服务器传下来的控制速度，左右时最大此速度
    [System.NonSerialized]
    public float maxDodgeSpeed = 0.0f; //服务器传下来的控制速度，轻功时最大此速度

    //[HideInInspector]
    //public float basicBackwardSpeed; //property配置的，后退动作播放速度为1的时候的位移速度
    //[HideInInspector]
    //public float basicForwardSpeed; //property配置的，前进动作播放速度为1的时候的位移速度
    //[HideInInspector]
    //public float basicSidewaysSpeed; //property配置的，左右移动动作播放速度为1的时候的位移速度
    [System.NonSerialized]
    public int entityID = 0;

     [System.NonSerialized]
    public Animator animator;                  // 动画系统对象

    [System.NonSerialized]
    public Transform creature;                 // 生物对象


   [System.NonSerialized]
    public Transform wound;                     // 生物对象受击点，同其他模块取受击点用

    [System.NonSerialized]
    public CreatureProperty creaturePropety;                 // 生物对象属性

    [System.NonSerialized]
    public bool isNotAttack=false;                 // 是否是不可攻击的类型，例如NPC


    protected EntitySkinConfig entitySkinConfig
    {
        get
        {
            if(!SkinControl)
            {
                return null;
            }

            if(SkinControl.CurrentSkinInstance == null)
            {
                return null;
            }

            return SkinControl.CurrentSkinInstance.entitySkinConfig;
        }
    }

    public EntitySkinControl SkinControl
    {
        get
        {
            if (m_SkinControl == null)
            {
                m_SkinControl = GetComponent<EntitySkinControl>();
                m_SkinControl.UnRegsiterSkinCullEvent(OnSkinCull);
                m_SkinControl.UnRegsiterSkinNotCullEvent( OnSkinNotCull);
                m_SkinControl.UnRegsiterSkinChangedEvent(OnSkinChanged);

                m_SkinControl.RegsiterSkinCullEvent(OnSkinCull);
                m_SkinControl.RegsiterSkinNotCullEvent(OnSkinNotCull);
                m_SkinControl.RegsiterSkinChangedEvent(OnSkinChanged);
            } 
            return m_SkinControl;
        }
    }
    protected EntitySkinControl m_SkinControl;

    protected Transform currentSkinDataObj
    {
        get
        {
            if(this.SkinControl)
            {
                if(!SkinInstance.isNullOrEmpry(SkinControl.CurrentSkinInstance))
                {
                    return SkinControl.CurrentSkinInstance.SkinDataObj;
                }
            }
            return null;
        }
    }

    public Transform[] excludeParentList
    {
        get
        {
            if(null == m_excludeParentList)
            {
                m_excludeParentList = new Transform[1];
            }
            m_excludeParentList[0] = currentSkinDataObj;
            return m_excludeParentList;
        }
    }
    protected Transform[] m_excludeParentList = null;

    /// <summary>
    /// 重置所有成员变量，恢复到实例化之前的状态
    /// </summary>
    public virtual void ResetAllMembers()
    {
        if (null != m_stateHandlers)
        {
            m_stateHandlers.Initialize();
        }

        m_CurStateHandler = null;

        m_MaskVisible = true;                       // Mask控制的可见性
        m_OnViewVisible = false;                    // 视野控制的可见,默认不在视野
        m_bVisible = true;                          // 可见性，这个可见性由上面的多个可见性一起决定

        currentGravity = 0.0f;
        isHero = false;
        canControl = true;
        isInScreen = true;
        bPauseMachineUpdate = false;

        bNotMove = false;
        outlineColorFriend=new Color(0.51f,0.84f,0.137f,1.0f);
        outlineColorEnemy = new Color(1.0f, 0.0f, 0.0f, 1.0f);
        outlineWitdh=0.008f;
        maxRunForwardSpeed = 0.0f; //考虑行走/轻功状态后，前进时最大此速度
        maxForwardSpeed = 0.0f; //服务器传下来的控制速度，前进时最大此速度
        maxRunBackwardSpeed = 0.0f; //服务器传下来的控制速度，后退时最大此速度
        maxRunSidewaySpeed = 0.0f; //服务器传下来的控制速度，左右时最大此速度

        entityID = 0;
        animator = null;                  // 动画系统对象
        creature= null;                 // 生物对象

        wound= null;                     // 生物对象受击点，同其他模块取受击点用
        creaturePropety = null;                 // 生物对象属性

        if(m_SkinControl)
        {
            m_SkinControl.UnRegsiterSkinCullEvent(OnSkinCull);
            m_SkinControl.UnRegsiterSkinNotCullEvent(OnSkinNotCull);
            m_SkinControl.UnRegsiterSkinChangedEvent(OnSkinChanged);
        }
        m_SkinControl = null;
        //castLayer = null;
        showLog = false;
        animationTarget = null;
        m_pickableCount = 1;
        groundNormal = Vector3.zero;
        baseLayer = LayerMask.NameToLayer(Config.LayerDefault);

        m_highLightEnable = false;
        AssetBundleManager.DeleteAssets(ref m_highLightMaterialRes,false);
        m_highLightMaterial = null;
        m_highLightMaterialInstance = null;
        m_nLastHighLightTick = 0;
        fDelightTime=0.15f; //亮0.15秒后灭
        fDisableTime = 3.0f; //3.0秒后删除，设一个比fDelightTime大的时间，避免反复删除
        g_highLightColor = Color.white;
        fMaxHighLightAlpha = 0.7f;
        m_fHighLightAlpha = 0.0f;
        if(recordMaterial != null)
        {
            recordMaterial.Clear();
        }
        
        //recordMaterial = null;


        
       // m_recordTransform = null;

        m_outlineEnable = false;
        outlineColor=Color.gray;
        m_outLineCount = 0;
        AssetBundleManager.DeleteAssets(ref m_goOutlineFrameRes,false);
        if(m_goOutlineFrame)
        {
            ResNode.DestroyRes(ref m_goOutlineFrame);
        }
        m_goOutlineFrame = null;

        if (m_outlineComponent)
        {
            ResNode.DestroyRes(ref m_outlineComponent);
        }
        m_outlineComponent = null;

        m_fOrgScale = Vector3.one;
        m_fShineScale = 1.0f;
        shineScale = 0.92f;
        shineSeconds = 0.05f;

        checkPosition=false;
        requestServerPos = false;
        serverPosition = Vector3.zero;
        severClientPos = Vector3.zero;
        serverpossphere = null;
        
        m_bUnPickableBuff = false;
        m_bUnPickableBuffList = null;

        AssetBundleManager.DeleteAssets(ref m_XRayMaterialRes,false);
        m_XRayMaterial = null;
    }


    public virtual void Init()
    {
        if (m_bUnPickableBuffList == null)
        {
            m_bUnPickableBuffList = new List<int>();
        }
       
        m_bUnPickableBuff = false;

        if(null == recordMaterial)
        {
            recordMaterial = new Dictionary<Renderer, Dictionary<int, Material>>();
        }
        
        
    }

    /// <summary>
    /// 重置对象
    /// </summary>
    public virtual void Clear()
    {
        //castLayer = null;
        animator = null;
        creature = null;
        creaturePropety = null;
        m_SkinControl = null;
    }

    ///// <summary>
    ///// 目标检测
    ///// </summary>
    //[HideInInspector]
    //public ScreenCastLayer castLayer = null;

    #region LOD相关

    public virtual bool GetAnimatorBool(string name)
    {
        return false;
    }

    public virtual void SetAnimatorBool(string name, bool value)
    {
        
    }

    public virtual int GetAnimatorInteger(string name)
    {
        return 0;
    }

    public virtual void SetAnimatorInteger(string name, int value)
    {

    }

    public virtual void SetAnimatorTrigger(string name)
    {

    }

    public virtual float GetAnimatorFloat(string name)
    {
        return 0.0f;
    }

    public virtual void SetAnimatorFloat(string name, float value)
    {

    }

    public virtual void SetAnimatorLayerWeight(int LayerIndex, int value)
    {

    }

    public virtual void AnimatorPlay(string name, float durationTime = 0.1f)
    {

    }
    #endregion

    public bool showLog = false;

 

    public virtual bool DoAct(ActID id)
    {
        return true;
    }

    public virtual bool ChangeState(int id, IntPtr lParam)
    {

        return true;
    }

    public virtual EntityState GetState()
    {
        return EntityState.Standing;
    }
    public virtual bool IsCurAct(string tag, int layer = 0)
    {
        return false;
    }


    private Vector3 m_groundNormal = Vector3.zero;
    public Vector3 groundNormal
    {
        set
        {
            float yDis = Mathf.Abs(m_groundNormal.y - value.y);
            m_groundNormal = value;
        }
        get
        {
            return m_groundNormal;
        }
    }


    public virtual bool IsGrounded()
    {
        return false;
    }

    public virtual bool isFloating()
    {
        return false;
    }

    public virtual bool DoJump(ActID id, BaseStateMachine fsm)
    {
        return false;
    }

    public virtual bool ChangeWingState(IntPtr ptrParam)
    {
        return false;
    }
    public virtual bool ChangeGlideState()
    {
        return false;
    }

    public virtual bool UpdateGlideData(IntPtr ptrParam)
    {
        return false;
    }

    public virtual void CtrlSync(ref cmd_creature_act_sync ctrl)
    {
    }

    public virtual void PositionSync(ref cmd_creature_rigidbody_sync sync)
    {
    }

    public virtual void ForceMove(cmd_force_move param)
    {
    }

    public virtual void ForceFlight(cmd_effect_Flight param)
    {
    }

    public virtual void AutoMoveTo(Vector3 targetPos, bool rotateCamera = true)
    {
    }
    public virtual void StopAutoMove(bool userStop = true)
    {
    }

    public virtual void FaceTo(Vector3 targetPos)
    {
    }

    public virtual void PlayAnim(string name, bool react=false, float speed=1.0f, float durationTime = 0.0f, float normalizedTime = 0.0f)
    {
    }

    public Transform animationTarget = null; //动作目标，用作attackMove判断位移使用
    public void setAnimatorTarget(int id)
    {
        animationTarget = null;
        if (id > 0)
        {
            U3D_Render.EntityView view = EntityFactory.getEntityViewByID(id);
            if (view != null && view.gameObject)
            {
                animationTarget = view.gameObject.transform;
            }
        }
    }

    public virtual void PlayWeaponAnim(string name, int targetID, float speed = 1.0f)
    {

    }

    public virtual void BreakAttackAnim(string animName="",float crossFaceTime=0.1f)
    {
    }

    /// <summary>
    /// 传送
    /// </summary>
    /// <param name="pos"></param>
    public virtual void Transport(cmd_creature_transport pos)
    {

    }

    public virtual void SyncPos2Server()
    {

    }

    /// <summary>
    /// 实体进入视野
    /// </summary>
    /// <param name="data"></param>
    public virtual void OnEnterView()
    {
        m_OnViewVisible = true;
        UpdateVisible();
    }

    /// <summary>
    /// 是否在视野内，这个视野指逻辑层视野，不是指摄像机的视野
    /// </summary>
    /// <returns></returns>
    public virtual bool isOnView_Logic()
    {
        return m_OnViewVisible;
    }

    public System.Action<int> deadEvent = null;
    /// <summary>
    /// 实体死亡
    /// </summary>
    public virtual void OnEntityDead(IntPtr param)
    {
        if (deadEvent != null)
        {
            deadEvent(entityID);
        }

        ChangeState((int)EntityState.Dead, param);
    }

    #region PK设置用

    /// <summary>
    /// 设置PK层
    /// </summary>
    /// <param name="data"></param>
    private bool m_isPK = false;
    private int m_PKLayer;
    //private SPELL.CampFlag m_baseFlag;
    public void setPK(bool enable)
    {
        U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(entityID);
        if(null == objev)
        {
            return;
        }
        m_isPK = enable;
        if (m_isPK)
        {
            if (m_outlineComponent)
            {
                outlineColor = outlineColorEnemy;
                m_outlineComponent.color=outlineColorEnemy;
            }
            m_PKLayer = LayerMask.NameToLayer(Config.LayerPlayerEnemy);
        }
        else
        {
            switch (objev.CampFlag)
            {
                case SPELL.CampFlag.CampFlag_Friend:
                {
                    baseLayer = LayerMask.NameToLayer(Config.LayerPlayerFriend);
                }
                break;
                case SPELL.CampFlag.CampFlag_Enemy:
                {
                    baseLayer = LayerMask.NameToLayer(Config.LayerPlayerEnemy);
                }
                break;
                default:
                break;
            }

            m_PKLayer = baseLayer;
            if (m_outlineComponent)
            {
                outlineColor = outlineColorFriend;
                if (SPELL.CampFlag.CampFlag_Enemy == objev.CampFlag)
                {
                    outlineColor = outlineColorEnemy;
                }
                m_outlineComponent.color = outlineColor;
            }
        }

        if (m_pickableCount > 0)
        {

            GameUtil.SetLayer(m_PKLayer, transform.gameObject, true, excludeParentList);
        }

        //更新选中框
        if (m_goOutlineFrame)
        {
            m_goOutlineFrameRes.DestroyInstanceRes(ref m_goOutlineFrame);
            m_goOutlineFrame = null;
        }
    }

    /// <summary>
    /// 主角记录PK对象的ID，当PK对象离开视野再进入时，以此恢复敌方的layer
    /// </summary>
    public int PK_targetID=0;

    #endregion


    /// <summary>
    /// 实体离开视野
    /// </summary>
    /// <param name="data"></param>
    public virtual void OnLeaveView()
    {
        m_OnViewVisible = false;
        UpdateVisible();
    }

    private int m_pickableCount = 1;
    public int pickableCount
    {
        
        set
        {
            //if (this is CreatureStateMachine)
            //{
            //    //StackTrace stst = new StackTrace(true);
            //    //StackFrame[] sf = stst.GetFrames();
            //    //string info = "";
            //    //for (int i = 0; i < sf.Length; ++i)
            //    //{
            //    //    info = info + "\r\n" + " FileName=" + sf[i].GetFileName() + " fullname=" + sf[i].GetMethod().DeclaringType.FullName + " function=" + sf[i].GetMethod().Name + " FileLineNumber=" + sf[i].GetFileLineNumber();
            //    //}
            //    //UnityEngine.Debug.LogWarning("跟踪无法选中BUG专用LOG。" + this.name + " pickableCount=" + value);
            //    //UnityEngine.Debug.LogWarning("跟踪无法选中BUG专用LOG-Stack:" + info);
            //}
            if (m_pickableCount > 0 && value <= 0)
            {
                //if (this is CreatureStateMachine)
                //{
                //    UnityEngine.Debug.LogWarning("跟踪无法选中BUG专用LOGzccccc。" + this.name + " pickableCount=" + value);
                //}
                U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(this.entityID);
                if (ev != null &&
                    ev.Flag != (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_HP_SUPPLIES &&
                    ev.Flag != (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_DEFENSIVE_TOWER &&
                    ev.Flag != (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_BASE && 
                    ev.Flag != (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_CRYSTAL_TOWER)
                {
                    GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerIngoreRayCast), transform.gameObject, true);
                }
            }
            else if (m_pickableCount <= 0 && value > 0)
            {
                if (m_isPK)
                {
                    GameUtil.SetLayer(m_PKLayer, transform.gameObject, true);
                }
                else
                {
                    GameUtil.SetLayer(baseLayer, transform.gameObject, true);
                    //关于ward结界类型的特殊处理
                    //结界类型的物理盒子放在LayerWard层，用于阻挡指定类型的玩家
                    //但是结界有可以选中和不可以选中，
                    //这里只将配置的ScreenCastObject改为其他层，那么不可选中的结界不配置ScreenCastObject，就不影响结界后面实体的技能选中，
                    //除了ScreenCastObject之外，实体的其他部分仍然为LayerWard，
                    //那么hit到了之后LayerWard在判断实体有效性的时候，也将LayerWard加入有效层的范围内，这样就结界的选中就是合法的了。

                    if (baseLayer == LayerMask.NameToLayer(Config.LayerWard_Enemy))
                    {
                        if (creaturePropety && creaturePropety.ScreenCastObject)
                        {
                            GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerMonsterEnemy), creaturePropety.ScreenCastObject, false);                            
                        }
                    }
                    else if (baseLayer == LayerMask.NameToLayer(Config.LayerWard_Friend))
                    {
                        if (creaturePropety && creaturePropety.ScreenCastObject)
                        {
                            GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerMonsterFriend), creaturePropety.ScreenCastObject, false);
                        }
                    }
                }
            }
            m_pickableCount = value;
        }
        get
        {
            return m_pickableCount;
        }
    }

    public virtual void SetVisible(bool bVisible)
    {
        LightingEffectManager leManager = GetComponent<LightingEffectManager>();
        if (leManager != null)
        {
            leManager.setEnable(bVisible);
        }
        if (bVisible == true && m_bVisible==false)
        {
            pickableCount++;
            //if (GetState() != EntityState.Dead)
            //{
                //GameUtil.SetLayer(baseLayer, transform.gameObject, true);                
            //}
        }
        else if (bVisible == false && m_bVisible==true)
        {
            //GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerIngoreRayCast), transform.gameObject, true);
            pickableCount--;
        }
        m_bVisible = bVisible;
    }

    [System.NonSerialized]
    public int baseLayer = 1;
    protected virtual void UpdateVisible()
    {
        bool visible = m_OnViewVisible && m_MaskVisible;
        //if ((m_bVisible == false && visible == true) || (m_bVisible == true && visible == false)) 注释掉，处理以下下矛盾：以皮肤A出视野（隐藏），再以皮肤B进视野（可见），然后再切回皮肤A（再次被隐藏）
        {
            SetVisible(visible);
        }
        //m_bVisible =visible; SetVisible里已经更新
    }

    /// <summary>
    /// 当皮肤已经被改变-消息通知
    /// </summary>
    /// <param name="nSkinID">皮肤ID</param>
    /// <param name="nPart">哪个部位</param>
    protected virtual void OnSkinChanged(SkinInstance sk, int nPart)
    {
        if (creaturePropety != null && creaturePropety.effectIDForChangeSkin > 0)
        {
            creaturePropety.Master.LightingEffectManager.BindLighting(creaturePropety.effectIDForChangeSkin);
        }
        wound = FindTransformEx("Wound");
        currentGravity = creaturePropety.downGravity;
        UpdateVisible();
    }

    protected virtual void OnSkinCull()
    {

    }

    protected virtual void OnSkinNotCull()
    {

    }

    public virtual void ChangeMask(U3D_Render.EntityView ev)
    {

    }

	public virtual void gotoGuard(bool bGuard)
	{

	}

    public virtual void enableCollider(bool enable)
    {
    }

    public void Start()
    {
        m_bUnPickableBuffList = new List<int>();
        m_bUnPickableBuff = false;
        recordMaterial = new Dictionary<Renderer, Dictionary<int, Material>>();
    }

    //找人物下面某个骨骼
    public Transform FindTransformEx(string boneName)
    {
        if(SkinControl)
        {
            if(null != SkinControl.CurrentSkinInstance)
            {
                return SkinControl.CurrentSkinInstance.GetBonesTransform(boneName);
            }
        }
        return null;
    }


    public Vector3 getVelocity()
    {
        return m_CurStateHandler.getVelocity();
    }
    private bool m_highLightEnable = false;
    private ResNode m_highLightMaterialRes = null;
    private Material m_highLightMaterial = null;
    private List<Material> m_highLightMaterialInstance = null;
    private uint m_nLastHighLightTick = 0;
    public float fDelightTime=0.15f; //亮0.15秒后灭
    private float fDisableTime = 3.0f; //3.0秒后删除，设一个比fDelightTime大的时间，避免反复删除
    public Color g_highLightColor = Color.white;
    public float fMaxHighLightAlpha = 0.7f;
    private float m_fHighLightAlpha = 0.0f;

    private void setHighLightColor(float time)
    {
        if (m_highLightMaterialInstance == null || m_highLightEnable == false)
        {
            return;
        }
        foreach (Material mat in m_highLightMaterialInstance)        
        {
            mat.SetFloat("_time", time);
        }
    }

    private void updateHighLightColor()
    {
        if (m_highLightEnable==false)
        {
            return;
        }
        uint tick = GameLogicAPI.getTickCount();
        float deltaTime = ((float)(tick - m_nLastHighLightTick)) / 1000.0f;
        if (deltaTime > fDisableTime)
        {
            setHightLight(false);
            return;
        }
        setHighLightColor(deltaTime);
    }

    private Dictionary<Renderer, Dictionary<int, Material>> recordMaterial;

    public void changeMaterial(int index, Material material)
    {
        if (SkinControl == null || SkinControl.CurrentSkinInstance == null)
        {
            return;
        }

        if (SkinControl.CurrentSkinInstance.AllRenders == null)
        {
            return;
        }
        foreach (Renderer r in SkinControl.CurrentSkinInstance.AllRenders)
        {
            if (r != null)
            {
                if (r.materials != null && r.materials.Length>index)
                {
                    if (!recordMaterial.ContainsKey(r))
                    {
                        recordMaterial[r] = new Dictionary<int, Material>();
                    }

                    if (!recordMaterial[r].ContainsKey(index))
                    {
                        recordMaterial[r][index] = r.materials[index];                        
                    }
                    //r.materials[index] = material;
                    int mLength = r.materials.Length;
                    Material[] newMaterials = new Material[mLength];
                    Array.Copy(r.materials, newMaterials, mLength);
                    newMaterials[index] = material;
                    r.materials = newMaterials;
                }
            }
        }
        
    }

    public void recoverMaterial(int index)
    {
        if (SkinControl == null)
        {
            return;
        }

        if (SkinControl.CurrentSkinInstance == null || SkinControl.CurrentSkinInstance.AllRenders == null)
        {
                return;
        }
        foreach (Renderer r in SkinControl.CurrentSkinInstance.AllRenders)
        {
            if (r != null)
            {
                if (r.materials != null && r.materials.Length > index)
                {
                    if (recordMaterial.ContainsKey(r))
                    {
                        if (recordMaterial[r].ContainsKey(index))
                        {
                            //r.materials[index] = recordMaterial[r][index];
                            int mLength = r.materials.Length;
                            Material[] newMaterials = new Material[mLength];
                            Array.Copy(r.materials, newMaterials, mLength);
                            newMaterials[index] = recordMaterial[r][index];
                            r.materials = newMaterials;
                        }
                    }
                }
            }
        }
        
    }



    public void setHightLight(bool enable)
    {
        if (m_highLightMaterial == null)
        {
            m_highLightMaterialRes = AssetBundleManager.GetAssets(AssetType.Asset_Material, "CommonArtist/Effect/selectEffect/HighLightMaterial");
            if (null != m_highLightMaterialRes)
            {
                m_highLightMaterial = m_highLightMaterialRes.ConvertRes<Material>();
            }
            
        }

        if (m_highLightMaterialInstance == null)
        {
            m_highLightMaterialInstance = new List<Material>();
        }

        if (enable != m_highLightEnable)
        {
            //material需要调整，否则只调亮度

            if (SkinControl.CurrentSkinInstance == null || SkinControl.CurrentSkinInstance.AllRenders == null)
            {
                return;
            }
            foreach (Renderer r in SkinControl.CurrentSkinInstance.AllRenders)
            {
                if (r != null)
                {
                    if (r.materials != null)
                    {
                        if (enable && m_highLightMaterial != null)
                        {
                            //新增一个material
                            int mLength = r.materials.Length;
                            Material[] newMaterials = new Material[mLength + 1];                                
                            Array.Copy(r.materials, newMaterials, mLength);
                            newMaterials[mLength] = m_highLightMaterial;
                            r.materials = newMaterials;

                            m_highLightMaterialInstance.Add(r.materials[mLength]);
                        }
                        else if (m_highLightMaterial != null)
                        {
                            //删除新增的material
                            List<Material> recoverMaterials = new List<Material>();
                            string instanceName = m_highLightMaterial.name + " (Instance)";
                            foreach (Material mat in r.materials)
                            {
                                if ((mat.name != instanceName) && (mat.name != m_highLightMaterial.name))
                                {
                                    recoverMaterials.Add(mat);
                                }
                                    
                            }
                            r.materials = recoverMaterials.ToArray();
                            m_highLightMaterialInstance.Clear();
                        }
                    }
                }
            }
            
        }

        m_highLightEnable = enable;
        if (enable)
        {
            m_nLastHighLightTick = GameLogicAPI.getTickCount();
            setHighLightColor(0);
        }
        else
        {
            m_fHighLightAlpha = 0.0f;
            m_nLastHighLightTick = 0;
        }
    }

    public bool OutlineEnable
    {
        get { return m_outlineEnable; }
    }
    private bool m_outlineEnable = false;
    //private Material m_outlineMaterial = null;

   [System.NonSerialized]
    public Color outlineColor=Color.gray;
    private int m_outLineCount = 0;
    public int outLineCount
    {
        get { return m_outLineCount; }
        set
        {
            if (m_outLineCount > 0 && value <= 0) setOutline(false);
            if (m_outLineCount <= 0 && value > 0) setOutline(true);
            m_outLineCount = value;
        }
    }

    ResNode m_goOutlineFrameRes = null;
    GameObject m_goOutlineFrame = null;
    GlowOutLineObject m_outlineComponent = null;
    private void setOutline(bool enable)
    {
        if (enable == m_outlineEnable)
        {
            return;
        }
        U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(entityID);
        if (objev == null)
        {
            return;
        }
        m_outlineEnable = enable;

        if (enable)
        {
            //画框框
            if (!m_goOutlineFrame)
            {
                if (((SPELL.CampFlag.CampFlag_Enemy == objev.CampFlag) && (!isNotAttack)) || m_isPK)
                {
                    m_goOutlineFrameRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"CommonPrefab/Effect/selectEffect/selectFrameRed");
                    if (null != m_goOutlineFrameRes)
                    {
                        m_goOutlineFrame = m_goOutlineFrameRes.InstanceMainRes();
                    }
                    
                }
                else
                {
                    m_goOutlineFrameRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"CommonPrefab/Effect/selectEffect/selectFrameGreen");
                    if (null != m_goOutlineFrameRes)
                    {
                        m_goOutlineFrame = m_goOutlineFrameRes.InstanceMainRes();
                    }

                }
            }
            if (m_goOutlineFrame)
            {
                m_goOutlineFrame.SetActive(true);
                m_goOutlineFrame.transform.parent = transform;
                //float scale=creaturePropety.CreatureBoundingRad*6.0f;                
                //m_goOutlineFrame.transform.localScale = new Vector3(scale, scale, scale);
            }

            //加勾边
            GameObject skin = getCurrentSkin();
            if (!m_outlineComponent)
            {
                if (skin)
                {
                    m_outlineComponent = skin.AddComponent<GlowOutLineObject>();
                    outlineColor = outlineColorFriend;
                    if (((SPELL.CampFlag.CampFlag_Enemy == objev.CampFlag) && (!isNotAttack)) || m_isPK)
                    {
                        outlineColor = outlineColorEnemy;
                    }
                    m_outlineComponent.color = outlineColor;
                }
            }
            else
            {
                m_outlineComponent.enabled = true;
            }
        }
        else
        {
            if (m_goOutlineFrame)
            {
                m_goOutlineFrame.SetActive(false);
            }
            if (m_outlineComponent)
            {

                m_outlineComponent.enabled = false;
            }
        }
    }

    private ResNode m_XRayMaterialRes = null;
    private Material m_XRayMaterial = null;
    private bool m_XRayEffect = false;

    public void setXRay(bool enable)
    {
        if (m_XRayMaterial == null)
        {
            m_XRayMaterialRes = AssetBundleManager.GetAssets(AssetType.Asset_Material, "CommonArtist/Effect/selectEffect/XRay");
            if(null != m_XRayMaterialRes)
            {
                m_XRayMaterial = m_XRayMaterialRes.ConvertRes<Material>();
            }
        }
        List<SkinnedMeshRenderer> renders = SkinControl.CurrentSkinInstance.SkinnedMeshRenderList;
        if (renders == null)
        {
            return;
        }
        foreach (SkinnedMeshRenderer r in renders)
        {
            if (r != null && r.GetComponent<ParticleSystem>() == null)
            {
                if (r.materials != null && m_XRayMaterial != null)
                {
                    if (enable && !m_XRayEffect)
                    {
                        //新增一个material
                        int mLength = r.materials.Length;
                        Material[] newMaterials = new Material[mLength + 1];
                        Array.Copy(r.materials, newMaterials, mLength);
                        newMaterials[mLength] = m_XRayMaterial;
                        r.materials = newMaterials;
                        m_XRayEffect = true;

                        //outlineColor = outlineColorFriend;
                        //if (SPELL.CampFlag.CampFlag_Enemy == objev.CampFlag)
                        //{
                        //    outlineColor = outlineColorEnemy;
                        //}


                        //if (m_outlineMaterial.HasProperty("_OutlineColor"))
                        //{
                        //    m_outlineMaterial.SetColor("_OutlineColor", outlineColor);
                        //}
                        //if (m_outlineMaterial.HasProperty("_Factor2"))
                        //{
                        //    m_outlineMaterial.SetFloat("_Factor2", 1.0f);
                        //}
                        //if (m_outlineMaterial.HasProperty("_Outline2"))
                        //{
                        //    m_outlineMaterial.SetFloat("_Outline2", 0.005f);
                        //}
                    }
                    else if (!enable && m_XRayEffect)
                    {
                        //删除新增的material
                        List<Material> recoverMaterials = new List<Material>();
                        foreach (Material mat in r.materials)
                        {
                            string instanceName = m_XRayMaterial.name + " (Instance)";
                            if ((mat.name != instanceName) && (mat.name != m_XRayMaterial.name))
                            {
                                recoverMaterials.Add(mat);
                            }
                        }
                        r.materials = recoverMaterials.ToArray();
                        m_XRayEffect = false;
                    }
                }
            }
        }
    }

    private void updateOutlineFrame()
    {
        if (!m_outlineEnable) return;
        if (!m_goOutlineFrame) return;
        Transform trans = m_goOutlineFrame.transform;
        if (!trans) return;
        Vector3 from = transform.position;
        from.y += creaturePropety.CreatureBoundingRad.z;
        Vector3 deta = Initialize.mainCam.transform.position - from;
        //为了方便策划配置，这里实时调整
        trans.localScale = new Vector3(6.0f * creaturePropety.CreatureBoundingRad.x,6.0f * creaturePropety.CreatureBoundingRad.y,1.0f);
        float dis = Mathf.Max(creaturePropety.CreatureBoundingRad.x, creaturePropety.CreatureBoundingRad.y);
        from = from + deta.normalized * dis;
        trans.SetPosition(from);
        trans.LookAt(Initialize.mainCam.transform.position);

        if (!m_outlineComponent) //可能之前skin还没生成
        {
            GameObject skin = getCurrentSkin();
            if (!skin) return;
            U3D_Render.EntityView objev = EntityFactory.getEntityViewByID(entityID);
            if (objev == null) return;
            m_outlineComponent = skin.AddComponent<GlowOutLineObject>();
            outlineColor = outlineColorFriend;
            if (SPELL.CampFlag.CampFlag_Enemy == objev.CampFlag || m_isPK)
            {
                outlineColor = outlineColorEnemy;
            }
            m_outlineComponent.color = outlineColor;
            
        }
    }


    //怪物变大变小
    private Vector3 m_fOrgScale = Vector3.one;
    private float m_fShineScale = 1.0f;

    public float shineScale = 0.92f;
    public float shineSeconds = 0.05f;


    public void ShineScale()
    {        
        if (m_fShineScale == 1.0f)
        {
            m_fOrgScale = transform.localScale;
        }
        transform.localScale = m_fOrgScale * shineScale;
        m_fShineScale = shineScale;

        StartCoroutine(resetShineScale(shineSeconds));
    }

    IEnumerator resetShineScale(float seconds)
    {
        yield return new WaitForSeconds(seconds);        
        transform.localScale = m_fOrgScale;
        m_fShineScale = 1.0f;
    }

    public GameObject getCurrentSkin()
    {
        return SkinControl.GetCurrentModelObject();
    }



    private static void EnumStatesImp(Animator animator, ref Hashtable statedic, object sm)
    {
        Type t;

        t = sm.GetType();
        if (t == null)
        {
            return;
        }

        //Debug.Log("sm:-----------------------------------");
        //MethodInfo[] mis = t.GetMethods();
        //PropertyInfo[] pis = t.GetProperties();
        //foreach (MethodInfo mi in mis)
        //{
        //    Debug.Log("MethodInfo:" + mi.Name);
        //}
        //foreach (PropertyInfo pi in pis)
        //{
        //    Debug.Log("PropertyInfo:" + pi.Name);
        //}

        MethodInfo mi4 = t.GetMethod("get_stateCount");
        if (mi4 == null)
        {
            return;
        }

        int stateCount = (int)mi4.Invoke(sm, null); //stateCount=sm.get_stateCount

        MethodInfo mi5 = t.GetMethod("GetState");
        if (mi5 == null)
        {
            return;
        }

        for (int i = 0; i < stateCount; i++)
        {
            object state = mi5.Invoke(sm, new object[] { i });  //state=sm.GetState(i)
            if (state != null)
            {
                t = state.GetType();
                if (t == null)
                {
                    continue;
                }

                MethodInfo mi6 = t.GetMethod("get_name");
                if (mi6 == null)
                {
                    continue;
                }

                string statename = (string)mi6.Invoke(state, null);
                if (statename == null)
                {
                    continue;
                }

                statedic[statename] = state;

                //Debug.Log("state:-----------------------------------");
                //MethodInfo[] mis = t.GetMethods();
                //PropertyInfo[] pis = t.GetProperties();
                //foreach (MethodInfo mi in mis)
                //{
                //    Debug.Log("MethodInfo:" + mi.Name);
                //}
                //foreach (PropertyInfo pi in pis)
                //{
                //    Debug.Log("PropertyInfo:" + pi.Name);
                //}
            }
        }

        t = sm.GetType();
        if (t == null)
        {
            return;
        }

        MethodInfo mi7 = t.GetMethod("get_stateMachineCount");
        if (mi7 == null)
        {
            return;
        }

        int stateMachineCount = (int)mi7.Invoke(sm, null); //stateCount=sm.get_stateCount

        MethodInfo mi8 = t.GetMethod("GetStateMachine");
        if (mi8 == null)
        {
            return;
        }

        for (int i = 0; i < stateMachineCount; i++)
        {
            object childsm = mi8.Invoke(sm, new object[] { i });  //state=sm.GetState(i)
            if (childsm != null)
            {
                EnumStatesImp(animator, ref statedic, childsm);

                //Debug.Log("childsm:-----------------------------------");
                //MethodInfo[] mis = t.GetMethods();
                //PropertyInfo[] pis = t.GetProperties();
                //foreach (MethodInfo mi in mis)
                //{
                //    Debug.Log("MethodInfo:" + mi.Name);
                //}
                //foreach (PropertyInfo pi in pis)
                //{
                //    Debug.Log("PropertyInfo:" + pi.Name);
                //}
            }
        }
    }



    public static bool BuildStateMapImp(Animator animator, ref Hashtable statedic)
    {
        if (animator == null)
        {
            return false;
        }
        RuntimeAnimatorController ac = animator.runtimeAnimatorController;
        if (ac == null)
        {
            return false;
        }

        Type t;

        t = ac.GetType();
        if (t == null)
        {
            return false;
        }

        PropertyInfo pi1 = t.GetProperty("layerCount");
        if (pi1 == null)
        {
            return false;
        }
        MethodInfo mi1 = pi1.GetGetMethod();
        if (mi1 == null)
        {
            return false;
        }
        int layerCount = (int)mi1.Invoke(ac, null);


   
        for (int k = 0; k < layerCount; k++)
        {
            t = ac.GetType();
            if (t == null)
            {
                continue;
            }

            MethodInfo mi2 = t.GetMethod("GetLayer");
            if (mi2 == null)
            {
                continue;
            }

            object layer = mi2.Invoke(ac, new object[] { k });
            if (layer == null)
            {
                continue;
            }

            t = layer.GetType();
            if (t == null)
            {
                continue;
            }

            MethodInfo mi3 = t.GetMethod("get_stateMachine");
            if (mi3 == null)
            {
                continue;
            }

            object sm = mi3.Invoke(layer, null);  //sm=layer.get_stateMachine
            if (sm == null)
            {
                continue;
            }

            EnumStatesImp(animator, ref statedic, sm);

        }

        return true;
    }

    public delegate bool BuildStateMapHanlder(Animator animator, ref Hashtable statedic);
    public static event BuildStateMapHanlder BuildStateMapFun = BuildStateMapImp;


    public virtual bool GetVisible()
    {
        return m_bVisible;
    }

    /// <summary>
    /// 打开位置检测，会有一个小圆球下来
    /// </summary>
    public bool checkPosition=false;

    /// <summary>
    /// 请求在上的服务器位置
    /// </summary>
    [System.NonSerialized]
    public bool requestServerPos = false;

    /// <summary>
    /// 在服务器上的位置
    /// </summary>
    public Vector3 ServerPos
    {
        get
        {
            return serverPosition;
        }
    }
    private void updatePositionCheck()
    {
        if (Time.frameCount % 4 == 0)
        {
            if (checkPosition || requestServerPos)
            {
                SendCheckPositionMsg();
            }
        }

    }

    private void SendCheckPositionMsg()
    {
        string strParam = "SupperStone_ShowPosition(" + entityID.ToString() + ")";
        IntPtrHelper helper = new IntPtrHelper();
        IntPtr strParam_ptr = helper.Str2Ptr(strParam);
        int eventID = (int)EntityLogicDef.ENTITY_CMD_DRAGONBALL;
        GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, eventID, 0, strParam, strParam_ptr, GameLogicAPI.getStringLen(strParam_ptr));
    }
    private Vector3 serverPosition = Vector3.zero;
    private Vector3 severClientPos = Vector3.zero;
    protected GameObject serverpossphere = null;
    public void onRecivePositoinCheck(Vector3 severPos)
    {
        serverPosition = severPos;

        if (!checkPosition)
        {
            return;
        }

        severClientPos = transform.position;

        if (serverpossphere == null)
        {
            serverpossphere = GameObject.CreatePrimitive(PrimitiveType.Capsule);
            Collider c = serverpossphere.GetComponent<Collider>();
            if (c != null)
            {
                ResNode.DestroyRes(ref c);
            }
        }
        serverpossphere.transform.SetPosition(serverPosition);
    }


    public void update()
    {
        isInScreen = ScreenRaycast.isInScreen(transform.position);

        updateHighLightColor();
        updateOutlineFrame();
        updatePositionCheck();
    }

    //服务器传下来的移动速度改变时，调用这个函数，更新下移动动画的速率，造成快跑或者慢跑的效果
    public virtual void updateMovingAnimSpeed()
    {
    }



    public void OnGUIEx()
    {
        if (checkPosition)
        {
            Vector3 cpos = transform.position;
            string disTip = "EntityID=" + entityID + "\n" + "最近服务器位置" + serverPosition.ToString() + "\n" + "同期客户端位置" + severClientPos.ToString() + "\n" + "距离" + (serverPosition - cpos).magnitude.ToString();
            Rect TipsRec = new Rect(50, 50, 250, 75);
            GUI.Label(TipsRec, disTip);
        }
    }

    ////判断自身是否被地面遮挡,startPos出发点，为0时表示摄像机点
    //public bool isBlocked(Vector3 startPos)
    //{
    //    RaycastHit hitInfo;
    //    Vector3 cameraPos=Initialize.mainCam.transform.position;
    //    if (startPos.sqrMagnitude > 0.001f)
    //    {
    //        cameraPos = startPos;
    //    }
    //    Vector3 creaturePos=transform.position;
    //    if (wound!=null)
    //    {
    //        creaturePos=wound.position;
    //    }
    //    Vector3 ray=creaturePos-cameraPos;
    //    Vector3 rayDir=ray.normalized;
    //    Ray castRay = new Ray(cameraPos, rayDir);
    //    int LayerMaskOnGournd = (1 << LayerMask.NameToLayer(Config.LayerDefault) | 1 << LayerMask.NameToLayer(Config.LayerBuilding));

    //    bool isHit = Physics.Raycast(castRay, out hitInfo, ray.magnitude, LayerMaskOnGournd);

    //    if (isHit)
    //    {
    //        return true;
    //    }
    //    return false;
    //}


    #region 根据BUFF状态控制是否可以被鼠标选中

    private bool m_bUnPickableBuff = false;
    private List<int> m_bUnPickableBuffList;
    public void SetPickableBuff(bool unpickable, int buffID, int targetFilter)
    {
        if (isHero) //本身不受影响
        {
            return;
        }

        if (m_bUnPickableBuffList == null)
        {
            return;
        }

        if (unpickable)
        {
            //过滤目标
            if (TargetFilter(targetFilter))
                return;

            //没有被过滤的则设为不可选中
            if (!m_bUnPickableBuffList.Contains(buffID))
            {
                m_bUnPickableBuffList.Add(buffID);
            }
        }
        else
        {
            if (m_bUnPickableBuffList.Contains(buffID))
            {
                m_bUnPickableBuffList.Remove(buffID);
            }
        }

        if (m_bUnPickableBuffList.Count > 0)
        {
            if (m_bUnPickableBuff == false)
            {
                pickableCount--;
            }
            m_bUnPickableBuff = true;
        }
        else
        {
            if (m_bUnPickableBuff == true)
            {
                pickableCount++;
            }
            m_bUnPickableBuff = false;
        }
    }

    #endregion


    //飞檐走壁

    public virtual bool startWallWalk(IntPtr lParam)
    {
        return true;
    }

    public static Vector3 CorrectPosFromPhysic(Vector3 Pos, float UpCheckDistance, float DownGroundCheckDistance = 5.0f)
    {
        Vector3 res = Pos;
        RaycastHit hitInfo;

        //其实位置
        Vector3 startPos = Pos;
        startPos.y += UpCheckDistance;

        //结束位置
        Vector3 endPos = Pos;
        endPos.y -= DownGroundCheckDistance;

        if (Application.isEditor)
        {
            UnityEngine.Debug.DrawLine(startPos, endPos, Color.white);
        }
        int Y_AdjustLayerMask = 1 << LayerMask.NameToLayer(Config.LayerDefault) | 1 << LayerMask.NameToLayer(Config.LayerBuilding);
        //如果检测到碰撞体，则设置为碰撞体的Y值，使得怪物贴地面
        if (Physics.Linecast(startPos, endPos, out hitInfo, Y_AdjustLayerMask))
        {
            res.y = hitInfo.point.y;
        }

        return res;
    }

    //过滤目标
    private bool TargetFilter(int targetFilter)
    {
        if(creaturePropety.Master != null && creaturePropety != null)
        {
                    
            switch (creaturePropety.Master.CampFlag)
            {
                case SPELL.CampFlag.CampFlag_Friend:
                    {
                        if (creaturePropety.Master.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                        {
                            if ((targetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_ROLE) != 0)
                                return true;
                        }
                        else if (creaturePropety.Master.Type == ENTITY_TYPE.TYPE_MONSTER)
                        {
                            int sex = creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);
                            if(sex == (int)MONSTER_TYPE.MONSTER_TYPE_TOWER)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_TOWER) != 0)
                                    return true;
                            }

                            int vocation = creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                            if(vocation == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SOLDIER)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_SOLDIER) != 0)
                                    return true;
                            }
                            else if(vocation == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SUMMON_SMALL_DRAGON)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_SMALL_DRAGON) != 0)
                                    return true;
                            }
                            else if (vocation == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SUMMON_ENTITY)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_FRIENDLY_SUMMON_ENTITY) != 0)
                                    return true;
                            }
                        }
                    }
                    break;
                case SPELL.CampFlag.CampFlag_Enemy:
                    {
                        if (creaturePropety.Master.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                        {
                            if ((targetFilter & (int)TARGET_FILTER.TARGET_ENEMY_ROLE) != 0)
                                return true;
                        }
                        else if (creaturePropety.Master.Type == ENTITY_TYPE.TYPE_MONSTER)
                        {
                            int sex = creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);
                            if (sex == (int)MONSTER_TYPE.MONSTER_TYPE_TOWER)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_ENEMY_TOWER) != 0)
                                    return true;
                            }

                            int vocation = creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                            if (vocation == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SOLDIER)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_ENEMY_SOLDIER) != 0)
                                    return true;
                            }
                            else if (vocation == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SUMMON_SMALL_DRAGON)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_ENEMY_SMALL_DRAGON) != 0)
                                    return true;
                            }
                            else if (vocation == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SUMMON_ENTITY)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_ENEMY_SUMMON_ENTITY) != 0)
                                    return true;
                            }
                        }
                    }
                    break;
                case SPELL.CampFlag.CampFlag_Neutral:
                    {
                        if (creaturePropety.Master.Type == ENTITY_TYPE.TYPE_PLAYER_ROLE)
                        {
                            if ((targetFilter & (int)TARGET_FILTER.TARGET_NEUTRAL_ROLE) != 0)
                                return true;
                        }
                        else if (creaturePropety.Master.Type == ENTITY_TYPE.TYPE_MONSTER)
                        {
                            int sex = creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);
                            if (sex == (int)MONSTER_TYPE.MONSTER_TYPE_TOWER)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_NEUTRAL_TOWER) != 0)
                                    return true;
                            }

                            int vocation = creaturePropety.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                            if (vocation == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_SMALL_DRAGON)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_NEUTRAL_SMALL_DRAGON) != 0)
                                    return true;
                            }
                            else if (vocation == (int)MONSTER_SUB_TYPE.MONSTER_SUB_TYPE_HUGE_DRAGON)
                            {
                                if ((targetFilter & (int)TARGET_FILTER.TARGET_NEUTRAL_HUGE_DRAGON) != 0)
                                    return true;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        return false;
    }
}
